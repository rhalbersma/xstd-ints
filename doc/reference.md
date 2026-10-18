# Reference

Each public entity of xstd-ints is specified here the way the standard specifies the library: a synopsis per header,
then for each entity the elements of [structure.specifications] that apply. The baseline is
[C++23](https://wg21.link/N4950). A facility from a later working draft or a proposal cites the paper it follows, and a
name with no namespace is in `std`.

The synopses leave out `[[nodiscard]]`, as the standard's own have since [P2422R1](https://wg21.link/p2422r1), which
made it a recommended practice rather than part of a library's specification. The code carries it on every function that
returns a value, so discarding a result here draws a warning. *Untested:* the attribute changes only diagnostics, which
no test can observe, so the repository's code conventions are its only authority.

Every element names the test that holds the code to it, as `test-file: TestCase` under `test/src/ints/`, or `ints.cpp`
for the front door's own test in `test/src/`. A check that is not a Boost.Test case, such as a header
self-sufficiency build, a codegen test or a proof, is named as *Checked by*. An element that no test can hold names why
instead: *Definition only* where no existing type makes the mistake it turns away, which the repository's test rules
then leave to the definition, or *Untested* with the reason. An element that disagrees with the code is a defect in one
of the two.

## `<xstd/ints/concepts.hpp>`

```cpp
namespace xstd {
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept integer_class = see below;
  template<class T>
    concept integer = see below;
  template<class T>
    concept signed_integer = see below;
  template<class T>
    concept unsigned_integer = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept bit_mask = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept alignable = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept nothrow_alignable = see below;
  template<class T_cv, class T = remove_cv_t<T_cv>>
    concept nothrow_const_operators = see below;
}
```

In the definitions below, `numeric_limits`, `promoted_t`, `make_signed_t`, `make_unsigned_t`, `is_signed_v` and
`is_unsigned_v` are xstd's.

The umbrella exports the seven headers under `<xstd/ints/concepts/>`. Over the built-in integers, the 128-bit pair and
the near-misses `bool`, `char`, `char32_t`, `double`, `void`, `int*`, `int&` and `complex<double>`, cv-qualified or not:
`integer` holds exactly where one of its two halves does and never both; `integer` implies `integer_class`;
`nothrow_const_operators` implies `integer`; every `unsigned_integer` is `alignable` and no `signed_integer` is; and
`nothrow_alignable` implies `alignable`.
*Test:* `concepts.cpp: TheRefinementsHoldOverTheWholeDomain`, `TheRefinementsSurviveCvQualification`.

### `integer_class`

```cpp
template<class T_cv, class T = remove_cv_t<T_cv>>
concept integer_class =
  same_as<T, remove_cv_t<T_cv>> and
  numeric_limits<T>::is_specialized and
  numeric_limits<T>::is_integer and
  (numeric_limits<T>::radix == 2) and
  convertible_to<int, T> and
  convertible_to<size_t, T> and
  constructible_from<int, T> and
  constructible_from<size_t, T> and
  requires(T a) {
    { a++ } -> same_as<T>;
    { a-- } -> same_as<T>;
  } and
  requires(T a) {
    { ++a } -> same_as<T&>;
    { --a } -> same_as<T&>;
  } and
  requires(const T a) {
    { +a } -> same_as<promoted_t<T>>;
    { -a } -> same_as<promoted_t<T>>;
    { !a } -> same_as<bool>;
    { ~a } -> same_as<promoted_t<T>>;
  } and
  requires(T a, const T b) {
    { a *= b } -> same_as<T&>;
    { a /= b } -> same_as<T&>;
    { a %= b } -> same_as<T&>;
    { a += b } -> same_as<T&>;
    { a -= b } -> same_as<T&>;
  } and
  requires(T a, const size_t n) {
    { a <<= n } -> same_as<T&>;
    { a >>= n } -> same_as<T&>;
  } and
  requires(T a, const T b) {
    { a &= b } -> same_as<T&>;
    { a ^= b } -> same_as<T&>;
    { a |= b } -> same_as<T&>;
  } and
  requires(const T a, const T b) {
    { a * b } -> same_as<promoted_t<T>>;
    { a / b } -> same_as<promoted_t<T>>;
    { a % b } -> same_as<promoted_t<T>>;
    { a + b } -> same_as<promoted_t<T>>;
    { a - b } -> same_as<promoted_t<T>>;
  } and
  requires(const T a, const size_t n) {
    { a << n } -> same_as<promoted_t<T>>;
    { a >> n } -> same_as<promoted_t<T>>;
  } and
  requires(const T a, const T b) {
    { a & b } -> same_as<promoted_t<T>>;
    { a ^ b } -> same_as<promoted_t<T>>;
    { a | b } -> same_as<promoted_t<T>>;
  } and
  constructible_from<bool, T> and
  regular<T> and
  three_way_comparable<T, strong_ordering> and
  requires {
    { numeric_limits<T>::min() } -> same_as<T>;
    { numeric_limits<T>::max() } -> same_as<T>;
  };
```

The definition follows the order of [iterator.concept.winc], generalized to integers of any width and opened to
built-in types, which that subclause's note admits.

- Every standard integer type models it, the five character types included, and narrow types qualify because the
  closure requirements are stated against `promoted_t`.
  *Test:* `concepts/integer_class.cpp: StandardIntegral`.
- Every exact-width type models it, the 128-bit, Boost, Abseil and bit-precise ones included, under every
  cv-qualification. *Test:* `concepts/integer_class.cpp: AdmitsTheExactWidthTypesThroughCv`.
- `bool` does not: C++17 removed its `++` and `--`. *Test:* `concepts/integer_class.cpp: StandardIntegral`.
- `byte` does not: it has every bitwise operator an integer has, and no arithmetic and no `numeric_limits`.
  *Test:* `concepts/integer_class.cpp: TheByteIsNotAnIntegerClass`.
- A type models it only if its range is the one [iterator.concept.winc]/3 gives a width: `min() + max()` is −1 for
  the signed half of a pair, and the unsigned half runs from 0 to twice the signed maximum plus one, with one digit
  more.
  *Test:* `concepts/integer_class.cpp: RepresentableRange`, over the signed exact-width types and their pairs.
- A type models it only if value-initialization produces zero, [iterator.concept.winc]/10: it does for every signed
  exact-width type and its unsigned counterpart in constant evaluation, Abseil's pair where it has a 128-bit intrinsic.
  *Test:* `concepts/integer_class.cpp: ValueInitializationIsZero`.
- *Remarks:* It does not ask for the width requirement of /3, mixed integral and integer-class compound assignment
  (/7.4) or the common types of /12. *Definition only:* an omission admits types, and no model in the test matrix
  lacks what is omitted.

### `integer`

```cpp
template<class T>
concept integer =
  integer_class<T> and
  requires {
    typename make_signed_t<T>;
    typename make_unsigned_t<T>;
  } and
  same_as<make_signed_t<make_unsigned_t<T>>, make_signed_t<T>> and
  same_as<make_unsigned_t<make_signed_t<T>>, make_unsigned_t<T>> and
  (same_as<T, make_signed_t<T>> or same_as<T, make_unsigned_t<T>>);
```

It is [P3701R0](https://wg21.link/P3701R0)'s arithmetic boundary over the built-ins, extended to integer-class
types that a specialization of `make_signed` and `make_unsigned` pairs.

- Every exact-width type models it, the 128-bit, Boost, Abseil and bit-precise ones included, under every
  cv-qualification. *Test:* `concepts/integer.cpp: AdmitsTheExactWidthTypesThroughCv`.
- `signed char`, `unsigned char`, `int8_t` and `uint8_t` model it; `char`, `wchar_t`, `char8_t`, `char16_t`,
  `char32_t` and `bool` do not under any cv-qualification, and neither do `double`, `void` and `int*`.
  *Test:* `concepts/integer.cpp: RejectsTheCharacterAndNonIntegralTypes`.
- The last clause is what turns the character types away, none being named: a character type is neither half of its
  own pair, `make_signed_t<char>` being `signed char`. *Test:* `concepts/integer.cpp: SignedPairing`.
- `bool` fails `integer_class` and never reaches the pair.
  *Test:* `concepts/integer.cpp: RejectsTheCharacterAndNonIntegralTypes`.
- `unsigned _BitInt(1)`, which has no signed counterpart, does not model it, and asking is not an error.
  *Test:* `cstdint/bit_int.cpp: OneBitHasNoSignedCounterpart`.
- A trip through the other signedness and back is the identity.
  *Test:* `concepts/integer.cpp: SignedRoundTrip`, `UnsignedRoundTrip`.
- An integer-class type that no specialization pairs does not model it.
  *Untested:* every test reaches Boost's and Abseil's types through the headers that pair them.
- A pair whose round trip lands on a third type is turned away by the two `same_as` clauses that compose the
  transformations. *Definition only:* no existing type has one.

### `signed_integer`

```cpp
template<class T>
concept signed_integer = integer<T> and is_signed_v<T>;
```

- Every signed exact-width type models it under every cv-qualification, and no unsigned one does.
  *Test:* `concepts/signed_integer.cpp: AdmitsTheExactWidthTypesThroughCv`, `UnsignedIsNotSigned`.
- *Remarks:* It subsumes `integer<T>`, which it names as an atomic conjunct, as `std::signed_integral` subsumes
  `std::integral`. An overload on `signed_integer` is chosen over one on `integer` for `int`, not ambiguous with it.
  *Test:* `concepts/signed_integer.cpp: SubsumesInteger`.

### `unsigned_integer`

```cpp
template<class T>
concept unsigned_integer = integer<T> and is_unsigned_v<T>;
```

- Every unsigned exact-width type models it under every cv-qualification, and no signed one does.
  *Test:* `concepts/unsigned_integer.cpp: AdmitsTheExactWidthTypesThroughCv`, `SignedIsNotUnsigned`.
- *Remarks:* It subsumes `integer<T>`, as `std::unsigned_integral` subsumes `std::integral`. An overload on
  `unsigned_integer` is chosen over one on `integer` for `unsigned`, not ambiguous with it.
  *Test:* `concepts/unsigned_integer.cpp: SubsumesInteger`.

### `bit_mask`

```cpp
template<class T_cv, class T = remove_cv_t<T_cv>>
concept bit_mask =
  same_as<T, remove_cv_t<T_cv>> and
  (unsigned_integer<T> or (not signed_integer<T> and not integral<T>)) and
  regular<T> and
  requires(const T a) {
    { ~a } -> same_as<promoted_t<T>>;
  } and
  requires(T a, const T b) {
    a &= b;
    a ^= b;
    a |= b;
  } and
  requires(const T a, const T b) {
    { a & b } -> same_as<promoted_t<T>>;
    { a ^ b } -> same_as<promoted_t<T>>;
    { a | b } -> same_as<promoted_t<T>>;
  };
```

It is the bitmask type of [bitmask.types]: an unsigned integer, or any type that is no kind of integer and has the
operators.

- Every unsigned exact-width type models it under every cv-qualification, and so does every unsigned built-in, the
  narrow ones through promotion.
  *Test:* `concepts/bit_mask.cpp: AdmitsTheUnsignedExactWidthTypesThroughCv`, `TheIntegerMembersAreTheUnsignedIntegers`.
- No signed exact-width type models it, `const` or not, nor `signed char`, `int` or `long long`, whose `~` is negative.
  *Test:* `concepts/bit_mask.cpp: RejectsTheSignedExactWidthTypesThroughCv`, `TheIntegerMembersAreTheUnsignedIntegers`.
- `bool` and the five character types do not, cv-qualified or not, although they answer every operator clause
  through promotion: `integral<T>` turns them away.
  *Test:* `concepts/bit_mask.cpp: BoolAndTheCharacterTypesAreRefusedThroughCv`,
  `TheIntegerMembersAreTheUnsignedIntegers`.
- `bitset<N>` models it, `const` too. *Test:* `concepts/bit_mask.cpp: StdBitsetIsOne`.
- The standard's own bitmask enumerations model it: `filesystem::perms`, `perm_options`, `copy_options`,
  `directory_options`, `launch`, `chars_format`, `regex_constants::syntax_option_type` and `match_flag_type`.
  *Test:* `concepts/bit_mask.cpp: TheStandardBitmaskTypesAreAdmitted`.
- An enumeration is one by its operators alone: `byte`, which the standard never calls a bitmask type, models it,
  and `filesystem::file_type` and `regex_constants::error_type`, enumerated types declared beside bitmask types, do
  not. *Test:* `concepts/bit_mask.cpp: AnEnumerationIsOneByItsOperators`.
- The compound forms need only be valid, whatever they return, so libstdc++'s `ios_base` flag types, whose compound
  assignments return `const X&`, model it where they are not signed integers.
  *Test:* `concepts/bit_mask.cpp: TheCompoundFormsNeedOnlyBeValid`.
- Over the built-in types it admits exactly what `popcount`, `countr_zero`, `rotl` and `bit_width` accept.
  *Test:* `concepts/bit_mask.cpp: OverTheBuiltInsThisIsExactlyTheDomainOfBit`.
- No ordering is asked for: `bitset`, which has no `operator<`, models it.
  *Test:* `concepts/bit_mask.cpp: NoOrderingIsRequired`.
- No shifts are asked for: the standard's bitmask enumerations, which have none, model it.
  *Test:* `concepts/bit_mask.cpp: TheStandardBitmaskTypesAreAdmitted`.

### `alignable`

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

It shares the opening clauses of `integer_class`, then adds two of its own, unsigned and wider than one bit, and asks
what alignment needs: a `size_t` in and out, totally ordered, and closed under `+`, `-` and `&`.

- The unsigned built-ins, `size_t`, `uintptr_t`, the unsigned exact-width types, `uint128` and `unsigned _BitInt(N)`
  model it. *Test:* `concepts/alignable.cpp: AdmitsTheUnsignedIntegers`.
- Signed types do not, `ptrdiff_t`, `int128` and `signed _BitInt(24)` included, since a wrapped signed sum is
  undefined rather than a value to check; `bool` does not, having one digit; and neither do `void*` and `uint8_t*`,
  for which `numeric_limits` is not specialized. *Test:* `concepts/alignable.cpp: AndRejectsTheRestOnTheirMerits`.
- Every `unsigned_integer` models it and no `signed_integer` does, while `char8_t`, `char16_t` and `char32_t` model it
  without being integers. *Test:* `concepts/alignable.cpp: IsIntegerLight`.
- A cv-qualified type models it exactly when the unqualified type does.
  *Test:* `concepts/alignable.cpp: IsCvTransparent`.
- `absl::uint128` models it. *Test:* `concepts/alignable.cpp: TheNothrowRefinementNarrowsIt`.
- An explicit second argument other than the default is turned away by the first clause.
  *Definition only:* no user names the second parameter except by mistake, and no existing type is involved.

### `nothrow_alignable`

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

- `unsigned char`, `unsigned`, `size_t` and `uint128` model it, cv-qualified too.
  *Test:* `concepts/alignable.cpp: TheNothrowRefinementCoversEveryRelation`, `TheNothrowRefinementNarrowsIt`,
  `IsCvTransparent`.
- `absl::uint128`, whose operations declare no `noexcept`, is `alignable` and does not model it.
  *Test:* `concepts/alignable.cpp: TheNothrowRefinementNarrowsIt`.
- It asks the special members a call by value spends and every relation, not only the ones the alignment functions
  use. *Test:* `concepts/alignable.cpp: TheNothrowRefinementCoversWhatTheCallSpends`,
  `TheNothrowRefinementCoversEveryRelation`.

### `nothrow_const_operators`

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

It asks that every operation and conversion `integer_class` states over `const` operands, and the special members
a call by value spends, be `noexcept`.

- `int`, `int128`, `uint128`, `bit_int<17>`, `bit_uint<17>`, `boost::int128::int128` and `boost::int128::uint128`
  model it; `char32_t` and `bool`, which are not integers, do not; `absl::int128` and `absl::uint128` are integers and
  do not, declaring no `noexcept`. *Test:* `concepts/nothrow_const_operators.cpp: HoldsForTheTypesThatDeclareIt`.
- It asks for the copy, the move and the destructor that passing and returning by value spend.
  *Test:* `concepts/nothrow_const_operators.cpp: IncludesWhatPassingAndReturningCosts`.
- A cv-qualified type models it exactly when the unqualified type does, on both sides of the answer.
  *Test:* `concepts/nothrow_const_operators.cpp: IsCvTransparentOnBothBranches`.
- It is total: `void`, `int&`, `int[3]`, `int()`, `complex<double>` and `double` give `false` without naming an
  operator, the conjunction stopping at `integer`. *Test:* `concepts/nothrow_const_operators.cpp: IsTotal`.
- It is the exception specification of the integer functions of `<xstd/ints/cstdlib.hpp>` and `<xstd/ints/bit.hpp>`.
  *Test:* `concepts/nothrow_const_operators.cpp: IsTheExceptionSpecification`; `bit/bit_floor.cpp:
  NoexceptFollowsTheOperators`.

## `<xstd/ints/type_traits.hpp>`

```cpp
namespace xstd {
  template<class T>
    constexpr bool is_signed_v = see below;
  template<class T>
    using is_signed = bool_constant<is_signed_v<T>>;
  template<class T>
    constexpr bool is_unsigned_v = see below;
  template<class T>
    using is_unsigned = bool_constant<is_unsigned_v<T>>;

  template<class T>
    struct make_signed;
  template<class T>
    using make_signed_t = typename make_signed<T>::type;
  template<class T>
    struct make_unsigned;
  template<class T>
    using make_unsigned_t = typename make_unsigned<T>::type;

  // where __BITINT_MAXWIDTH__ is defined
  template<> struct make_signed<unsigned _BitInt(1)>;
  template<size_t N> struct make_signed<unsigned _BitInt(N)>;
  template<> struct make_unsigned<unsigned _BitInt(1)>;
  template<size_t N> struct make_unsigned<signed _BitInt(N)>;

  template<class T>
    using promoted_t = see below;
}
```

The umbrella exports the five headers under `<xstd/ints/type_traits/>`. Over every exact-width type, an integer is in
exactly one half, each modifier lands in the half it names and the two are mutually inverse, and promotion keeps a
signed type signed. *Test:* `type_traits.cpp: TheTraitsAndTheModifiersAgree`, `TheTraitsAndTheModifiersAgreeUnsigned`,
`PromotionKeepsASignedTypeSigned`.

### `is_signed_v`, `is_signed`

The value is `std::is_signed_v<T>`, except for a possibly cv-qualified type that models `integer_class` and is
not `std::integral`; there, `static_cast<I>(-1) < static_cast<I>(0)` for `I` the cv-unqualified type.

- It agrees with `std::is_signed_v` on `int`, `unsigned`, `double` and `void`.
  *Test:* `type_traits/is_signed.cpp: AgreesWithStd`.
- It is `true` for every signed exact-width type, the 128-bit, Boost, Abseil and bit-precise ones included, `const`
  or not, and `false` for every unsigned one.
  *Test:* `type_traits/is_signed.cpp: ExactWidthIntegers`, `OppositeSignExactWidthIntegers`.
- It is `true` for `int128` under every cv-qualification, a class type on one toolchain.
  *Test:* `cstdint/int128.cpp: Signedness`.
- An incomplete class type gives `false` rather than an error. *Test:* `type_traits/is_signed.cpp:
  AnswersForIncompleteTypes`.
- *Remarks:* `is_signed` is an alias, not a class template. An integer-class type reaches the trait by modelling
  `integer_class`, not by specializing it. *Definition only:* a test defines no type to show it.

### `is_unsigned_v`, `is_unsigned`

The value is `std::is_unsigned_v<T>`, except for a possibly cv-qualified type that models `integer_class` and is
not `std::integral`; there, `static_cast<I>(0) < static_cast<I>(-1)` for `I` the cv-unqualified type.

- It agrees with `std::is_unsigned_v` on `int`, `unsigned`, `double` and `void`.
  *Test:* `type_traits/is_unsigned.cpp: AgreesWithStd`.
- It is `true` for every unsigned exact-width type, `const` or not, and `false` for every signed one.
  *Test:* `type_traits/is_unsigned.cpp: ExactWidthIntegers`, `OppositeSignExactWidthIntegers`.
- It is `true` for `uint128` under every cv-qualification. *Test:* `cstdint/int128.cpp: Signedness`.
- An incomplete class type gives `false` rather than an error.
  *Test:* `type_traits/is_unsigned.cpp: AnswersForIncompleteTypes`.

### `make_signed`, `make_signed_t`

- For a cv-unqualified integral or enumeration type other than `bool`, the member `type` is `std::make_signed_t<T>`.
  *Test:* `type_traits/make_signed.cpp: MirrorsTheUnsignedHalf`.
- So `make_signed_t<char>` is `signed char`, a third type beside `char`.
  *Test:* `concepts/integer.cpp: SignedPairing`.
- For a cv-unqualified type that models `integer_class`, is not `std::integral` and is signed, `type` is `T`.
  *Test:* `ext/boost/int128.cpp: AssociationsPairTheTwoHalves`; `ext/absl/int128.cpp: AssociationsPairTheTwoHalves`.
- For a cv-qualified `T` whose unqualified type has a member `type`, `type` is that with the same qualifiers. A
  cv-qualified type takes no other path, so it has a member `type` exactly when its unqualified type does.
  *Test:* `type_traits/make_signed.cpp: MirrorsTheUnsignedHalf`; `cstdint/int128.cpp:
  AssociatesSignedAndUnsignedPair`; `cstdint/bit_int.cpp: OneBitHasNoSignedCounterpart`.
- Otherwise there is no member `type`, so `make_signed_t<T>` is a substitution failure: `bool` and `double` have none.
  *Test:* `type_traits/make_signed.cpp: MirrorsTheUnsignedHalf`.
- An unsigned integer-class type that no specialization pairs has none either.
  *Untested:* every test reaches Boost's and Abseil's types through the headers that pair them.
- Every unsigned exact-width type maps to a type modelling `signed_integer`.
  *Test:* `type_traits/make_signed.cpp: ExactWidthIntegers`.
- *Remarks:* A program may specialize it for the unsigned half of an integer-class pair, as the `ext` headers do.
  *Test:* `ext/boost/int128.cpp: AssociationsPairTheTwoHalves`; `ext/absl/int128.cpp: AssociationsPairTheTwoHalves`.

### `make_unsigned`, `make_unsigned_t`

- For a cv-unqualified integral or enumeration type other than `bool`, the member `type` is `std::make_unsigned_t<T>`.
  *Test:* `type_traits/make_unsigned.cpp: MirrorsTheSignedHalf`.
- For a cv-unqualified type that models `integer_class`, is not `std::integral` and is unsigned, `type` is `T`.
  *Test:* `ext/boost/int128.cpp: AssociationsPairTheTwoHalves`; `ext/absl/int128.cpp: AssociationsPairTheTwoHalves`.
- For a cv-qualified `T` whose unqualified type has a member `type`, `type` is that with the same qualifiers. A
  cv-qualified type takes no other path, so it has a member `type` exactly when its unqualified type does.
  *Test:* `type_traits/make_unsigned.cpp: MirrorsTheSignedHalf`; `cstdint/int128.cpp:
  AssociatesSignedAndUnsignedPair`; `cstdint/bit_int.cpp: OneBitHasNoSignedCounterpart`.
- Otherwise there is no member `type`, so `make_unsigned_t<T>` is a substitution failure where
  `std::make_unsigned_t<T>` would be a hard error: `bool`, `double`, `int*`, `complex<double>` and `void` have none.
  *Test:* `type_traits/make_unsigned.cpp: MirrorsTheSignedHalf`.
- Every signed exact-width type maps to a type modelling `unsigned_integer`.
  *Test:* `type_traits/make_unsigned.cpp: ExactWidthIntegers`.
- *Remarks:* A program may specialize it for the signed half of an integer-class pair, as the `ext` headers do.
  *Test:* `ext/boost/int128.cpp: AssociationsPairTheTwoHalves`; `ext/absl/int128.cpp: AssociationsPairTheTwoHalves`.

### `make_signed<unsigned _BitInt(N)>`, `make_unsigned<signed _BitInt(N)>`

Each is declared, where the compiler defines `__BITINT_MAXWIDTH__`, in the header that declares its primary template.

- The member `type` is the same width with the other signedness. They are more specialized than the partial
  specialization that forwards to the standard library, so they apply on libc++, which makes `_BitInt` integral, as
  well as elsewhere.
  *Test:* `cstdint/bit_int.cpp: AliasesModelTheIntegerConcepts`; `cstdint.cpp: EveryTypeNamedHereIsHalfOfAPair`.
- `make_signed<unsigned _BitInt(1)>` is an explicit specialization with no member `type`: Clang does not yet implement
  N3747's `signed _BitInt(1)`. So `make_signed_t` of `unsigned _BitInt(1)`, cv-qualified or not, is a substitution
  failure, and `integer` and `unsigned_integer` are `false` for it rather than ill-formed.
  *Test:* `cstdint/bit_int.cpp: OneBitHasNoSignedCounterpart`.
- `make_unsigned<unsigned _BitInt(1)>::type` is `unsigned _BitInt(1)`, which libc++'s `std::make_unsigned` refuses,
  and through the cv-qualified specializations the qualifiers carry over.
  *Test:* `cstdint/bit_int.cpp: OneBitHasNoSignedCounterpart`.
- *Remarks:* A translation unit that can name either trait has them, whatever else it includes.
  *Untested:* each test source that asks either trait of a `_BitInt` also reaches `<xstd/ints/cstdint/bit_int.hpp>`.

### `promoted_t`

- `promoted_t<T>` is the type of `+a` for an lvalue `a` of type `const T` where `T` is `std::integral`, which is what
  [conv.prom] gives, and `T` otherwise. Narrow built-ins promote to `int`, or to `int` or `unsigned` as the platform
  decides, `bool` to `int`, and `int` and wider are their own; a cv-qualified type promotes as the unqualified type
  does, `short const` to `int`. *Test:* `type_traits/promoted.cpp: MatchesTheIntegralPromotions`.
- A non-integral type's own `operator+` is never asked: `decltype(+a)` is formed only for an integral `T`.
  *Test:* `type_traits/promoted.cpp: MatchesTheIntegralPromotions`, over `type_identity<void>`.
- For an integer-class type it is the type itself, which the closure requirements of `integer_class` rely on, and so
  it is for a bit-precise type, which is exempt from integral promotion whichever branch it takes: `uint128`,
  `absl::uint128`, `bit_uint<17>` and `bit_int<9>` are their own.
  *Test:* `type_traits/promoted.cpp: LeavesTheWideTypesAlone`;
  `concepts/integer_class.cpp: AdmitsTheExactWidthTypesThroughCv`.
- It keeps every signed exact-width type signed. *Test:* `type_traits.cpp: PromotionKeepsASignedTypeSigned`.

## `<xstd/ints/limits.hpp>`

```cpp
namespace xstd {
  template<class T>
    struct numeric_limits {};
  template<class T>
    requires (is_object_v<T> and not is_array_v<T>)
    struct numeric_limits<T> : std::numeric_limits<T> {};
  template<class T>
    struct numeric_limits<const T> : numeric_limits<T> {};
  template<class T>
    struct numeric_limits<volatile T> : numeric_limits<T> {};
  template<class T>
    struct numeric_limits<const volatile T> : numeric_limits<T> {};

  // where __BITINT_MAXWIDTH__ is defined
  template<size_t N> struct numeric_limits<signed _BitInt(N)>;
  template<size_t N> struct numeric_limits<unsigned _BitInt(N)>;
}
```

The umbrella exports `<xstd/ints/limits/numeric_limits.hpp>`.
*Test:* `limits.cpp: TheTraitArrivesThroughTheUmbrella`.

### `numeric_limits`

- For an object type that is not an array, it derives from `std::numeric_limits<T>` and so gives the same answers;
  for a cv-qualified type it derives from the specialization for the unqualified type.
  *Test:* `limits/numeric_limits.cpp: DelegatesToTheStandardDomain`.
- For `void`, a reference, an array, a cv-qualified array and a function type it is an empty class, so naming a
  member is a substitution failure rather than an error inside `std::numeric_limits`.
  *Test:* `limits/numeric_limits.cpp: SaysNothingOutsideThatDomain`.
- The cv-qualified specializations forward to a specialization of xstd's own, not only to the standard library's:
  `bit_int<17> const` and `signed _BitInt(24) const` read the `_BitInt` digits.
  *Test:* `cstdint.cpp: ThePairingCarriesCvQualification`;
  `limits/numeric_limits.cpp: DescribesABitPreciseTypeWithNoOtherHeader`.
- *Remarks:* A program may specialize it for a type of its own; the cv-qualified forms then follow.
  *Definition only:* the library's own `_BitInt` specializations are its one instance, and a test defines no type to
  specialize it for.

### `numeric_limits<signed _BitInt(N)>`, `numeric_limits<unsigned _BitInt(N)>`

```cpp
template<size_t N>
struct numeric_limits<signed _BitInt(N)> : std::numeric_limits<signed _BitInt(N)> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = false;
  static constexpr int digits = N - 1;
  static constexpr int digits10 = see below;
  static constexpr int radix = 2;
  static constexpr auto min() noexcept -> signed _BitInt(N);     // -max() - 1
  static constexpr auto max() noexcept -> signed _BitInt(N);     // 2^(N-1) - 1
  static constexpr auto lowest() noexcept -> signed _BitInt(N);  // min()
};

template<size_t N>
struct numeric_limits<unsigned _BitInt(N)> : std::numeric_limits<unsigned _BitInt(N)> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = true;
  static constexpr int digits = N;
  static constexpr int digits10 = see below;
  static constexpr int radix = 2;
  static constexpr auto min() noexcept -> unsigned _BitInt(N);     // 0
  static constexpr auto max() noexcept -> unsigned _BitInt(N);     // 2^N - 1
  static constexpr auto lowest() noexcept -> unsigned _BitInt(N);  // min()
};
```

Here `numeric_limits` is xstd's, and the base is the standard library's. Both are declared, where the compiler defines
`__BITINT_MAXWIDTH__`, in the header that declares the primary template.

- A translation unit that can name `xstd::numeric_limits` has them, whatever else it includes: `_BitInt` is a keyword,
  so a program can ask about one with no other xstd header, and gets the answer it would get with every header.
  *Test:* `limits/numeric_limits.cpp: DescribesABitPreciseTypeWithNoOtherHeader`.
- `digits`, `min()` and `max()` follow the requested width, not the object's size, and are constant expressions:
  at 17 bits, 16 and 17 digits, a signed range of −65536 to 65535 and an unsigned maximum of 131071. On libc++, which
  specializes `std::numeric_limits` for `_BitInt` with `digits` taken from `sizeof`, these replace its values.
  *Test:* `cstdint/bit_int.cpp: LimitsFollowTheRequestedWidth`.
- `digits10` is `floor(digits * log10(2))`, the most decimal digits the type represents without change: 7 for
  `unsigned _BitInt(24)` and 6 for `signed _BitInt(24)`, and at 8 and 64 bits the value `std::numeric_limits` gives
  the exact-width type of that width. It replaces libstdc++'s zero and libc++'s value for the width of the object.
  *Test:* `cstdint/bit_int.cpp: Digits10CountsTheDecimalDigitsEveryValueHas`;
  `limits/numeric_limits.cpp: DescribesABitPreciseTypeWithNoOtherHeader`.
- *Remarks:* `digits10` is computed in 64.64 fixed point, which is exact for every `digits` up to 2^23, and the
  header is ill-formed where `__BITINT_MAXWIDTH__` exceeds 8'388'608. *Untested:* the cases read three widths, and
  every tested Clang defines exactly that maximum, so the `static_assert` never fires.
- `is_specialized`, `is_integer` and `radix == 2` hold, as `integer_class` requires.
  *Test:* `cstdint/bit_int.cpp: AliasesModelTheIntegerConcepts`.
- `is_signed` follows the type's signedness, `is_exact` and `is_bounded` are `true`, `is_modulo` is `false` for the
  signed type and `true` for the unsigned, and `lowest()` is `min()`.
  *Test:* `cstdint/bit_int.cpp: LimitsFollowTheRequestedWidth`.
- `max_digits10` is inherited, and is 0, as for every integer.
  *Test:* `cstdint/bit_int.cpp: Digits10CountsTheDecimalDigitsEveryValueHas`.
- *Remarks:* Every other member is inherited from `std::numeric_limits<_BitInt(N)>`: the primary template's where the
  standard library does not specialize it, as libstdc++ does not, and libc++'s where it does.
  *Untested:* no case reads another inherited member.

## `<xstd/ints/cstdint.hpp>`

```cpp
#define XSTD_HAS_INT128 see below
#define XSTD_HAS_BIT_INT see below

namespace xstd {
  // where XSTD_HAS_INT128 is defined
  using int128 = see below;
  using uint128 = see below;
  template<> struct make_unsigned<int128>;
  template<> struct make_signed<uint128>;

  // where XSTD_HAS_BIT_INT is defined
  inline constexpr size_t bit_int_max_width = __BITINT_MAXWIDTH__;
  template<size_t N>
    requires (2 <= N and N <= bit_int_max_width)
    using bit_int = signed _BitInt(N);
  template<size_t N>
    requires (2 <= N and N <= bit_int_max_width)
    using bit_uint = unsigned _BitInt(N);
}
```

The umbrella exports `<xstd/ints/cstdint/int128.hpp>` and `<xstd/ints/cstdint/bit_int.hpp>`. Every type it names is
half of a pair: each transformation leads to the other half and back, and the unsigned half has one more digit than
the signed half, cv-qualified or not.
*Test:* `cstdint.cpp: EveryTypeNamedHereIsHalfOfAPair`, `ThePairingCarriesCvQualification`.

`<xstd/ints/cstdint/bit_int.hpp>` exports `<xstd/ints/limits/numeric_limits.hpp>`,
`<xstd/ints/type_traits/make_signed.hpp>` and `<xstd/ints/type_traits/make_unsigned.hpp>`, which declare the `_BitInt`
specializations of `numeric_limits`, `make_signed` and `make_unsigned`, so including it is enough to ask them of the
aliases. *Untested:* each test source that asks them also includes the trait's own header; an export is include
analysis, not a test case.

### `XSTD_HAS_INT128`

- Defined as `1` where `_MSC_VER` is defined or, failing that, where `__SIZEOF_INT128__` is; not defined otherwise.
  *Test:* `cstdint/int128.cpp: AbsentOnlyWhereTheTargetHasNone`, which holds that it is defined wherever either
  macro is.
- Where it is not defined, as with GCC and Clang on 32-bit x86 and ARM, `int128`, `uint128` and their two
  associations are not declared, and including the header is not an error.
  *Test:* `cstdint/int128.cpp: AbsentOnlyWhereTheTargetHasNone`.

### `int128`, `uint128`

- Where `_MSC_VER` is defined, `std::_Signed128` and `std::_Unsigned128` from the Microsoft STL's
  `<__msvc_int128.hpp>`; otherwise `__int128` and `unsigned __int128`.
  *Untested:* every case holds a property of the alias, and none names the type it aliases.
- Each occupies 16 bytes, and `std::numeric_limits` gives `int128` 127 digits and `uint128` 128.
  *Test:* `cstdint/int128.cpp: WidthAndDigits`.
- `int128` is signed and `uint128` unsigned, by `std::numeric_limits`, by `is_signed_v` and `is_unsigned_v` under
  every cv-qualification, and by `signed_integer` and `unsigned_integer`.
  *Test:* `cstdint/int128.cpp: Signedness`.
- The width carries values: `int128{-1} < int128{0}`, and `uint128{1} << 127U` is nonzero, as constant expressions.
  *Test:* `cstdint/int128.cpp: Ordering`.
- `make_unsigned<int128>::type` is `uint128` and `make_signed<uint128>::type` is `int128`, and through the
  cv-qualified specializations of either trait, `const`, `volatile` and `const volatile` carry over.
  *Test:* `cstdint/int128.cpp: AssociatesSignedAndUnsignedPair`.
- Both model `nothrow_const_operators`. *Test:* `concepts/nothrow_const_operators.cpp: HoldsForTheTypesThatDeclareIt`.
- *Remarks:* The aliases and the two associations are declared in one header, so a translation unit that names
  either type has both specializations. *Definition only:* nothing can name the alias without that header.
- *Remarks:* GCC's `-Wpedantic` diagnostic on `__int128` is suppressed around the two alias declarations.
  *Checked by* the header self-sufficiency build of `<xstd/ints/cstdint/int128.hpp>` in `test/CMakeLists.txt`,
  which compiles under `-pedantic-errors`.

### `XSTD_HAS_BIT_INT`

- Defined as `1` exactly where the compiler defines `__BITINT_MAXWIDTH__`, which among the tested compilers only
  Clang does in C++. *Test:* `cstdint/bit_int.cpp: AbsentOnlyWithoutCompilerSupport`, which fails to compile where
  `__BITINT_MAXWIDTH__` is defined and the macro is not.

### `bit_int_max_width`

- `__BITINT_MAXWIDTH__`, as a `size_t`. *Test:* `cstdint/bit_int.cpp: AliasesNameTheBitPreciseTypes`.

### `bit_int`, `bit_uint`

- *Constraints:* `2 <= N` and `N <= bit_int_max_width`. The lower bound pairs every exposed width: Clang does not yet
  implement N3747's `signed _BitInt(1)`. Both aliases name a type at 2 and at `bit_int_max_width`, and neither at 1 or
  at `bit_int_max_width + 1`. *Test:* `cstdint/bit_int.cpp: AliasesNameTheBitPreciseTypes`.
- `bit_int<N>` is `signed _BitInt(N)` and `bit_uint<N>` is `unsigned _BitInt(N)`: aliases, not wrappers, so the
  conversions, promotions and operators are the compiler's.
  *Test:* `cstdint/bit_int.cpp: AliasesNameTheBitPreciseTypes`.
- `bit_int<17>` models `signed_integer` and `bit_uint<17>` models `unsigned_integer`, and each is the other's
  counterpart. *Test:* `cstdint/bit_int.cpp: AliasesModelTheIntegerConcepts`.
- At widths 2, 17 and 64 the two are a pair, and `bit_int<17> const` pairs with `bit_uint<17> const`.
  *Test:* `cstdint.cpp: EveryTypeNamedHereIsHalfOfAPair`, `ThePairingCarriesCvQualification`.
- `bit_int<17>` and `bit_uint<17>` model `nothrow_const_operators`.
  *Test:* `concepts/nothrow_const_operators.cpp: HoldsForTheTypesThatDeclareIt`.
- *Remarks:* Clang's `-Wbit-int-extension` is suppressed within the header, and within each of the three headers that
  declare a `_BitInt` specialization. *Untested:* the test build disables that warning everywhere, so nothing would
  see it fire.

## `<xstd/ints/bit.hpp>`

```cpp
namespace xstd {
  // [bit.byteswap], byteswap
  template<class T>
    constexpr auto byteswap(T x) noexcept(see below) -> T;

  // [bit.pow.two], integral powers of 2
  template<class T>
    constexpr auto has_single_bit(T x) noexcept(see below) -> bool;
  template<class T>
    constexpr auto bit_ceil(T x) -> T;
  template<class T>
    constexpr auto bit_floor(T x) noexcept(see below) -> T;
  template<class T>
    constexpr auto bit_width(T x) noexcept(see below) -> int;

  // [bit.rotate], rotating
  template<class T>
    constexpr auto rotl(T x, int s) noexcept(see below) -> T;
  template<class T>
    constexpr auto rotr(T x, int s) noexcept(see below) -> T;

  // [bit.count], counting
  template<class T>
    constexpr auto countl_zero(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto countl_one(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto countr_zero(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto countr_one(T x) noexcept(see below) -> int;
  template<class T>
    constexpr auto popcount(T x) noexcept(see below) -> int;

  // P3793R2, shifting
  template<class T, class S>
    constexpr auto shl(T x, S s) noexcept(see below) -> T;
  template<class T, class S>
    constexpr auto shr(T x, S s) noexcept(see below) -> T;

  // P3104R6, permuting
  template<class T>
    constexpr auto bit_compress(T src, T mask) noexcept(see below) -> T;
  template<class T>
    constexpr auto bit_expand(T src, T mask) noexcept(see below) -> T;
  template<class T>
    constexpr auto bit_repeat(T x, int l) -> T;
  template<class T>
    constexpr auto bit_reverse(T x) noexcept(see below) -> T;

  // selecting, an xstd extension
  template<unsigned_integer T>
    constexpr auto bit_select(T x, int k) -> int;
}
```

The umbrella exports each header under `<xstd/ints/bit/>`, one function to a header. *Test:* `bit.cpp:
ReExportsTheWholeDirectory`, which names each of the nineteen functions. The test helpers that file includes
declare five of them; the other fourteen, `bit_ceil`, `byteswap`, `shl` and `shr` among them, reach it through the
umbrella alone.

Each function here is one overload set. Its first overload forwards to the standard function, constrained on the
standard call with the same arguments being well-formed rather than on `std::unsigned_integral`, which admits `bool`
and `char8_t` where `<bit>` refuses them. Beside it, constrained on that call being ill-formed, one template answers
every other type the function takes, working one `size_t` block at a time, since [iterator.concept.winc] gives every
integer-class type an explicit conversion to and from `size_t`. The two constraints are complementary, so at most one
of them is viable for any argument, and the overload set is the same whichever header a translation unit includes
first. *Test:* `bit/countl_zero.cpp: TheConstraintIsTheBodys`, `bit/countr_zero.cpp: TheConstraintIsTheBodys`,
`bit/popcount.cpp: TheConstraintIsTheBodys`.

### `byteswap`

```cpp
template<class T>
  constexpr auto byteswap(T x) noexcept(see below) -> T;
```

- *Constraints:* Either `std::byteswap(x)` is well-formed, `T` is integral and no wider, sign bit included, than the
  widest unsigned type xstd hands to the standard library (`uint128` where `XSTD_HAS_INT128` is defined, and
  `unsigned long long` otherwise), and `has_unique_object_representations_v<T>` is `true`; or `T` models `integer` and
  `numeric_limits<make_unsigned_t<T>>::digits` is a multiple of `numeric_limits<unsigned char>::digits`. So
  `bit_int<24>` and `bit_uint<24>` are taken and `bit_int<13>` and `bit_uint<13>` are not, and `bool` and `char8_t` are
  taken exactly where `std::byteswap` takes them.
  *Test:* `bit/byteswap.cpp: TheConstraintIsAWholeNumberOfBytes`.
- *Returns:* `std::byteswap(x)` where the first branch of the constraint holds.
  *Test:* `bit/byteswap.cpp: AgreesWithStdWhereStdAnswers`.
- *Returns:* Otherwise `static_cast<T>(u)`, where `u` is the value of `static_cast<make_unsigned_t<T>>(x)` with its
  bytes in reverse order: the byte `n` places from the least significant end lands `n` places from the most
  significant end. *Test:* `bit/byteswap.cpp: TheEndsTrade`, `AgreesWithTheOracle`.
- Swapping twice is the identity. *Test:* `bit/byteswap.cpp: AgreesWithTheOracle`.
- A signed `T` is swapped through its unsigned counterpart, as `std::byteswap` swaps a signed type.
  *Test:* `bit/byteswap.cpp: AgreesWithStdWhereStdAnswers`, `TheEndsTrade`, over the signed exact widths.
- A width of whole bytes that fills no whole object, such as `bit_uint<24>`, is swapped as its value, three bytes
  where the object has four. *Test:* `bit/byteswap.cpp: TheEndsTrade`, `AgreesWithTheOracle`, over `bit_int<24>` and
  `bit_uint<24>` where `XSTD_HAS_BIT_INT` is defined.
- *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
  `nothrow_const_operators<T> and nothrow_const_operators<make_unsigned_t<T>>`.
  *Test:* `bit/byteswap.cpp: NoexceptFollowsTheOperators`.
- *Remarks:* [bit.byteswap] reverses the object representation and mandates that `T` have no padding bits. The
  template reverses the value representation, so it takes a type with padding bits that the standard function's
  Mandates element rejects, and the constraint keeps every such type away from `std::byteswap`, including the
  `_BitInt` widths libc++ calls integral and then refuses to swap.
  *Test:* `bit/byteswap.cpp: TheConstraintIsAWholeNumberOfBytes`, `AgreesWithTheOracle`.
- A call is a constant expression. *Test:* `bit/byteswap.cpp: TheEndsTrade`, `AgreesWithTheOracle`.

### `has_single_bit`, `bit_ceil`, `bit_floor`, `bit_width`

```cpp
template<class T>
  constexpr auto has_single_bit(T x) noexcept(see below) -> bool;
template<class T>
  constexpr auto bit_ceil(T x) -> T;
template<class T>
  constexpr auto bit_floor(T x) noexcept(see below) -> T;
template<class T>
  constexpr auto bit_width(T x) noexcept(see below) -> int;
```

- *Constraints:* For each function `f`, `std::f(x)` is well-formed, or `T` models `unsigned_integer`. `bool`, the
  character types and every signed type are refused, as `<bit>` refuses them; a 128-bit integer class and an
  `unsigned _BitInt(N)` are taken. *Test:* `bit/has_single_bit.cpp: TheConstraintIsUnsignedInteger`,
  `bit/bit_ceil.cpp: TheConstraintIsUnsignedInteger`, `bit/bit_floor.cpp: TheConstraintIsUnsignedInteger`,
  `bit/bit_width.cpp: TheConstraintIsUnsignedInteger`.
- Where `std::f(x)` is well-formed, each returns `std::f(x)`.
  *Test:* `bit/has_single_bit.cpp: AgreesWithStdWhereStdAnswers`, `bit/bit_ceil.cpp: AgreesWithStdWhereStdAnswers`,
  `bit/bit_floor.cpp: AgreesWithStdWhereStdAnswers`, `bit/bit_width.cpp: AgreesWithStdWhereStdAnswers`.
- `has_single_bit`:
  - *Returns:* `true` if `x` is an integral power of two, and `false` otherwise.
    *Test:* `bit/has_single_bit.cpp: TheEndsOfEveryWidth`, `AgreesWithTheOracle`; `bit.cpp: TheFunctionsAgree`.
  - *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
    `nothrow_const_operators<T>`. *Test:* `bit/has_single_bit.cpp: NoexceptFollowsTheOperators`.
- `bit_ceil`:
  - *Preconditions:* The smallest integral power of two not less than `x` is representable as a value of type `T`:
    `x` is at most the highest power of two of its type. The block template diagnoses a violation by `assert`, and
    the forwarding overload as `std::bit_ceil` does. *Untested:* a violation at run time aborts, which a Boost.Test
    case cannot observe and survive.
  - *Returns:* The smallest integral power of two that is not less than `x`, so `1` for both `0` and `1`.
    *Test:* `bit/bit_ceil.cpp: TheEndsOfEveryWidth`, `AgreesWithTheOracle`.
  - *Remarks:* A call that violates the precondition is not a core constant expression, whether or not `NDEBUG` is
    defined, as [bit.pow.two] requires of `std::bit_ceil`. *Test:* `bit/bit_ceil.cpp: AValuePastTheTopBitIsNoConstant`,
    one past the top bit of every width, against the top bit itself.
  - *Remarks:* It is not `noexcept`, as `std::bit_ceil` is not: its contract is narrow.
    *Test:* `bit/bit_ceil.cpp: IsNotNoexcept`.
- `bit_floor`:
  - *Returns:* `0` if `x` is `0`, and otherwise the largest integral power of two not greater than `x`.
    *Test:* `bit/bit_floor.cpp: TheEndsOfEveryWidth`, `AgreesWithTheOracle`; `bit.cpp: TheFunctionsAgree`.
  - *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
    `nothrow_const_operators<T>`. *Test:* `bit/bit_floor.cpp: NoexceptFollowsTheOperators`.
- `bit_width`:
  - *Returns:* `0` if `x` is `0`, and otherwise one plus the base-2 logarithm of `x`, with any fractional part
    discarded, as an `int`, the type LWG 3656 gave `std::bit_width`.
    *Test:* `bit/bit_width.cpp: TheEndsOfEveryWidth`, `AgreesWithTheOracle`; `bit.cpp: TheFunctionsAgree`.
  - *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
    `nothrow_const_operators<T>`. *Test:* `bit/bit_width.cpp: NoexceptFollowsTheOperators`.
- A call is a constant expression, for `bit_ceil` one within its precondition.
  *Test:* `bit/has_single_bit.cpp: TheEndsOfEveryWidth`, `bit/bit_ceil.cpp: TheEndsOfEveryWidth`,
  `bit/bit_floor.cpp: TheEndsOfEveryWidth`, `bit/bit_width.cpp: TheEndsOfEveryWidth`.

### `rotl`, `rotr`

```cpp
template<class T>
  constexpr auto rotl(T x, int s) noexcept(see below) -> T;
template<class T>
  constexpr auto rotr(T x, int s) noexcept(see below) -> T;
```

In the following, let `N` be `numeric_limits<T>::digits` and `r` be `s % N`.

- *Constraints:* For each function `f`, `std::f(x, s)` is well-formed, or `T` models `unsigned_integer`.
  *Test:* `bit/rotl.cpp: TheConstraintIsUnsignedInteger`, `bit/rotr.cpp: TheConstraintIsUnsignedInteger`.
- Where `std::f(x, s)` is well-formed, each returns `std::f(x, s)`.
  *Test:* `bit/rotl.cpp: AgreesWithStdWhereStdAnswers`, `bit/rotr.cpp: AgreesWithStdWhereStdAnswers`.
- `rotl`: *Returns:* `x` if `r` is `0`; `(x << r) | (x >> (N - r))` if `r` is positive; `rotr(x, -r)` if `r` is
  negative. This is [bit.rotate]'s element. *Test:* `bit/rotl.cpp: TheEndsTrade`, `AgreesWithTheOracle`.
- `rotr`: *Returns:* `x` if `r` is `0`; `(x >> r) | (x << (N - r))` if `r` is positive; `rotl(x, -r)` if `r` is
  negative. This is [bit.rotate]'s element. *Test:* `bit/rotr.cpp: TheEndsTrade`, `AgreesWithTheOracle`.
- Every `int` count is in the domain, `INT_MIN` and `INT_MAX` included, and turning one way and then the other by the
  same count is the identity. *Test:* `bit/rotl.cpp: RotlUndoesRotr`, `bit/rotr.cpp: RotrUndoesRotl`;
  `bit.cpp: TheFunctionsAgree`.
- *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
  `nothrow_const_operators<T>`. *Test:* `bit/rotl.cpp: NoexceptFollowsTheOperators`,
  `bit/rotr.cpp: NoexceptFollowsTheOperators`.
- A call is a constant expression. *Test:* `bit/rotl.cpp: TheEndsTrade`, `bit/rotr.cpp: TheEndsTrade`.

### `countl_zero`, `countl_one`, `countr_zero`, `countr_one`, `popcount`

```cpp
template<class T>
  constexpr auto countl_zero(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto countl_one(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto countr_zero(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto countr_one(T x) noexcept(see below) -> int;
template<class T>
  constexpr auto popcount(T x) noexcept(see below) -> int;
```

- *Constraints:* For each function `f`, `std::f(x)` is well-formed, or `T` models `unsigned_integer`.
  *Test:* `bit/countl_zero.cpp: TheConstraintIsTheBodys`, `bit/countl_one.cpp: TheConstraintIsUnsignedInteger`,
  `bit/countr_zero.cpp: TheConstraintIsTheBodys`, `bit/countr_one.cpp: TheConstraintIsUnsignedInteger`,
  `bit/popcount.cpp: TheConstraintIsTheBodys`.
- A signed 128-bit class has no bits to count, as a signed built-in has none for `std::popcount`.
  *Test:* `ext/absl/int128.cpp: NearMissesStayRejected`, `ext/boost/int128.cpp: NearMissesStayRejected`.
- Where `std::f(x)` is well-formed, each returns `std::f(x)`.
  *Test:* `bit/countl_zero.cpp: AgreesWithStdWhereStdAnswers`, `bit/countl_one.cpp: AgreesWithStdWhereStdAnswers`,
  `bit/countr_zero.cpp: AgreesWithStdWhereStdAnswers`, `bit/countr_one.cpp: AgreesWithStdWhereStdAnswers`,
  `bit/popcount.cpp: AgreesWithStdWhereStdAnswers`.
- `countl_zero`: *Returns:* The number of consecutive `0` bits in the value of `x`, starting from the most
  significant bit, so `N` for `0`. *Test:* `bit/countl_zero.cpp: TheEndsOfEveryWidth`, `TheWordBoundary`,
  `BitPreciseAgreesWithTheOracle`; `bit.cpp: TheThreeAgreeOnASingleBit`.
- `countl_one`: *Returns:* The number of consecutive `1` bits in the value of `x`, starting from the most significant
  bit. *Test:* `bit/countl_one.cpp: TheEndsOfEveryWidth`, `AgreesWithTheOracle`; `bit.cpp: TheFunctionsAgree`.
- `countr_zero`: *Returns:* The number of consecutive `0` bits in the value of `x`, starting from the least
  significant bit, so `N` for `0`. *Test:* `bit/countr_zero.cpp: TheEndsOfEveryWidth`, `TheWordBoundary`,
  `BitPreciseAgreesWithTheOracle`; `bit.cpp: TheThreeAgreeOnASingleBit`, `CountingTrailingZerosByRepetition`.
- `countr_one`: *Returns:* The number of consecutive `1` bits in the value of `x`, starting from the least
  significant bit. *Test:* `bit/countr_one.cpp: TheEndsOfEveryWidth`, `AgreesWithTheOracle`;
  `bit.cpp: TheFunctionsAgree`.
- `popcount`: *Returns:* The number of `1` bits in the value of `x`. *Test:* `bit/popcount.cpp: TheEndsOfEveryWidth`,
  `TheWordBoundary`, `BitPreciseAgreesWithTheOracle`; `bit.cpp: TheThreeAgreeOnASingleBit`.
- *Remarks:* Where `XSTD_HAS_BIT_INT` is defined, `std::f` refuses `bit_uint<N>`, and `__has_builtin` reports
  `__builtin_clzg`, `__builtin_ctzg` or `__builtin_popcountg` respectively, `countl_zero`, `countr_zero` and
  `popcount` have a third overload taking `bit_uint<N>` and deducing `N`, which returns the builtin's answer, `N` for
  zero where it counts zeros, and is `noexcept`. It is more specialized than the block template, so for each width
  exactly one of the three overloads is viable. *Test:* `bit/countl_zero.cpp: TheConstraintIsTheBodys`,
  `BitPreciseAgreesWithTheOracle`; `bit/countr_zero.cpp: TheConstraintIsTheBodys`, `BitPreciseAgreesWithTheOracle`;
  `bit/popcount.cpp: TheConstraintIsTheBodys`, `BitPreciseAgreesWithTheOracle`.
- *Remarks:* The exception specification of `countl_one` and `countr_one` is `true` for the forwarding overload, and
  otherwise `nothrow_const_operators<T>`. *Test:* `bit/countl_one.cpp: NoexceptFollowsTheOperators`,
  `bit/countr_one.cpp: NoexceptFollowsTheOperators`.
- *Remarks:* The exception specification of `countl_zero`, `countr_zero` and `popcount` is `true` for the forwarding
  overload and the builtin one, and otherwise `nothrow_const_operators<T>`.
  *Test:* `bit/countl_zero.cpp: NoexceptFollowsTheOperators`, `bit/countr_zero.cpp: NoexceptFollowsTheOperators`,
  `bit/popcount.cpp: NoexceptFollowsTheOperators`.
- A call is a constant expression. *Test:* `bit/countl_zero.cpp: TheEndsOfEveryWidth`,
  `bit/countl_one.cpp: TheEndsOfEveryWidth`, `bit/countr_zero.cpp: TheEndsOfEveryWidth`,
  `bit/countr_one.cpp: TheEndsOfEveryWidth`, `bit/popcount.cpp: TheEndsOfEveryWidth`.

### `shl`, `shr`

```cpp
template<class T, class S>
  constexpr auto shl(T x, S s) noexcept(see below) -> T;
template<class T, class S>
  constexpr auto shr(T x, S s) noexcept(see below) -> T;
```

In the following, let `W` be `numeric_limits<make_unsigned_t<T>>::digits`, the width of `T` with any sign bit.

- *Constraints:* Either the standard library defines `__cpp_lib_bitops` as at least `202606L` and `std::shl(x, s)`
  and `std::shr(x, s)` are both well-formed; or `T` and `S` both model `integer`. Either operand may be signed or
  unsigned, the two of different types, an integer class or bit-precise; `bool` and a character type are refused on
  either side. *Test:* `bit/shl.cpp: TheConstraintIsInteger`, `bit/shr.cpp: TheConstraintIsInteger`.
- Where the standard functions take the pair, each returns `std::shl(x, s)` or `std::shr(x, s)`.
  *Test:* `bit/shl.cpp: AgreesWithStdWhereStdAnswers`, `bit/shr.cpp: AgreesWithStdWhereStdAnswers`, each compiled only
  where `__cpp_lib_bitops` is at least `202606L`.
- `shl`: *Returns:* The value of x × 2<sup>s</sup>, rounded towards negative infinity, reduced modulo 2<sup>W</sup>
  to a value of `T`. *Test:* `bit/shl.cpp: EveryCountIsDefined`, `AgreesWithTheOracle`.
- `shr`: *Returns:* The value of x × 2<sup>−s</sup>, rounded towards negative infinity, reduced modulo
  2<sup>W</sup> to a value of `T`. *Test:* `bit/shr.cpp: EveryCountIsDefined`, `AgreesWithTheOracle`.
- Every value of `S` is in the domain: a count of magnitude `W` or more gives `0` towards the left, and towards the
  right `0` for a value that is not negative and `-1` for one that is, the most negative count of a signed `S`
  included. *Test:* `bit/shl.cpp: EveryCountIsDefined`, `AgreesWithTheOracle`;
  `bit/shr.cpp: EveryCountIsDefined`, `AgreesWithTheOracle`.
- The count's type changes nothing but which counts it can hold.
  *Test:* `bit/shl.cpp: EveryCountTypeAgrees`, `bit/shr.cpp: EveryCountTypeAgrees`.
- `shr(x, s)` equals `shl(x, -s)` wherever `-s` is representable.
  *Test:* `bit/shr.cpp: ShrIsShlTheOtherWay`.
- *Remarks:* The exception specification is `true` for the forwarding overload, and otherwise
  `nothrow_const_operators<T> and nothrow_const_operators<S>`.
  *Test:* `bit/shl.cpp: NoexceptFollowsTheOperators`, `bit/shr.cpp: NoexceptFollowsTheOperators`.
- A call is a constant expression. *Test:* `bit/shl.cpp: EveryCountIsDefined`, `TheEndsOfEveryWidth`;
  `bit/shr.cpp: EveryCountIsDefined`, `TheEndsOfEveryWidth`, the last two over every exact width, signed and
  unsigned, the 128-bit and bit-precise ones included, except Abseil's where it lacks an intrinsic 128-bit type.

In the entries for the four permutations and `bit_select`, as in P3104R5's [bit.permute]/1, `N` is
`numeric_limits<T>::digits`, xstd's, which answers for the integer classes and the bit-precise types too, and `α[n]` is
bit `n` of `α`, counted from zero at the least significant end, so that `α` is the sum of `α[n]·2^n` over `n` in
`[0, N)`. A block is one `size_t`. A check that is not a Boost.Test case is named by its source file from the
repository root and the ctest test that runs it, as *Checked by*.

### `bit_reverse`

```cpp
#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS
template<class T>
  requires ints::detail::std_permutes_bits<T>
  constexpr auto bit_reverse(T x) noexcept -> T;
#endif
template<unsigned_integer T>
  requires (not ints::detail::std_permutes_bits<T>)
  constexpr auto bit_reverse(T x) noexcept(nothrow_const_operators<T>) -> T;
```

- *Constraints:* `T` models `unsigned_integer`, which takes the 128-bit integer classes and `bit_uint<N>` besides the
  standard unsigned types, or `std::bit_reverse` takes it. `bool`, the character types, a signed type and a signed
  bit-precise type are turned away. *Test:* `bit/bit_reverse.cpp: TheConstraintIsUnsignedInteger`.
- *Returns:* `reverse(x)`, P3104R5's sum of `x[n]·2^(N-n-1)` over `n` in `[0, N)`: bit `n` of `x` becomes bit
  `N - 1 - n`. *Test:* `bit/bit_reverse.cpp: AgreesWithTheOracle`, `TheEndsTrade`.
- The paper's example, `bit_reverse(uint32_t{0x0000'1234})`, is `0x2c48'0000`, the mirror image, where the paper
  prints `0x24c80000`. *Test:* `bit/bit_reverse.cpp: ThePapersExample`.
