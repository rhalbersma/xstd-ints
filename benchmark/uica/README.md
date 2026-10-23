# uiCA's cycles per iteration

Written by `benchmark/static_analysis.py --uica`; do not edit. uiCA simulates the same regions llvm-mca
analyses, so a row means what it means in `benchmark/mca/README.md`; it models Intel cores only. Each
cell is uiCA's throughput prediction and the bottleneck it names. Each full report, with the
assembly it read, is beside this file.

Simulated by uiCA at commit `6ebeeab0f870`, with the uops.info data its setup downloaded.

## Ubuntu clang version 22.1.8 (++20260714014902+ca7933e47d3a-1~exp1~20260714135019.80), x86_64

| kernel | haswell | cascadelake | icelake-server |
| :-- | --: | --: | --: |
| `reverse_by_swaps` | 8.03, unknown | 8.61, unknown | 2.02, decoder |
| `reverse_intrinsic` | 8.03, unknown | 8.61, unknown | 2.02, decoder |
| `compress_by_rank`, *loop* | 2.62, unknown | 3.00, decoder | 3.08, unknown |
| `compress_pext` | 1.00, ports | 1.00, ports | 1.00, ports |
| `compress_by_walk_with_if`, *loop* | 2.43, unknown | 2.19, unknown | 2.30, unknown |
| `compress_by_parallel_suffix` | 93.50, unknown | 95.00, unknown | 116.25, unknown |
| `compress_by_zp7` | 56.75, unknown | 48.25, unknown | 59.25, unknown |
| `compress_by_zp7_clmul` | 44.50, unknown | 50.25, unknown | 43.50, unknown |
| `expand_by_walk`, *loop* | 2.33, lsd | 3.01, decoder, predecoder | 2.60, unknown |
| `expand_by_select`, *loop* | 6.84, unknown | 5.70, unknown | 6.41, unknown |
| `expand_pdep` | 1.00, ports | 1.00, ports | 1.00, ports |
| `expand_by_walk_with_if`, *loop* | 2.39, unknown | 3.00, decoder, predecoder | 2.73, unknown |
| `expand_by_parallel_suffix` | 91.25, unknown | 74.00, unknown | 69.00, unknown |
| `expand_by_zp7` | 88.00, unknown | 89.75, unknown | 103.00, unknown |
| `expand_by_zp7_clmul` | 60.75, unknown | 68.75, unknown | 61.25, unknown |
| `select_by_broadword` | 14.40, unknown | 14.07, dependencies | 15.50, unknown |
| `select_pdep` | 2.00, ports | 2.00, ports | 2.00, ports |

## g++-15 (Ubuntu 15.2.0-14ubuntu1~24~ppa1) 15.2.0, x86_64

| kernel | haswell | cascadelake | icelake-server |
| :-- | --: | --: | --: |
| `reverse_by_swaps` | 8.67, unknown | 9.42, unknown | 10.95, unknown |
| `reverse_intrinsic` |  |  | 2.02, decoder |
| `compress_by_rank`, *loop* | 2.89, unknown | 2.97, predecoder | 3.05, unknown |
| `compress_pext` | 1.00, ports | 1.00, ports | 1.00, ports |
| `compress_by_walk_with_if`, *loop* | 4.00, unknown | 3.01, lsd | 3.00, dependencies |
| `compress_by_parallel_suffix` | 94.00, unknown | 91.75, unknown | 115.00, unknown |
| `compress_by_zp7` | 51.50, unknown | 53.25, unknown | 69.00, unknown |
| `compress_by_zp7_clmul` | 38.80, unknown | 18.09, unknown | 16.92, predecoder |
| `expand_by_walk`, *loop* | 2.36, lsd | 3.00, decoder, predecoder | 2.65, unknown |
| `expand_by_select`, *loop* | 7.39, unknown | 6.00, unknown | 5.89, scheduling |
| `expand_pdep` | 1.00, ports | 1.00, ports | 1.00, ports |
| `expand_by_walk_with_if`, *loop* | 3.00, dependencies | 2.15, unknown | 2.26, unknown |
| `expand_by_parallel_suffix` | 94.50, unknown | 70.25, unknown | 71.50, unknown |
| `expand_by_zp7` | 91.75, unknown | 94.75, unknown | 107.00, unknown |
| `expand_by_zp7_clmul` | 32.00, unknown | 22.88, unknown | 18.27, unknown |
| `select_by_broadword` | 21.60, unknown | 21.60, unknown | 23.44, unknown |
| `select_pdep` | 2.00, ports | 2.00, ports | 2.00, ports |
