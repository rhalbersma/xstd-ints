# Cascade Lake, a cloud container

Four virtual cores of a Xeon that reports itself as an "Intel(R) Xeon(R) Processor @ 2.80GHz", family 6, model 85,
stepping 7, which is Cascade Lake. The hypervisor exposes no performance counters, so each cell is time converted to
cycles at the clock a chain of dependent adds ran at, and there is no instruction count. Its neighbours make the
slowest pass of a cell take up to nearly three times its fastest; the fastest is reported, and two runs of the same
build agreed to within about 1%.

Both runs used the stable rung from `tools/setup-toolchain.sh`, built with the command in CONTRIBUTING.md, with
`clang++-22` in place of `g++-15` for the second:

```sh
cmake -S . -B build-benchmark -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-15 -DCMAKE_CXX_FLAGS=-march=native -DBUILD_TESTING=OFF -DXSTD_INTS_BUILD_BENCHMARKS=ON
cmake --build build-benchmark
build-benchmark/benchmark/bench_bit_permutation
```

## GCC 15.2.0

- **Machine:** Intel(R) Xeon(R) Processor @ 2.80GHz (family 6, model 85, stepping 7)
- **Flags:** `-march=native -O3 -DNDEBUG`
- **Commit:** `e19a96642225`
- **Cells:** cycles per call, from time at 3.32 GHz, the fastest clock a chain of dependent adds ran at
- **Calls:** 4096 independent calls on fixed pseudo-random inputs; the fastest of 5 passes of 11 runs each
- **Noise:** a cell's slowest pass took 72% longer than its fastest at the median, 184% at the most

### `bit_compress` on one 64-bit block

| inputs | by rank | PEXT | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 selected | 5.9 | 1.8 | 13.1 | 70.0 | 52.4 | 25.5 |
| 8 mask ones, 4 selected | 11.5 | 1.7 | 24.5 | 69.8 | 52.6 | 25.5 |
| 16 mask ones, 8 selected | 23.7 | 1.7 | 46.2 | 70.4 | 52.5 | 25.5 |
| 32 mask ones, 16 selected | 50.4 | 1.7 | 90.7 | 71.6 | 53.3 | 25.4 |
| 48 mask ones, 24 selected | 76.4 | 1.7 | 135.2 | 70.0 | 52.5 | 25.4 |
| 64 mask ones, 32 selected | 102.3 | 1.7 | 179.5 | 70.7 | 52.5 | 25.4 |

### `bit_expand` on one 64-bit block, its source ones counted among the mask's width

| inputs | by walk | by select | PDEP | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 source ones | 12.1 | 24.8 | 1.7 | 10.1 | 77.4 | 61.4 | 34.6 |
| 8 mask ones, 2 source ones | 24.1 | 24.7 | 1.7 | 18.2 | 77.5 | 61.0 | 34.6 |
| 8 mask ones, 4 source ones | 24.1 | 41.1 | 1.7 | 18.3 | 77.3 | 60.9 | 34.0 |
| 16 mask ones, 2 source ones | 48.7 | 24.9 | 1.8 | 34.8 | 77.3 | 61.0 | 34.4 |
| 16 mask ones, 8 source ones | 48.4 | 70.3 | 1.8 | 34.9 | 77.7 | 61.0 | 34.3 |
| 32 mask ones, 2 source ones | 97.1 | 24.8 | 1.7 | 68.7 | 77.4 | 61.1 | 34.3 |
| 32 mask ones, 16 source ones | 97.7 | 130.7 | 1.7 | 68.5 | 77.5 | 61.0 | 34.4 |
| 48 mask ones, 2 source ones | 144.8 | 24.8 | 1.7 | 102.5 | 77.4 | 61.4 | 34.6 |
| 48 mask ones, 24 source ones | 145.0 | 189.1 | 1.7 | 102.4 | 77.3 | 61.3 | 34.5 |
| 64 mask ones, 2 source ones | 193.3 | 24.8 | 1.8 | 135.9 | 77.1 | 60.8 | 34.3 |
| 64 mask ones, 32 source ones | 193.3 | 248.2 | 1.8 | 136.0 | 77.2 | 60.9 | 34.3 |

### `bit_select` on one 64-bit block