- `bit_reverse(bit_reverse(x))` is `x`, [bit.permute]'s note.
  *Test:* `bit/bit_reverse.cpp: ReversingTwiceIsTheIdentity`.
- Compressing from the top is reversal around compressing from the bottom: reversing `x` and `m`, compressing and
  reversing back gives `bit_compress(x, m)` shifted up by the count of `m`'s zeros.
  *Test:* `bit.cpp: ReversalMirrorsCompression`.
- A call is a constant expression for every type it takes. *Test:* `bit/bit_reverse.cpp: TheEndsTrade`,
  `AgreesWithTheOracle`.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.
  *Test:* `bit/bit_reverse.cpp: ReturnsTheArgumentsType`.
- *Remarks:* The exception specification is `nothrow_const_operators<T>`, so the call is `noexcept` for every built-in
  type and not for `absl::uint128`. *Test:* `bit/bit_reverse.cpp: NoexceptFollowsTheOperators`.
- *Remarks:* Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`,
  `std::bit_repeat` and `std::bit_reverse` all take `T`, the call is `std::bit_reverse(x)`, unconditionally
  `noexcept`, and the template does not take `T`. *Untested:* no leg's standard library reaches `202607L`, and no
  test compares the result with `std::bit_reverse`.
- *Remarks:* The template mirrors each block on its own and lands it at the mirror of its offset; a top block that
  `N` fills only in part is mirrored whole and shifted down by its slack. The bit just past the lowest 64 lands that
  far below the top. *Test:* `bit/bit_reverse.cpp: TheEndsTrade`, `AgreesWithTheOracle`, over 65, 127, 128, 129 and
  147 bits where the build has them.
- *Remarks:* The blocks are unrolled at compile time, so a 128-bit call compiles to no branch or call on x86-64, and
  to two `rbit` with no branch on AArch64; under `-mgfni` it holds two `gf2p8affineqb`.
  *Checked by* `codegen/bit_permutation.cpp: codegen.bit_permutation.*`, every x86-64 configuration but
  `bmi2-excluded`, and `armv8-a`.
- *Remarks:* A block is mirrored by the compiler's `__builtin_bitreverse32` and `__builtin_bitreverse64` wherever it
  has both, in a constant expression too. Otherwise, outside a constant expression, by one `RBIT` where the target is
  AArch64 with `<arm_acle.h>`, or by one `GF2P8AFFINEQB` and a byte swap where it is x86-64 with `__GFNI__`; and by
  the portable kernel, a byte swap and three masked swaps, everywhere else. Each run-time kernel agrees with the
  portable one, and each overload with the oracle at its own width.
  *Test:* `bit/detail/intrinsic.cpp: TheRunTimeReversalAgreesWithThePortableKernel`,
  `TheIntrinsicsReverseAsTheOracle`.
- *Remarks:* A 64-bit call is a `gf2p8affineqb` and a `bswap` with no branch under `-mgfni`, and one `rbit` with no
  branch on AArch64. *Checked by* `codegen/bit_permutation.cpp: codegen.bit_permutation.gfni`,
  `codegen.bit_permutation.armv8-a`.
- The portable kernel agrees with the oracle at compile time and at run time, and is RBIT for every input at 32 and
  64 bits. *Test:* `bit/detail/portable.cpp: KernelsAgreeWithTheOracleAtCompileTime`, `KernelsAgreeWithTheOracle`.
  *Checked by* `proofs/prove.py: proofs.z3`, against a model `proofs/drift.py: proofs.drift` holds to the C++.
- It agrees with the oracle on every input the fuzzer finds, at every width from 8 bits up.
  *Checked by* `fuzz/fuzz_bit_permutation.cpp: fuzz.fuzz_bit_permutation`.

### `bit_repeat`

```cpp
#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS
template<class T>
  requires ints::detail::std_permutes_bits<T>
  constexpr auto bit_repeat(T x, int l) -> T;
