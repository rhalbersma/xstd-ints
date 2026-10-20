# `<xstd/ints/type_traits/is_unsigned.hpp>`

## `is_unsigned_v`, `is_unsigned`

The value is `std::is_unsigned_v<T>`, except for a possibly cv-qualified type that models `integer_class` and is
not `std::integral`; there, `static_cast<I>(0) < static_cast<I>(-1)` for `I` the cv-unqualified type.

- It agrees with `std::is_unsigned_v` on `int`, `unsigned`, `double` and `void`.
- It is `true` for every unsigned exact-width type, `const` or not, and `false` for every signed one.
- It is `true` for `uint128` under every cv-qualification.
- An incomplete class type gives `false` rather than an error.
