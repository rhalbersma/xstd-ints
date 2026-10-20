# `<xstd/ints/bit/rotl.hpp>`

This page also specifies `<xstd/ints/bit/rotr.hpp>`, whose functions share this text.

## `xstd::rotl`, `xstd::rotr`

```cpp
// (1)
template<class T>
  constexpr auto rotl(T x, int s) noexcept(see below) -> T;

// (2)
template<class T>
  constexpr auto rotr(T x, int s) noexcept(see below) -> T;
```

Rotate the bits of an unsigned integer, for every type the standard functions take and every `unsigned_integer`.

1. `rotl` rotates to the left.
2. `rotr` rotates to the right.

In the following, let `N` be `numeric_limits<T>::digits` and `r` be `s % N`.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | the value to rotate |
| `s` | the count to rotate by, which may be negative |

*Constraints:* For each function `f`, `std::f(x, s)` is well-formed, or `T` models `unsigned_integer`.

### Return value

Where `std::f(x, s)` is well-formed, each returns `std::f(x, s)`.

1. *Returns:* `x` if `r` is `0`; `(x << r) | (x >> (N - r))` if `r` is positive; `rotr(x, -r)` if `r` is negative.
   This is [[bit.rotate]](https://eel.is/c++draft/bit.rotate)'s element.
2. *Returns:* `x` if `r` is `0`; `(x >> r) | (x << (N - r))` if `r` is positive; `rotl(x, -r)` if `r` is negative.
   This is [[bit.rotate]](https://eel.is/c++draft/bit.rotate)'s element.

### Exceptions

The exception specification is `true` for the forwarding overload, and otherwise `nothrow_const_operators<T>`.

### Notes

- Every `int` count is in the domain, `INT_MIN` and `INT_MAX` included, and turning one way and then the other by the
  same count is the identity.
- A call is a constant expression.

### See also

| Entity | Relation |
| :----- | :------- |
| [`shl`, `shr`](shl.md) | shift rather than rotate |
| [`unsigned_integer`](../concepts/unsigned_integer.md) | what the block template takes |
| [`std::rotl`](https://en.cppreference.com/w/cpp/numeric/rotl) | the standard function (1) forwards to |
| [`std::rotr`](https://en.cppreference.com/w/cpp/numeric/rotr) | the standard function (2) forwards to |
