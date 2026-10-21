# `<xstd/ints/bit/bit_repeat.hpp>`

## `xstd::bit_repeat`

```cpp
#ifdef XSTD_HAS_STD_BIT_PERMUTATIONS
// (1)
template<class T>
  requires ints::detail::std_permutes_bits<T>
  constexpr auto bit_repeat(T x, int l) -> T;
#endif

// (2)
template<unsigned_integer T>
  requires (not ints::detail::std_permutes_bits<T>)
  constexpr auto bit_repeat(T x, int l) -> T;
```

Repeats the low bits of an unsigned integer across its width.

1. The forwarding overload, which calls `std::bit_repeat`.
2. The template, for every other `unsigned_integer`.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `x` | the value whose low bits are repeated |
| `l` | the period, the number of low bits repeated |

*Constraints:* As for `bit_reverse`.

*Preconditions:* `l` is greater than zero.

### Return value

*Returns:* `repeat(x, l)`, [P3104R5](https://github.com/cplusplus/papers/issues/1768)'s sum of `x[n mod l]·2^n` over
`n` in `[0, N)`: the low `l` bits of `x` repeated across the width.

### Exceptions

- *Throws:* Nothing, for a `T` whose operators throw nothing.
- It is not `noexcept`, for any type, the forwarding overload included.

### Remarks

- A call that violates the precondition is not a core constant expression, whether or not `NDEBUG` is defined, as
  [P3104R6](https://github.com/cplusplus/papers/issues/1768) [[bit.permute]/8](https://eel.is/c++draft/bit.permute#8)
  adds.
- Where `__cpp_lib_bitops` is at least `202607L` and `std::bit_compress`, `std::bit_expand`, `std::bit_repeat` and
  `std::bit_reverse` all take `T`, the call is `std::bit_repeat(x, l)`, and the template does not take `T`.
- The template has no block kernel: it masks the low `l` bits and doubles the pattern until it fills the width, so it
  runs `ceil(log2(N / l))` shift-or steps.

### Notes

- A period of one copies the lowest bit everywhere, and a period of `N` or more, `INT_MAX` included, returns `x`.
- The paper's examples hold: `bit_repeat(uint32_t{0xc}, 4)` is `0xcccc'cccc`, the alternating masks of its `countr_zero`
  are `bit_repeat((1u << i) - 1, 2 * i)`, and that `countr_zero` agrees with xstd's.
- A call is a constant expression for every type it takes.
- The result has type `T`, not the `int` that integral promotion makes of a narrow `T`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`bit_expand`](bit_expand.md) | interleaves through masks it builds |
| [`bit_reverse`](bit_reverse.md), [`bit_compress`](bit_compress.md) | the other permutations |
| [Notation](shl.md#notation-for-the-bit-permutations) | what `N` and `x[n]` mean here |
| [P3104R6](https://github.com/cplusplus/papers/issues/1768) | the paper that proposes `std::bit_repeat` |
