# Measurements

The design notes state each measured claim once and link here. Each section gives the machine, the compiler and its
flags, and where the full results and the code that produced them are kept.

## The bit-permutation kernels

[`bench/`](../bench) times each kernel and the alternatives it beat, in cycles per independent call on a 64-bit block
of stated density, the harness that feeds each call included. [`bench/results/`](../bench/results) keeps each run with
the machine, compiler, flags and commit it names, and [CONTRIBUTING.md](../CONTRIBUTING.md#benchmarks) gives the
command.

From the [Cascade Lake run](../bench/results/cascade-lake.md), at `-march=native -O3`, with a cost per bit
taken as the slope from the sparsest row of its table to the densest:

| Kernel | Cost | GCC 15 | Clang 22 |
| :-- | :-- | --: | --: |
| `PEXT`, `PDEP` | per call | 1.7 | 1.7 to 2.0 |
| `block_compress_by_rank` | per selected bit | 3.2 | 2.5 |
| `block_expand_by_walk` | per mask bit | 3.0 | 2.5 |
| `block_expand_by_select` | per source bit | 7.4 | 7.0 |
| `std::countr_zero` of `PDEP` | per call | 3.0 | 2.0 |
| `block_select_by_broadword` | per call | 12.1 | 11.7 |
| the harness alone | per call | 2.1 | 1.8 |

With two source ones, `block_expand_by_select` beats the walk from about nine mask ones up. The
alternatives measured and left out cost, on the same run: the mask walk with an `if` on each bit,
which both compilers turn into a conditional move rather than a branch, 1.75 times the rank kernel
to compress at half density, since it pays for every mask bit rather than every selected one, and
to expand as much as the masked walk with Clang and less with GCC, 2.1 cycles a mask bit against
3.0; Hacker's Delight's parallel suffix, a flat 70 cycles to compress and 77 to expand; and
Wegner's zp7, a flat 52 to 58 and 61 to 66 without `PCLMULQDQ`, and 25 to 26 and 34 with it.

## Secret-dependent control flow

Whether a kernel's control flow or memory addresses depend on secret data is checked per kernel and per secret
argument in [`constant_time/`](../constant_time), with its [results](../constant_time/results.md). Valgrind's memcheck,
with the secret marked undefined, reports every branch and address computed from it, the same for
GCC 15 and Clang 22 with and without `-march=x86-64-v3`:

| Kernel | Secret | Memcheck |
| :-- | :-- | :-- |
| `block_reverse_by_swaps` | the word | clean |
| `block_expand_by_walk` | the source, the mask public | clean |
| `block_expand_by_walk` | the mask | a branch: one trip per mask one |
| `block_compress_by_rank` | the source | a branch: one trip per selected bit |
| `block_expand_by_select` | the source | a branch, and the table lookup |
| `block_select_by_broadword` | the word and the rank | no branch, but one address from them: `select_in_byte` |
| `PEXT`, `PDEP` | the source or the mask | clean |

So only the byte reversal, the walk over a public mask, and the two instructions are free of
secret-dependent branches and addresses. The select never branches, yet indexes a table by the
secret, which a cache-timing attacker can observe, and the loops branch on no bit of the value but
run a number of times that depends on it. Memcheck says nothing about time itself: the instructions'
own latency can depend on their operands, as uops.info measures for
[`PEXT (R64, R64, R64)`](https://uops.info/html-instr/PEXT_R64_R64_R64.html) on Zen 2.
