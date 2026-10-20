# `<xstd/ints/charconv/to_chars.hpp>`

## `to_chars_max_size`

- The value is `xstd::numeric_limits<I>::digits`, plus `2` where `I` is signed: one character per value bit, and for a
  signed type one more digit for the magnitude of `min()` and one for the sign.
- A buffer of that many characters holds every value of `I` at every base, and one character fewer does not hold the
  worst case, `min()` in base 2 for a signed type and `max()` for an unsigned one.
- *Constraints:* `I` is a type `to_chars` takes: `char` or an `integer`.

## `to_chars`

```cpp
template<class I>
  constexpr bool delegates-to-std =  // exposition only
    integral<I> and
    xstd::numeric_limits<I>::digits + (xstd::numeric_limits<I>::is_signed ? 1 : 0) <=
      xstd::numeric_limits<widest>::digits;

template<class I>
  concept std-writes-chars =  // exposition only
    delegates-to-std<I> and (same_as<I, char> or integer<I>) and
    requires (char* first, I value, int base) { std::to_chars(first, first, value, base); };
```

where `widest` is `xstd::uint128` where `XSTD_HAS_INT128` is defined and `unsigned long long` elsewhere.

- *Constraints:* The first overload takes `char` and every `integer` that is integral, no wider, sign bit included, than
  `widest`, and that `std::to_chars` takes. [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) declares overloads for the integer types and
  `char`, so these are every standard integer type and `char`.
- *Constraints:* The last takes every `integer` the first does not: every integer-class type, a 128-bit or bit-precise
  type the standard library does not count integral or gives no `std::to_chars`, and one it counts integral but wider
  than `widest`, such as libc++'s `_BitInt(129)`. The two are mutually exclusive.
- A call with `wchar_t`, `char8_t`, `char16_t` or `char32_t` is ill-formed. `std::to_chars` would take each only by
  integral promotion, writing the code unit as a number.
- A call with `bool`, `const` or not, is ill-formed, as the standard deletes its overload.
- *Preconditions:* `2 <= base` and `base <= 36`, and `[first, last)` is a valid range.
- *Effects:* The first overload is equivalent to `return std::to_chars(first, last, value, base);`. Forwarding writes
  what `std::to_chars` writes, so only the output is observable.
- *Effects:* The last writes `value` as [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) specifies: its digits in `base`, `a` through `z` for the
  values 10 through 35, preceded by `-` where it is negative, with no leading zeros, and `0` for zero.
- The two write the same characters for the same value, at every base.
- A type narrower than the radix takes every base: `bit_uint<2>` in base 36, `bit_uint<4>` in base 16.
- *Returns:* On success, `{ptr, errc{}}`, where `ptr` is one past the last character written.
- *Returns:* Where `[first, last)` is too short for the result, an empty range included,
  `{last, errc::value_too_large}`; the contents of `[first, last)` are unspecified.
- `base` defaults to `10`.
- A call is a constant expression on either path, for every type whose own arithmetic is `constexpr`.
- *Remarks:* It is not `noexcept`, as `std::to_chars` is not.
