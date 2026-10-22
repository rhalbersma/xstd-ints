# Mutation testing

Coverage shows that a line ran under test, not that a test would fail if the line were wrong: `<` turned into `<=` at a boundary leaves every line covered and every test green. [Mull](https://github.com/mull-project/mull) checks the stronger claim. Its clang plugin compiles a mutant into every arithmetic, comparison, bitwise and logical operator it finds in the headers [`test/mutation/mull.yml`](../test/mutation/mull.yml) names: the portable bit kernels, the shift behind `shl` and `shr`, and everything under `cstdlib/`. Each mutant then runs against the unit tests, and must make one of them fail.

[`test/mutation/`](../test/mutation) builds every unit test under `bit/` and `cstdlib/` a second time, one executable each, unoptimized and with the plugin, so that a mutant reruns only the tests compiled with it. The intrinsic kernels' test is left out: without BMI2 it reaches no mutated line, and it is the slowest of all. [`test/mutation/score.py`](../test/mutation/score.py) runs `mull-runner` over each executable and counts a mutant as killed if any executable kills it. The test fails if a mutant survives, or if no executable reaches its line.

A surviving mutant gets one of two answers. Either a test kills it, built from the cases users meet as the [test requirements](../CONTRIBUTING.md#test-suite-requirements) ask, or it is an equivalent mutant: the same function in a different spelling, such as `x < 0 ? -x : x` against `x <= 0 ? -x : x`, where both arms are zero at zero. An equivalent mutant is suppressed at its line with `// mull-ignore: <mutator>`, and the reason goes on its own comment line above. Mull reads every character after the colon as part of a mutator name, so a reason on the same line breaks the directive. A mutant is never suppressed just to raise the score.

Mull's plugin is built against one LLVM major, and the newest it supports is 20, behind the stable rung's clang. Noble's own archive carries clang 20; Mull comes from its [package repository](https://cloudsmith.io/~mull-project/repos/mull-stable), checked against the hash the workflow pins. The suite needs Boost.Test, which noble's `libboost-test-dev` provides, and no vcpkg:

```sh
sudo apt-get install clang-20 libboost-test-dev ./Mull-20-0.34.1-LLVM-20.1.2-ubuntu-amd64-24.04.deb
cmake -S . -B build-mutation -DCMAKE_CXX_COMPILER=clang++-20 -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_MUTATION_TESTS=ON
cmake --build build-mutation --parallel
ctest --test-dir build-mutation --output-on-failure
```

A run takes about eight minutes on four cores and leaves one SQLite report per executable under `build-mutation/test/mutation/reports/`. The [Mutation workflow](../.github/workflows/mutation.yml) runs it weekly, by hand, and on a pull request that touches the mutated headers, the tests that kill their mutants, or `test/mutation/`. It is not a required check while its runtime and noise are still being learned.
