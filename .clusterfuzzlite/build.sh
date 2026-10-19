#!/bin/bash -eu
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The sanitizer and the coverage instrumentation arrive in CXXFLAGS, and the engine in LIB_FUZZING_ENGINE. No build
# type, so CXXFLAGS alone sets the optimization and NDEBUG stays undefined: the library's asserts are findings too.
cmake -S "$SRC/xstd-ints" -B "$WORK/build" -DBUILD_TESTING=OFF \
        -DXSTD_INTS_BUILD_FUZZERS=ON \
        -DXSTD_INTS_FUZZ_COMPILE_OPTIONS="" \
        -DXSTD_INTS_FUZZ_LINK_OPTIONS="$LIB_FUZZING_ENGINE"
targets=(fuzz_align fuzz_bit_permutation fuzz_div fuzz_shift fuzz_to_chars)
cmake --build "$WORK/build" --parallel "$(nproc)" --target "${targets[@]}"

for target in "${targets[@]}"; do
        cp "$WORK/build/fuzz/$target" "$OUT/"
        # The checked-in seeds, which ClusterFuzzLite unpacks beside the corpus it keeps.
        (cd "$SRC/xstd-ints/fuzz/corpus/$target" && zip -q -j "$OUT/${target}_seed_corpus.zip" ./*)
done
