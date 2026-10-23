# Constant-time checks on Cascade Lake

Four virtual cores, in a cloud container, of a Xeon that reports family 6, model 85, stepping 7, which is Cascade Lake.
Built with GCC 15.2.0 at `-march=x86-64-v3 -O3` from commit 54e0442 with this suite added, and run with the commands in
CONTRIBUTING.md.

## Memcheck

Valgrind 3.22 with each case's secret marked undefined. The same verdicts hold for GCC 15 and Clang 22, each with and
without `-march=x86-64-v3`; the PEXT and PDEP cases exist only where the build enables BMI2.

| Case | Verdict | What memcheck reports |
| :-- | :-- | :-- |
| `reverse_by_swaps.word` | clean | nothing |
| `expand_by_walk.source` | clean | nothing |
| `select_by_broadword.dense`, `.one_bit` | flagged | one use of the secret as an address per call: the `select_in_byte` lookup |
| `expand_by_walk.mask` | flagged | a conditional jump on the secret: the loop runs once per mask one |
| `compress_by_rank.source` | flagged | a conditional jump on the secret: the loop runs once per selected bit |
| `expand_by_select.source` | flagged | a conditional jump on the secret, and the `select_in_byte` lookup |
| `pext.source`, `pext.mask`, `pdep.mask` | clean | nothing |

Memcheck sees control flow and addresses, not time. A clean case has no branch and no address that depends on its
secret; whether its instructions take the same time for every operand is a question about the processor, which this
suite does not measure.
