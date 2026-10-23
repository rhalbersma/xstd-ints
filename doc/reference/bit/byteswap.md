# `<xstd/ints/bit/byteswap.hpp>`

## `xstd::byteswap`

```cpp
template<class T>
  constexpr auto byteswap(T x) noexcept(see below) -> T;
```

Reverses the bytes of an integer's value, for every `integer` whose width is a whole number of bytes.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | the value whose bytes are reversed |

*Constraints:* Either `std::byteswap(x)` is well-formed, `T` is integral and no wider, sign bit included, than the
widest unsigned type xstd hands to the standard library (`uint128` where `XSTD_HAS_INT128` is defined, and
`unsigned long long` otherwise), and `has_unique_object_representations_v<T>` is `true`; or `T` models `integer` and
`numeric_limits<make_unsigned_t<T>>::digits` is a multiple of `numeric_limits<unsigned char>::digits`. So
`bit_int<24>` and `bit_uint<24>` are taken and `bit_int<13>` and `bit_uint<13>` are not, and `bool` and `char8_t` are
taken exactly where `std::byteswap` takes them.

### Return value

- *Returns:* `std::byteswap(x)` where the first branch of the constraint holds.
- *Returns:* Otherwise `static_cast<T>(u)`, where `u` is the value of `static_cast<make_unsigned_t<T>>(x)` with its
  bytes in reverse order: the byte `n` places from the least significant end lands `n` places from the most significant
  end.

### Exceptions

The exception specification is `true` for the forwarding overload, and otherwise
`nothrow_const_operators<T> and nothrow_const_operators<make_unsigned_t<T>>`.

### Remarks

[[bit.byteswap]](https://eel.is/c++draft/bit.byteswap) reverses the object representation and mandates that `T` have
no padding bits. The template reverses the value representation, so it takes a type with padding bits that the
standard function's Mandates element rejects, and the constraint keeps every such type away from `std::byteswap`,
including the `_BitInt` widths libc++ calls integral and then refuses to swap.

### Notes

- Swapping twice is the identity.
- A signed `T` is swapped through its unsigned counterpart, as `std::byteswap` swaps a signed type.
- A width of whole bytes that fills no whole object, such as `bit_uint<24>`, is swapped as its value, three bytes where
  the object has four.
- A call is a constant expression.

### See also

| Entity | Relation |
| :----- | :------- |
| [`bit_reverse`](bit_reverse.md) | reverses the bits rather than the bytes |
| [`bit_int`, `bit_uint`](../cstdint/bit_int.md) | the bit-precise types, swapped as their value |
| [`std::byteswap`](https://en.cppreference.com/w/cpp/numeric/byteswap) | the standard function it forwards to |
