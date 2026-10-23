# `<xstd/ints/type_traits/make_signed.hpp>`

## `xstd::make_signed`, `xstd::make_signed_t`

```cpp
template<class T>
struct make_signed;

template<class T>
using make_signed_t = typename make_signed<T>::type;
```

The signed counterpart of an integral, enumeration or integer-class type.

### Member types

| Type | Definition |
| :--- | :--------- |
| `type` | the signed counterpart of `T`, where it has one, as below |

- For a cv-unqualified integral or enumeration type other than `bool`, the member `type` is `std::make_signed_t<T>`.
- So `make_signed_t<char>` is `signed char`, a third type beside `char`.
- For a cv-unqualified type that models `integer_class`, is not `std::integral` and is signed, `type` is `T`.
- For a cv-qualified `T` whose unqualified type has a member `type`, `type` is that with the same qualifiers. A
  cv-qualified type takes no other path, so it has a member `type` exactly when its unqualified type does.
- Otherwise there is no member `type`, so `make_signed_t<T>` is a substitution failure: `bool` and `double` have none.
- An unsigned integer-class type that no specialization pairs has none either.

### Remarks

A program may specialize it for the unsigned half of an integer-class pair, as the `ext` headers do.

### Notes

Every unsigned exact-width type maps to a type modelling `signed_integer`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`make_unsigned`](make_unsigned.md) | the other direction |
| [`integer`](../concepts/integer.md) | asks that the two be mutually inverse |
| [`boost::int128`](../boost/int128.md), [`absl::int128`](../absl/int128.md) | the `ext` headers' specializations |
| [`std::make_signed`](https://en.cppreference.com/w/cpp/types/make_signed) | the standard trait it extends |

## `xstd::make_signed<unsigned _BitInt(N)>`, `xstd::make_unsigned<signed _BitInt(N)>`

```cpp
// (1)
template<> struct make_signed<unsigned _BitInt(1)>;

// (2)
template<size_t N> struct make_signed<unsigned _BitInt(N)>;

// (3)
template<> struct make_unsigned<unsigned _BitInt(1)>;

// (4)
template<size_t N> struct make_unsigned<signed _BitInt(N)>;
```

The bit-precise specializations. Each is declared, where the compiler defines `__BITINT_MAXWIDTH__`, in the header that
declares its primary template.

1. `make_signed<unsigned _BitInt(1)>` is an explicit specialization with no member `type`: Clang does not yet implement
   [WG14 N3747](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3747.pdf)'s `signed _BitInt(1)`. So `make_signed_t`
   of `unsigned _BitInt(1)`, cv-qualified or not, is a substitution failure, and `integer` and `unsigned_integer` are
   `false` for it rather than ill-formed.
2. The member `type` is the same width with the other signedness.
3. `make_unsigned<unsigned _BitInt(1)>::type` is `unsigned _BitInt(1)`, which libc++'s `std::make_unsigned` refuses, and
   through the cv-qualified specializations the qualifiers carry over.
4. The member `type` is the same width with the other signedness.

### Remarks

A translation unit that can name either trait has them, whatever else it includes.

### Notes

They are more specialized than the partial specialization that forwards to the standard library, so they apply on
libc++, which makes `_BitInt` integral, as well as elsewhere.

### See also

| Entity | Relation |
| :----- | :------- |
| [`bit_int`, `bit_uint`](../cstdint/bit_int.md) | the aliases that name the bit-precise types |
| [`numeric_limits<signed _BitInt(N)>`](../limits/numeric_limits.md#xstdnumeric_limitssigned-_bitintn-xstdnumeric_limitsunsigned-_bitintn) | the limits of the same types |
