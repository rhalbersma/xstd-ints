#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Holds the models the proofs are about against the compiled C++ kernels they transcribe.

A proof is about kernels.py, not about the C++. This runs the compiled kernels, through drift.cpp, on edge values and
on pseudo-random words, and evaluates the models on the same inputs at the same width, so a kernel edited without its
model, or a model that misreads its kernel, fails here.

    drift.py path/to/proofs_drift
"""

import subprocess
import sys

import kernels
import operations


def splitmix(state):
    """SplitMix64: the next state and its output, so the inputs are the same on every run."""
    state = (state + 0x9E3779B97F4A7C15) % 2**64
    z = state
    z = ((z ^ (z >> 30)) * 0xBF58476D1CE4E5B9) % 2**64
    z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) % 2**64
    return state, z ^ (z >> 31)


def edges(width):
    """Zero, all ones, and at every position the single bit, its complement, and the masks below and from it."""
    ones = (1 << width) - 1
    values = {0, ones}
    for i in range(width):
        values |= {1 << i, ones ^ (1 << i), (1 << i) - 1, ones ^ ((1 << i) - 1)}
    return sorted(values)


def inputs(width, samples=20000):
    """Every pair of edge values that are each at most two bits from an extreme, then pseudo-random pairs of every density."""
    ones = (1 << width) - 1
    near = [v for v in edges(width) if min(bin(v).count("1"), bin(ones ^ v).count("1")) <= 1 or v in (0, ones)]
    pairs = [(x, y) for x in edges(width) for y in near]
    state = 0
    for _ in range(samples):
        draws = []
        for _ in range(4):
            state, z = splitmix(state)
            draws.append(z & ones)
        pairs += [(draws[0], draws[1]), (draws[0] & draws[2], draws[1] | draws[3]), (draws[0] | draws[2], draws[1] & draws[3])]
    return pairs


def modelled(o, x, y):
    x, y = o.word(x), o.word(y)
    outputs = [kernels.block_reverse_by_swaps(o, x), kernels.block_compress_by_rank(o, x, y),
               kernels.block_expand_by_walk(o, x, y), kernels.block_expand_by_select(o, x, y)]
    text = [format(v.value, "x") for v in outputs]
    if x.value == 0:
        text.append("-")
    else:
        k = y.value % x.value.bit_count()
        text.append(format(kernels.block_select_by_broadword(o, x, k).value, "x"))
    return text


def main():
    if len(sys.argv) != 2:
        sys.stderr.write(__doc__)
        return 2
    executable = sys.argv[1]
    width = int(subprocess.run([executable, "--width"], capture_output=True, text=True, check=True).stdout)
    pairs = inputs(width)
    feed = "".join(f"{x:x} {y:x}\n" for x, y in pairs)
    compiled = subprocess.run([executable], input=feed, capture_output=True, text=True, check=True).stdout.splitlines()
    if len(compiled) != len(pairs):
        sys.stderr.write(f"the kernels answered {len(compiled)} of {len(pairs)} pairs\n")
        return 1
    o = operations.IntOps(width)
    names = ("block_reverse_by_swaps", "block_compress_by_rank", "block_expand_by_walk", "block_expand_by_select",
             "block_select_by_broadword")
    drifted = 0
    for (x, y), line in zip(pairs, compiled):
        for name, got, want in zip(names, line.split(), modelled(o, x, y)):
            if got != want:
                drifted += 1
                if drifted <= 10:
                    sys.stderr.write(f"{name}({x:#x}, {y:#x}): compiled {got}, modelled {want}\n")
    print(f"{width}-bit kernels and models agree on {len(pairs) - drifted // 5} of {len(pairs)} pairs" if not drifted
          else f"{drifted} outputs differ")
    return 0 if drifted == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
