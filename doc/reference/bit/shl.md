# `<xstd/ints/bit/shl.hpp>`

This page also specifies `<xstd/ints/bit/shr.hpp>`, whose functions share this text.

## `xstd::shl`, `xstd::shr`

```cpp
// (1)
template<class T, class S>
  constexpr auto shl(T x, S s) noexcept(see below) -> T;

// (2)
template<class T, class S>
  constexpr auto shr(T x, S s) noexcept(see below) -> T;
```

Shift an integer by any count of any integer type, with the value semantics of multiplying by a power of two.

1. `shl` shifts to the left.
2. `shr` shifts to the right.

In the following, let `W` be `numeric_limits<make_unsigned_t<T>>::digits`, the width of `T` with any sign bit.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | the value to shift |
| `s` | the count to shift by, which may be negative |

*Constraints:* Either the standard library defines `__cpp_lib_bitops` as at least `202606L` and `std::shl(x, s)` and
`std::shr(x, s)` are both well-formed; or `T` and `S` both model `integer`. Either operand may be signed or unsigned,
the two of different types, an integer class or bit-precise; `bool` and a character type are refused on either side.

### Return value

Where the standard functions take the pair, each returns `std::shl(x, s)` or `std::shr(x, s)`.

1. *Returns:* The value of x × 2<sup>s</sup>, rounded towards negative infinity, reduced modulo 2<sup>W</sup> to a
   value of `T`.
2. *Returns:* The value of x × 2<sup>−s</sup>, rounded towards negative infinity, reduced modulo 2<sup>W</sup> to a
   value of `T`.

### Exceptions

The exception specification is `true` for the forwarding overload, and otherwise
`nothrow_const_operators<T> and nothrow_const_operators<S>`.

### Notes

- Every value of `S` is in the domain: a count of magnitude `W` or more gives `0` towards the left, and towards the
  right `0` for a value that is not negative and `-1` for one that is, the most negative count of a signed `S` included.
- The count's type changes nothing but which counts it can hold.
- `shr(x, s)` equals `shl(x, -s)` wherever `-s` is representable.
- A call is a constant expression.

### See also

| Entity | Relation |
| :----- | :------- |
| [`rotl`, `rotr`](rotl.md) | rotate rather than shift |
| [`integer`](../concepts/integer.md) | what the template takes, on either side |

## Notation for the bit permutations

In the entries for the four permutations and `bit_select`, as in
[P3104R5](https://github.com/cplusplus/papers/issues/1768)'s [[bit.permute]/1](https://eel.is/c++draft/bit.permute#1),
`N` is `numeric_limits<T>::digits`, xstd's, which answers for the integer classes and the bit-precise types too, and
`α[n]` is bit `n` of `α`, counted from zero at the least significant end, so that `α` is the sum of `α[n]·2^n` over `n`
in `[0, N)`. A block is one `size_t`.
