# P3104R6 for libc++

A draft patch implementing P3104R6's bit permutations (`bit_repeat`, `bit_reverse`, `bit_compress`, `bit_expand`)
in libc++, for llvm/llvm-project#204404. It is held here for review before it is submitted upstream; it is not part
of xstd-ints and is not meant to merge into `main`.

It applies to llvm-project `6f03a15a531b5a6d04ca925c922b6c4fe47a10a9` (2026-10-16):

```sh
git -C llvm-project am path/to/0001-libc-Implement-P3104R6-Bit-permutations.patch
```

## What was verified, and how

libc++ could not be built in the session that wrote the patch, so its lit suite has not run. What did run:

- The four `std/numerics/bit/bit.permute/*.pass.cpp` tests, bundled with the patched `<__bit/permute.h>` on top of
  libc++ trunk's `<bit>`, built and run with Clang trunk on Compiler Explorer at `-O0` and at `-O2 -march=x86-64-v3`
  with `-Wall -Wextra`: all pass, no diagnostics, their `static_assert(test<T>())` included at libc++'s default
  constexpr limits. They cover every standard unsigned type, `unsigned __int128`, and `unsigned _BitInt(N)` for N in
  2, 13, 32, 64, 65, 128, 147 and 256.
- `permute.verify.cpp` under `-Xclang -verify`: passes.
- The `x86-64-v3` build of `bit_compress.pass.cpp` contains 164 `pext`/`pdep` instructions; the `znver2` and baseline
  builds contain none.
- The header's logic, extracted with libc++'s macros stubbed, against bit-at-a-time oracles with GCC 13 and Clang 18:
  0 mismatches at run time and in constant evaluation.
- `assert.bit_repeat.pass.cpp` has not run; it needs libc++'s `check_assertion.h` harness.

Clang 22 does not accept `-std=c++2d`, so the C++29 code is reachable from Clang 23 on. Clang trunk dropped
`__builtin_ia32_pext_di`/`pdep_di` for `__builtin_elementwise_pext`/`pdep`, which the patch prefers where present.

## Before submitting

- The portable kernels and block composition come from xstd-ints, under the Boost Software License; contributing them
  under LLVM's Apache-2.0 with LLVM exception is the copyright holder's call.
- LLVM's policy on tool-assisted contributions asks for disclosure in the PR description.
- Upstream CI decides the rest: the full lit run, the GCC and modules configurations, and the transitive-includes
  lists, which may change now that `<bit>` reaches `<__assert>` in C++29 mode.
