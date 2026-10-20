# Perft on Cascade Lake

Four virtual cores, in a cloud container, of a Xeon that reports family 6, model 85, stepping 7, which is Cascade Lake.
Each build is the stable rung at `-O3`, with and without `-march=x86-64-v3`, running `perft full` once from the commit
that added this example. Without the flag there is no BMI2, so `xstd::bit_compress` takes its portable kernel, and the
`_pext_u64` column does not exist.

Every count equals the published one in every build: Kiwipete to depth 5 and the start position to depth 6 are 194 and
119 million leaf nodes. Each cell is millions of leaf nodes a second, for perft with copy-make and a legality test after
every move, so it measures the move generator as a whole, sliders and all.

| Build | `xstd::bit_compress` | `xstd::bit_reverse` | magic | `_pext_u64` |
| :-- | --: | --: | --: | --: |
| GCC 15 | 13.4 | 16.2 | 31.9 |  |
| GCC 15, `-march=x86-64-v3` | 26.9 | 15.3 | 26.5 | 27.2 |
| Clang 22 | 18.7 | 15.2 | 32.0 |  |
| Clang 22, `-march=x86-64-v3` | 28.8 | 20.7 | 28.5 | 29.5 |

With BMI2, `xstd::bit_compress` runs within the noise of the raw `_pext_u64` it compiles to, and of magic bitboards.
Without it, the portable kernel halves the move generator's speed against magic bitboards, 13 to 19 million nodes a
second against 32: the rank kernel loops once per occupied square of a slider's relevant mask, where magic bitboards
spend one multiply. An engine without BMI2 is better served by magic bitboards than by `bit_compress`. Hyperbola
quintessence over `xstd::bit_reverse` needs no table and runs at 15 to 21 million.

## Per position

### g++-15

| position | depth | nodes | xstd::bit_compress | xstd::bit_reverse | magic |
| :-- | --: | --: | --: | --: | --: |
| start | 6 | 119060324 | 13.0 | 16.9 | 32.6 |
| kiwipete | 5 | 193690690 | 12.8 | 15.7 | 34.0 |
| position 3 | 6 | 11030083 | 17.9 | 17.2 | 29.0 |
| position 4 | 5 | 15833292 | 14.0 | 16.6 | 33.8 |
| position 5 | 5 | 89941194 | 16.2 | 16.1 | 28.9 |
| position 6 | 5 | 164075551 | 13.1 | 16.3 | 30.9 |
| all | | | 13.4 | 16.2 | 31.9 |

Every count equals the published one; cells are millions of leaf nodes a second.

### g++-15-v3

| position | depth | nodes | xstd::bit_compress | xstd::bit_reverse | magic | _pext_u64 |
| :-- | --: | --: | --: | --: | --: | --: |
| start | 6 | 119060324 | 29.6 | 16.5 | 29.7 | 31.5 |
| kiwipete | 5 | 193690690 | 27.8 | 15.4 | 26.7 | 26.4 |
| position 3 | 6 | 11030083 | 31.0 | 17.1 | 25.5 | 29.5 |
| position 4 | 5 | 15833292 | 25.2 | 13.5 | 25.5 | 26.3 |
| position 5 | 5 | 89941194 | 25.7 | 14.5 | 24.0 | 25.9 |
| position 6 | 5 | 164075551 | 24.9 | 14.9 | 25.9 | 26.0 |
| all | | | 26.9 | 15.3 | 26.5 | 27.2 |

Every count equals the published one; cells are millions of leaf nodes a second.

### clang++-22

| position | depth | nodes | xstd::bit_compress | xstd::bit_reverse | magic |
| :-- | --: | --: | --: | --: | --: |
| start | 6 | 119060324 | 16.3 | 15.5 | 33.6 |
| kiwipete | 5 | 193690690 | 18.3 | 15.0 | 29.7 |
| position 3 | 6 | 11030083 | 18.0 | 13.1 | 30.6 |
| position 4 | 5 | 15833292 | 19.0 | 15.3 | 33.7 |
| position 5 | 5 | 89941194 | 20.7 | 15.8 | 30.7 |
| position 6 | 5 | 164075551 | 20.2 | 15.2 | 34.7 |
| all | | | 18.7 | 15.2 | 32.0 |

Every count equals the published one; cells are millions of leaf nodes a second.

### clang++-22-v3

| position | depth | nodes | xstd::bit_compress | xstd::bit_reverse | magic | _pext_u64 |
| :-- | --: | --: | --: | --: | --: | --: |
| start | 6 | 119060324 | 30.8 | 24.4 | 35.7 | 33.8 |
| kiwipete | 5 | 193690690 | 27.8 | 20.1 | 26.6 | 29.2 |
| position 3 | 6 | 11030083 | 22.4 | 16.8 | 20.1 | 26.2 |
| position 4 | 5 | 15833292 | 28.4 | 21.7 | 27.6 | 21.7 |
| position 5 | 5 | 89941194 | 28.5 | 20.5 | 26.3 | 28.2 |
| position 6 | 5 | 164075551 | 29.3 | 19.7 | 28.8 | 29.2 |
| all | | | 28.8 | 20.7 | 28.5 | 29.5 |

Every count equals the published one; cells are millions of leaf nodes a second.
