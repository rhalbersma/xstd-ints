# `<xstd/ints/format/div_result.hpp>`

## `std::formatter<xstd::div_result<I>, charT>`

```cpp
template<xstd::integer I, class charT>
struct formatter<xstd::div_result<I>, charT> : see below {
  template<class FormatContext>
    constexpr auto format(const xstd::div_result<I>& d, FormatContext& ctx) const -> typename FormatContext::iterator;
};
```

Formats a `div_result` as its quotient and remainder, each in decimal.

Its base, which supplies `parse` and with it the grammar of the format specification, is the first of these that
applies: `formatter<tuple<const I&, const I&>, charT>` where that tuple is formattable; `formatter<tuple<D, D>, charT>`
where the library formats tuples, for an exposition-only `D` holding the digits `xstd::to_chars` writes, which its own
formatter copies out unchanged; and `formatter<basic_string_view<charT>, charT>` otherwise.

### Member functions

- `parse`:
    - *Effects:* Where the library formats tuples, accepts the tuple specification of
      [[format.tuple]](https://eel.is/c++draft/format.tuple), fill and align, width, and `n` or `m`, whatever `I` is.
    - *Effects:* On a library that formats no tuple, accepts a string's specification.
    - *Throws:* `format_error` for any other specification, which a format string checked at compile time makes an
      ill-formed call.
- `format`:
    - *Effects:* Writes `(`, the quotient, `, `, the remainder and `)`, each member in decimal as `xstd::to_chars` writes
      it; `n` leaves out the brackets, and `m` leaves them out and writes `: ` as the separator. The fill, alignment and
      width then apply to the whole.
    - *Remarks:* It is `constexpr` where `__cpp_lib_constexpr_format` is at least `202511L`, as
      [P3391R2](https://github.com/cplusplus/papers/issues/2046) makes the base's, and not otherwise.

### Notes

- The text is the same whichever base applies, and over every signed exact-width type, standard, 128-bit and
  bit-precise.
- The text is the same for a wide format string.
- The text is the same over an unsigned element type, `std::uint8_t` included, whose members are written as numbers
  rather than as characters.

### See also

| Entity | Relation |
| :----- | :------- |
| [`div_result`](../cstdlib/div_result.md) | the class this formats |
| [`to_chars`](../charconv/to_chars.md) | writes each member |
| [`std::formatter`](https://en.cppreference.com/w/cpp/utility/format/formatter) | the standard's formatter, which this specializes |
