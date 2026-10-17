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

## dudect

`constant_time_dudect 60`: up to 60 seconds a case, each measurement drawing its class at random, the secret held at one
value in class 0 and drawn at random in class 1. dudect calls a maximum |t| above 10 probable leakage and above 500
certain; below 10 it has only found nothing yet.

| Case | Measurements | max t | max tau | Verdict |
| :-- | --: | --: | --: | :-- |
| `reverse_by_swaps.word` | 41.37 M | +2.33 | 3.62e-04 | no leakage found within the budget |
| `select_by_broadword.dense` | 1.26 M | +10.14 | 9.03e-03 | leakage found |
| `select_by_broadword.one_bit` | 0.49 M | +10.43 | 1.50e-02 | leakage found |
| `expand_by_walk.source` | 42.05 M | +1.83 | 2.83e-04 | no leakage found within the budget |
| `expand_by_walk.mask` | 0.09 M | +889.07 | 2.92e+00 | leakage found |
| `compress_by_rank.source` | 0.09 M | +1555.06 | 5.31e+00 | leakage found |
| `expand_by_select.source` | 0.08 M | +1516.97 | 5.50e+00 | leakage found |
| `pext.source` | 40.81 M | +1.71 | 2.67e-04 | no leakage found within the budget |
| `pext.mask` | 40.91 M | +2.49 | 3.89e-04 | no leakage found within the budget |
| `pdep.mask` | 40.39 M | +1.70 | 2.67e-04 | no leakage found within the budget |