#endif
template<unsigned_integer T>
  requires (not ints::detail::std_permutes_bits<T>)
  constexpr auto bit_repeat(T x, int l) -> T;
```

- *Constraints:* As for `bit_reverse`. *Test:* `bit/bit_repeat.cpp: TheConstraintIsUnsignedInteger`.
- *Preconditions:* `l` is greater than zero.
  *Untested:* a call that violates it at run time fails an `assert`, and no test observes an abort.
- *Returns:* `repeat(x, l)`, P3104R5's sum of `x[n mod l]·2^n` over `n` in `[0, N)`: the low `l` bits of `x` repeated
  across the width. *Test:* `bit/bit_repeat.cpp: AgreesWithTheOracle`, every `l` from 1 to `N + 1`.
- A period of one copies the lowest bit everywhere, and a period of `N` or more, `INT_MAX` included, returns `x`.
  *Test:* `bit/bit_repeat.cpp: TheExtremePeriods`.
- The paper's examples hold: `bit_repeat(uint32_t{0xc}, 4)` is `0xcccc'cccc`, the alternating masks of its
  `countr_zero` are `bit_repeat((1u << i) - 1, 2 * i)`, and that `countr_zero` agrees with xstd's.
  *Test:* `bit/bit_repeat.cpp: ThePapersExamples`; `bit.cpp: CountingTrailingZerosByRepetition`.
