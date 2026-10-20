# `<xstd/ints/limits/numeric_limits.hpp>`

## `numeric_limits`

- For an object type that is not an array, it derives from `std::numeric_limits<T>` and so gives the same answers; for a
  cv-qualified type it derives from the specialization for the unqualified type.
- For `void`, a reference, an array, a cv-qualified array and a function type it is an empty class, so naming a member
  is a substitution failure rather than an error inside `std::numeric_limits`.
- The cv-qualified specializations forward to a specialization of xstd's own, not only to the standard library's:
  `bit_int<17> const` and `signed _BitInt(24) const` read the `_BitInt` digits.
- *Remarks:* A program may specialize it for a type of its own; the cv-qualified forms then follow.

## `numeric_limits<signed _BitInt(N)>`, `numeric_limits<unsigned _BitInt(N)>`

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
- `digits`, `min()` and `max()` follow the requested width, not the object's size, and are constant expressions: at 17
  bits, 16 and 17 digits, a signed range of −65536 to 65535 and an unsigned maximum of 131071. On libc++, which
  specializes `std::numeric_limits` for `_BitInt` with `digits` taken from `sizeof`, these replace its values.
- `digits10` is `floor(digits * log10(2))`, the most decimal digits the type represents without change: 7 for
  `unsigned _BitInt(24)` and 6 for `signed _BitInt(24)`, and at 8 and 64 bits the value `std::numeric_limits` gives the
  exact-width type of that width. It replaces libstdc++'s zero and libc++'s value for the width of the object.
- *Remarks:* `digits10` is computed in 64.64 fixed point, which is exact for every `digits` up to 2^23, and the header
  is ill-formed where `__BITINT_MAXWIDTH__` exceeds 8'388'608.
- `is_specialized`, `is_integer` and `radix == 2` hold, as `integer_class` requires.
- `is_signed` follows the type's signedness, `is_exact` and `is_bounded` are `true`, `is_modulo` is `false` for the
  signed type and `true` for the unsigned, and `lowest()` is `min()`.
- `max_digits10` is inherited, and is 0, as for every integer.
- *Remarks:* Every other member is inherited from `std::numeric_limits<_BitInt(N)>`: the primary template's where the
  standard library does not specialize it, as libstdc++ does not, and libc++'s where it does.
