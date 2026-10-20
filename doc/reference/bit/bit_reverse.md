# `<xstd/ints/bit/bit_reverse.hpp>`

## `bit_reverse`

```cpp
#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS
template<class T>
  requires ints::detail::std_permutes_bits<T>
  constexpr auto bit_reverse(T x) noexcept -> T;
#endif
template<unsigned_integer T>
  requires (not ints::detail::std_permutes_bits<T>)
  constexpr auto bit_reverse(T x) noexcept(nothrow_const_operators<T>) -> T;
```

- *Constraints:* `T` models `unsigned_integer`, which takes the 128-bit integer classes and `bit_uint<N>` besides the
  standard unsigned types, or `std::bit_reverse` takes it. `bool`, the character types, a signed type and a signed
  bit-precise type are turned away.
- *Returns:* `reverse(x)`, [P3104R5](https://github.com/cplusplus/papers/issues/1768)'s sum of `x[n]·2^(N-n-1)` over `n` in `[0, N)`: bit `n` of `x` becomes bit
  `N - 1 - n`.
- The paper's example, `bit_reverse(uint32_t{0x0000'1234})`, is `0x2c48'0000`, the mirror image, where the paper prints
  `0x24c80000`.
- `bit_reverse(bit_reverse(x))` is `x`, [[bit.permute]](https://eel.is/c++draft/bit.permute)'s note.
- Compressing from the top is reversal around compressing from the bottom: reversing `x` and `m`, compressing and
  reversing back gives `bit_compress(x, m)` shifted up by the count of `m`'s zeros.
- A call is a constant expression for every type it takes.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.
- *Remarks:* The exception specification is `nothrow_const_operators<T>`, so the call is `noexcept` for every built-in
  type and not for `absl::uint128`.
- *Remarks:* Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`,
  `std::bit_repeat` and `std::bit_reverse` all take `T`, the call is `std::bit_reverse(x)`, unconditionally `noexcept`,
  and the template does not take `T`.
- *Remarks:* The template mirrors each block on its own and lands it at the mirror of its offset; a top block that `N`
  fills only in part is mirrored whole and shifted down by its slack. The bit just past the lowest 64 lands that far
  below the top.
- *Remarks:* The blocks are unrolled at compile time, so a 128-bit call compiles to no branch or call on x86-64, and to
  two `rbit` with no branch on AArch64; under `-mgfni` it holds two `gf2p8affineqb`.
- *Remarks:* A block is mirrored by the compiler's `__builtin_bitreverse32` and `__builtin_bitreverse64` wherever it has
  both, in a constant expression too. Otherwise, outside a constant expression, by one `RBIT` where the target is
  AArch64 with `<arm_acle.h>`, or by one `GF2P8AFFINEQB` and a byte swap where it is x86-64 with `__GFNI__`; and by the
  portable kernel, a byte swap and three masked swaps, everywhere else.
- *Remarks:* A 64-bit call is a `gf2p8affineqb` and a `bswap` with no branch under `-mgfni`, and one `rbit` with no
  branch on AArch64.
