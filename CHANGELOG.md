# Changelog

Each release of xstd-ints, newest first. A version is a tag `vX.Y.Z` matching `project(VERSION)` in
[CMakeLists.txt](CMakeLists.txt); an entry is written from the pull requests merged since the previous tag, grouped
by what a user of the library meets. The numbers are this repository's pull requests; what predates the first, #239, which split the repository out of
earlier work, carries none.

## 0.1.0 (unreleased)

The first release: integer extensions to the C++ standard library, open to the 128-bit integer classes and to
bit-precise integers where the standard is closed over the built-in types. It needs a C++23 compiler, and CMake 3.30
or later for the supplied CMake project.

### `<bit>`

- The C++20 and C++23 functions, `has_single_bit`, `bit_ceil`, `bit_floor`, `bit_width`, `rotl`, `rotr`,
  `countl_zero`, `countl_one`, `countr_zero`, `countr_one`, `popcount` and `byteswap`, for every xstd integer: the
  standard integer types, `int128` and `uint128`, `unsigned _BitInt(N)` and the 128-bit types of Boost.Int128 and
  Abseil (#271, #278).
- C++29's bit permutations from P3104R6, `bit_reverse`, `bit_repeat`, `bit_compress` and `bit_expand` (#275), and its
  shifts from P3793R2, `shl` and `shr`, for every pair of xstd integers (#279).
- `bit_select`, the position of the k-th one bit, which has no standard counterpart (#289).
- Each function forwards to `std::` where the standard library declares the call for the type, and runs its own
  kernel otherwise: `PEXT` and `PDEP` where BMI2 is fast, `RBIT` on AArch64, GFNI on x86-64, and portable `constexpr`
  kernels everywhere else (#280, #300, #336).

### Integers, limits and traits

- `int128` and `uint128`, wherever the target has a 128-bit integer and absent elsewhere (#284), and `bit_int<N>` and
  `bit_uint<N>` for bit-precise integers, with `XSTD_HAS_INT128` and `XSTD_HAS_BIT_INT` to test for them.
- `numeric_limits`, `is_signed`, `is_unsigned`, `make_signed`, `make_unsigned` and `promoted_t`, answering for the
  128-bit classes and bit-precise integers as the standard's answer for the built-in types.
- The concepts `integer`, `signed_integer`, `unsigned_integer` and `integer_class`, which admit paired integer-class
  types where `std::integral` does not and ask an integer-class type for a width (#376), `nothrow_const_operators`,
  `bit_mask` for the standard's bitmask types (#270) and `alignable`.

### Division, `abs` and `to_chars`

- `div`, `div_floor`, `div_ceil` and `div_euclid`, each returning a `div_result` at the operands' own width (#274),
  with `sign` and `unsigned_abs`.
- `abs`, returning the promoted type as `std::abs` does for the built-in types (#370).
- `to_chars` for every xstd integer, forwarding where `std::to_chars` takes the type (#339), and a `std::formatter`
  for `div_result` with the tuple's format specifications (#269, #333).
- `align_up`, `align_down` and `is_aligned`.

### Other libraries

- `<xstd/ints/ext/boost/int128.hpp>` and `<xstd/ints/ext/absl/int128.hpp>` make Boost.Int128's and Abseil's 128-bit
  types xstd integers, held to the integer concepts by tests over both (#333).
- `<xstd/ints/ext/boost/hash2.hpp>` hashes every xstd integer through Boost.Hash2 with `hash_append_int` (#273).
- A vcpkg overlay port and a Conan 2 recipe in the repository, tested from a clean project on Linux, macOS and Windows;
  neither goes to its registry before v1.0.0 (#373).

### Platforms

GCC 15, 16 and trunk; Clang 22, 23 and trunk, with libstdc++ and with libc++; Apple Clang from Xcode 16.4 and 26.6;
MSVC and clang-cl on Visual Studio 2022, 2026 and 2026 Preview, x64 and x86; MinGW; Emscripten for wasm32; i686; and
s390x, big-endian, under QEMU (#284, #291, #295, #363). The README's matrix gives the exact versions.

### How it is checked

- Every precondition the library asserts is held by a death test (#365), and line and branch coverage stays at 100%.
- Every 16-bit pair is swept through the bit permutations and the divisions (#310), and the portable kernels are
  checked against implementations xstd did not write, SIMDe, sux and Boost.Multiprecision, up to 1024 bits (#371).
- The portable kernels are proved equal to their specifications with Z3 (#323), checked for secret-dependent branches
  and addresses under memcheck (#338), fuzzed (#285, #332) and scored by the mutants Mull plants (#324).
- libc++'s, libstdc++'s and the MSVC STL's own tests of what xstd mirrors run against xstd (#368, #370, #383, #385).
- The instructions the bit kernels compile to are checked on x86-64 and AArch64 (#308), and the kernels are
  benchmarked (#325) and run in a chess move generator against the published perft counts (#342).
- Every public header is a leaf over its definition, so deleting one breaks no other (#386), and every reference page
  shows a compiled example whose output is checked on every platform (#389).
