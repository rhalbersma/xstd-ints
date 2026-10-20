# `<xstd/ints/ext/boost/hash2.hpp>`

## `hash_append_int`

```cpp
template<class Hash, class Flavor, integer T>
  constexpr auto hash_append_int(Hash& h, const Flavor& f, T v) -> void;
```

- *Constraints:* `T` models `integer`. `char` and `bool`, which Hash2's own `hash_append` treats as integers, are turned
  away.
- *Effects:* Let `w` be `numeric_limits<T>::digits + (is_signed_v<T> ? 1 : 0)`.
  - If `w <= 64`, calls `boost::hash2::hash_append(h, f, static_cast<N>(v))`, where `N` is the first of `uint8_t`,
    `uint16_t`, `uint32_t` and `uint64_t` with at least `w` digits, made signed when `T` is signed. So every name for a
    standard width hashes as the exact-width type of that width, and a bit-precise type as the narrowest standard type
    that holds it.
  - Otherwise, with `u` the value converted to `make_unsigned_t<T>`, calls
    `boost::hash2::hash_append(h, f, static_cast<uint64_t>(u >> 64 * k))` for each `k` below `w / 64`, low word first,
    and then, if `w % 64` is not zero, appends one more `uint64_t` holding the remaining `w % 64` bits, sign-extended
    when `T` is signed and zero-extended otherwise.
- Under `little_endian_flavor` the message is the value's two's-complement bytes, least significant first, for every
  type it takes, at run time and in constant evaluation.
- Every 128-bit type, `int128`, Boost's, Abseil's and `_BitInt(128)`, gives the same FNV-1a digests, and under
  `big_endian_flavor` the words stay low first while the bytes within each reverse.
- Only the value is hashed: the padding of a `bit_int<24>` never reaches the message, under the default flavor too.
- *Throws:* Any exception `hash_append` or `h.update` throws. It is not `noexcept`.
- *Remarks:* It is a function to call in place of `boost::hash2::hash_append`, not a customization Hash2 finds.
- The header compiles on its own wherever [Boost.Hash2](https://www.boost.org/doc/libs/latest/libs/hash2/) is found.
