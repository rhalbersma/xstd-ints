# Emerald Rapids, a GitHub-hosted runner

An `ubuntu-24.04` runner of the [Benchmark workflow](../../.github/workflows/benchmark.yml), on a Xeon that reports itself as an
"INTEL(R) XEON(R) PLATINUM 8573C", family 6, model 207, which is Emerald Rapids. The runner exposes no performance
counters, so each cell is time converted to cycles at the clock a chain of dependent adds ran at. Its slowest pass of a
cell took at most 12% longer than its fastest. Runners are drawn from a pool, so another run may land on another model.

Both pages come from one workflow run on pull request #325, whose commit `23c95b589524` is GitHub's merge of the branch
head `ee28f47` into `main`, each job building with the command in CONTRIBUTING.md and its compiler in place of `g++-15`.

## GCC 15.2.0

- **Machine:** INTEL(R) XEON(R) PLATINUM 8573C (family 6, model 207, stepping 2)
- **Flags:** `-march=native -O3 -DNDEBUG`
- **Commit:** `23c95b589524`
- **Cells:** cycles per call, from time at 3.63 GHz, the fastest clock a chain of dependent adds ran at
- **Calls:** 4096 independent calls on fixed pseudo-random inputs; the fastest of 5 passes of 11 runs each
- **Noise:** a cell's slowest pass took 0% longer than its fastest at the median, 12% at the most

### `bit_compress` on one 64-bit block

| inputs | by rank | PEXT | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 selected | 4.9 | 1.4 | 9.3 | 53.8 | 42.6 | 14.1 |
| 8 mask ones, 4 selected | 6.6 | 1.4 | 17.3 | 53.8 | 42.6 | 14.1 |
| 16 mask ones, 8 selected | 13.0 | 1.4 | 33.6 | 53.8 | 42.6 | 14.1 |
| 32 mask ones, 16 selected | 27.5 | 1.4 | 69.1 | 53.8 | 42.6 | 14.1 |
| 48 mask ones, 24 selected | 40.9 | 1.4 | 107.6 | 53.8 | 42.6 | 14.1 |
| 64 mask ones, 32 selected | 55.3 | 1.4 | 148.0 | 53.8 | 42.6 | 14.1 |

### `bit_expand` on one 64-bit block, its source ones counted among the mask's width

| inputs | by walk | by select | PDEP | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 source ones | 8.9 | 17.4 | 1.4 | 7.6 | 57.9 | 49.3 | 17.3 |
| 8 mask ones, 2 source ones | 17.2 | 17.4 | 1.4 | 13.5 | 57.9 | 49.3 | 17.3 |
| 8 mask ones, 4 source ones | 16.9 | 29.7 | 1.4 | 13.5 | 58.0 | 49.3 | 17.3 |
| 16 mask ones, 2 source ones | 33.2 | 17.4 | 1.4 | 28.4 | 57.9 | 49.3 | 17.3 |
| 16 mask ones, 8 source ones | 33.1 | 52.9 | 1.4 | 28.4 | 57.9 | 49.3 | 17.3 |
| 32 mask ones, 2 source ones | 66.0 | 17.4 | 1.4 | 59.9 | 57.9 | 49.3 | 17.3 |
| 32 mask ones, 16 source ones | 66.0 | 97.6 | 1.4 | 59.9 | 58.0 | 49.3 | 17.3 |
| 48 mask ones, 2 source ones | 99.1 | 17.4 | 1.4 | 92.8 | 57.9 | 49.3 | 17.3 |
| 48 mask ones, 24 source ones | 99.1 | 141.7 | 1.4 | 92.8 | 58.0 | 49.3 | 17.3 |
| 64 mask ones, 2 source ones | 132.5 | 17.4 | 1.4 | 127.5 | 57.9 | 49.3 | 17.3 |
| 64 mask ones, 32 source ones | 132.5 | 186.2 | 1.4 | 127.5 | 57.9 | 49.3 | 17.3 |

### `bit_select` on one 64-bit block

| inputs | by broadword | PDEP, TZCNT |
| :-- | --: | --: |
| 8 ones, any rank | 8.4 | 2.0 |
| 32 ones, any rank | 8.4 | 2.0 |
| 64 ones, any rank | 8.4 | 2.0 |

