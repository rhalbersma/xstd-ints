# `<xstd/ints/bit/rotl.hpp>`

This page also specifies `<xstd/ints/bit/rotr.hpp>`, whose functions share this text.

## `rotl`, `rotr`

```cpp
template<class T>
  constexpr auto rotl(T x, int s) noexcept(see below) -> T;
template<class T>
  constexpr auto rotr(T x, int s) noexcept(see below) -> T;
```

In the following, let `N` be `numeric_limits<T>::digits` and `r` be `s % N`.

- *Constraints:* For each function `f`, `std::f(x, s)` is well-formed, or `T` models `unsigned_integer`.
- Where `std::f(x, s)` is well-formed, each returns `std::f(x, s)`.
- `rotl`: *Returns:* `x` if `r` is `0`; `(x << r) | (x >> (N - r))` if `r` is positive; `rotr(x, -r)` if `r` is
  negative. This is [[bit.rotate]](https://eel.is/c++draft/bit.rotate)'s element.
- `rotr`: *Returns:* `x` if `r` is `0`; `(x >> r) | (x << (N - r))` if `r` is positive; `rotl(x, -r)` if `r` is
  negative. This is [[bit.rotate]](https://eel.is/c++draft/bit.rotate)'s element.
- Every `int` count is in the domain, `INT_MIN` and `INT_MAX` included, and turning one way and then the other by the
  same count is the identity.
- *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
  `nothrow_const_operators<T>`.
- A call is a constant expression.
