# `<xstd/ints/memory/align_up.hpp>`

## `xstd::align_up`

```cpp
// (1)
template<alignable T>
  constexpr auto align_up(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> T;

// (2)
template<class T>
  auto align_up(T* pointer, size_t alignment) noexcept -> T*;
```

Rounds up to a multiple of a power-of-two alignment.

1. For an `alignable` value.
2. For a pointer, through its address.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `value` | the value to round up |
| `pointer` | the pointer whose address is rounded up |
| `alignment` | a power of two |

*Constraints:* (1) `T` models `alignable`. A signed integer-class type is turned away rather than converted.

*Preconditions:* (1) `alignment` is a power of two, and `value + (alignment - 1)` does not wrap in `T`, so the result is
representable.

### Return value

1. *Returns:* The least multiple of `alignment` not less than `value`, as a `T`, computed as `s - (s & m)` for
   `m = T(alignment - 1)` and `s = value + m`. A narrow type is returned as itself rather than promoted. It is a
   constant expression.
2. *Effects:* Equivalent to `return reinterpret_cast<T*>(align_up(reinterpret_cast<uintptr_t>(pointer), alignment));`
   so it answers as Boost.Align's pointer form does and has the integer form's preconditions.

### Exceptions

1. The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not for
   `absl::uint128`.
2. It is `noexcept`.

### Remarks

- (2) It is not `constexpr`, a `reinterpret_cast` being barred from constant expressions.
- (2) The result is an address: it is not checked against the object `pointer` points into, and no object's lifetime
  begins there.

### Notes

- (1) It is exact up to the greatest multiple of `alignment` that `size_t` represents.
- (1) It takes `uint128`.
- (2) It returns a `T*`, keeping the pointee type where Boost.Align returns `void*`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`align_down`](align_down.md) | rounds down instead |
| [`is_aligned`](is_aligned.md) | holds of what it returns |
| [`alignable`](../concepts/alignable.md) | what (1) asks of `T` |
| [`boost::alignment::align_up`](https://www.boost.org/doc/libs/latest/libs/align/) | the Boost.Align function it agrees with |
