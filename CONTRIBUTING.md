# Contributing to xstd-ints

## Workflow

When adding or changing a public utility:

1. Add or update the relevant header under `include/xstd/`.
2. Add or update matching tests under `test/src/`; CMake creates one test executable per `.cpp` file in that directory.
3. Build and test locally (see below).
4. Update the feature table and examples in [README.md](README.md) when the public API changes.

## What a PR must satisfy before it can merge

This repository enforces its quality bar through CI rather than through review discretion. A PR is mergeable once every required check below is green; none of these are aspirational:

- **Every compiler/platform leg passes.** See the table in [README.md](README.md) for the current matrix (GCC, Clang, Clang-CL, MSVC, MinGW, Apple Clang). Every leg is required, including every `Development` leg (`17-SVN`, `24-SVN`, `2026-Preview`) and the `libc++` leg: xstd tracks the development channel deliberately and none of these are advisory.
- **`clang-tidy` is clean.** The [Clang-Tidy workflow](.github/workflows/clang-tidy.yml) runs the checks in [`.clang-tidy`](.clang-tidy) with `WarningsAsErrors: '*'` over the public headers, so any finding fails the job outright - there is no "advisory, fix later" mode.
- **Line and branch coverage stay at 100%, project-wide and for the PR's own diff.** [`codecov.yml`](.github/codecov.yml) sets both the `project` and `patch` Codecov status checks to a 100% target with zero tolerance, backed by the [Coverage workflow](.github/workflows/coverage.yml)'s own `gcovr --fail-under-line 100 --fail-under-branch 100` gate. New code needs a test that exercises every line and branch it adds; existing coverage may not regress. Excluded from this bar: `assert(...)` contract checks (their failure path is undefined behavior by design, not something a correct test can hit), compiler-synthesized `= default;` special members (gcov cannot attribute a hit counter to them regardless of how often they run), and the exception-unwinding branch gcc/gcov attaches to any call that could throw (`--exclude-throw-branches`/`--exclude-unreachable-branches`) - not a code path a test can meaningfully hit either.
- **No new sanitizer failures.** The [sanitizers workflow](.github/workflows/sanitizers.yml) must stay green.
- **The bit kernels compile to the instructions they exist for.** The [Codegen workflow](.github/workflows/codegen.yml) compiles [`codegen/`](codegen) to assembly with the stable rung's GCC and Clang, on an x86-64 and an AArch64 runner, and reads it with FileCheck - see [Codegen tests](#codegen-tests) below.
- **The public headers stay self-sufficient.** Each header is compiled as its own translation unit (see `test/CMakeLists.txt`); don't rely on include order from another header.
- **`clang-format` is clean.** The [Clang-Format workflow](.github/workflows/clang-format.yml) runs `clang-format --dry-run --Werror` over every header and test source against [`.clang-format`](.clang-format), so any diff fails the job. Run `clang-format -i` on changed files before pushing, with **version 22 or newer**: before 22, clang-format reads `{ a * b }` in a requires-expression as a pointer declaration and rewrites the binary-operator rows of `integer_class.hpp` and `nothrow_const_operators.hpp`. The remaining `// clang-format off` / `// clang-format on` guards are the hand-laid-out test-data tables, which clang-format cannot express; don't add new guards without a comparable reason.
- **Workflow files pass `actionlint`.** The [Actionlint workflow](.github/workflows/actionlint.yml) validates GitHub Actions syntax and expressions.
- **The documented consumption methods work.** The [Consumption workflow](.github/workflows/consumption.yml) builds a consumer using `find_package`, `add_subdirectory`, and `FetchContent`.
- **CodeQL analysis is clean.** The [CodeQL workflow](.github/workflows/codeql.yml) runs the C/C++ `security-extended` query suite.

Match the surrounding code's style by eye where `.clang-format` doesn't have an opinion, including the Boost Software License header comment at the top of every source and workflow file.

## Test suite requirements

Keep a focused test source for each public function, concept, and trait, mirroring its header path under `test/src/`. A source matching an umbrella header (for example, `cstdlib.cpp`, `concepts.cpp`, or `type_traits.cpp`) tests only identities that span several focused facilities. Integer-generic tests use the signed, unsigned, or combined exact-width type tuples in `test/include/test/exact_width_types.hpp` so the standard, xstd, optional third-party, and compiler bit-precise implementations stay in the same test matrix.

