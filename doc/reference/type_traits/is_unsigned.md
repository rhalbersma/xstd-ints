# `<xstd/ints/type_traits/is_unsigned.hpp>`

## `xstd::is_unsigned`, `xstd::is_unsigned_v`

```cpp
template<class T>
constexpr bool is_unsigned_v = see below;

template<class T>
using is_unsigned = bool_constant<is_unsigned_v<T>>;
```

Whether a type is unsigned, for the built-in types and for integer-class types alike.

### Value

`std::is_unsigned_v<T>`, except for a possibly cv-qualified type that models `integer_class` and is not
`std::integral`; there, `static_cast<I>(0) < static_cast<I>(-1)` for `I` the cv-unqualified type.

### Notes

- It agrees with `std::is_unsigned_v` on `int`, `unsigned`, `double` and `void`.
- It is `true` for every unsigned exact-width type, `const` or not, and `false` for every signed one.
- It is `true` for `uint128` under every cv-qualification.
- An incomplete class type gives `false` rather than an error.

### See also

| Entity | Relation |
| :----- | :------- |
| [`is_signed`](is_signed.md) | the other half |
| [`unsigned_integer`](../concepts/unsigned_integer.md) | the concept that reads it |
| [`integer_class`](../concepts/integer_class.md) | what a class type models to reach it |
| [`std::is_unsigned`](https://en.cppreference.com/w/cpp/types/is_unsigned) | the standard trait it extends |
