# `<xstd/ints/bit/countl_zero.hpp>`

## `countl_zero`, `countl_one`, `countr_zero`, `countr_one`, `popcount`

```cpp
template<class T>
  constexpr auto countl_zero(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto countl_one(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto countr_zero(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto countr_one(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto popcount(T x) noexcept(see below) -> int;
```

- *Constraints:* For each function `f`, `std::f(x)` is well-formed, or `T` models `unsigned_integer`.
- A signed 128-bit class has no bits to count, as a signed built-in has none for `std::popcount`.
- Where `std::f(x)` is well-formed, each returns `std::f(x)`.
- `countl_zero`: *Returns:* The number of consecutive `0` bits in the value of `x`, starting from the most significant
  bit, so `N` for `0`.
- `countl_one`: *Returns:* The number of consecutive `1` bits in the value of `x`, starting from the most significant
  bit.
- `countr_zero`: *Returns:* The number of consecutive `0` bits in the value of `x`, starting from the least significant
  bit, so `N` for `0`.
- `countr_one`: *Returns:* The number of consecutive `1` bits in the value of `x`, starting from the least significant
  bit.
- `popcount`: *Returns:* The number of `1` bits in the value of `x`.
- *Remarks:* Where `XSTD_HAS_BIT_INT` is defined, `std::f` refuses `bit_uint<N>`, and `__has_builtin` reports
  `__builtin_clzg`, `__builtin_ctzg` or `__builtin_popcountg` respectively, `countl_zero`, `countr_zero` and `popcount`
  have a third overload taking `bit_uint<N>` and deducing `N`, which returns the builtin's answer, `N` for zero where it
  counts zeros, and is `noexcept`. It is more specialized than the block template, so for each width exactly one of the
  three overloads is viable.
- *Remarks:* The exception specification of `countl_one` and `countr_one` is `true` for the forwarding overload, and
  otherwise `nothrow_const_operators<T>`.
- *Remarks:* The exception specification of `countl_zero`, `countr_zero` and `popcount` is `true` for the forwarding
  overload and the builtin one, and otherwise `nothrow_const_operators<T>`.
- A call is a constant expression.