The library itself has no dependencies outside `<xstd/ext/>`, whose headers each need the library they adapt - see [README.md](README.md). Its test suite has more, and none of those are needed to *use* xstd:

| Tool | Needed for | Notes |
| :--- | :--------- | :---- |
| A conforming C++23 compiler | everything | Same requirement as the library; see the table in [README.md](README.md) for the versions under CI |
| [CMake](https://cmake.org/) 3.28+ | configuring and building | `cmake_minimum_required` in [`CMakeLists.txt`](CMakeLists.txt); CTest ships with it |
| [Boost.Test](https://www.boost.org/doc/libs/release/libs/test/) 1.70+ | the unit tests under `test/src/` | 1.70 introduced arbitrary type-list support for `BOOST_AUTO_TEST_CASE_TEMPLATE`, which the tests use with `std::tuple`; declared under the opt-in `test` feature of the checked-in [`vcpkg.json`](vcpkg.json) manifest, which the `*-vcpkg` presets select from a `VCPKG_ROOT`-configured vcpkg, or install it with your system package manager |
| [Boost.Int128](https://github.com/cppalliance/int128) | testing the integer-class extension points against a type from outside the library, and the `<xstd/ext/boost/int128.hpp>` associations that pair it | Optional, and resolved without a manual step: `test/CMakeLists.txt` uses an installed copy where `find_package(boost_int128 CONFIG)` finds one - Boost 1.92 ships it - and otherwise fetches a pinned upstream commit. Configure with `-DXSTD_INTS_TEST_FETCH_BOOST_INT128=OFF` to build offline without it; the affected cases then run over xstd's own types alone |
| [Abseil](https://github.com/abseil/abseil-cpp) | testing the same extension points against a type that declares no `noexcept` anywhere, and the `<xstd/ext/absl/int128.hpp>` associations that pair it | Optional, and resolved the same way: an installed copy where `find_package(absl CONFIG)` finds one, and otherwise a pinned release tag, of which only `absl/numeric/int128.cc` is compiled. Configure with `-DXSTD_INTS_TEST_FETCH_ABSL_INT128=OFF` to build offline without it; the affected cases then do not run, no other type in the suite leaving `noexcept` off |
| [Boost.Hash2](https://www.boost.org/doc/libs/release/libs/hash2/) | testing `<xstd/ints/ext/boost/hash2.hpp>`, and the `<xstd/ints/ext/boost.hpp>` umbrella that exports it | Optional: `test/CMakeLists.txt` uses an installed copy where `find_package(boost_hash2 CONFIG)` finds one - Boost ships it from 1.88, and the `test` feature of [`vcpkg.json`](vcpkg.json) names it - and fetches nothing otherwise, its own Boost dependencies being too many to fetch alone; the hashing cases then do not run |
| [gcovr](https://gcovr.com/) | reproducing the coverage gate | Only for the workflow below; `pip install gcovr` |
| `clang-tidy` and `run-clang-tidy` | reproducing the clang-tidy gate | Only for the workflow below |
| `clang-format` 22+ | the formatting gate | Run `clang-format -i` on changed files before pushing; older versions rewrite the requires-expressions in `concepts/` |
| [FileCheck](https://pypi.org/project/filecheck/) | the codegen tests | The pure-Python port of LLVM's, pinned by hash in [`tools/filecheck-requirements.txt`](tools/filecheck-requirements.txt), which `setup-toolchain.sh` installs; LLVM's own `FileCheck` reads the same directives |

## Getting a toolchain

Ubuntu 24.04 ships GCC 13 and clang 18, neither of which can build this library: GCC 13 rejects `-std=c++2c`, and
clang-format before 22 reads `{ a * b }` in a requires-expression as a pointer declaration, so it calls files dirty
that are clean against [`.clang-format`](.clang-format). [`tools/setup-toolchain.sh`](tools/setup-toolchain.sh)
installs the `stable` column of [README.md](README.md)'s matrix — GCC 15, clang 22, libc++ 22, clang-format 22, and Boost with
Boost.Test — from apt.llvm.org and the Ubuntu toolchain PPA:

```sh
tools/setup-toolchain.sh
```

`XSTD_TOOLCHAIN_FULL=1` adds GCC 16, the qualification rung, whose libstdc++ is the oldest carrying
`<inplace_vector>`. The script is idempotent, so re-running it on a warm container is safe.

In a [Claude Code cloud](https://code.claude.com/docs/en/claude-code-on-the-web) environment, the environment's
setup script runs it, so every session starts with the rung already in place. That field holds a script rather than
a command, and it runs from outside the checkout, so it needs a shebang and an absolute path:

```sh
#!/usr/bin/env bash
set -euo pipefail
for dir in /home/user/*/; do
        script="${dir}tools/setup-toolchain.sh"
        if [ -x "$script" ]; then
                exec "$script"
        fi
done
echo "setup-toolchain.sh not found in any checkout under /home/user" >&2
exit 1
```

The field is configured on the environment, not in this repository, and one environment serves every repository it
opens — so each of them needs this script at this path.

## Building and testing locally

Two paths, and which one you want depends on whether you are developing xstd-ints or only checking that it configures, installs and consumes. The library is header-only, so without the tests there is nothing to find and nothing to compile:

```sh
cmake --preset no-tests
cmake --build --preset no-tests
```

The full suite needs Boost.Test, from the table above:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The repository also provides CMake presets for common local configurations:

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The `*-vcpkg` presets additionally resolve the Boost dependencies through vcpkg, using the toolchain at `VCPKG_ROOT`:

```sh
cmake --preset dev-vcpkg
cmake --build --preset dev-vcpkg
ctest --preset dev-vcpkg
```

Those dependencies are the test suite's rather than the package's, and [`vcpkg.json`](vcpkg.json) says so: `test` is a feature and not a *default* feature, so an ordinary manifest-mode install of xstd-ints resolves no Boost at all. Whoever wants the tests names the feature. The `*-vcpkg` presets do it through `VCPKG_MANIFEST_FEATURES`; the CI stubs do it through `vcpkg_features`, which every shared workflow that installs dependencies hands to both the `vcpkg install` step and the configure line. Nothing in the manifest pins a `builtin-baseline`, so the version each dependency resolves to is whatever the vcpkg checkout at `VCPKG_ROOT` carries; pin that checkout if you need one build to reproduce another.

### Reproducing the coverage gate

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build
ctest --test-dir build --output-on-failure
gcovr --root . --exclude 'test/.*' --exclude 'build/.*' \
  --exclude-lines-by-pattern '^\s*assert\(' \
  --exclude-lines-by-pattern '=\s*default;' \
  --exclude-branches-by-pattern '^\s*assert\(' \
  --exclude-branches-by-pattern '^\s*.*=\s*default;' \
  --exclude-throw-branches --exclude-unreachable-branches \
  --print-summary --fail-under-line 100 --fail-under-branch 100
```

### Reproducing the clang-tidy gate

```sh
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
run-clang-tidy -quiet -p build "$PWD/build/test/header_self_sufficiency/.*"
```

## Fuzzing

The tests sweep fixed values and a seeded random sample; the libFuzzer targets under [`fuzz/`](fuzz) search for the inputs they miss, one per header family. `fuzz_bit_permutation` reads a source and a mask from its input bytes and checks, at one `std::size_t` block, that both portable expansions agree and each round-trips with the portable compression, that every overload of PEXT and PDEP agrees with those kernels and round-trips on its own, and that the portable select and the one through PDEP find the source's one of every rank where the oracle does. It then reads the same bytes at every width from 8 bits up, through `unsigned _BitInt(147)` where the compiler has it, and checks that `bit_compress` and `bit_expand` agree with their oracles and undo each other, that `bit_select` agrees with its oracle and inverts rank at the source's lowest and highest ones and at the rank the mask's population names, and that `bit_reverse` and `bit_repeat` agree with theirs, the period one more than the mask's population. The checks live in [`test/include/test/bit/permutation_properties.hpp`](test/include/test/bit/permutation_properties.hpp) and [`test/include/test/bit/select_properties.hpp`](test/include/test/bit/select_properties.hpp), which the unit tests run as well; a failing one aborts.

The other four read each operand as an anchor byte, choosing zero, the type's minimum or its maximum, plus a 32-byte little-endian offset that wraps at the type's width. The same input therefore lands on every width's boundary at once: `INT_MIN / -1` is one seed, not one per type. They run the standard types from 8 to 64 bits, `__int128`, and the bit-precise widths `fuzz_bit_permutation` reads, each signed and unsigned. Signed starts at 4 bits, because the shift oracle halves by dividing by 2, which a signed `_BitInt(2)` cannot hold. Their oracles are computed in `_BitInt(256)` or taken from the unit tests:

| target | checks |
| :-- | :-- |
| `fuzz_div` | `div`, `div_floor`, `div_ceil` and `div_euclid` give the quotient the exact one rounds to, and the remainder that leaves, skipping only a zero denominator and `MIN / -1` |
| `fuzz_shift` | `shl` and `shr` agree with `test::oracle_shift` for `int`, `std::size_t`, `long long` and `std::int8_t` counts, the extremes included; `rotl` and `rotr` agree with `test::oracle_rotl` for every `int` count |
| `fuzz_align` | `align_down` and `is_aligned` at every power-of-two alignment up to 2⁶³, and `align_up` wherever its answer fits the type |
| `fuzz_to_chars` | `to_chars` in every base writes the digits long division gives, at buffer lengths from 0 to 255: it fails with `value_too_large` and `ptr == last` when the buffer is short, and writes nothing past its answer or past `last` |

`fuzz_to_chars` found a bug in its first run. For a bit-precise type narrower than 6 bits, `to_chars` converted the base to the type's own unsigned counterpart, which wraps base 16 to 0 in `unsigned _BitInt(4)`. `to_chars(first, last, xstd::bit_uint<4>{15}, 16)` reported `value_too_large` into a 1-character buffer, and divided by zero into a longer one.

It needs clang, which is what ships libFuzzer. `XSTD_INTS_BUILD_FUZZERS` is off by default and independent of `BUILD_TESTING`. On x86-64, `XSTD_INTS_FUZZ_BMI2` is on by default and builds the targets with `-mbmi2`, so they run only on a processor with BMI2:

```sh
cmake -S . -B build-fuzz -DCMAKE_CXX_COMPILER=clang++-22 -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_FUZZERS=ON
cmake --build build-fuzz --parallel 2
mkdir -p corpus && build-fuzz/fuzz/fuzz_div -max_total_time=120 corpus fuzz/corpus/fuzz_div
```

The targets are built with ASan and UBSan, and `XSTD_INTS_FUZZ_COMPILE_OPTIONS` and `XSTD_INTS_FUZZ_LINK_OPTIONS` replace those flags. The first directory on the command line receives what the run discovers; the checked-in seeds under `fuzz/corpus/<target>/` are read but left alone. A crash is written as `crash-<sha1>` in the working directory, and reproduces by passing that file to the target binary. With `BUILD_TESTING` on as well, CTest replays each seed corpus once as `fuzz.<target>`. A seed named `regression-...` is a minimized input that once found a bug, and stays to keep it found.

[ClusterFuzzLite](https://google.github.io/clusterfuzzlite/) runs the same targets in CI, built by [`.clusterfuzzlite/build.sh`](.clusterfuzzlite/build.sh) in the OSS-Fuzz builder image: [`cflite_pr.yml`](.github/workflows/cflite_pr.yml) for five minutes on every pull request, under ASan and UBSan, and [`cflite_batch.yml`](.github/workflows/cflite_batch.yml) nightly for half an hour, each budget split across the targets, keeping the corpus it grows. A crash fails the job and is uploaded as an artifact.

The unit tests reach PEXT and PDEP only where the build targets BMI2, so [`gcc.yml`](.github/workflows/gcc.yml) and [`clang.yml`](.github/workflows/clang.yml) each add a leg at `-march=x86-64-v3` on the stable rung; every other leg runs the portable kernels.

## Codegen tests

The unit tests and the fuzzer check that the intrinsic kernels agree with the portable ones, which they must, and which also means a detection macro that stops firing goes unseen: the portable kernel answers, every value stays right, and every test stays green. The codegen tests close that gap. Each function in [`codegen/`](codegen) wraps one public call at a fixed width; CTest compiles the file to assembly once per target configuration, with `-std=c++23 -O2 -DNDEBUG` as a consumer builds, and FileCheck reads it against the directives above each function, under the prefixes that configuration names:

| Configuration | Flags | Expected |
| :--- | :--- | :--- |
| `x86-64` | `-march=x86-64` | no `pext` or `pdep`; the 128-bit `bit_reverse` without a branch, in this and every x86-64 configuration below |
| `bmi2` | `-mbmi2` | `bit_compress` a `pext` and `bit_expand` a `pdep`, neither with a branch; `bit_select` through `pdep` |
| `x86-64-v3` | `-march=x86-64-v3` | as `bmi2`, and `bit_select` a `pdep` followed by `tzcnt` with no branch, `tzcnt` being BMI1's |
| `gfni` | `-mgfni` | `bit_reverse` a `gf2p8affineqb` and a `bswap`, twice that for 128 bits, without a branch |
| `bdver4`, `znver1`, `znver2` | `-march=` each | no `pext` or `pdep`, BMI2 enabled but microcoded there |
| `bmi2-excluded` | `-march=znver2` | the `x86-64-v3` checks **rejected**, which proves they see the kernels gone while BMI2 is still enabled |
| `armv8-a` | `-march=armv8-a` | `bit_reverse` an `rbit`, two for 128 bits, without a branch |

[`codegen/alignment.cpp`](codegen/alignment.cpp) puts `align_up` and `align_down` beside the complement idiom they decline, and its tests require each pair to compile to the same instructions, as often each, in whatever order and registers the compiler picks. The x86-64 configurations run on an x86-64 host and `armv8-a` on an AArch64 one; nothing cross-compiles. They need GCC or Clang with its GNU-style driver, and no Boost:

```sh
tools/setup-toolchain.sh
cmake -S . -B build-codegen -DCMAKE_CXX_COMPILER=clang++-22 -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_CODEGEN_TESTS=ON
ctest --test-dir build-codegen --output-on-failure
```

Each test leaves its assembly beside it as `build-codegen/codegen/<source>.<configuration>.s`. The [Codegen workflow](.github/workflows/codegen.yml) runs them with GCC 15 and Clang 22 on `ubuntu-24.04` and `ubuntu-24.04-arm`.

## Exhaustive sweeps

At 16 bits every pair of operands can be tried, 2^32 of them, which the unit tests and the fuzzer only sample. The two sweeps under [`exhaustive/`](exhaustive) try them all, split across the machine's threads:

- `exhaustive_bit_permutation` checks every 16-bit source against every 16-bit mask with the fuzzer's own properties: `bit_compress` and `bit_expand` against their oracles and each other, the portable kernels at `std::size_t` width, and, where the build has fast BMI2, `PEXT` and `PDEP` against those kernels.
- `exhaustive_division` checks `div`, `div_floor`, `div_euclid` and `div_ceil` over every `int16_t` and `uint16_t` pair in their domain against their postconditions, which alone determine each result, and folds every numerator, denominator, quotient and remainder into a digest per type and function. Those eight digests must equal [`exhaustive/division_digests.txt`](exhaustive/division_digests.txt), which [`tools/division_digests.py`](tools/division_digests.py) computes with numpy's own division routines, so the postconditions are held to an implementation xstd did not write.

They take minutes each and need no Boost:

```sh
cmake -S . -B build-exhaustive -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_EXHAUSTIVE_TESTS=ON
cmake --build build-exhaustive --parallel
ctest --test-dir build-exhaustive --output-on-failure
```

The reference digests need numpy, pinned by hash in [`tools/numpy-requirements.txt`](tools/numpy-requirements.txt), and take several minutes more:

```sh
pip install --user --require-hashes -r tools/numpy-requirements.txt
python3 tools/division_digests.py --check exhaustive/division_digests.txt
```

The [Exhaustive workflow](.github/workflows/exhaustive.yml) runs the sweeps weekly, by hand, and on a pull request touching what they cover: GCC 15 with the portable kernels, Clang 22 at `-march=x86-64-v3`, GCC 15 on AArch64 and MSVC on x64, with the digests recomputed beside them. It is not a required check, since its path filter would leave it pending on every pull request it does not cover.

## Proofs

The portable kernels under `include/xstd/ints/bit/detail/portable/` answer every constant expression and every target without the instructions, and the tests and the fuzzer can only sample their 2^128 pairs of inputs. [`proofs/`](proofs) proves them equal to their specifications for every input, with [Z3](https://github.com/Z3Prover/z3)'s Python bindings, at both widths `std::size_t` has, 32 and 64 bits:

- [`specifications.py`](proofs/specifications.py) transcribes what each kernel must compute from its primary source: PEXT and PDEP from the Operation pseudocode of the Intel 64 and IA-32 Architectures Software Developer's Manual, RBIT from the Arm Architecture Reference Manual's ASL, and select from its defining property.
- [`kernels.py`](proofs/kernels.py) transcribes each kernel statement by statement, over the word operations of [`operations.py`](proofs/operations.py), with each loop given as its parts. Each kernel records what the C++ needs there to be defined: a shift within the width, the select's precondition, a table index in range and a rank that names a one of its byte.
- [`prove.py`](proofs/prove.py) proves `block_reverse_by_swaps` directly, and each loop by an inductive invariant: it holds on entry, one pass keeps it while a variable falls, so the loop ends, and on exit it makes the result a closed form that a further claim proves equal to the SDM's pseudocode. `block_select_by_broadword` is proved in two parts that meet at its counts, the second a case for each byte and each bit within it, with the `select_in_byte` table standing in by a contract that is checked entry by entry. `block_expand_by_select` relies on the select's proved contract rather than on its code. Three mutants, each a kernel with one deliberate error, must be refuted.
- [`drift.py`](proofs/drift.py) holds the models to the C++, since a proof is about the transcription: it runs the compiled kernels through [`drift.cpp`](proofs/drift.cpp) on edge values and pseudo-random words and requires every output to equal the model's at the same width.

Z3 is pinned by hash in [`tools/z3-requirements.txt`](tools/z3-requirements.txt):

```sh
pip install --user --require-hashes -r tools/z3-requirements.txt
cmake -S . -B build-proofs -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_PROOFS=ON
cmake --build build-proofs
ctest --test-dir build-proofs --output-on-failure --verbose
```

`python3 proofs/prove.py --widths 16 --jobs 4` runs every claim at 16 bits in seconds, which is the quick check while editing a kernel or its model. The [Proofs workflow](.github/workflows/proofs.yml) runs both widths weekly, by hand, and on a pull request touching the portable kernels or the proofs. It is not a required check, since its path filter would leave it pending on every pull request it does not cover.

## Mutation tests

Coverage shows that a line ran under test, not that a test would fail if the line were wrong: `<` turned into `<=` at a boundary leaves every line covered and every test green. [Mull](https://github.com/mull-project/mull) checks the stronger claim. Its clang plugin compiles a mutant into every arithmetic, comparison, bitwise and logical operator it finds in the headers [`mutation/mull.yml`](mutation/mull.yml) names: the portable bit kernels, the shift behind `shl` and `shr`, and everything under `cstdlib/`. Each mutant then runs against the unit tests, and must make one of them fail.

[`mutation/`](mutation) builds every unit test under `bit/` and `cstdlib/` a second time, one executable each, unoptimized and with the plugin, so that a mutant reruns only the tests compiled with it. The intrinsic kernels' test is left out: without BMI2 it reaches no mutated line, and it is the slowest of all. [`mutation/score.py`](mutation/score.py) runs `mull-runner` over each executable and counts a mutant as killed if any executable kills it. The test fails if a mutant survives, or if no executable reaches its line.

A surviving mutant gets one of two answers. Either a test kills it, built from the cases users meet as the [test requirements](#test-suite-requirements) ask, or it is an equivalent mutant: the same function in a different spelling, such as `x < 0 ? -x : x` against `x <= 0 ? -x : x`, where both arms are zero at zero. An equivalent mutant is suppressed at its line with `// mull-ignore: <mutator>`, and the reason goes on its own comment line above. Mull reads every character after the colon as part of a mutator name, so a reason on the same line breaks the directive. A mutant is never suppressed just to raise the score.

Mull's plugin is built against one LLVM major, and the newest it supports is 20, behind the stable rung's clang. Noble's own archive carries clang 20; Mull comes from its [package repository](https://cloudsmith.io/~mull-project/repos/mull-stable), checked against the hash the workflow pins. The suite needs Boost.Test, which noble's `libboost-test-dev` provides, and no vcpkg:

```sh
sudo apt-get install clang-20 libboost-test-dev ./Mull-20-0.34.1-LLVM-20.1.2-ubuntu-amd64-24.04.deb
cmake -S . -B build-mutation -DCMAKE_CXX_COMPILER=clang++-20 -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_MUTATION_TESTS=ON
cmake --build build-mutation --parallel
ctest --test-dir build-mutation --output-on-failure
```

A run takes about eight minutes on four cores and leaves one SQLite report per executable under `build-mutation/mutation/reports/`. The [Mutation workflow](.github/workflows/mutation.yml) runs it weekly, by hand, and on a pull request that touches the mutated headers, the tests that kill their mutants, or `mutation/`. It is not a required check while its runtime and noise are still being learned.

## Benchmarks

What [design.md](doc/design.md) says the bit kernels cost comes from [`bench/`](bench), not from figures quoted by hand. `bench_bit_permutation` times every kernel in `detail::portable`; `PEXT`, `PDEP`, and `PDEP` followed by `TZCNT` where the build enables BMI2; and the alternatives design.md names as rejected: the mask walk with an `if`, Hacker's Delight's parallel suffix, and [zp7](https://github.com/zwegner/zp7), with `PCLMULQDQ` where the build enables it. A cell is the cost of one call among 4096 independent calls on fixed pseudo-random inputs whose density its row states, the fastest of five passes. Where the system exposes performance counters, cycles and instructions are counted; elsewhere cycles are time at the clock a chain of dependent adds runs at. Every kernel must agree with its table's first on every input before any is timed, and the executable prints a Markdown page that names the compiler, the machine, the flags and the commit. CMake fetches nanobench and zp7, each pinned by commit:

```sh
tools/setup-toolchain.sh
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-15 -DCMAKE_CXX_FLAGS=-march=native -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_BENCHMARKS=ON
cmake --build build-bench
build-bench/bench/bench_bit_permutation
```

A run takes about half a minute. One worth keeping goes under [`bench/results/`](bench/results), a file per machine that opens with what the machine is and how it was run, and a figure in design.md names the file it comes from. Configure afresh before a run to keep, since CMake reads the commit when it configures.

[`bench/static_analysis.py`](bench/static_analysis.py) compiles each kernel with GCC 15 and Clang 22 for Haswell, Cascade Lake, Ice Lake and Zen 2 to 4, and with Clang for Neoverse N1, N2 and V2, and writes llvm-mca's analysis of each under [`bench/mca/`](bench/mca/README.md). It needs the build configured above, for zp7, and noble's `libstdc++-14-dev-arm64-cross` for the AArch64 targets:

```sh
sudo apt-get install libstdc++-14-dev-arm64-cross
python3 bench/static_analysis.py --zp7 build-bench/_deps/zp7-src
```

Given `--uica` and a [uiCA](https://github.com/andreas-abel/uiCA) checkout that its own `setup.sh` has prepared, the script also has uiCA simulate the same regions of the x86-64 code on the three targets it models, Haswell, Cascade Lake and Ice Lake, under [`bench/uica/`](bench/uica). llvm-mca reports its model, not the hardware, and its Zen 2 model misses the microcode behind `PEXT` and `PDEP`. A claim about how fast an instruction is cites [uops.info](https://uops.info) by instruction and microarchitecture rather than restating a number.

The [Bench workflow](.github/workflows/bench.yml) runs both, by hand and on a pull request that touches the kernels or `bench/`: the benchmark with GCC 15 and Clang 22 on `ubuntu-24.04` and `ubuntu-24.04-arm` at `-march=native`, `-mcpu=native` on Arm, with each job's page in its summary, and the analysis beside them, uiCA included, pinned by commit, reporting any difference from the checked-in files. A benchmark reports a number and never fails on one, so it is not a required check.

## Standards-paper tracking

Implementation-relevant proposals must record the paper number, the revision-pinned wording reviewed by xstd, the corresponding live `cplusplus/papers` tracking issue, the last-reviewed date, and a short statement saying whether xstd implements, partially follows, extends, or merely references the paper. Unversioned `wg21.link/P####` links are reserved for links explicitly described as the latest revision; implementation and conformance claims use pinned `P####R#` links. Durable prose does not copy volatile WG21 status labels.

A new paper revision or a change on its tracking issue triggers a review, not an automatic code change. Update the review date only after inspecting the relevant revision and reconsidering xstd's stated relationship.

| Paper | Reviewed wording | Tracking issue | Last reviewed | Relationship |
|---|---|---|---|---|
| P3104R6 | [P3104R6](https://wg21.link/P3104R6) | [cplusplus/papers#1768](https://github.com/cplusplus/papers/issues/1768) | 2026-10-10 | Supplies `bit_reverse`, `bit_repeat`, `bit_compress` and `bit_expand`. xstd implements the wording, R5's plus R6's constant-evaluation remark on `bit_repeat`, forwards to `<bit>` once `__cpp_lib_bitops` reaches `202607L`, and extends the domain to every xstd unsigned integer. |
| P3666R0 | [P3666R0](https://wg21.link/P3666R0) | [cplusplus/papers#2420](https://github.com/cplusplus/papers/issues/2420) | 2026-08-19 | Supplies the proposed native bit-precise aliases and library integration direction. xstd exposes Clang's existing `_BitInt` types through equivalent aliases and its open traits, without implementing the paper's core-language changes. |
| P3701R0 | [P3701R0](https://wg21.link/P3701R0) | [cplusplus/papers#2330](https://github.com/cplusplus/papers/issues/2330) | 2026-08-18 | Supplies arithmetic-integer vocabulary and the built-in type boundary. xstd additionally admits paired integer-class types, requires valid signed/unsigned transformations, and is cv-transparent where the paper excludes cv-qualified types. |
| P3724R4 | [P3724R4](https://wg21.link/P3724R4) | [cplusplus/papers#2354](https://github.com/cplusplus/papers/issues/2354) | 2026-08-18 | Supplies division-result vocabulary and relevant rounding semantics. xstd intentionally supports only truncating, Euclidean, and floored quotient/remainder operations over its broader integer domain. |

## Required status checks

The names to tick under branch protection, exactly as GitHub reports them:

| Check | Covers |
| :--- | :--- |
| `actionlint / actionlint` | workflow syntax |
| `apple_clang / all` | Xcode 16.4 and 26.6, Debug and Release |
| `clang / all` | Clang 22, 23, 24-SVN with libstdc++ |
| `clang_cl / all` | clang-cl on VS 2022, 2026, 2026-Preview |
| `clang_format / clang-format` | `clang-format --dry-run --Werror` against [`.clang-format`](.clang-format) |
| `clang_libcxx / all` | Clang 22, 23, 24-SVN with libc++ |
| `clang_tidy / all` | clang-tidy on all three rungs |
| `codegen / all` | GCC 15 and Clang 22 assembly on x86-64 and AArch64 |
| `codeql / Analyze` | `security-extended` |
| `consumption / Consume` | the three CMake consumption models |
| `coverage / gcovr` | 100% of lines and branches |
| `gcc / all` | GCC 15, 16, 17-SVN |
| `mingw / all` | MinGW 15 and 16 |
| `msvc / all` | cl on VS 2022, 2026 and 2026-Preview |
| `msvc_analyze / all` | `/analyze` on the same three rungs |
| `sanitizers / all` | all fifteen sanitizer legs |

Codecov posts two more, `codecov/project` and `codecov/patch`, which carry the same 100% bar for the whole
tree and for the diff.

The [Scorecard workflow](.github/workflows/scorecard.yml) cannot be required: it runs on pushes to `main` and
on a schedule, never on a pull request.

Each ladder ends in an `all` job that is red unless every one of its legs succeeded, and that gate is what
branch protection requires: a leg name changes whenever a rung moves, the gate name does not. This table is
[xstd-bits](https://github.com/rhalbersma/xstd-bits/blob/main/CONTRIBUTING.md)' as well, less the C++26 legs
it alone carries, the three repositories holding one bar between them.

## License

By contributing, you agree that your contributions will be licensed under the [Boost Software License, Version 1.0](LICENSE_1_0.txt), the same license that covers the rest of this repository.
