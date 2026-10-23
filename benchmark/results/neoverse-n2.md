# Neoverse N2, a GitHub-hosted runner

An `ubuntu-24.04-arm` runner of the [Benchmark workflow](../../.github/workflows/benchmark.yml), on a core that reports itself as
an Arm Neoverse N2. AArch64 has no bit-permute instruction outside SVE2's optional BITPERM, which this core does not
report, so only the portable kernels and `RBIT` run. The runner exposes no performance counters, so each cell is time
converted to cycles at the clock a chain of dependent adds ran at. Its slowest pass of a cell took at most 4% longer than
its fastest.

Both pages come from one workflow run on pull request #325, whose commit `23c95b589524` is GitHub's merge of the branch
head `ee28f47` into `main`, each job building with the command in CONTRIBUTING.md, `-mcpu=native` in place of
`-march=native`, and its compiler in place of `g++-15`.

## GCC 15.2.0

- **Machine:** Arm Neoverse N2
- **Flags:** `-mcpu=native -O3 -DNDEBUG`
- **Commit:** `23c95b589524`
- **Cells:** cycles per call, from time at 3.43 GHz, the fastest clock a chain of dependent adds ran at
- **Calls:** 4096 independent calls on fixed pseudo-random inputs; the fastest of 5 passes of 11 runs each
- **Noise:** a cell's slowest pass took 0% longer than its fastest at the median, 4% at the most

### `bit_compress` on one 64-bit block

| inputs | by rank | walk with `if` | parallel suffix | zp7 |
| :-- | --: | --: | --: | --: |
| 4 mask ones, 2 selected | 6.8 | 10.9 | 33.2 | 25.6 |
| 8 mask ones, 4 selected | 12.8 | 22.9 | 33.2 | 25.6 |
| 16 mask ones, 8 selected | 25.7 | 47.1 | 33.2 | 25.6 |
| 32 mask ones, 16 selected | 53.4 | 97.0 | 33.2 | 25.6 |
| 48 mask ones, 24 selected | 81.6 | 146.9 | 33.2 | 25.6 |
| 64 mask ones, 32 selected | 109.9 | 197.2 | 33.2 | 25.6 |

### `bit_expand` on one 64-bit block, its source ones counted among the mask's width

| inputs | by walk | by select | walk with `if` | parallel suffix | zp7 |
| :-- | --: | --: | --: | --: | --: |
| 4 mask ones, 2 source ones | 11.2 | 22.9 | 11.1 | 35.2 | 32.0 |
| 8 mask ones, 2 source ones | 22.7 | 22.9 | 22.5 | 35.2 | 32.0 |
| 8 mask ones, 4 source ones | 22.7 | 37.4 | 22.5 | 35.2 | 32.0 |
| 16 mask ones, 2 source ones | 47.0 | 22.9 | 46.9 | 35.2 | 32.0 |
| 16 mask ones, 8 source ones | 46.8 | 66.8 | 46.9 | 35.2 | 32.0 |
| 32 mask ones, 2 source ones | 97.9 | 22.9 | 96.4 | 35.2 | 32.0 |
| 32 mask ones, 16 source ones | 98.0 | 122.7 | 96.5 | 35.2 | 32.0 |
| 48 mask ones, 2 source ones | 148.8 | 22.9 | 145.2 | 35.2 | 32.0 |
| 48 mask ones, 24 source ones | 148.9 | 179.2 | 144.9 | 35.2 | 32.0 |
| 64 mask ones, 2 source ones | 198.8 | 22.9 | 195.2 | 35.2 | 32.0 |
| 64 mask ones, 32 source ones | 198.5 | 235.7 | 193.3 | 35.2 | 32.0 |

### `bit_select` on one 64-bit block

| inputs | by broadword |
| :-- | --: |
| 8 ones, any rank | 10.0 |
| 32 ones, any rank | 10.0 |
| 64 ones, any rank | 10.0 |

### `bit_reverse` on one 64-bit block

| inputs | by swaps | RBIT |
| :-- | --: | --: |
| any word | 4.3 | 1.2 |

### The harness alone

| inputs | XOR |
| :-- | --: |
| any two words | 1.3 |

## Clang 22.1.8

- **Machine:** Arm Neoverse N2
- **Flags:** `-mcpu=native -O3 -DNDEBUG`
- **Commit:** `23c95b589524`
- **Cells:** cycles per call, from time at 3.43 GHz, the fastest clock a chain of dependent adds ran at
- **Calls:** 4096 independent calls on fixed pseudo-random inputs; the fastest of 5 passes of 11 runs each
- **Noise:** a cell's slowest pass took 0% longer than its fastest at the median, 3% at the most

### `bit_compress` on one 64-bit block

| inputs | by rank | walk with `if` | parallel suffix | zp7 |
| :-- | --: | --: | --: | --: |
| 4 mask ones, 2 selected | 7.1 | 11.4 | 32.9 | 29.4 |
| 8 mask ones, 4 selected | 13.0 | 24.2 | 32.8 | 29.4 |
| 16 mask ones, 8 selected | 25.4 | 51.5 | 32.9 | 29.4 |
| 32 mask ones, 16 selected | 50.0 | 108.4 | 32.9 | 29.4 |
| 48 mask ones, 24 selected | 74.8 | 165.4 | 32.9 | 29.4 |
| 64 mask ones, 32 selected | 99.7 | 222.0 | 32.9 | 29.4 |

### `bit_expand` on one 64-bit block, its source ones counted among the mask's width

| inputs | by walk | by select | walk with `if` | parallel suffix | zp7 |
| :-- | --: | --: | --: | --: | --: |
| 4 mask ones, 2 source ones | 11.0 | 22.4 | 11.3 | 35.7 | 36.7 |
| 8 mask ones, 2 source ones | 22.3 | 22.4 | 23.0 | 35.7 | 36.7 |
| 8 mask ones, 4 source ones | 22.4 | 36.9 | 23.0 | 35.7 | 36.7 |
| 16 mask ones, 2 source ones | 45.5 | 22.4 | 48.8 | 35.7 | 36.7 |
| 16 mask ones, 8 source ones | 45.5 | 65.9 | 48.8 | 35.7 | 36.7 |
| 32 mask ones, 2 source ones | 92.6 | 22.4 | 104.1 | 35.7 | 36.7 |
| 32 mask ones, 16 source ones | 91.9 | 122.1 | 104.2 | 35.7 | 36.7 |
| 48 mask ones, 2 source ones | 140.5 | 22.4 | 160.4 | 35.7 | 36.7 |
| 48 mask ones, 24 source ones | 140.4 | 178.6 | 160.3 | 35.7 | 36.7 |
| 64 mask ones, 2 source ones | 188.2 | 22.4 | 216.7 | 35.7 | 36.7 |
| 64 mask ones, 32 source ones | 187.6 | 235.0 | 216.5 | 35.7 | 36.7 |

### `bit_select` on one 64-bit block

| inputs | by broadword |
| :-- | --: |
| 8 ones, any rank | 8.9 |
| 32 ones, any rank | 8.9 |
| 64 ones, any rank | 8.9 |

### `bit_reverse` on one 64-bit block

| inputs | by swaps | `__builtin_bitreverse64` |
| :-- | --: | --: |
| any word | 1.3 | 1.3 |

### The harness alone

| inputs | XOR |
| :-- | --: |
| any two words | 1.5 |
