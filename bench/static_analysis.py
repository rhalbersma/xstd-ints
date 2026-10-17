#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Writes llvm-mca's analysis of every bit kernel, for each compiler and each named microarchitecture, under mca/.

static_analysis.cpp gives each kernel a function of its own. Each compiler turns it into assembly for each target, and
llvm-mca simulates the result on that target's scheduling model. A function without a loop is analysed whole, so its
figure is the cost of one call; a function with a loop is analysed by its innermost loop's body, so its figure is the
cost of one trip, that is of one selected bit, one mask bit or one source bit. mca/README.md tabulates llvm-mca's
cycles per iteration, and mca/<compiler>-<cpu>.txt holds its summary beside the assembly it read.

    static_analysis.py --zp7 build/_deps/zp7-src [--clang clang++-22] [--gcc g++-15] [--llvm-mca llvm-mca-22]

zp7 is the checkout CMake fetches when it configures the benchmarks.

AArch64 needs a cross standard library for clang to compile against, such as noble's libstdc++-14-dev-arm64-cross.
"""

import argparse
import pathlib
import re
import subprocess

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent

X86 = ["haswell", "cascadelake", "icelake-server", "znver2", "znver3", "znver4"]
AARCH64 = ["neoverse-n1", "neoverse-n2", "neoverse-v2"]

# Two loop-free kernels before the loops, then the loops by family, in the order the tables in the README list them.
KERNELS = [
    "reverse_by_swaps", "reverse_intrinsic",
    "compress_by_rank", "compress_pext", "compress_by_walk_with_if", "compress_by_parallel_suffix", "compress_by_zp7",
    "compress_by_zp7_clmul",
    "expand_by_walk", "expand_by_select", "expand_pdep", "expand_by_walk_with_if", "expand_by_parallel_suffix",
    "expand_by_zp7", "expand_by_zp7_clmul",
    "select_by_broadword", "select_pdep",
]


def assembly(compiler, flags, zp7):
    command = [compiler, "-std=c++2c", "-O3", "-DNDEBUG", "-S", "-o", "-", "-fno-asynchronous-unwind-tables",
               "-fno-exceptions", "-fcf-protection=none", f"-I{ROOT / 'include'}", f"-I{HERE}", f"-I{zp7}", *flags,
               str(HERE / "static_analysis.cpp")]
    return subprocess.run(command, check=True, capture_output=True, text=True).stdout


def functions(text, comment):
    """Each function's lines, labels and instructions alike, with directives and comments gone."""
    bodies = {}
    current = None
    for line in text.splitlines():
        code = line.split(comment, 1)[0].rstrip()
        stripped = code.strip()
        label = re.fullmatch(r"([.\w$]+):", stripped)
        if label and not code[0].isspace() and not label.group(1).startswith("."):
            current = label.group(1)
            bodies[current] = []
        elif current is None or not stripped:
            continue
        elif stripped.startswith((".Lfunc_end", ".size")) or stripped == ".cfi_endproc":
            current = None
        elif label:
            bodies[current].append(stripped)
        elif not stripped.startswith("."):
            bodies[current].append("\t" + " ".join(stripped.split()))
    return bodies


def is_branch(instruction):
    mnemonic = instruction.split()[0]
    return mnemonic.startswith(("j", "b.")) or mnemonic in ("b", "cbz", "cbnz", "tbz", "tbnz")


def region(body, bodies):
    """The innermost loop's body if the function loops, else the whole function less its return."""
    instructions = [line for line in body if not line.endswith(":")]
    if len(instructions) == 1 and instructions[0].split()[0] in ("jmp", "b") and instructions[0].split()[-1] in bodies:
        # A kernel the compiler kept out of line is reached by a tail call, so its own body is what runs.
        return region(bodies[instructions[0].split()[-1]], bodies)
    labels = {line[:-1]: i for i, line in enumerate(body) if line.endswith(":")}
    loops = []
    for i, line in enumerate(body):
        if line.endswith(":") or not is_branch(line.strip()):
            continue
        target = line.split()[-1]
        if target in labels and labels[target] < i:
            loops.append((i - labels[target], labels[target], i))
    if loops:
        _, start, end = min(loops)
        return "loop body", [line for line in body[start + 1:end + 1] if not line.endswith(":")]
    kept = [line for line in body if not line.endswith(":") and line.split()[0] not in ("ret", "retq")]
    return "whole function", kept