- *Throws:* Nothing, for a `T` whose operators throw nothing.
  *Untested:* it is not `noexcept`, so only an exception from a type whose operators throw could show otherwise, and
  no such type is tested.
- *Remarks:* A call that violates the precondition is not a core constant expression, whether or not `NDEBUG` is
  defined, as P3104R6 [bit.permute]/8 adds. *Test:* `bit/bit_repeat.cpp: ALengthBelowOneIsNoConstant`, at
  `uint32_t` and `l` of 0, -1 and `INT_MIN`.
- *Remarks:* It is not `noexcept`, for any type, the forwarding overload included.
  *Test:* `bit/bit_repeat.cpp: IsNotNoexcept`.
- A call is a constant expression for every type it takes. *Test:* `bit/bit_repeat.cpp: TheExtremePeriods`,
  `AgreesWithTheOracle`.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.
  *Test:* `bit/bit_repeat.cpp: ReturnsTheArgumentsType`.
- *Remarks:* Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`,
  `std::bit_repeat` and `std::bit_reverse` all take `T`, the call is `std::bit_repeat(x, l)`, and the template does
  not take `T`. *Untested:* no leg's standard library reaches `202607L`, and no test compares the result with
  `std::bit_repeat`.
- *Remarks:* The template has no block kernel: it masks the low `l` bits and doubles the pattern until it fills the
  width, so it runs `ceil(log2(N / l))` shift-or steps. *Untested:* no codegen check reads its assembly.
- It agrees with the oracle on every input the fuzzer finds, at a period one more than the mask's population, past the
  width included. *Checked by* `fuzz/fuzz_bit_permutation.cpp: fuzz.fuzz_bit_permutation`.

### `bit_compress`

```cpp
#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS
template<class T>
  requires ints::detail::std_permutes_bits<T>
  constexpr auto bit_compress(T src, T mask) noexcept -> T;
