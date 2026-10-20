# `<xstd/ints/bit/countl_zero.hpp>`

This page also specifies `<xstd/ints/bit/countl_one.hpp>`, `<xstd/ints/bit/countr_one.hpp>`, `<xstd/ints/bit/countr_zero.hpp>` and `<xstd/ints/bit/popcount.hpp>`, whose functions share this text.

## `xstd::countl_zero`, `xstd::countl_one`, `xstd::countr_zero`, `xstd::countr_one`, `xstd::popcount`

```cpp
// (1)
template<class T>
  constexpr auto countl_zero(T x) noexcept(see below) -> int;

// (2)
template<class T>
  constexpr auto countl_one(T x) noexcept(see below) -> int;

// (3)
template<class T>
  constexpr auto countr_zero(T x) noexcept(see below) -> int;

// (4)
template<class T>
  constexpr auto countr_one(T x) noexcept(see below) -> int;

// (5)
template<class T>
  constexpr auto popcount(T x) noexcept(see below) -> int;
```

Count the bits of an unsigned integer's value, for every type the standard functions take and every
`unsigned_integer`.

1. `countl_zero` counts the leading `0` bits.
2. `countl_one` counts the leading `1` bits.
3. `countr_zero` counts the trailing `0` bits.
4. `countr_one` counts the trailing `1` bits.
5. `popcount` counts the `1` bits.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | the value whose bits are counted |

*Constraints:* For each function `f`, `std::f(x)` is well-formed, or `T` models `unsigned_integer`.

### Return value

Where `std::f(x)` is well-formed, each returns `std::f(x)`.

1. *Returns:* The number of consecutive `0` bits in the value of `x`, starting from the most significant bit, so `N`
   for `0`.
2. *Returns:* The number of consecutive `1` bits in the value of `x`, starting from the most significant bit.
3. *Returns:* The number of consecutive `0` bits in the value of `x`, starting from the least significant bit, so `N`
   for `0`.
4. *Returns:* The number of consecutive `1` bits in the value of `x`, starting from the least significant bit.
5. *Returns:* The number of `1` bits in the value of `x`.

### Exceptions

1. The exception specification is `true` for the forwarding overload and the builtin one, and otherwise
   `nothrow_const_operators<T>`.
2. The exception specification is `true` for the forwarding overload, and otherwise `nothrow_const_operators<T>`.
3. As (1).
4. As (2).
5. As (1).

### Remarks

Where `XSTD_HAS_BIT_INT` is defined, `std::f` refuses `bit_uint<N>`, and `__has_builtin` reports `__builtin_clzg`,
`__builtin_ctzg` or `__builtin_popcountg` respectively, `countl_zero`, `countr_zero` and `popcount` have a third
overload taking `bit_uint<N>` and deducing `N`, which returns the builtin's answer, `N` for zero where it counts zeros,
and is `noexcept`. It is more specialized than the block template, so for each width exactly one of the three
overloads is viable.

### Notes

- A signed 128-bit class has no bits to count, as a signed built-in has none for `std::popcount`.
- A call is a constant expression.

### See also

| Entity | Relation |
| :----- | :------- |
| [`bit_width`](has_single_bit.md) | the position past the highest `1` bit |
| [`bit_select`](bit_select.md) | the position of the `1` bit of a given rank |
| [`unsigned_integer`](../concepts/unsigned_integer.md) | what the block template takes |
| [`std::countl_zero`](https://en.cppreference.com/w/cpp/numeric/countl_zero) | the standard function (1) forwards to |
| [`std::countl_one`](https://en.cppreference.com/w/cpp/numeric/countl_one) | the standard function (2) forwards to |
| [`std::countr_zero`](https://en.cppreference.com/w/cpp/numeric/countr_zero) | the standard function (3) forwards to |
| [`std::countr_one`](https://en.cppreference.com/w/cpp/numeric/countr_one) | the standard function (4) forwards to |
| [`std::popcount`](https://en.cppreference.com/w/cpp/numeric/popcount) | the standard function (5) forwards to |
