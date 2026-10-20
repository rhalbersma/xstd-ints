# `<xstd/ints/bit/has_single_bit.hpp>`

This page also specifies `<xstd/ints/bit/bit_ceil.hpp>`, `<xstd/ints/bit/bit_floor.hpp>` and `<xstd/ints/bit/bit_width.hpp>`, whose functions share this text.

## `xstd::has_single_bit`, `xstd::bit_ceil`, `xstd::bit_floor`, `xstd::bit_width`

```cpp
// (1)
template<class T>
  constexpr auto has_single_bit(T x) noexcept(see below) -> bool;

// (2)
template<class T>
  constexpr auto bit_ceil(T x) -> T;

// (3)
template<class T>
  constexpr auto bit_floor(T x) noexcept(see below) -> T;

// (4)
template<class T>
  constexpr auto bit_width(T x) noexcept(see below) -> int;
```

The integral powers of two around an unsigned integer, for every type the standard functions take and every
`unsigned_integer`.

1. `has_single_bit` asks whether `x` is a power of two.
2. `bit_ceil` rounds `x` up to a power of two.
3. `bit_floor` rounds `x` down to a power of two.
4. `bit_width` counts the bits `x` needs.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | an unsigned integer value |

*Constraints:* For each function `f`, `std::f(x)` is well-formed, or `T` models `unsigned_integer`. `bool`, the
character types and every signed type are refused, as `<bit>` refuses them; a 128-bit integer class and an
`unsigned _BitInt(N)` are taken.

*Preconditions:* (2) The smallest integral power of two not less than `x` is representable as a value of type `T`:
`x` is at most the highest power of two of its type. The block template diagnoses a violation by `assert`, and the
forwarding overload as `std::bit_ceil` does.

### Return value

Where `std::f(x)` is well-formed, each returns `std::f(x)`.

1. *Returns:* `true` if `x` is an integral power of two, and `false` otherwise.
2. *Returns:* The smallest integral power of two that is not less than `x`, so `1` for both `0` and `1`.
3. *Returns:* `0` if `x` is `0`, and otherwise the largest integral power of two not greater than `x`.
4. *Returns:* `0` if `x` is `0`, and otherwise one plus the base-2 logarithm of `x`, with any fractional part
   discarded, as an `int`, the type [LWG 3656](https://wg21.link/LWG3656) gave `std::bit_width`.

### Exceptions

1. The exception specification is `true` for the forwarding overload, and otherwise `nothrow_const_operators<T>`.
2. It is not `noexcept`, as `std::bit_ceil` is not: its contract is narrow.
3. The exception specification is `true` for the forwarding overload, and otherwise `nothrow_const_operators<T>`.
4. The exception specification is `true` for the forwarding overload, and otherwise `nothrow_const_operators<T>`.

### Remarks

(2) A call that violates the precondition is not a core constant expression, whether or not `NDEBUG` is defined, as
[[bit.pow.two]](https://eel.is/c++draft/bit.pow.two) requires of `std::bit_ceil`.

### Notes

A call is a constant expression, for `bit_ceil` one within its precondition.

### See also

| Entity | Relation |
| :----- | :------- |
| [`countl_zero`, `popcount`](countl_zero.md) | count the bits of the same types |
| [`unsigned_integer`](../concepts/unsigned_integer.md) | what the block template takes |
| [`std::has_single_bit`](https://en.cppreference.com/w/cpp/numeric/has_single_bit) | the standard function (1) forwards to |
| [`std::bit_ceil`](https://en.cppreference.com/w/cpp/numeric/bit_ceil) | the standard function (2) forwards to |
| [`std::bit_floor`](https://en.cppreference.com/w/cpp/numeric/bit_floor) | the standard function (3) forwards to |
| [`std::bit_width`](https://en.cppreference.com/w/cpp/numeric/bit_width) | the standard function (4) forwards to |