#endif
template<unsigned_integer T>
  requires (not ints::detail::std_permutes_bits<T>)
  constexpr auto bit_compress(T src, T mask) noexcept(nothrow_const_operators<T>) -> T;
```

- *Constraints:* As for `bit_reverse`. *Test:* `bit/bit_compress.cpp: TheConstraintIsUnsignedInteger`.
- *Returns:* `compress(src, mask)`, P3104R5's sum of `mask[n]·src[n]·2^(mask[0] + … + mask[n-1])` over `n` in
  `[0, N)`: the bits of `src` that `mask` selects, packed into the low bits in their order.
  *Test:* `bit/bit_compress.cpp: AgreesWithTheOracle`.
- [bit.permute]'s example, as P3104R6 words it, holds: `bit_compress(0bABCD, 0b0101)` is `0b00BD` for any bits `A`,
  `B`, `C` and `D`. *Test:* `bit/bit_compress.cpp: TheWordingsExample`.
- The paper's equivalences hold: a mask of contiguous ones is a shift and an and, a single-bit mask reads that bit,
  and compressing `x` through itself, or all ones through `x`, gives `popcount(x)` low ones.
  *Test:* `bit/bit_compress.cpp: ThePapersEquivalences`; `bit.cpp: CompressionCountsThePopulation`.
- An empty mask gives zero, a full one `src`, and the top bit through itself gives one.
  *Test:* `bit/bit_compress.cpp: TheExtremeMasks`.
- It undoes `bit_expand` through the same mask, and is undone by it as far as the mask's population lets it:
  `bit_expand(bit_compress(x, m), m)` is `x & m`, and `bit_compress(bit_expand(x, m), m)` is `x` cut to its
  `popcount(m)` low bits. *Test:* `bit.cpp: CompressAndExpandAreInverse`, `InterleavingRoundTrips`.
- A call is a constant expression for every type it takes. *Test:* `bit/bit_compress.cpp: TheExtremeMasks`,
  `AgreesWithTheOracle`.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.
  *Test:* `bit/bit_compress.cpp: ReturnsTheArgumentsType`.
- *Remarks:* The exception specification is `nothrow_const_operators<T>`, so the call is `noexcept` for every built-in
  type and not for `absl::uint128`. *Test:* `bit/bit_compress.cpp: NoexceptFollowsTheOperators`.
- *Remarks:* Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`,
  `std::bit_repeat` and `std::bit_reverse` all take `T`, the call is `std::bit_compress(src, mask)`, unconditionally
  `noexcept`, and the template does not take `T`. *Untested:* no leg's standard library reaches `202607L`, and no
  test compares the result with `std::bit_compress`.
- *Remarks:* The template compresses each block of `src` through the same block of `mask` and packs the result
  directly above the bits the lower blocks selected, so selected bits on either side of bit 64 land adjacent.
  *Test:* `bit/bit_compress.cpp: TheExtremeMasks`, `AgreesWithTheOracle`, over 65, 127, 128, 129 and 147 bits where
  the build has them.
- *Remarks:* Outside a constant expression, where `XSTD_INTS_HAS_FAST_BMI2` is defined, a block is one `PEXT`:
  x86-64 with `__BMI2__`, or MSVC other than clang-cl with `/arch:AVX2`, but not a build for `bdver4`, `znver1` or
  `znver2`, on which `PEXT` is microcoded. Everywhere else, and in every constant expression, a block takes the
  portable kernel, which places each selected bit at its rank among the mask's ones by one `popcount`. The run-time
  kernel agrees with the portable one, and each `PEXT` overload with it at its own width.
  *Test:* `bit/detail/intrinsic.cpp: TheRunTimeKernelsAgreeWithThePortableOnes`,
  `TheInstructionsAgreeWithThePortableKernels`.
- *Remarks:* A 64-bit call is one `pext` with no branch or call under `-mbmi2` and `-march=x86-64-v3`, and has no
  `pext` or `pdep` under `-march=x86-64`, `-mgfni`, `bdver4`, `znver1` or `znver2`; the `x86-64-v3` checks fail
  under `znver2`, so they see the excluded kernel gone though BMI2 is enabled.
  *Checked by* `codegen/bit_permutation.cpp: codegen.bit_permutation.bmi2`, `codegen.bit_permutation.x86-64-v3`,
  `codegen.bit_permutation.x86-64`, `codegen.bit_permutation.gfni`, `codegen.bit_permutation.bdver4`,
  `codegen.bit_permutation.znver1`, `codegen.bit_permutation.znver2`, `codegen.bit_permutation.bmi2-excluded`.
- The portable kernel agrees with the oracle at compile time and at run time, dense and sparse, round-trips with both
  portable expansions, and is PEXT for every input at 32 and 64 bits.
  *Test:* `bit/detail/portable.cpp: KernelsAgreeWithTheOracleAtCompileTime`, `KernelsAgreeWithTheOracle`,
  `KernelsRoundTrip`. *Checked by* `proofs/prove.py: proofs.z3`, against a model `proofs/drift.py: proofs.drift`
  holds to the C++.
- It agrees with the oracle and round-trips with `bit_expand` for every 16-bit source and mask, the portable kernel
  round-trips with both portable expansions, and, where the build has fast BMI2, `PEXT` agrees with the portable
  kernel. *Checked by* `exhaustive/bit_permutation.cpp: exhaustive.bit_permutation`.
- It agrees with the oracle and round-trips with `bit_expand` on every input the fuzzer finds, at every width from 8
  bits up. *Checked by* `fuzz/fuzz_bit_permutation.cpp: fuzz.fuzz_bit_permutation`.

### `bit_expand`

```cpp
#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS
template<class T>
  requires ints::detail::std_permutes_bits<T>
  constexpr auto bit_expand(T src, T mask) noexcept -> T;
#endif
template<unsigned_integer T>
  requires (not ints::detail::std_permutes_bits<T>)
  constexpr auto bit_expand(T src, T mask) noexcept(nothrow_const_operators<T>) -> T;
```

- *Constraints:* As for `bit_reverse`. *Test:* `bit/bit_expand.cpp: TheConstraintIsUnsignedInteger`.
- *Returns:* `expand(src, mask)`, P3104R5's sum of `mask[n]·src[mask[0] + … + mask[n-1]]·2^n` over `n` in `[0, N)`:
  the low bits of `src` deposited, in their order, on the positions of `mask`'s ones.
  *Test:* `bit/bit_expand.cpp: AgreesWithTheOracle`.
- [bit.permute]'s example, as P3104R6 words it, holds: `bit_expand(0bABCD, 0b0101)` is `0b0C0D` for any bits `A`,
  `B`, `C` and `D`. *Test:* `bit/bit_expand.cpp: TheWordingsExample`.
- The paper's equivalences hold: a mask of contiguous ones is an and and a shift, and expanding `2^n` through `x`
  gives `x`'s one of rank `n`, and `2^n - 1` its `n` lowest ones. *Test:* `bit/bit_expand.cpp: ThePapersEquivalences`.
- The paper's interleaving holds: `bit_expand(x, bit_repeat(0b10u, 2)) | bit_expand(y, bit_repeat(0b01u, 2))` is the
  Z-order index of `x` and `y`. *Test:* `bit.cpp: InterleavingByExpansion`, `InterleavingRoundTrips`.
- An empty mask gives zero, a full one `src`, and one through the top bit gives the top bit.
  *Test:* `bit/bit_expand.cpp: TheExtremeMasks`.
- It undoes `bit_compress` through the same mask, and is undone by it, as far as the mask's population lets it.
  *Test:* `bit.cpp: CompressAndExpandAreInverse`.
- A call is a constant expression for every type it takes. *Test:* `bit/bit_expand.cpp: TheExtremeMasks`,
  `AgreesWithTheOracle`.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.
  *Test:* `bit/bit_expand.cpp: ReturnsTheArgumentsType`.
- *Remarks:* The exception specification is `nothrow_const_operators<T>`, so the call is `noexcept` for every built-in
  type and not for `absl::uint128`. *Test:* `bit/bit_expand.cpp: NoexceptFollowsTheOperators`.
