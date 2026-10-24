# Select against sux and SDSL

`bench_select` run on 2026-10-17 at commit `5d9833b`, building `select_dictionary.hpp` on `xstd::popcount` and `xstd::bit_select` and timing it against [sux](https://github.com/vigna/sux)'s `Rank9Sel` (commit `568903f1`) and [SDSL](https://github.com/xxsds/sdsl-lite)'s `select_support_mcl` (commit `5a7e423b`).

- **Machine:** a cloud VM on an Intel Xeon at 2.80 GHz (Cascade Lake, family 6, model 85), 4 vCPUs, 33 MiB of L3, Linux 6.18, with BMI2 and `POPCNT`.
- **Compilers:** GCC 15.2 and Clang 22.1, at `-O3 -DNDEBUG`.
- **Builds:** `-march=x86-64-v3`, where `bit_select` is `PDEP` and `TZCNT` and `popcount` is `POPCNT`, and GCC with no `-march`, where both are the portable kernels, and so is every library's popcount.
- **Inputs:** each bit a one with the probability its row names, or, clustered, runs of 90% and 1% averaging 4096 bits each; 4 million selects of uniformly random ranks.
- **Cells:** nanoseconds per select, the fastest of five rounds in which each structure runs once. The last column is xstd's dictionary with `bit_select` replaced by one `countr_zero`, which answers wrongly whenever the word holds more than one one: it is the same search and scan without the in-word select.

Every select agreed across the three libraries, and every rank of xstd's dictionary with sux's.

## 2<sup>20</sup> bits, in cache

| Distribution | GCC, v3: xstd | sux | SDSL | `countr_zero` | GCC, portable: xstd | sux | SDSL | `countr_zero` | Clang, v3: xstd | sux | SDSL | `countr_zero` |
| :-- | --: | --: | --: | --: | --: | --: | --: | --: | --: | --: | --: | --: |
| 1% | 79.6 | 3.9 | 7.5 | 81.9 | 106.4 | 5.3 | 7.5 | 98.9 | 55.7 | 4.6 | 6.7 | 51.9 |
| 5% | 62.5 | 20.1 | 34.3 | 69.0 | 87.3 | 35.4 | 68.4 | 79.7 | 47.9 | 19.5 | 31.6 | 45.5 |
| 10% | 58.9 | 20.6 | 31.8 | 62.4 | 89.1 | 34.8 | 54.9 | 81.1 | 47.5 | 20.1 | 30.0 | 43.7 |
| 25% | 53.2 | 14.6 | 28.6 | 56.2 | 76.9 | 28.4 | 54.4 | 68.4 | 45.1 | 18.0 | 31.3 | 39.4 |
| 50% | 43.3 | 14.6 | 22.9 | 47.2 | 69.9 | 28.4 | 56.6 | 61.1 | 35.4 | 15.9 | 25.0 | 35.2 |
| 75% | 42.2 | 14.9 | 22.5 | 45.1 | 72.4 | 27.6 | 50.9 | 61.8 | 40.1 | 15.4 | 21.8 | 35.7 |
| 90% | 40.9 | 15.1 | 22.4 | 42.0 | 71.4 | 30.2 | 48.4 | 58.4 | 42.9 | 22.7 | 25.4 | 41.2 |
| clustered | 46.1 | 16.8 | 22.6 | 50.0 | 75.9 | 28.3 | 46.2 | 64.1 | 38.4 | 15.1 | 22.3 | 36.2 |

## 2<sup>30</sup> bits, a 128 MiB vector

| Distribution | GCC, v3: xstd | sux | SDSL | `countr_zero` | GCC, portable: xstd | sux | SDSL | `countr_zero` | Clang, v3: xstd | sux | SDSL | `countr_zero` |
| :-- | --: | --: | --: | --: | --: | --: | --: | --: | --: | --: | --: | --: |
| 1% | 364.7 | 28.4 | 617.4 | 377.7 | 564.7 | 37.8 | 713.0 | 509.9 | 875.5 | 36.8 | 646.9 | 920.0 |
| 5% | 251.7 | 354.5 | 419.6 | 268.0 | 596.0 | 652.9 | 580.6 | 528.1 | 773.0 | 410.4 | 499.2 | 699.5 |
| 10% | 298.3 | 365.9 | 267.8 | 299.3 | 533.4 | 545.3 | 489.0 | 503.4 | 619.6 | 379.3 | 325.8 | 616.1 |
| 25% | 286.2 | 350.7 | 245.4 | 255.8 | 489.2 | 490.0 | 639.7 | 521.1 | 512.7 | 325.3 | 230.0 | 524.9 |
| 50% | 211.9 | 341.4 | 226.1 | 247.2 | 569.9 | 619.2 | 715.7 | 562.9 | 461.4 | 332.3 | 257.4 | 466.2 |
| 75% | 208.5 | 299.5 | 213.2 | 238.9 | 569.8 | 600.9 | 639.4 | 504.1 | 462.3 | 370.1 | 326.9 | 435.6 |
| 90% | 189.6 | 339.0 | 248.2 | 209.4 | 502.3 | 508.8 | 540.5 | 435.8 | 385.6 | 310.1 | 272.5 | 366.0 |
| clustered | 219.8 | 304.6 | 228.5 | 202.1 | 555.1 | 454.9 | 474.4 | 519.4 | 491.5 | 321.1 | 241.7 | 484.2 |

On this VM, two runs of the same 2<sup>30</sup>-bit build differ by up to a fifth in a cell, so differences smaller than that are noise.

## What it says

**The in-word select is not the bottleneck.** With `PDEP`, the `bit_select` column and the `countr_zero` column are within noise of each other at both sizes: three instructions out of a query that costs 40 ns in cache and 200 to 900 ns out of it. In the portable build, the broadword kernel costs about 10 ns a query in cache, the gap between its xstd and `countr_zero` columns at 50% and denser, which is an eighth of the query; at 2<sup>30</sup> bits it is lost in the cache misses.

**What dominates is how the dictionary finds the word.** sux's `Rank9Sel` answers a 1% query in 4 to 38 ns, where xstd's dictionary and SDSL take hundreds, because its inventory points straight at the word where the other two search for it. In cache, sux is two to three times faster than this dictionary at every density, and twenty times at 1%. Out of cache, at 5% and denser in the v3 build, GCC's branching binary search lets the processor run ahead into the next probe's load, and the dictionary keeps up with SDSL and beats sux. Clang turns the same search into conditional moves, which make each probe wait for the one before: faster than GCC in cache, half as fast out of it. The in-word step it emits is the same `SHLX`, `PDEP` and `TZCNT`.

For a proposal to standardize `bit_select`, then: every one of these libraries carries its own in-word select, and xstd's agrees with both on every query, but the step is a small part of what a select costs. Its case is a primitive each library rewrites, not a hotspot.

## Reproducing

```sh
cmake -S . -B build-succinct -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-15 -DCMAKE_CXX_FLAGS=-march=x86-64-v3 -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_SUCCINCT_BENCHMARKS=ON
cmake --build build-succinct
build-succinct/benchmark/succinct/bench_select 1048576
build-succinct/benchmark/succinct/bench_select
```

A 2<sup>30</sup>-bit run took 5 to 8 minutes and peaked at 576 MB of memory.
