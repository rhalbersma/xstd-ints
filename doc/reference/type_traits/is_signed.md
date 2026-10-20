# `<xstd/ints/type_traits/is_signed.hpp>`

## `is_signed_v`, `is_signed`

The value is `std::is_signed_v<T>`, except for a possibly cv-qualified type that models `integer_class` and is
not `std::integral`; there, `static_cast<I>(-1) < static_cast<I>(0)` for `I` the cv-unqualified type.

- It agrees with `std::is_signed_v` on `int`, `unsigned`, `double` and `void`.
- It is `true` for every signed exact-width type, the 128-bit, Boost, Abseil and bit-precise ones included, `const` or
  not, and `false` for every unsigned one.
- It is `true` for `int128` under every cv-qualification, a class type on one toolchain.
- An incomplete class type gives `false` rather than an error.
- *Remarks:* `is_signed` is an alias, not a class template. An integer-class type reaches the trait by modelling
  `integer_class`, not by specializing it.
