# `<xstd/ints/bit/bit_select.hpp>`

## `bit_select`

```cpp
template<unsigned_integer T>
  constexpr auto bit_select(T x, int k) -> int;
```

An xstd extension beside [P3104](https://github.com/cplusplus/papers/issues/1768)'s permutations: no paper proposes it, and it has no `<bit>` counterpart to forward to.

- *Constraints:* `T` models `unsigned_integer`. `bool`, the character types, a signed type and a signed bit-precise type
  are turned away.
- *Preconditions:* `0 <= k` and `k < popcount(x)`.
- *Returns:* The `n` in `[0, N)` for which `x[n]` is one and `x[0] + … + x[n-1]` is `k`: the position of `x`'s one of
  rank `k`, counting from zero at the least significant end.
- It is the inverse of rank: exactly `k` ones of `x` lie below the one it finds, and expanding `2^k` through `x`
  deposits on it. At rank zero it is `countr_zero(x)`, and at rank `popcount(x) - 1` it is `bit_width(x) - 1`.
- Every rank of a full value is its own position, and the ones on either side of bit 64 are found in order.
- A call is a constant expression for every type it takes.
- *Throws:* Nothing, for a `T` whose operators throw nothing.
- *Remarks:* A call that violates the precondition is not a core constant expression, whether or not `NDEBUG` is
  defined.
- *Remarks:* It is not `noexcept`, for any type.
- *Remarks:* The result is an `int`, whatever the width of `T`.
- *Remarks:* A type no wider than a block calls the block kernel once. A wider type skips whole blocks by their
  population, subtracting it from `k`, until the one of rank `k` lies in the current block, and adds that block's offset
  to what the block kernel finds there.
- *Remarks:* Outside a constant expression, where `XSTD_INTS_HAS_FAST_BMI2` is defined, as for `bit_compress`, the block
  kernel deposits `2^k` through `x` by one `PDEP` and returns the deposit's `countr_zero`. Everywhere else, and in every
  constant expression, it is Vigna's broadword select, which finds the byte holding the one by one multiply and one
  compare and the bit within it from a table, with no loop or branch on the value.
- *Remarks:* A 64-bit call contains a `pdep` under `-mbmi2`, and under `-march=x86-64-v3` is a `pdep` followed at once
  by `tzcnt`, with no branch or call; it has no `pext` or `pdep` under `-march=x86-64`, `-mgfni`, `bdver4`, `znver1` or
  `znver2`.