### `bit_reverse` on one 64-bit block

| inputs | by swaps | GF2P8AFFINEQB, BSWAP |
| :-- | --: | --: |
| any word | 4.6 | 1.7 |

### The harness alone

| inputs | XOR |
| :-- | --: |
| any two words | 1.2 |

## Clang 22.1.8

- **Machine:** INTEL(R) XEON(R) PLATINUM 8573C (family 6, model 207, stepping 2)
- **Flags:** `-march=native -O3 -DNDEBUG`
- **Commit:** `23c95b589524`
- **Cells:** cycles per call, from time at 3.03 GHz, the fastest clock a chain of dependent adds ran at
- **Calls:** 4096 independent calls on fixed pseudo-random inputs; the fastest of 5 passes of 11 runs each
- **Noise:** a cell's slowest pass took 1% longer than its fastest at the median, 4% at the most

### `bit_compress` on one 64-bit block

| inputs | by rank | PEXT | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 selected | 4.1 | 1.4 | 9.2 | 53.2 | 44.6 | 14.0 |
| 8 mask ones, 4 selected | 8.1 | 1.3 | 13.6 | 53.0 | 44.3 | 13.8 |
| 16 mask ones, 8 selected | 16.0 | 1.3 | 29.4 | 52.5 | 43.6 | 14.0 |
| 32 mask ones, 16 selected | 32.6 | 1.4 | 62.9 | 53.2 | 44.3 | 13.5 |
| 48 mask ones, 24 selected | 48.1 | 1.4 | 99.2 | 52.7 | 44.6 | 14.0 |
| 64 mask ones, 32 selected | 66.2 | 1.4 | 134.7 | 53.5 | 44.6 | 13.9 |

### `bit_expand` on one 64-bit block, its source ones counted among the mask's width

| inputs | by walk | by select | PDEP | walk with `if` | parallel suffix | zp7 | zp7, PCLMULQDQ |
| :-- | --: | --: | --: | --: | --: | --: | --: |
| 4 mask ones, 2 source ones | 8.1 | 17.1 | 1.4 | 8.9 | 58.0 | 50.4 | 17.4 |
| 8 mask ones, 2 source ones | 16.3 | 17.0 | 1.4 | 16.6 | 58.6 | 50.4 | 17.2 |
| 8 mask ones, 4 source ones | 16.1 | 28.8 | 1.3 | 16.9 | 58.4 | 50.3 | 17.2 |
| 16 mask ones, 2 source ones | 32.5 | 17.1 | 1.4 | 33.2 | 57.7 | 50.3 | 17.1 |
| 16 mask ones, 8 source ones | 32.5 | 52.9 | 1.4 | 33.3 | 58.7 | 50.2 | 17.2 |
| 32 mask ones, 2 source ones | 67.4 | 16.9 | 1.3 | 66.0 | 58.6 | 50.3 | 17.3 |
| 32 mask ones, 16 source ones | 67.4 | 94.0 | 1.4 | 64.4 | 58.3 | 50.2 | 17.4 |
| 48 mask ones, 2 source ones | 100.2 | 17.1 | 1.4 | 98.6 | 58.4 | 50.3 | 17.3 |
| 48 mask ones, 24 source ones | 102.1 | 133.7 | 1.4 | 99.1 | 58.2 | 49.4 | 17.4 |
| 64 mask ones, 2 source ones | 136.8 | 17.1 | 1.4 | 132.9 | 58.7 | 50.1 | 17.1 |
| 64 mask ones, 32 source ones | 136.8 | 174.0 | 1.4 | 133.6 | 58.7 | 50.3 | 17.3 |

### `bit_select` on one 64-bit block

| inputs | by broadword | PDEP, TZCNT |
| :-- | --: | --: |
| 8 ones, any rank | 8.2 | 2.0 |
| 32 ones, any rank | 8.2 | 2.0 |
| 64 ones, any rank | 8.3 | 2.0 |

### `bit_reverse` on one 64-bit block

| inputs | by swaps | `__builtin_bitreverse64` |
| :-- | --: | --: |
| any word | 1.7 | 1.7 |

### The harness alone

| inputs | XOR |
| :-- | --: |
| any two words | 1.4 |
