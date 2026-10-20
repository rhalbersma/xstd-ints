# `<xstd/ints/bit/has_single_bit.hpp>`

## `has_single_bit`, `bit_ceil`, `bit_floor`, `bit_width`

```cpp
template<class T>
  constexpr auto has_single_bit(T x) noexcept(see below) -> bool;
template<class T>
  constexpr auto bit_ceil(T x) -> T;
template<class T>
  constexpr auto bit_floor(T x) noexcept(see below) -> T;
template<class T>
  constexpr auto bit_width(T x) noexcept(see below) -> int;
```

- *Constraints:* For each function `f`, `std::f(x)` is well-formed, or `T` models `unsigned_integer`. `bool`, the
  character types and every signed type are refused, as `<bit>` refuses them; a 128-bit integer class and an
  `unsigned _BitInt(N)` are taken.
- Where `std::f(x)` is well-formed, each returns `std::f(x)`.
- `has_single_bit`:
  - *Returns:* `true` if `x` is an integral power of two, and `false` otherwise.
  - *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
    `nothrow_const_operators<T>`.
- `bit_ceil`:
  - *Preconditions:* The smallest integral power of two not less than `x` is representable as a value of type `T`: `x`
    is at most the highest power of two of its type. The block template diagnoses a violation by `assert`, and the
    forwarding overload as `std::bit_ceil` does.
  - *Returns:* The smallest integral power of two that is not less than `x`, so `1` for both `0` and `1`.
  - *Remarks:* A call that violates the precondition is not a core constant expression, whether or not `NDEBUG` is
    defined, as [[bit.pow.two]](https://eel.is/c++draft/bit.pow.two) requires of `std::bit_ceil`.
  - *Remarks:* It is not `noexcept`, as `std::bit_ceil` is not: its contract is narrow.
- `bit_floor`:
  - *Returns:* `0` if `x` is `0`, and otherwise the largest integral power of two not greater than `x`.
  - *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
    `nothrow_const_operators<T>`.
- `bit_width`:
  - *Returns:* `0` if `x` is `0`, and otherwise one plus the base-2 logarithm of `x`, with any fractional part
    discarded, as an `int`, the type [LWG 3656](https://wg21.link/LWG3656) gave `std::bit_width`.
  - *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
    `nothrow_const_operators<T>`.
- A call is a constant expression, for `bit_ceil` one within its precondition.