| inputs | by broadword | PDEP, TZCNT |
| :-- | --: | --: |
| 8 ones, any rank | 12.1 | 3.0 |
| 32 ones, any rank | 12.1 | 3.0 |
| 64 ones, any rank | 12.1 | 3.0 |

### `bit_reverse` on one 64-bit block

| inputs | by swaps |
| :-- | --: |
| any word | 6.4 |

### The harness alone

| inputs | XOR |
| :-- | --: |
| any two words | 2.1 |

## Clang 22.1.8 (++20260714014902+ca7933e47d3a-1~exp1~20260714135019.80)

- **Machine:** Intel(R) Xeon(R) Processor @ 2.80GHz (family 6, model 85, stepping 7)
- **Flags:** `-march=native -O3 -DNDEBUG`
- **Commit:** `e19a96642225`
- **Cells:** cycles per call, from time at 3.33 GHz, the fastest clock a chain of dependent adds ran at
- **Calls:** 4096 independent calls on fixed pseudo-random inputs; the fastest of 5 passes of 11 runs each
- **Noise:** a cell's slowest pass took 2% longer than its fastest at the median, 141% at the most

### `bit_compress` on one 64-bit block

| inputs | by rank | PEXT | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 selected | 8.0 | 1.7 | 9.8 | 69.8 | 58.1 | 26.3 |
| 8 mask ones, 4 selected | 10.1 | 1.7 | 18.4 | 69.7 | 57.1 | 26.3 |
| 16 mask ones, 8 selected | 18.8 | 1.7 | 35.6 | 69.4 | 58.0 | 25.6 |
| 32 mask ones, 16 selected | 39.0 | 1.7 | 70.7 | 69.8 | 58.1 | 26.2 |
| 48 mask ones, 24 selected | 60.7 | 1.7 | 106.2 | 69.9 | 58.2 | 26.2 |
| 64 mask ones, 32 selected | 81.7 | 1.7 | 141.5 | 69.9 | 58.0 | 26.4 |

### `bit_expand` on one 64-bit block, its source ones counted among the mask's width

| inputs | by walk | by select | PDEP | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 source ones | 12.1 | 23.9 | 2.0 | 12.6 | 77.7 | 66.6 | 33.9 |
| 8 mask ones, 2 source ones | 21.6 | 23.9 | 2.0 | 21.6 | 77.6 | 66.5 | 34.0 |
| 8 mask ones, 4 source ones | 21.5 | 40.3 | 2.0 | 21.8 | 77.5 | 66.3 | 34.0 |
| 16 mask ones, 2 source ones | 40.9 | 23.9 | 2.0 | 40.5 | 77.6 | 66.8 | 34.1 |
| 16 mask ones, 8 source ones | 40.7 | 67.6 | 2.0 | 40.5 | 77.7 | 66.5 | 33.9 |
| 32 mask ones, 2 source ones | 80.2 | 23.9 | 2.0 | 80.0 | 77.9 | 66.6 | 33.9 |
| 32 mask ones, 16 source ones | 80.1 | 123.2 | 2.0 | 80.6 | 77.7 | 66.5 | 34.0 |
| 48 mask ones, 2 source ones | 119.8 | 23.9 | 2.0 | 120.1 | 77.8 | 66.3 | 34.0 |
| 48 mask ones, 24 source ones | 120.5 | 178.4 | 2.0 | 120.0 | 77.5 | 66.4 | 34.0 |
| 64 mask ones, 2 source ones | 159.9 | 23.9 | 2.0 | 160.1 | 77.5 | 66.5 | 34.1 |
| 64 mask ones, 32 source ones | 160.5 | 233.8 | 2.0 | 160.6 | 77.4 | 66.8 | 34.2 |

### `bit_select` on one 64-bit block

| inputs | by broadword | PDEP, TZCNT |
| :-- | --: | --: |
| 8 ones, any rank | 11.7 | 2.0 |
| 32 ones, any rank | 11.7 | 2.0 |
| 64 ones, any rank | 11.7 | 2.0 |

### `bit_reverse` on one 64-bit block

| inputs | by swaps | `__builtin_bitreverse64` |
| :-- | --: | --: |
| any word | 5.8 | 5.8 |

### The harness alone

| inputs | XOR |
| :-- | --: |
| any two words | 1.8 |
