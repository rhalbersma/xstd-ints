# `<xstd/ints/charconv/to_chars.hpp>`

## `xstd::to_chars_max_size`

```cpp
template<class I>
  requires (same_as<remove_cv_t<I>, char> or integer<I>)
  constexpr size_t to_chars_max_size = see below;
```

The size of a buffer that `to_chars` cannot overflow for a value of type `I`.

### Template parameters

| Parameter | Meaning |
| :-------- | :------ |
| `I` | the type of the value written |

*Constraints:* `I` is a type `to_chars` takes: `char` or an `integer`.

### Value

`xstd::numeric_limits<I>::digits`, plus `2` where `I` is signed: one character per value bit, and for a signed type one
more digit for the magnitude of `min()` and one for the sign.

### Notes

A buffer of that many characters holds every value of `I` at every base, and one character fewer does not hold the
worst case, `min()` in base 2 for a signed type and `max()` for an unsigned one.

### See also

| Entity | Relation |
| :----- | :------- |
| [`to_chars`](#xstdto_chars) | the function whose output it bounds |
| [`numeric_limits`](../limits/numeric_limits.md) | the `digits` it counts |

## `xstd::to_chars`

```cpp
// (1)
template<integral I>
  requires std-writes-chars<I>
  constexpr auto to_chars(char* first, char* last, I value, int base = 10) -> to_chars_result;

// (2)
auto to_chars(char* first, char* last, bool value, int base = 10) -> to_chars_result = delete;

// (3)
template<integer I>
  requires (not std-writes-chars<I>)
  constexpr auto to_chars(char* first, char* last, I value, int base = 10) -> to_chars_result;
```

Writes the digits of an integer into a character buffer, in a base from 2 to 36.

1. Forwards to `std::to_chars`, for the types it takes.
2. A call with `bool`, `const` or not, is ill-formed, as the standard deletes its overload.
3. Writes the digits itself, for every other `integer`.

The constraints are written in two exposition-only names:

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

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `first`, `last` | the buffer to write into |
| `value` | the value to write |
| `base` | the base to write it in; `base` defaults to `10` |

- *Constraints:* The first overload takes `char` and every `integer` that is integral, no wider, sign bit included,
  than `widest`, and that `std::to_chars` takes. [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars)
  declares overloads for the integer types and `char`, so these are every standard integer type and `char`.
- *Constraints:* The last takes every `integer` the first does not: every integer-class type, a 128-bit or bit-precise
  type the standard library does not count integral or gives no `std::to_chars`, and one it counts integral but wider
  than `widest`, such as libc++'s `_BitInt(129)`. The two are mutually exclusive.
- *Preconditions:* `2 <= base` and `base <= 36`, and `[first, last)` is a valid range.

### Return value

- *Effects:* The first overload is equivalent to `return std::to_chars(first, last, value, base);`. Forwarding writes
  what `std::to_chars` writes, so only the output is observable.
- *Effects:* The last writes `value` as [[charconv.to.chars]](https://eel.is/c++draft/charconv.to.chars) specifies: its
  digits in `base`, `a` through `z` for the values 10 through 35, preceded by `-` where it is negative, with no leading
  zeros, and `0` for zero.
- *Returns:* On success, `{ptr, errc{}}`, where `ptr` is one past the last character written.
- *Returns:* Where `[first, last)` is too short for the result, an empty range included,
  `{last, errc::value_too_large}`; the contents of `[first, last)` are unspecified.

### Exceptions

It is not `noexcept`, as `std::to_chars` is not.

### Notes

- A call with `wchar_t`, `char8_t`, `char16_t` or `char32_t` is ill-formed. `std::to_chars` would take each only by
  integral promotion, writing the code unit as a number.
- The two write the same characters for the same value, at every base.
- A type narrower than the radix takes every base: `bit_uint<2>` in base 36, `bit_uint<4>` in base 16.
- A call is a constant expression on either path, for every type whose own arithmetic is `constexpr`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`to_chars_max_size`](#xstdto_chars_max_size) | a buffer size it cannot overflow |
| [`integer`](../concepts/integer.md) | what (3) takes |
| [`formatter<div_result>`](../format/div_result.md) | writes each member as it writes it |
| [`std::to_chars`](https://en.cppreference.com/w/cpp/utility/to_chars) | the standard function (1) forwards to |