- *Remarks:* Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`,
  `std::bit_repeat` and `std::bit_reverse` all take `T`, the call is `std::bit_expand(src, mask)`, unconditionally
  `noexcept`, and the template does not take `T`. *Untested:* no leg's standard library reaches `202607L`, and no
  test compares the result with `std::bit_expand`.
- *Remarks:* The template expands each block of `mask` on its own, drawing on the bits of `src` the lower blocks left
  unconsumed, so the bits of a mask on either side of bit 64 receive adjacent bits of `src`.
  *Test:* `bit/bit_expand.cpp: TheExtremeMasks`, `AgreesWithTheOracle`, over 65, 127, 128, 129 and 147 bits where the
  build has them.
- *Remarks:* Outside a constant expression, where `XSTD_INTS_HAS_FAST_BMI2` is defined, as for `bit_compress`, a block
  is one `PDEP`. Everywhere else, and in every constant expression, a block takes the portable kernel, which walks the
  mask's ones from the lowest and deposits the next bit of `src` on each by a mask rather than a branch. The run-time
  kernel agrees with the portable one, and each `PDEP` overload with it at its own width.
  *Test:* `bit/detail/intrinsic.cpp: TheRunTimeKernelsAgreeWithThePortableOnes`,
  `TheInstructionsAgreeWithThePortableKernels`.
- *Remarks:* A 64-bit call is one `pdep` with no branch or call under `-mbmi2` and `-march=x86-64-v3`, and has no
  `pext` or `pdep` under `-march=x86-64`, `-mgfni`, `bdver4`, `znver1` or `znver2`.
  *Checked by* `codegen/bit_permutation.cpp: codegen.bit_permutation.bmi2`, `codegen.bit_permutation.x86-64-v3`,
  `codegen.bit_permutation.x86-64`, `codegen.bit_permutation.gfni`, `codegen.bit_permutation.bdver4`,
  `codegen.bit_permutation.znver1`, `codegen.bit_permutation.znver2`, `codegen.bit_permutation.bmi2-excluded`.
- The portable kernel agrees with the oracle at compile time and at run time, dense and sparse, round-trips with the
  portable compression, and is PDEP for every input at 32 and 64 bits.
  *Test:* `bit/detail/portable.cpp: KernelsAgreeWithTheOracleAtCompileTime`, `KernelsAgreeWithTheOracle`,
  `KernelsRoundTrip`. *Checked by* `proofs/prove.py: proofs.z3`, against a model `proofs/drift.py: proofs.drift`
  holds to the C++.
- It agrees with the oracle for every 16-bit source and mask, and on every input the fuzzer finds, at every width
  from 8 bits up. *Checked by* `exhaustive/bit_permutation.cpp: exhaustive.bit_permutation`;
  `fuzz/fuzz_bit_permutation.cpp: fuzz.fuzz_bit_permutation`.

### `bit_select`

```cpp
template<unsigned_integer T>
  constexpr auto bit_select(T x, int k) -> int;
```

An xstd extension beside P3104's permutations: no paper proposes it, and it has no `<bit>` counterpart to forward to.

- *Constraints:* `T` models `unsigned_integer`. `bool`, the character types, a signed type and a signed bit-precise
  type are turned away. *Test:* `bit/bit_select.cpp: TheConstraintIsUnsignedInteger`.
- *Preconditions:* `0 <= k` and `k < popcount(x)`.
  *Untested:* a call that violates it at run time fails an `assert`, and no test observes an abort.
- *Returns:* The `n` in `[0, N)` for which `x[n]` is one and `x[0] + … + x[n-1]` is `k`: the position of `x`'s one
  of rank `k`, counting from zero at the least significant end.
  *Test:* `bit/bit_select.cpp: AgreesWithTheOracle`, `AnExample`.
- It is the inverse of rank: exactly `k` ones of `x` lie below the one it finds, and expanding `2^k` through `x`
  deposits on it. At rank zero it is `countr_zero(x)`, and at rank `popcount(x) - 1` it is `bit_width(x) - 1`.
  *Test:* `bit.cpp: SelectionInvertsRank`.
- Every rank of a full value is its own position, and the ones on either side of bit 64 are found in order.
  *Test:* `bit/bit_select.cpp: TheExtremeValues`.
- A call is a constant expression for every type it takes. *Test:* `bit/bit_select.cpp: TheExtremeValues`,
  `AgreesWithTheOracle`.
- *Throws:* Nothing, for a `T` whose operators throw nothing.
  *Untested:* it is not `noexcept`, so only an exception from a type whose operators throw could show otherwise, and
  no such type is tested.
- *Remarks:* A call that violates the precondition is not a core constant expression, whether or not `NDEBUG` is
  defined. *Test:* `bit/bit_select.cpp: ARankOutsideThePopulationIsNoConstant`, at `uint32_t`, `k` of -1, 4,
  `INT_MIN` and `INT_MAX` over four ones, and `k` of 0 over none.
- *Remarks:* It is not `noexcept`, for any type. *Test:* `bit/bit_select.cpp: IsNotNoexcept`.
- *Remarks:* The result is an `int`, whatever the width of `T`. *Test:* `bit/bit_select.cpp: ReturnsAnInt`.
- *Remarks:* A type no wider than a block calls the block kernel once. A wider type skips whole blocks by their
  population, subtracting it from `k`, until the one of rank `k` lies in the current block, and adds that block's
  offset to what the block kernel finds there. *Test:* `bit/bit_select.cpp: TheExtremeValues`, `AgreesWithTheOracle`,
  over 65, 127, 128, 129 and 147 bits where the build has them.
- *Remarks:* Outside a constant expression, where `XSTD_INTS_HAS_FAST_BMI2` is defined, as for `bit_compress`, the
  block kernel deposits `2^k` through `x` by one `PDEP` and returns the deposit's `countr_zero`. Everywhere else, and
  in every constant expression, it is Vigna's broadword select, which finds the byte holding the one by one multiply
  and one compare and the bit within it from a table, with no loop or branch on the value. The run-time kernel agrees
  with the portable one at every rank. *Test:* `bit/detail/intrinsic.cpp: TheRunTimeKernelsAgreeWithThePortableOnes`,
  `TheInstructionSelectsAsThePortableKernel`.
- *Remarks:* A 64-bit call contains a `pdep` under `-mbmi2`, and under `-march=x86-64-v3` is a `pdep` followed at
  once by `tzcnt`, with no branch or call; it has no `pext` or `pdep` under `-march=x86-64`, `-mgfni`, `bdver4`,
  `znver1` or `znver2`. *Checked by* `codegen/bit_permutation.cpp: codegen.bit_permutation.bmi2`,
  `codegen.bit_permutation.x86-64-v3`, `codegen.bit_permutation.x86-64`, `codegen.bit_permutation.gfni`,
  `codegen.bit_permutation.bdver4`, `codegen.bit_permutation.znver1`, `codegen.bit_permutation.znver2`,
  `codegen.bit_permutation.bmi2-excluded`.
- The portable kernel finds the one the oracle does at every rank, at compile time and at run time, and does so for
  every input at 32 and 64 bits, meeting its precondition wherever the portable expansion calls it.
  *Test:* `bit/detail/portable.cpp: SelectAgreesWithTheOracle`, `KernelsAgreeWithTheOracleAtCompileTime`.
  *Checked by* `proofs/prove.py: proofs.z3`, against a model `proofs/drift.py: proofs.drift` holds to the C++.
- It agrees with the oracle and inverts rank on every input the fuzzer finds, at the lowest and highest ones and the
  rank the mask's population names, at every width from 8 bits up.
  *Checked by* `fuzz/fuzz_bit_permutation.cpp: fuzz.fuzz_bit_permutation`.

## `<xstd/ints/cstdlib.hpp>`

```cpp
namespace xstd {
  template<integer I>
    struct div_result;
  template<integer I>
    div_result(I, I) -> div_result<I>;

  template<integer I>
    constexpr auto sign(I x) noexcept(nothrow_const_operators<I>) -> int;
  template<integer I>
    constexpr auto abs(I x) noexcept(nothrow_const_operators<I>) -> I;
  template<integer I>
    constexpr auto unsigned_abs(I x) noexcept(nothrow_const_operators<I>) -> xstd::make_unsigned_t<I>;

