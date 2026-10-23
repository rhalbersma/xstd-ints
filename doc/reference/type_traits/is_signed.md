# `<xstd/ints/type_traits/is_signed.hpp>`

## `xstd::is_signed`, `xstd::is_signed_v`

```cpp
template<class T>
constexpr bool is_signed_v = see below;

template<class T>
using is_signed = bool_constant<is_signed_v<T>>;
```

Whether a type is signed, for the built-in types and for integer-class types alike.

### Value

`std::is_signed_v<T>`, except for a possibly cv-qualified type that models `integer_class` and is not
`std::integral`; there, `static_cast<I>(-1) < static_cast<I>(0)` for `I` the cv-unqualified type.

### Remarks

`is_signed` is an alias, not a class template. An integer-class type reaches the trait by modelling `integer_class`,
not by specializing it.

### Notes

- It agrees with `std::is_signed_v` on `int`, `unsigned`, `double` and `void`.
- It is `true` for every signed exact-width type, the 128-bit, Boost, Abseil and bit-precise ones included, `const` or
  not, and `false` for every unsigned one.
- It is `true` for `int128` under every cv-qualification, a class type on one toolchain.
- An incomplete class type gives `false` rather than an error.

### See also

| Entity | Relation |
| :----- | :------- |
| [`is_unsigned`](is_unsigned.md) | the other half |
| [`signed_integer`](../concepts/signed_integer.md) | the concept that reads it |
| [`integer_class`](../concepts/integer_class.md) | what a class type models to reach it |
| [`std::is_signed`](https://en.cppreference.com/w/cpp/types/is_signed) | the standard trait it extends |
