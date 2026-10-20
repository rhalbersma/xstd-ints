# `<xstd/ints/bit/bit_expand.hpp>`

## `bit_expand`

```cpp
#ifdef XSTD_HAS_STD_BIT_COMPRESS
template<class T>
  requires ints::detail::std_compresses_bits<T>
  constexpr auto bit_expand(T src, T mask) noexcept -> T;
#endif
template<unsigned_integer T>
  requires (not ints::detail::std_compresses_bits<T>)
  constexpr auto bit_expand(T src, T mask) noexcept(nothrow_const_operators<T>) -> T;
```

- *Constraints:* As for `bit_reverse`.
- *Returns:* `expand(src, mask)`, [P3104R5](https://github.com/cplusplus/papers/issues/1768)'s sum of `mask[n]·src[mask[0] + … + mask[n-1]]·2^n` over `n` in `[0, N)`: the
  low bits of `src` deposited, in their order, on the positions of `mask`'s ones.
- [[bit.permute]](https://eel.is/c++draft/bit.permute)'s example, as [P3104R6](https://github.com/cplusplus/papers/issues/1768) words it, holds: `bit_expand(0bABCD, 0b0101)` is `0b0C0D` for any bits `A`, `B`,
  `C` and `D`.
- The paper's equivalences hold: a mask of contiguous ones is an and and a shift, and expanding `2^n` through `x` gives
  `x`'s one of rank `n`, and `2^n - 1` its `n` lowest ones.
- The paper's interleaving holds: `bit_expand(x, bit_repeat(0b10u, 2)) | bit_expand(y, bit_repeat(0b01u, 2))` is the
  Z-order index of `x` and `y`.
- An empty mask gives zero, a full one `src`, and one through the top bit gives the top bit.
- It undoes `bit_compress` through the same mask, and is undone by it, as far as the mask's population lets it.
- A call is a constant expression for every type it takes.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.
- *Remarks:* The exception specification is `nothrow_const_operators<T>`, so the call is `noexcept` for every built-in
  type and not for `absl::uint128`.
- *Remarks:* Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`,
  `std::bit_repeat` and `std::bit_reverse` all take `T`, the call is `std::bit_expand(src, mask)`, unconditionally
  `noexcept`, and the template does not take `T`. On libstdc++ where `XSTD_INTS_HAS_FAST_BMI2` is defined it is not,
  since that `<bit>` emits no `PEXT` or `PDEP`, and the template takes every `unsigned_integer`.
- *Remarks:* The template expands each block of `mask` on its own, drawing on the bits of `src` the lower blocks left
  unconsumed, so the bits of a mask on either side of bit 64 receive adjacent bits of `src`.
- *Remarks:* Outside a constant expression, where `XSTD_INTS_HAS_FAST_BMI2` is defined, as for `bit_compress`, a block
  is one `PDEP`. Everywhere else, and in every constant expression, a block takes the portable kernel, which walks the
  mask's ones from the lowest and deposits the next bit of `src` on each by a mask rather than a branch.
- *Remarks:* A 64-bit call is one `pdep` with no branch or call under `-mbmi2` and `-march=x86-64-v3`, and has no `pext`
  or `pdep` under `-march=x86-64`, `-mgfni`, `bdver4`, `znver1` or `znver2`.