  template<integer I>
    constexpr auto div(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
  template<integer I>
    constexpr auto div_euclid(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
  template<integer I>
    constexpr auto div_floor(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
  template<integer I>
    constexpr auto div_ceil(I numer, I denom) noexcept(nothrow_const_operators<I>) -> div_result<I>;
}
```

`xstd::make_unsigned_t` and `xstd::numeric_limits` below are xstd's, which extend the standard's to every `integer`.

The umbrella exports each header below. *Test:* `cstdlib.cpp: ReExportsTheWholeDirectory`, which names `div_result`
and each of the seven functions through it alone.

Each function is constrained on `integer`, so a character type and `bool`, which are integral, are turned away.
*Test:* `cstdlib.cpp: TurnsAwayCharacterTypesAndBool`, over `char`, `char8_t` and `bool`, beside `signed char` and
`unsigned char`, which each function takes.

Each division header exports `div_result` with it.
*Test:* `cstdlib/div.cpp: TruncatesTowardZero`; `cstdlib/div_euclid.cpp: LeavesANonNegativeRemainder`;
`cstdlib/div_floor.cpp: RoundsTowardNegativeInfinity`; `cstdlib/div_ceil.cpp: RoundsTowardPositiveInfinity`, each
including only its own header and naming `div_result<T>`.

The four divisions take their result vocabulary and their rounding semantics from
[P3724R4](https://wg21.link/P3724R4), and only these four of its rounding modes.
*Definition only:* a citation names the authority for a convention, which each Returns element below states and its
tests hold.

### `div_result`

```cpp
template<integer I>
struct div_result {
  I quotient, remainder;

  friend auto operator==(const div_result&, const div_result&) -> bool = default;
};
```

- An aggregate of two members of type `I`, `quotient` then `remainder`, which designated initializers and a
  structured binding name in that order. It is usable in constant expressions.
  *Test:* `cstdlib/div_result.cpp: AggregateMembersAndStructuredBindings`.
- *Remarks:* The members carry P3724R4's names where `div_t`, `ldiv_t`, `lldiv_t` and `imaxdiv_t` have `quot` and
  `rem`, and one template serves every `integer`. *Definition only:* a name is the declaration itself.
- *Remarks:* It declares no `tuple_size`, `tuple_element` or `get`, so it is not tuple-like: a structured binding
  decomposes its public members, and `apply` does not take it.
  *Test:* `cstdlib/div_result.cpp: IsNotTupleLike`.
- `I` is constrained to model `integer`, so a character type and `bool` are turned away.
  *Test:* `cstdlib/div_result.cpp: TurnsAwayCharacterTypesAndBool`.
- The deduction guide deduces `I` from two arguments of one type, signed or unsigned, at every standard width.
  *Test:* `cstdlib/div_result.cpp: DeducesItsElementTypeFromTheArguments`.
- `operator==`:
  - *Returns:* `x.quotient == y.quotient and x.remainder == y.remainder`, by defaulted memberwise comparison, in a
    constant expression as well. *Test:* `cstdlib/div_result.cpp: AggregateMembersAndStructuredBindings`.
  - *Remarks:* The exception specification is deduced, and is that of `I`'s own `==`.
    *Test:* `cstdlib/div_result.cpp: EqualityTracksTheElementType`, `EqualityTracksTheUnsignedElementType`.

### `sign`

- *Returns:* `-1` if `x` is negative, `0` if it is zero, and `+1` if it is positive, so `0` or `+1` over an unsigned
  type. *Test:* `cstdlib/sign.cpp: IsMinusOneZeroOrPlusOne`, `IsZeroOrPlusOneOnUnsigned`;
  `cstdlib.cpp: SignedExtremes`, `UnsignedExtremes`.
- It is total, `min()` and `max()` included, and a call is a constant expression.
  *Test:* `cstdlib/sign.cpp: IsMinusOneZeroOrPlusOne`, `IsZeroOrPlusOneOnUnsigned`.
- The result has type `int` whatever `I` is.
  *Test:* `cstdlib/sign.cpp: IsMinusOneZeroOrPlusOne`, `IsZeroOrPlusOneOnUnsigned`.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
  *Test:* `cstdlib/sign.cpp: IsNoexceptWhereTheOperatorsAre`.

### `abs`

- *Constraints:* `I` models `integer`. *Test:* `cstdlib.cpp: TurnsAwayCharacterTypesAndBool`.
- *Preconditions:* For a signed `I`, `x != xstd::numeric_limits<I>::min()`, whose negation `I` cannot hold.
  *Untested:* the precondition is an `assert`, and no test violates it.
- *Returns:* `x` if it is not negative, and `-x` otherwise; over an unsigned type, `x`.
  *Test:* `cstdlib/abs.cpp: ReturnsTheMagnitude`, `IsTheIdentityOnUnsigned`; `cstdlib.cpp: SignedExtremes`,
  `UnsignedExtremes`.
- A call is a constant expression. *Test:* `cstdlib/abs.cpp: ReturnsTheMagnitude`, `IsTheIdentityOnUnsigned`.
- *Remarks:* It generalizes `abs`, `labs`, `llabs` and `imaxabs` ([c.math.abs], `constexpr` since
  [P0533R9](https://wg21.link/P0533R9)) to every `integer`, `xstd::int128` among them, and returns `I` rather than
  the promoted type: the magnitude of an `int8_t` is an `int8_t`.
  *Test:* `ints.cpp: TheWidestTypeReachesEveryModule` for the width; `cstdlib/abs.cpp: ReturnsTheMagnitude`,
  `IsTheIdentityOnUnsigned` for the type.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
  *Test:* `concepts/nothrow_const_operators.cpp: IsTheExceptionSpecification`.

### `unsigned_abs`

- *Returns:* The magnitude of `x` as `xstd::make_unsigned_t<I>`: for a signed `I`, `U(0) - U(x)` if `x` is
  negative and `U(x)` otherwise, where `U` is that type; over an unsigned type, `x`. It is the counterpart of Rust's
  `unsigned_abs`, which the standard library has none of.
  *Test:* `cstdlib/unsigned_abs.cpp: ReturnsTheMagnitudeIncludingAtMin`, `IsTheIdentityOnUnsigned`;
  `cstdlib.cpp: SignedExtremes`, `UnsignedExtremes`.
- It is total: the magnitude of `min()` is `U(max()) + 1`, which no signed width holds and the unsigned counterpart
  does. A call is a constant expression.
  *Test:* `cstdlib/unsigned_abs.cpp: ReturnsTheMagnitudeIncludingAtMin`; `cstdlib.cpp: SignedExtremes`.
- Over an unsigned type the result type is `I` itself.
  *Test:* `cstdlib/unsigned_abs.cpp: IsTheIdentityOnUnsigned`.
- Over a signed type the result type is the unsigned counterpart.
  *Test:* `cstdlib/unsigned_abs.cpp: ReturnsTheMagnitudeIncludingAtMin`.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
  *Test:* `cstdlib/unsigned_abs.cpp: IsNoexceptWhereTheOperatorsAre`.

### `div`

- *Preconditions:* `denom != 0`, and for a signed `I`, not both `numer == xstd::numeric_limits<I>::min()` and
  `denom == -1`. Each is undefined behaviour that `/` and `%` already have.
  *Untested:* each precondition is an `assert`, and no test violates one.
- The second precondition does not apply to an unsigned `I`, where `min()` is `0` and `-1` converts to `max()`:
  `div(0, max())` is `{0, 0}`. *Test:* `cstdlib/div.cpp: TruncatesTowardZeroOnUnsigned`.
- *Returns:* `{.quotient = numer / denom, .remainder = numer % denom}`, the truncated division of [expr.mul]: the
  quotient rounds toward zero, `numer == denom * quotient + remainder`, `|remainder| < |denom|`, and a nonzero
  remainder has the sign of `numer`.
  *Test:* `cstdlib/div.cpp: TruncatesTowardZero`, `TruncatesTowardZeroOnUnsigned`;
  `cstdlib.cpp: DivisionOverEveryPair`, over every pair of operands at bit-precise widths 2 through 4.
- A call is a constant expression wherever `I`'s own `/` and `%` are.
  *Test:* `cstdlib/div.cpp: TruncatesTowardZero`, `TruncatesTowardZeroOnUnsigned`.
- Both operands have the one type `I`: a literal divisor beside an integer-class numerator deduces `int` for it, and
  the call is ill-formed rather than converted.
  *Test:* `ext/absl/int128.cpp: NearMissesStayRejected`; `ext/boost/int128.cpp: NearMissesStayRejected`.
- *Remarks:* It generalizes `div`, `ldiv`, `lldiv` and `imaxdiv` ([div.syn], `constexpr` since P0533R9) to every
  `integer`, `xstd::int128` among them, which `div` does not take, and returns `div_result<I>` at the operands' own
  width rather than at a promoted one.
  *Test:* `ints.cpp: TheWidestTypeReachesEveryModule`; `cstdlib/div.cpp: TruncatesTowardZero`, which compares with
  a `div_result<T>`.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
  *Test:* `concepts/nothrow_const_operators.cpp: IsTheExceptionSpecification`.

### `div_euclid`

- *Preconditions:* As for `div`. *Untested:* each precondition is an `assert`, and no test violates one.
- *Returns:* `{.quotient = q, .remainder = numer - denom * q}`, where `q` is the quotient for which
  `0 <= remainder < |denom|`: Euclidean division.
  *Test:* `cstdlib/div_euclid.cpp: LeavesANonNegativeRemainder`, `LeavesANonNegativeRemainderOnUnsigned`;
  `cstdlib.cpp: DivisionOverEveryPair`.
- Over an unsigned `I` it is `div`, and over a signed one it agrees with `div` wherever the division is exact.
  *Test:* `cstdlib.cpp: UnsignedDivisionConventionsAgree`, `ExactDivisionConventionsAgree`.
- A call is a constant expression wherever `I`'s own `/` and `%` are.
  *Test:* `cstdlib/div_euclid.cpp: LeavesANonNegativeRemainder`, `LeavesANonNegativeRemainderOnUnsigned`.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
  *Test:* `cstdlib/div_euclid.cpp: IsNoexceptWhereTheOperatorsAre`.

### `div_floor`

- *Preconditions:* As for `div`. *Untested:* each precondition is an `assert`, and no test violates one.
- *Returns:* `{.quotient = q, .remainder = numer - denom * q}`, where `q` is `numer / denom` rounded toward negative
  infinity: `|remainder| < |denom|`, and a nonzero remainder has the sign of `denom`.
  *Test:* `cstdlib/div_floor.cpp: RoundsTowardNegativeInfinity`, `RoundsTowardNegativeInfinityOnUnsigned`;
  `cstdlib.cpp: DivisionOverEveryPair`.
- Over an unsigned `I` it is `div`, and over a signed one it agrees with `div` wherever the division is exact.
  *Test:* `cstdlib.cpp: UnsignedDivisionConventionsAgree`, `ExactDivisionConventionsAgree`.
- A call is a constant expression wherever `I`'s own `/` and `%` are.
  *Test:* `cstdlib/div_floor.cpp: RoundsTowardNegativeInfinity`, `RoundsTowardNegativeInfinityOnUnsigned`.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
  *Test:* `cstdlib/div_floor.cpp: IsNoexceptWhereTheOperatorsAre`.

### `div_ceil`

- *Preconditions:* As for `div`. *Untested:* each precondition is an `assert`, and no test violates one.
- *Returns:* For a signed `I`, `{.quotient = q, .remainder = numer - denom * q}`, where `q` is `numer / denom`
  rounded toward positive infinity: `|remainder| < |denom|`, and a nonzero remainder has the sign opposite to
  `denom`'s. *Test:* `cstdlib/div_ceil.cpp: RoundsTowardPositiveInfinity`, `KeepsTheDivisionIdentity`;
  `cstdlib.cpp: DivisionOverEveryPair`.
- *Returns:* For an unsigned `I`, the same `q`, with `numer - denom * q` reduced modulo 2<sup>N</sup> for the width
  N of `I` as the remainder, as P3724R4's `div_rem_to_pos_inf` words it: an inexact quotient still rounds up, and
  `numer == denom * quotient + remainder` holds in `I`'s own arithmetic.
  *Test:* `cstdlib/div_ceil.cpp: RoundsTowardPositiveInfinityOnUnsigned`, `KeepsTheDivisionIdentityOnUnsigned`;
  `cstdlib.cpp: DivisionOverEveryPair`.
- The quotient is one more than `div_floor`'s wherever that leaves a remainder, and equal to it elsewhere.
  *Test:* `cstdlib.cpp: DivisionOverEveryPair`.
- A call is a constant expression wherever `I`'s own `/` and `%` are.
  *Test:* `cstdlib/div_ceil.cpp: RoundsTowardPositiveInfinity`, `RoundsTowardPositiveInfinityOnUnsigned`.
- *Remarks:* The exception specification is `nothrow_const_operators<I>`.
  *Test:* `cstdlib/div_ceil.cpp: IsNoexceptWhereTheOperatorsAre`.

## `<xstd/ints/charconv.hpp>`

```cpp
namespace xstd {
  template<class I>
    requires ((integral<I> and not same_as<remove_cv_t<I>, bool>) or integer<I>)
    constexpr size_t to_chars_max_size = see below;

  template<integral I>
    requires (not same_as<remove_cv_t<I>, bool>) and delegates-to-std<I>
    constexpr auto to_chars(char* first, char* last, I value, int base = 10) -> to_chars_result;
  auto to_chars(char* first, char* last, bool value, int base = 10) -> to_chars_result = delete;
  template<integer I>
    requires (not delegates-to-std<I>)
    constexpr auto to_chars(char* first, char* last, I value, int base = 10) -> to_chars_result;
}
```

The umbrella exports it. *Test:* `charconv.cpp: ReExportsTheWholeDirectory`.

### `to_chars_max_size`

- The value is `xstd::numeric_limits<I>::digits`, plus `2` where `I` is signed: one character per value bit, and for
  a signed type one more digit for the magnitude of `min()` and one for the sign.
  *Test:* `charconv/to_chars.cpp: MaxSizeHoldsTheWorstCase`, `MaxSizeHoldsTheWorstCaseUnsigned`.
- A buffer of that many characters holds every value of `I` at every base, and one character fewer does not hold the
  worst case, `min()` in base 2 for a signed type and `max()` for an unsigned one.
  *Test:* `charconv/to_chars.cpp: MaxSizeHoldsTheWorstCase`, `MaxSizeHoldsTheWorstCaseUnsigned`.
- *Constraints:* `I` is a type `to_chars` takes: an integral type other than `bool`, character types included, or
  an `integer`. *Test:* `charconv/to_chars.cpp: MaxSizeHoldsTheWorstCaseChar` for `char`,
  `DelegatesWhereTheStandardLibraryCovers` for `bool`, `const` or not.

### `to_chars`

```cpp
template<class I>
  constexpr bool delegates-to-std =  // exposition only
    integral<I> and
    xstd::numeric_limits<I>::digits + (xstd::numeric_limits<I>::is_signed ? 1 : 0) <=
      xstd::numeric_limits<widest>::digits;
```

where `widest` is `xstd::uint128` where `XSTD_HAS_INT128` is defined and `unsigned long long` elsewhere.

- *Constraints:* The first overload takes an integral type other than `bool` that is no wider, sign bit included,
  than the widest type `to_chars` takes: every standard integer type and character type. A character type, which is
  not an `integer`, reaches it. *Test:* `charconv/to_chars.cpp: StandardIntegralTypesDelegate`.
- *Constraints:* The last takes every `integer` the first does not: every integer-class type, a 128-bit or
  bit-precise type the standard library does not count integral, and one it counts integral but wider than its widest
  type, such as libc++'s `_BitInt(129)`. The two are mutually exclusive.
  *Test:* `charconv/to_chars.cpp: DelegatesWhereTheStandardLibraryCovers`,
  `SignedBitPreciseOnePastTheWidestStandardType`, `NarrowBitPreciseTakesEveryBase`.
- A call with `bool`, `const` or not, is ill-formed, as the standard deletes its overload.
  *Test:* `charconv/to_chars.cpp: DelegatesWhereTheStandardLibraryCovers`.
- *Preconditions:* `2 <= base` and `base <= 36`, and `[first, last)` is a valid range.
  *Untested:* the base precondition is an `assert`, and no test violates either.
- *Effects:* The first overload is equivalent to `return std::to_chars(first, last, value, base);`. Forwarding
  writes what `std::to_chars` writes, so only the output is observable.
  *Test:* `charconv/to_chars.cpp: HexBoundariesMatchGroundTruth`, `HexBoundariesMatchGroundTruthUnsigned`,
  `MaxSizeHoldsTheWorstCase`, `ShortBuffer`, over the standard's types.
- *Effects:* The last writes `value` as [charconv.to.chars] specifies: its digits in `base`, `a` through `z` for the
  values 10 through 35, preceded by `-` where it is negative, with no leading zeros, and `0` for zero.
  *Test:* `charconv/to_chars.cpp: HexBoundariesMatchGroundTruth`, `HexBoundariesMatchGroundTruthUnsigned`,
  `Int128Boundaries`, `NarrowBitPreciseTakesEveryBase`, `SignedBitPreciseOnePastTheWidestStandardType`.
- The two write the same characters for the same value, at every base.
  *Test:* `charconv/to_chars.cpp: Int128RendersAsTheStandardDoes`, which writes zero, ±1, ±7 and the extremes of
  each standard signed type through every signed 128-bit type in the build, Abseil's and Boost's by the last
  overload, and compares with `std::to_chars` over the standard type.
- A type narrower than the radix takes every base: `bit_uint<2>` in base 36, `bit_uint<4>` in base 16.
  *Test:* `charconv/to_chars.cpp: NarrowBitPreciseTakesEveryBase`.
- *Returns:* On success, `{ptr, errc{}}`, where `ptr` is one past the last character written.
  *Test:* `charconv/to_chars.cpp: MaxSizeHoldsTheWorstCase`, `MaxSizeHoldsTheWorstCaseUnsigned`,
  `Int128Boundaries`.
- *Returns:* Where `[first, last)` is too short for the result, an empty range included,
  `{last, errc::value_too_large}`; the contents of `[first, last)` are unspecified.
  *Test:* `charconv/to_chars.cpp: ShortBuffer`, `MaxSizeHoldsTheWorstCase`, `MaxSizeHoldsTheWorstCaseUnsigned`,
  `Int128Boundaries`.
- `base` defaults to `10`. *Test:* `charconv/to_chars.cpp: Int128Boundaries`;
  `ints.cpp: TheWidestTypeReachesEveryModule`.
- A call is a constant expression on either path, for every type whose own arithmetic is `constexpr`.
  *Test:* `charconv/to_chars.cpp: UsableInAConstantExpression`, `UsableInAConstantExpressionPerType`,
  `UsableInAConstantExpressionPerTypeUnsigned`.
- *Remarks:* It is not `noexcept`, as `std::to_chars` is not. *Test:* `charconv/to_chars.cpp: IsNotNoexcept`.

## `<xstd/ints/format.hpp>`

```cpp
namespace std {
  template<xstd::integer I, class charT>
    struct formatter<xstd::div_result<I>, charT>;
}
```

The umbrella exports it. *Test:* `format.cpp: TheFormatterArrivesThroughTheUmbrella`.

### `formatter<xstd::div_result<I>, charT>`

```cpp
template<xstd::integer I, class charT>
struct formatter<xstd::div_result<I>, charT> : see below {
  template<class FormatContext>
    constexpr auto format(const xstd::div_result<I>& d, FormatContext& ctx) const -> typename FormatContext::iterator;
};
```

- Its base, which supplies `parse` and with it the grammar of the format specification, is the first of these that
  applies: `formatter<tuple<const I&, const I&>, charT>` where that tuple is formattable;
  `formatter<tuple<D, D>, charT>` where the library formats tuples, for an exposition-only `D` holding the digits
  `xstd::to_chars` writes, which its own formatter copies out unchanged; and `formatter<basic_string_view<charT>,
  charT>` otherwise. *Test:* `format/div_result.cpp: TupleSpecs`, over `int` and `absl::int128`, which the library
  does not format.
- `parse`:
  - *Effects:* Where the library formats tuples, accepts the tuple specification of [format.tuple], fill and align,
    width, and `n` or `m`, whatever `I` is.
    *Test:* `format/div_result.cpp: FillAndAlign`, `TupleSpecs`.
  - *Effects:* On a library that formats no tuple, accepts a string's specification.
    *Untested:* every standard library CI builds with formats tuples.
  - *Throws:* `format_error` for any other specification, which a format string checked at compile time makes an
    ill-formed call. *Test:* `format/div_result.cpp: RejectsAnElementSpec`, with `x` and a precision, over `int`
    and `absl::int128`.
- `format`:
  - *Effects:* Writes `(`, the quotient, `, `, the remainder and `)`, each member in decimal as `xstd::to_chars`
    writes it; `n` leaves out the brackets, and `m` leaves them out and writes `: ` as the separator. The fill,
    alignment and width then apply to the whole.
    *Test:* `format/div_result.cpp: Formatter`, `Boundaries`, `FillAndAlign`, `TupleSpecs`;
    `ints.cpp: TheWidestTypeReachesEveryModule`.
  - The text is the same whichever base applies, and over every signed exact-width type, standard, 128-bit and
    bit-precise. *Test:* `format/div_result.cpp: Formatter`, `TupleSpecs`.
  - The text is the same for a wide format string. *Test:* `format/div_result.cpp: WideFormatter`, over `int` and
    `absl::int128`.
  - The text is the same over an unsigned element type, `std::uint8_t` included, whose members are written as
    numbers rather than as characters. *Test:* `format/div_result.cpp: FormatterOnUnsigned`.
  - *Remarks:* It is `constexpr` where `__cpp_lib_constexpr_format` is at least `202511L`, as
    [P3391R3](https://wg21.link/P3391R3) makes the base's, and not otherwise.
    *Test:* `format/div_result.cpp: Formatter`, which asserts the text at compile time where the macro says so.

## `<xstd/ints/memory.hpp>`

```cpp
namespace xstd {
  template<alignable T>
    constexpr auto align_down(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> T;
  template<class T>
    auto align_down(T* pointer, size_t alignment) noexcept -> T*;
  template<alignable T>
    constexpr auto align_up(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> T;
  template<class T>
    auto align_up(T* pointer, size_t alignment) noexcept -> T*;
  template<alignable T>
    constexpr auto is_aligned(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> bool;
  template<class T>
    auto is_aligned(T* pointer, size_t alignment) noexcept -> bool;
}
```

The umbrella exports the three headers under `<xstd/ints/memory/>`. The three agree with Boost.Align on values at
every standard unsigned width and on pointers, and what `align_up` and `align_down` return satisfies `is_aligned`.
*Test:* `memory.cpp: AgreesWithBoostAlignment`, `AgreesWithBoostAlignmentAtEveryWidth`,
`ThePredicateAgreesWithTheTwoAdjustments`, `PointerOverloadsAgreeWithBoostAlignment`.

### `align_up`

- `align_up(T value, size_t alignment)`:
  - *Constraints:* `T` models `alignable`. A signed integer-class type is turned away rather than converted.
    *Test:* `ext/boost/int128.cpp: NearMissesStayRejected`; `ext/absl/int128.cpp: NearMissesStayRejected`.
  - *Preconditions:* `alignment` is a power of two, and `value + (alignment - 1)` does not wrap in `T`, so the result
    is representable. *Untested:* each is an `assert`, and no case violates it.
  - *Returns:* The least multiple of `alignment` not less than `value`, as a `T`, computed as `s - (s & m)` for
    `m = T(alignment - 1)` and `s = value + m`. A narrow type is returned as itself rather than promoted. It is a
    constant expression.
    *Test:* `memory/align_up.cpp: RoundsUpToTheNextMultiple`, `KeepsTheNarrowOperandType`;
    `memory.cpp: AgreesWithBoostAlignment`, `AgreesWithBoostAlignmentAtEveryWidth`.
  - It is exact up to the greatest multiple of `alignment` that `size_t` represents.
    *Test:* `memory/align_up.cpp: RoundsUpToTheNextMultiple`.
  - It takes `uint128`. *Test:* `ints.cpp: TheAlignmentFunctionsMeetTheConcepts`.
  - *Remarks:* The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not
    for `absl::uint128`. *Test:* `memory/align_up.cpp: IsNoexceptExactlyWhereTheTypeIsNothrowAlignable`.
- `align_up(T* pointer, size_t alignment)`:
  - *Effects:* Equivalent to `return reinterpret_cast<T*>(align_up(reinterpret_cast<uintptr_t>(pointer), alignment));`
    so it answers as Boost.Align's pointer form does and has the integer form's preconditions.
    *Test:* `memory.cpp: PointerOverloadsAgreeWithBoostAlignment`.
  - It returns a `T*`, keeping the pointee type where Boost.Align returns `void*`.
    *Test:* `memory.cpp: PointerOverloadsKeepThePointeeType`.
  - *Remarks:* It is not `constexpr`, a `reinterpret_cast` being barred from constant expressions, and it is
    `noexcept`. *Definition only:* neither has a counterpart a test could contrast.
  - *Remarks:* The result is an address: it is not checked against the object `pointer` points into, and no
    object's lifetime begins there. *Definition only:* a statement of what the function does not do.

### `align_down`

- `align_down(T value, size_t alignment)`:
  - *Constraints:* `T` models `alignable`. *Test:* `concepts/alignable.cpp: AndRejectsTheRestOnTheirMerits`.
  - *Preconditions:* `alignment` is a power of two. *Untested:* it is an `assert`, and no case violates it.
  - *Returns:* The greatest multiple of `alignment` not greater than `value`, as a `T`, computed as `value - (value &
    m)` for `m = T(alignment - 1)`. A narrow type is returned as itself. It is a constant expression.
    *Test:* `memory/align_down.cpp: RoundsDownToThePrecedingMultiple`, `KeepsTheNarrowOperandType`;
    `memory.cpp: AgreesWithBoostAlignment`, `AgreesWithBoostAlignmentAtEveryWidth`.
  - It is exact at the maximum of `size_t`. *Test:* `memory/align_down.cpp: RoundsDownToThePrecedingMultiple`.
  - *Remarks:* The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not
    for `absl::uint128`. *Test:* `memory/align_down.cpp: IsNoexceptExactlyWhereTheTypeIsNothrowAlignable`.
- `align_down(T* pointer, size_t alignment)`:
  - *Effects:* Equivalent to
    `return reinterpret_cast<T*>(align_down(reinterpret_cast<uintptr_t>(pointer), alignment));`.
    *Test:* `memory.cpp: PointerOverloadsAgreeWithBoostAlignment`.
  - It returns a `T*`. *Test:* `memory.cpp: PointerOverloadsKeepThePointeeType`.
  - *Remarks:* As for `align_up`'s pointer form. *Definition only:* as there.

### `is_aligned`

- `is_aligned(T value, size_t alignment)`:
  - *Constraints:* `T` models `alignable`. *Test:* `concepts/alignable.cpp: AndRejectsTheRestOnTheirMerits`.
  - *Preconditions:* `alignment` is a power of two. *Untested:* it is an `assert`, and no case violates it.
  - *Returns:* `true` exactly when `value` is a multiple of `alignment`, computed as `T(value & m) == T(0UZ)` for
    `m = T(alignment - 1)`, as a `bool` whatever `T` is. It is a constant expression.
    *Test:* `memory/is_aligned.cpp: HoldsWhenTheRemainderIsZero`, `AnswersInBool`.
  - It holds of what `align_up` and `align_down` return, and of `value` exactly when `align_down` leaves it unchanged.
    *Test:* `memory.cpp: ThePredicateAgreesWithTheTwoAdjustments`.
  - *Remarks:* The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not
    for `absl::uint128`. *Test:* `memory/is_aligned.cpp: IsNoexceptExactlyWhereTheTypeIsNothrowAlignable`.
- `is_aligned(T* pointer, size_t alignment)`:
  - *Returns:* `is_aligned(reinterpret_cast<uintptr_t>(pointer), alignment)`.
    *Test:* `memory.cpp: PointerOverloadsAgreeWithBoostAlignment`, `PointerOverloadsKeepThePointeeType`.
  - *Remarks:* It is not `constexpr`, and it is `noexcept`. *Definition only:* as for `align_up`'s pointer form.

## `<xstd/ints/ext/boost.hpp>`

```cpp
#include <boost/int128.hpp>
#include <boost/hash2/hash_append.hpp>

namespace xstd {
  template<> struct make_unsigned<boost::int128::int128>;
  template<> struct make_signed<boost::int128::uint128>;

  template<class Hash, class Flavor, integer T>
    constexpr auto hash_append_int(Hash& h, const Flavor& f, T v) -> void;
}
```

The umbrella exports `<xstd/ints/ext/boost/int128.hpp>` and `<xstd/ints/ext/boost/hash2.hpp>`, and each exports the
Boost header it adapts, so including one is enough to name Boost's types and flavors. No header of the library
includes it: an adapted library is asked for by name. Through it, Boost.Int128's pair is paired and hashes as xstd's
own 128-bit pair does. *Test:* `ext/boost.cpp: TheAssociationsArriveThroughTheUmbrella`,
`TheirPairHashesAsTheLibrarysOwn`.

### `make_unsigned<boost::int128::int128>`, `make_signed<boost::int128::uint128>`

- The member `type` is the other half of Boost.Int128's pair. With the integer-class specializations each half is its
  own counterpart, the round trip is the identity, the cv-qualifiers carry over, and the pair's limits have the range
  of [iterator.concept.winc]/3. *Test:* `ext/boost/int128.cpp: AssociationsPairTheTwoHalves`.
- With them, `boost::int128::int128` models `signed_integer` and `boost::int128::uint128` models `unsigned_integer`,
  so every facility constrained on the integer concepts takes them; each case templated over the exact-width lists
  runs over them. *Test:* `ext/boost/int128.cpp: WhichIsWhatCarriesThePairIntoTheConcepts`;
  `concepts/integer.cpp: AdmitsTheExactWidthTypesThroughCv`; `type_traits.cpp: TheTraitsAndTheModifiersAgree`.
- The mistakes that port from built-in code stay compile errors for the signed half: `div` with an `int` literal
  divisor, `popcount`, and `align_up`.
  *Test:* `ext/boost/int128.cpp: NearMissesStayRejected`.
- *Remarks:* The specializations name the types without the `_t` suffix, the spelling after Boost.Int128's last
  release tag. *Untested:* the test build pins one commit of Boost.Int128.
- *Remarks:* The header carries `IWYU pragma: always_keep`. Boost declares the pair in `detail/` headers that no
  public Boost header re-exports, so the export stops one hop short of the declarations.
  *Untested:* include analysis is not a test case.
- The header compiles on its own wherever Boost.Int128 is found.
  *Checked by* the header self-sufficiency build in `test/CMakeLists.txt`.

### `hash_append_int`

```cpp
template<class Hash, class Flavor, integer T>
  constexpr auto hash_append_int(Hash& h, const Flavor& f, T v) -> void;
```

- *Constraints:* `T` models `integer`. `char` and `bool`, which Hash2's own `hash_append` treats as integers, are
  turned away. *Test:* `ext/boost/hash2.cpp: TakesIntegersAndNotTheCharacterTypesOrBool`.
- *Effects:* Let `w` be `numeric_limits<T>::digits + (is_signed_v<T> ? 1 : 0)`.
  - If `w <= 64`, calls `boost::hash2::hash_append(h, f, static_cast<N>(v))`, where `N` is the first of `uint8_t`,
    `uint16_t`, `uint32_t` and `uint64_t` with at least `w` digits, made signed when `T` is signed. So every name for
    a standard width hashes as the exact-width type of that width, and a bit-precise type as the narrowest standard
    type that holds it. *Test:* `ext/boost/hash2.cpp: StandardNamesHashAsTheExactWidthTypeOfTheirWidth`,
    `BitPreciseHashesAsTheStandardTypeThatHoldsIt`.
  - Otherwise, with `u` the value converted to `make_unsigned_t<T>`, calls
    `boost::hash2::hash_append(h, f, static_cast<uint64_t>(u >> 64 * k))` for each `k` below `w / 64`, low word first,
    and then, if `w % 64` is not zero, appends one more `uint64_t` holding the remaining `w % 64` bits, sign-extended
    when `T` is signed and zero-extended otherwise.
    *Test:* `ext/boost/hash2.cpp: BitPreciseAboveAWordHashesAsWholeWords`, `Every128BitTypeGivesTheSameDigests`.
- Under `little_endian_flavor` the message is the value's two's-complement bytes, least significant first, for
  every exact-width type and the widths 2, 24, 65, 100 and 129 the build reaches, at run time and in constant
  evaluation. *Test:* `ext/boost/hash2.cpp: EveryValueIsItsLittleEndianBytes`, `AndSoInConstantEvaluation`.
- Every 128-bit type, `int128`, Boost's, Abseil's and `_BitInt(128)`, gives the same FNV-1a digests, pinned against
  values computed outside the library, and under `big_endian_flavor` the words stay low first while the bytes within
  each reverse. *Test:* `ext/boost/hash2.cpp: Every128BitTypeGivesTheSameDigests`.
- Only the value is hashed: the padding of a `bit_int<24>` never reaches the message, under the default flavor too.
  *Test:* `ext/boost/hash2.cpp: PaddingNeverReachesTheMessage`.
- *Throws:* Any exception `hash_append` or `h.update` throws. It is not `noexcept`.
  *Test:* `ext/boost/hash2.cpp: TakesIntegersAndNotTheCharacterTypesOrBool`.
- *Remarks:* It is a function to call in place of `boost::hash2::hash_append`, not a customization Hash2 finds.
  *Definition only:* a fundamental type and a class in namespace `std` have no namespace in which a hook could be
  found.
- The header compiles on its own wherever Boost.Hash2 is found.
  *Checked by* the header self-sufficiency build in `test/CMakeLists.txt`.

## `<xstd/ints/ext/absl.hpp>`

```cpp
#include <absl/numeric/int128.h>

namespace xstd {
  template<> struct make_unsigned<absl::int128>;
  template<> struct make_signed<absl::uint128>;
}
```

The umbrella exports `<xstd/ints/ext/absl/int128.hpp>`, which exports `<absl/numeric/int128.h>`, so including either
is enough to name Abseil's pair. No header of the library includes it.
*Test:* `ext/absl.cpp: TheAssociationsArriveThroughTheUmbrella`.

### `make_unsigned<absl::int128>`, `make_signed<absl::uint128>`

- The member `type` is the other half of Abseil's pair. With the integer-class specializations each half is its own
  counterpart, the round trip is the identity, the cv-qualifiers carry over, and the pair's limits have the range of
  [iterator.concept.winc]/3. *Test:* `ext/absl/int128.cpp: AssociationsPairTheTwoHalves`.
- With them, `absl::int128` models `signed_integer` and `absl::uint128` models `unsigned_integer`, and each case
  templated over the exact-width lists runs over them.
  *Test:* `ext/absl/int128.cpp: WhichIsWhatCarriesThePairIntoTheConcepts`;
  `concepts/integer.cpp: AdmitsTheExactWidthTypesThroughCv`.
- The mistakes that port from built-in code stay compile errors for the signed half: `div` with an `int` literal
  divisor, `popcount`, and `align_up`. *Test:* `ext/absl/int128.cpp: NearMissesStayRejected`.
- Abseil declares no `noexcept`, so the pair models neither `nothrow_const_operators` nor, for `absl::uint128`,
  `nothrow_alignable`, and the integer functions over it are not `noexcept`.
  *Test:* `concepts/nothrow_const_operators.cpp: HoldsForTheTypesThatDeclareIt`, `IsTheExceptionSpecification`;
  `concepts/alignable.cpp: TheNothrowRefinementNarrowsIt`.
- The pair hashes as every other 128-bit type does. *Test:* `ext/boost/hash2.cpp: Every128BitTypeGivesTheSameDigests`.
- *Remarks:* Where Abseil has no 128-bit intrinsic, its `operator/` and `operator%` are not `constexpr`, and neither
  is a function that divides one. *Untested:* the test lists leave the type out of their constant-evaluated cases
  there, and nothing asserts the absence.
- The header and its umbrella each compile on their own wherever Abseil is found.
  *Checked by* the header self-sufficiency builds in `test/CMakeLists.txt`.

## `<xstd/ints.hpp>`

```cpp
#include <xstd/ints/bit.hpp>
#include <xstd/ints/charconv.hpp>
#include <xstd/ints/concepts.hpp>
#include <xstd/ints/cstdint.hpp>
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/format.hpp>
#include <xstd/ints/limits.hpp>
#include <xstd/ints/memory.hpp>
#include <xstd/ints/type_traits.hpp>
```

- It exports every domain umbrella, and through it `int128` reaches the concepts, `numeric_limits`, `abs`, `div`,
  `to_chars` with `to_chars_max_size`, and the formatting of `div_result`.
  *Test:* `ints.cpp: TheWidestTypeReachesEveryModule`, where `XSTD_HAS_INT128` is defined.
- Through it the alignment functions take `size_t` and `uint128`, which model `alignable`, and turn away `int128`,
  which does not. *Test:* `ints.cpp: TheAlignmentFunctionsMeetTheConcepts`.
- It does not export `<xstd/ints/ext/absl.hpp>` or `<xstd/ints/ext/boost.hpp>`, each of which hard-includes the
  library it adapts, nor any header under `<xstd/ints/ext/>`. *Test:* `ints.cpp: ExportsNoAdaptor`.
