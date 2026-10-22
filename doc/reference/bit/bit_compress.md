# `<xstd/ints/bit/bit_compress.hpp>`

## `xstd::bit_compress`

```cpp
#ifdef XSTD_HAS_STD_BIT_COMPRESS
// (1)
template<class T>
  requires ints::detail::std_compresses_bits<T>
  constexpr auto bit_compress(T src, T mask) noexcept -> T;
#endif

// (2)
template<unsigned_integer T>
  requires (not ints::detail::std_compresses_bits<T>)
  constexpr auto bit_compress(T src, T mask) noexcept(nothrow_const_operators<T>) -> T;
```

Gathers the bits of an unsigned integer that a mask selects into its low bits.

1. The forwarding overload, which calls `std::bit_compress`.
2. The template, for every other `unsigned_integer`.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `src` | the value whose bits are gathered |
| `mask` | the bits to gather |

*Constraints:* As for `bit_reverse`.

### Return value

*Returns:* `compress(src, mask)`, [P3104R5](https://github.com/cplusplus/papers/issues/1768)'s sum of
`mask[n]·src[n]·2^(mask[0] + … + mask[n-1])` over `n` in `[0, N)`: the bits of `src` that `mask` selects, packed into
the low bits in their order.

### Exceptions

The exception specification is `nothrow_const_operators<T>`, so the call is `noexcept` for every built-in type and not
for `absl::uint128`.

### Remarks

- Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`, `std::bit_repeat` and
  `std::bit_reverse` all take `T`, the call is `std::bit_compress(src, mask)`, unconditionally `noexcept`, and the
  template does not take `T`. On libstdc++ where `XSTD_INTS_HAS_FAST_BMI2` is defined it is not, since that `<bit>`
  emits no `PEXT` or `PDEP`, and the template takes every `unsigned_integer`.
- The template compresses each block of `src` through the same block of `mask` and packs the result directly above the
  bits the lower blocks selected, so selected bits on either side of bit 64 land adjacent.
- Outside a constant expression, where `XSTD_INTS_HAS_FAST_BMI2` is defined, a block is one `PEXT`: x86-64 with
  `__BMI2__`, or MSVC other than clang-cl with `/arch:AVX2`, but not a build for `bdver4`, `znver1` or `znver2`, on
  which `PEXT` is microcoded. Everywhere else, and in every constant expression, a block takes the portable kernel,
  which places each selected bit at its rank among the mask's ones by one `popcount`.
- A 64-bit call is one `pext` with no branch or call under `-mbmi2` and `-march=x86-64-v3`, and has no `pext` or `pdep`
  under `-march=x86-64`, `-mgfni`, `bdver4`, `znver1` or `znver2`.

### Notes

- [[bit.permute]](https://eel.is/c++draft/bit.permute)'s example, as
  [P3104R6](https://github.com/cplusplus/papers/issues/1768) words it, holds: `bit_compress(0bABCD, 0b0101)` is
  `0b00BD` for any bits `A`, `B`, `C` and `D`.
- The paper's equivalences hold: a mask of contiguous ones is a shift and an and, a single-bit mask reads that bit, and
  compressing `x` through itself, or all ones through `x`, gives `popcount(x)` low ones.
- An empty mask gives zero, a full one `src`, and the top bit through itself gives one.
- It undoes `bit_expand` through the same mask, and is undone by it as far as the mask's population lets it:
  `bit_expand(bit_compress(x, m), m)` is `x & m`, and `bit_compress(bit_expand(x, m), m)` is `x` cut to its
  `popcount(m)` low bits.
- A call is a constant expression for every type it takes.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`bit_expand`](bit_expand.md) | the inverse, through the same mask |
| [`bit_reverse`](bit_reverse.md), [`bit_repeat`](bit_repeat.md) | the other permutations |
| [`bit_select`](bit_select.md) | the position of the one of a given rank, an xstd extension beside the permutations |
| [Notation](shl.md#notation-for-the-bit-permutations) | what `N`, `x[n]` and a block mean here |
| [P3104R6](https://github.com/cplusplus/papers/issues/1768) | the paper that proposes `std::bit_compress` |