def analyse(triple, cpu, instructions, llvm_mca):
    command = [llvm_mca, f"-mtriple={triple}", f"-mcpu={cpu}", "-iterations=1000", "-resource-pressure=false",
               "-instruction-info=false"]
    report = subprocess.run(command, input="\n".join(instructions) + "\n", check=True, capture_output=True,
                            text=True).stdout
    cycles = int(re.search(r"Total Cycles:\s+(\d+)", report).group(1))
    return cycles / 1000, report


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--zp7", required=True, type=pathlib.Path)
    parser.add_argument("--clang", default="clang++-22")
    parser.add_argument("--gcc", default="g++-15")
    parser.add_argument("--llvm-mca", default="llvm-mca-22")
    args = parser.parse_args()

    out = HERE / "mca"
    out.mkdir(exist_ok=True)
    configurations = [(args.clang, "Clang", "x86_64-linux-gnu", cpu, [f"-march={cpu}"], "#") for cpu in X86]
    configurations += [(args.gcc, "GCC", "x86_64-linux-gnu", cpu, [f"-march={cpu}"], "#") for cpu in X86]
    configurations += [(args.clang, "Clang", "aarch64-linux-gnu", cpu, ["--target=aarch64-linux-gnu", f"-mcpu={cpu}"],
                        "//") for cpu in AARCH64]

    versions = {}
    tables = {}
    for compiler, family, triple, cpu, flags, comment in configurations:
        versions[family] = subprocess.run([compiler, "--version"], check=True, capture_output=True,
                                          text=True).stdout.splitlines()[0]
        bodies = functions(assembly(compiler, flags, args.zp7.resolve()), comment)
        sections = []
        for kernel in KERNELS:
            if kernel not in bodies:
                continue
            scope, instructions = region(bodies[kernel], bodies)
            cycles, report = analyse(triple, cpu, instructions, args.llvm_mca)
            tables.setdefault((family, triple), {}).setdefault(kernel, {})[cpu] = (scope, cycles)
            sections.append(f"## {kernel}, {scope}\n\n" + "\n".join(instructions) + "\n\n" + report)
        name = f"{family.lower()}-{cpu}.txt"
        (out / name).write_text(f"# {versions[family]}, {' '.join(flags)} -O3, analysed by llvm-mca -mcpu={cpu}\n\n"
                                + "\n".join(sections))

    mca_version = subprocess.run([args.llvm_mca, "--version"], check=True, capture_output=True, text=True).stdout
    mca_version = next(line.strip() for line in mca_version.splitlines() if "version" in line)
    lines = ["# llvm-mca's cycles per iteration", "",
             "Written by `bench/static_analysis.py`; do not edit. A kernel without a loop is one call per iteration;",
             "a kernel marked *loop* is one trip of its innermost loop, so one selected bit for `compress_by_rank`,",
             "one mask bit for the walks, and one source bit for `expand_by_select`. A blank cell is a kernel the",
             "target does not have. Each summary, with the assembly it read, is beside this file.", "",
             "A cell is the model's, not the hardware's. The Zen 2 model issues `PEXT` and `PDEP` as one fast uop",
             "and gives `PCLMULQDQ` a latency of 100 cycles, where uops.info measures",
             "[`PEXT`](https://uops.info/html-instr/PEXT_R64_R64_R64.html) and",
             "[`PCLMULQDQ`](https://uops.info/html-instr/PCLMULQDQ_XMM_XMM_I8.html) on Zen 2 otherwise, so the",
             "`znver2` cells of the kernels built on them follow the model.", "",
             f"Analysed by {mca_version}.", ""]
    for (family, triple), kernels in tables.items():
        cpus = X86 if triple.startswith("x86") else AARCH64
        lines += [f"## {versions[family]}, {triple.split('-')[0]}", "", "| kernel | " + " | ".join(cpus) + " |",
                  "| :-- | " + " | ".join("--:" for _ in cpus) + " |"]
        for kernel in KERNELS:
            if kernel not in kernels:
                continue
            cells = []
            for cpu in cpus:
                scope, cycles = kernels[kernel].get(cpu, ("", None))
                cells.append("" if cycles is None else f"{cycles:.2f}")
            loop = any(scope == "loop body" for scope, _ in kernels[kernel].values())
            lines.append(f"| `{kernel}`{', *loop*' if loop else ''} | " + " | ".join(cells) + " |")
        lines.append("")
    (out / "README.md").write_text("\n".join(lines))


if __name__ == "__main__":
    main()
