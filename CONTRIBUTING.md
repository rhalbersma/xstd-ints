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
| [gcovr](https://gcovr.com/) | reproducing the coverage gate | Only for the workflow below; `pip install gcovr` |
| `clang-tidy` and `run-clang-tidy` | reproducing the clang-tidy gate | Only for the workflow below |
| `clang-format` 22+ | the formatting gate | Run `clang-format -i` on changed files before pushing; older versions rewrite the requires-expressions in `concepts/` |

## Getting a toolchain

Ubuntu 24.04 ships GCC 13 and clang 18, neither of which can build this library: GCC 13 rejects `-std=c++2c`, and
clang-format before 22 reads `{ a * b }` in a requires-expression as a pointer declaration, so it calls files dirty
that are clean against [`.clang-format`](.clang-format). [`tools/setup-toolchain.sh`](tools/setup-toolchain.sh)
installs the `stable` column of [README.md](README.md)'s matrix — GCC 15, clang 22, libc++ 22, clang-format 22, and Boost with
Boost.Test — from apt.llvm.org and the Ubuntu toolchain PPA:

```sh
sudo tools/setup-toolchain.sh
```

`XSTD_TOOLCHAIN_FULL=1` adds GCC 16, the qualification rung, whose libstdc++ is the oldest carrying
`<inplace_vector>`. The script is idempotent, so re-running it on a warm container is safe.

In a [Claude Code cloud](https://code.claude.com/docs/en/claude-code-on-the-web) environment, point the
environment's setup script at it so every session starts with the rung already in place. That field is configured
on the environment itself, not in this repository.

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

## Standards-paper tracking

Implementation-relevant proposals must record the paper number, the revision-pinned wording reviewed by xstd, the corresponding live `cplusplus/papers` tracking issue, the last-reviewed date, and a short statement saying whether xstd implements, partially follows, extends, or merely references the paper. Unversioned `wg21.link/P####` links are reserved for links explicitly described as the latest revision; implementation and conformance claims use pinned `P####R#` links. Durable prose does not copy volatile WG21 status labels.

A new paper revision or a change on its tracking issue triggers a review, not an automatic code change. Update the review date only after inspecting the relevant revision and reconsidering xstd's stated relationship.

| Paper | Reviewed wording | Tracking issue | Last reviewed | Relationship |
|---|---|---|---|---|
| P3666R0 | [P3666R0](https://wg21.link/P3666R0) | [cplusplus/papers#2420](https://github.com/cplusplus/papers/issues/2420) | 2026-08-19 | Supplies the proposed native bit-precise aliases and library integration direction. xstd exposes Clang's existing `_BitInt` types through equivalent aliases and its open traits, without implementing the paper's core-language changes. |
| P3701R0 | [P3701R0](https://wg21.link/P3701R0) | [cplusplus/papers#2330](https://github.com/cplusplus/papers/issues/2330) | 2026-08-18 | Supplies arithmetic-integer vocabulary and the built-in type boundary. xstd additionally admits paired integer-class types, requires valid signed/unsigned transformations, and is cv-transparent where the paper excludes cv-qualified types. |
| P3724R4 | [P3724R4](https://wg21.link/P3724R4) | [cplusplus/papers#2354](https://github.com/cplusplus/papers/issues/2354) | 2026-08-18 | Supplies division-result vocabulary and relevant rounding semantics. xstd intentionally supports only truncating, Euclidean, and floored quotient/remainder operations over its broader integer domain. |

## License

By contributing, you agree that your contributions will be licensed under the [Boost Software License, Version 1.0](LICENSE_1_0.txt), the same license that covers the rest of this repository.
