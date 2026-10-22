# llvm-mca's cycles per iteration

Written by `benchmark/static_analysis.py`; do not edit. A kernel without a loop is one call per iteration;
a kernel marked *loop* is one trip of its innermost loop, so one selected bit for `compress_by_rank`,
one mask bit for the walks, and one source bit for `expand_by_select`. A blank cell is a kernel the
target does not have. Each summary, with the assembly it read, is beside this file.

A cell is the model's, not the hardware's. The Zen 2 model issues `PEXT` and `PDEP` as one fast uop
and gives `PCLMULQDQ` a latency of 100 cycles, where uops.info measures
[`PEXT`](https://uops.info/html-instr/PEXT_R64_R64_R64.html) and
[`PCLMULQDQ`](https://uops.info/html-instr/PCLMULQDQ_XMM_XMM_I8.html) on Zen 2 otherwise, so the
`znver2` cells of the kernels built on them follow the model.

Analysed by Ubuntu LLVM version 22.1.8.

## Ubuntu clang version 22.1.8 (++20260714014902+ca7933e47d3a-1~exp1~20260714135019.80), x86_64

| kernel | haswell | cascadelake | icelake-server | znver2 | znver3 | znver4 |
| :-- | --: | --: | --: | --: | --: | --: |
| `reverse_by_swaps` | 8.01 | 8.01 | 1.41 | 8.01 | 8.00 | 2.51 |
| `reverse_intrinsic` | 8.01 | 8.01 | 1.41 | 8.01 | 8.00 | 2.51 |
| `compress_by_rank`, *loop* | 2.01 | 2.01 | 2.01 | 2.34 | 2.01 | 2.01 |
| `compress_pext` | 1.00 | 1.00 | 1.00 | 0.65 | 1.00 | 1.00 |
| `compress_by_walk_with_if`, *loop* | 2.01 | 1.75 | 1.75 | 2.51 | 2.01 | 1.50 |
| `compress_by_parallel_suffix` | 107.00 | 107.00 | 107.00 | 104.01 | 85.01 | 85.01 |
| `compress_by_zp7` | 56.71 | 56.71 | 56.71 | 53.04 | 42.53 | 40.53 |
| `compress_by_zp7_clmul` | 65.01 | 39.01 | 39.01 | 510.01 | 27.01 | 27.01 |
| `expand_by_walk`, *loop* | 2.26 | 2.26 | 2.26 | 3.00 | 2.34 | 1.75 |
| `expand_by_select`, *loop* | 5.47 | 5.11 | 5.11 | 6.03 | 5.20 | 4.82 |
| `expand_pdep` | 1.00 | 1.00 | 1.00 | 0.65 | 1.00 | 1.00 |
| `expand_by_walk_with_if`, *loop* | 2.26 | 2.25 | 2.25 | 3.00 | 2.34 | 1.75 |
| `expand_by_parallel_suffix` | 81.05 | 82.04 | 82.04 | 74.05 | 56.52 | 54.78 |
| `expand_by_zp7` | 98.52 | 98.52 | 98.52 | 98.02 | 82.02 | 82.02 |
| `expand_by_zp7_clmul` | 80.01 | 55.01 | 55.01 | 527.01 | 44.01 | 44.01 |
| `select_by_broadword` | 15.02 | 15.02 | 15.02 | 14.02 | 13.53 | 13.04 |
| `select_pdep` | 2.01 | 2.01 | 2.01 | 3.18 | 1.51 | 1.51 |

## g++-15 (Ubuntu 15.2.0-14ubuntu1~24~ppa1) 15.2.0, x86_64

| kernel | haswell | cascadelake | icelake-server | znver2 | znver3 | znver4 |
| :-- | --: | --: | --: | --: | --: | --: |
| `reverse_by_swaps` | 9.01 | 9.01 | 9.01 | 9.01 | 10.97 | 11.00 |
| `reverse_intrinsic` |  |  | 1.41 |  |  | 2.51 |
| `compress_by_rank`, *loop* | 2.01 | 2.01 | 2.01 | 2.01 | 2.01 | 2.01 |
| `compress_pext` | 1.00 | 1.00 | 1.00 | 0.65 | 1.00 | 1.00 |
| `compress_by_walk_with_if`, *loop* | 4.00 | 3.00 | 3.00 | 3.00 | 2.01 | 2.00 |
| `compress_by_parallel_suffix` | 105.50 | 105.50 | 105.50 | 105.00 | 86.00 | 86.00 |
| `compress_by_zp7` | 55.04 | 55.04 | 55.04 | 53.04 | 46.04 | 46.04 |
| `compress_by_zp7_clmul` | 55.02 | 18.03 | 18.03 | 257.13 | 15.53 | 15.03 |
| `expand_by_walk`, *loop* | 2.26 | 2.25 | 2.25 | 3.00 | 2.34 | 1.75 |
| `expand_by_select`, *loop* | 5.82 | 5.64 | 5.64 | 6.03 | 5.36 | 4.65 |
| `expand_pdep` | 1.00 | 1.00 | 1.00 | 0.65 | 1.00 | 1.00 |
| `expand_by_walk_with_if`, *loop* | 3.00 | 2.00 | 2.00 | 2.51 | 2.01 | 2.00 |
| `expand_by_parallel_suffix` | 77.06 | 78.05 | 78.05 | 73.56 | 58.03 | 56.03 |
| `expand_by_zp7` | 104.01 | 104.00 | 104.00 | 103.01 | 92.01 | 92.01 |
| `expand_by_zp7_clmul` | 37.04 | 24.04 | 24.05 | 176.31 | 15.04 | 13.04 |
| `select_by_broadword` | 22.01 | 22.01 | 22.01 | 21.01 | 20.02 | 19.02 |
| `select_pdep` | 2.01 | 2.01 | 2.01 | 3.18 | 1.51 | 1.51 |

## Ubuntu clang version 22.1.8 (++20260714014902+ca7933e47d3a-1~exp1~20260714135019.80), aarch64

| kernel | neoverse-n1 | neoverse-n2 | neoverse-v2 |
| :-- | --: | --: | --: |
| `reverse_by_swaps` | 1.00 | 1.00 | 1.00 |
| `reverse_intrinsic` | 1.00 | 1.00 | 1.00 |
| `compress_by_rank`, *loop* | 4.01 | 3.01 | 2.01 |
| `compress_by_walk_with_if`, *loop* | 3.01 | 2.01 | 2.01 |
| `compress_by_parallel_suffix` | 27.04 | 21.03 | 20.03 |
| `compress_by_zp7` | 22.71 | 17.53 | 13.04 |
| `expand_by_walk`, *loop* | 3.01 | 2.00 | 2.00 |
| `expand_by_select`, *loop* | 9.03 | 5.38 | 4.03 |
| `expand_by_walk_with_if`, *loop* | 3.35 | 2.01 | 2.01 |
| `expand_by_parallel_suffix` | 28.70 | 22.53 | 14.05 |
| `expand_by_zp7` | 28.44 | 23.70 | 16.05 |
| `select_by_broadword` | 37.00 | 29.00 | 29.00 |
