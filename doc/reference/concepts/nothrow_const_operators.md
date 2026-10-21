# `<xstd/ints/concepts/nothrow_const_operators.hpp>`

## `xstd::nothrow_const_operators`

```cpp
template<class T_cv, class T = remove_cv_t<T_cv>>
concept nothrow_const_operators =
  same_as<T, remove_cv_t<T_cv>> and
  integer<T> and
  is_nothrow_destructible_v<T> and
  is_nothrow_move_constructible_v<T> and
  is_nothrow_copy_constructible_v<T> and
  is_nothrow_constructible_v<T, int> and
  is_nothrow_constructible_v<T, size_t> and
  is_nothrow_constructible_v<int, T> and
  is_nothrow_constructible_v<size_t, T> and
  requires(const T a) {
    { +a } noexcept;
    { -a } noexcept;
    { !a } noexcept;
    { ~a } noexcept;
  } and
  requires(const T a, const T b) {
    { a * b } noexcept;
    { a / b } noexcept;
    { a % b } noexcept;
    { a + b } noexcept;
    { a - b } noexcept;
  } and
  requires(const T a, const size_t n) {
    { a << n } noexcept;
    { a >> n } noexcept;
  } and
  requires(const T a, const T b) {
    { a <=> b } noexcept;
    { a < b } noexcept;
    { a > b } noexcept;
    { a <= b } noexcept;
    { a >= b } noexcept;
    { a == b } noexcept;
    { a != b } noexcept;
    { a & b } noexcept;
    { a ^ b } noexcept;
    { a | b } noexcept;
  } and
  is_nothrow_constructible_v<bool, T>;
```

An `integer` whose operations throw nothing. It asks that every operation and conversion `integer_class` states over
`const` operands, and the special members a call by value spends, be `noexcept`.

### Models

- `int`, `int128`, `uint128`, `bit_int<17>`, `bit_uint<17>`, `boost::int128::int128` and `boost::int128::uint128` model
  it; `char32_t` and `bool`, which are not integers, do not; `absl::int128` and `absl::uint128` are integers and do not,
  declaring no `noexcept`.
- A cv-qualified type models it exactly when the unqualified type does, on both sides of the answer.

### Notes

- It asks for the copy, the move and the destructor that passing and returning by value spend.
- It is total: `void`, `int&`, `int[3]`, `int()`, `complex<double>` and `double` give `false` without naming an
  operator, the conjunction stopping at `integer`.
- It is the exception specification of the integer functions of `<xstd/ints/cstdlib.hpp>` and `<xstd/ints/bit.hpp>`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`integer`](integer.md) | the concept it refines |
| [`integer_class`](integer_class.md) | states the operations it asks to be `noexcept` |
| [`nothrow_alignable`](alignable.md#xstdnothrow_alignable) | the same question asked of an `alignable` type |
| [`<xstd/ints/cstdlib.hpp>`](../cstdlib/index.md), [`<xstd/ints/bit.hpp>`](../bit/index.md) | the functions whose exception specification it is |
