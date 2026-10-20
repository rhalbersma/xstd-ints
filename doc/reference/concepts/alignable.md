# `<xstd/ints/concepts/alignable.hpp>`

## `xstd::alignable`

```cpp
template<class T_cv, class T = remove_cv_t<T_cv>>
concept alignable =
  same_as<T, remove_cv_t<T_cv>> and
  numeric_limits<T>::is_specialized and
  numeric_limits<T>::is_integer and
  (numeric_limits<T>::radix == 2) and
  not numeric_limits<T>::is_signed and
  (numeric_limits<T>::digits > 1) and
  convertible_to<size_t, T> and
  constructible_from<size_t, T> and
  three_way_comparable<T, strong_ordering> and
  requires(T x) {
    static_cast<T>(x + x);
    static_cast<T>(x - x);
    static_cast<T>(x & x);
  };
```

A type the alignment functions compute in. It shares the opening clauses of `integer_class`, then adds two of its own,
unsigned and wider than one bit, and asks what alignment needs: a `size_t` in and out, totally ordered, and closed
under `+`, `-` and `&`.

### Models

- The unsigned built-ins, `size_t`, `uintptr_t`, the unsigned exact-width types, `uint128` and `unsigned _BitInt(N)`
  model it.
- Signed types do not, `ptrdiff_t`, `int128` and `signed _BitInt(24)` included, since a wrapped signed sum is undefined
  rather than a value to check; `bool` does not, having one digit; and neither do `void*` and `uint8_t*`, for which
  `numeric_limits` is not specialized.
- Every `unsigned_integer` models it and no `signed_integer` does, while `char8_t`, `char16_t` and `char32_t` model it
  without being integers.
- A cv-qualified type models it exactly when the unqualified type does.
- `absl::uint128` models it.
- An explicit second argument other than the default is turned away by the first clause.

### See also

| Entity | Relation |
| :----- | :------- |
| [`nothrow_alignable`](#xstdnothrow_alignable) | the refinement whose operations are all `noexcept` |
| [`integer_class`](integer_class.md) | the concept whose opening clauses it shares |
| [`unsigned_integer`](unsigned_integer.md) | every model of it is `alignable` |
| [`align_up`](../memory/align_up.md), [`align_down`](../memory/align_down.md), [`is_aligned`](../memory/is_aligned.md) | constrain `T` on it |

## `xstd::nothrow_alignable`

```cpp
template<class T_cv, class T = remove_cv_t<T_cv>>
concept nothrow_alignable =
  same_as<T, remove_cv_t<T_cv>> and
  alignable<T> and
  is_nothrow_destructible_v<T> and
  is_nothrow_move_constructible_v<T> and
  is_nothrow_copy_constructible_v<T> and
  is_nothrow_constructible_v<T, size_t> and
  is_nothrow_constructible_v<size_t, T> and
  requires(T x) {
    { static_cast<T>(x + x) } noexcept;
    { static_cast<T>(x - x) } noexcept;
    { static_cast<T>(x & x) } noexcept;
  } and
  requires(const T a, const T b) {
    { a <=> b } noexcept;
    { a == b } noexcept;
    { a != b } noexcept;
    { a < b } noexcept;
    { a > b } noexcept;
    { a <= b } noexcept;
    { a >= b } noexcept;
  };
```

An `alignable` type whose conversions, operations and relations, and the special members a call by value spends, are
all `noexcept`.

### Models

- `unsigned char`, `unsigned`, `size_t` and `uint128` model it, cv-qualified too.
- `absl::uint128`, whose operations declare no `noexcept`, is `alignable` and does not model it.

### Notes

It asks the special members a call by value spends and every relation, not only the ones the alignment functions use.

### See also

| Entity | Relation |
| :----- | :------- |
| [`alignable`](#xstdalignable) | the concept it refines |
| [`nothrow_const_operators`](nothrow_const_operators.md) | the same question asked of an `integer` |
| [`align_up`](../memory/align_up.md), [`align_down`](../memory/align_down.md), [`is_aligned`](../memory/is_aligned.md) | are `noexcept` exactly where `T` models it |
