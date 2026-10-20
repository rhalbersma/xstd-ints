# `<xstd/ints/type_traits/make_unsigned.hpp>`

## `make_unsigned`, `make_unsigned_t`

- For a cv-unqualified integral or enumeration type other than `bool`, the member `type` is `std::make_unsigned_t<T>`.
- For a cv-unqualified type that models `integer_class`, is not `std::integral` and is unsigned, `type` is `T`.
- For a cv-qualified `T` whose unqualified type has a member `type`, `type` is that with the same qualifiers. A
  cv-qualified type takes no other path, so it has a member `type` exactly when its unqualified type does.
- Otherwise there is no member `type`, so `make_unsigned_t<T>` is a substitution failure where `std::make_unsigned_t<T>`
  would be a hard error: `bool`, `double`, `int*`, `complex<double>` and `void` have none.
- Every signed exact-width type maps to a type modelling `unsigned_integer`.
- *Remarks:* A program may specialize it for the signed half of an integer-class pair, as the `ext` headers do.
