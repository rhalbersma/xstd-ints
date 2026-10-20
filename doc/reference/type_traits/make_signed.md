# `<xstd/ints/type_traits/make_signed.hpp>`

## `make_signed`, `make_signed_t`

- For a cv-unqualified integral or enumeration type other than `bool`, the member `type` is `std::make_signed_t<T>`.
- So `make_signed_t<char>` is `signed char`, a third type beside `char`.
- For a cv-unqualified type that models `integer_class`, is not `std::integral` and is signed, `type` is `T`.
- For a cv-qualified `T` whose unqualified type has a member `type`, `type` is that with the same qualifiers. A
  cv-qualified type takes no other path, so it has a member `type` exactly when its unqualified type does.
- Otherwise there is no member `type`, so `make_signed_t<T>` is a substitution failure: `bool` and `double` have none.
- An unsigned integer-class type that no specialization pairs has none either.
- Every unsigned exact-width type maps to a type modelling `signed_integer`.
- *Remarks:* A program may specialize it for the unsigned half of an integer-class pair, as the `ext` headers do.

## `make_signed<unsigned _BitInt(N)>`, `make_unsigned<signed _BitInt(N)>`

Each is declared, where the compiler defines `__BITINT_MAXWIDTH__`, in the header that declares its primary template.

- The member `type` is the same width with the other signedness. They are more specialized than the partial
  specialization that forwards to the standard library, so they apply on libc++, which makes `_BitInt` integral, as well
  as elsewhere.
- `make_signed<unsigned _BitInt(1)>` is an explicit specialization with no member `type`: Clang does not yet implement
  [WG14 N3747](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3747.pdf)'s `signed _BitInt(1)`. So `make_signed_t` of `unsigned _BitInt(1)`, cv-qualified or not, is a substitution
  failure, and `integer` and `unsigned_integer` are `false` for it rather than ill-formed.
- `make_unsigned<unsigned _BitInt(1)>::type` is `unsigned _BitInt(1)`, which libc++'s `std::make_unsigned` refuses, and
  through the cv-qualified specializations the qualifiers carry over.
- *Remarks:* A translation unit that can name either trait has them, whatever else it includes.
