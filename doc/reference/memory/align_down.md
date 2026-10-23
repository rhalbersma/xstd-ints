# `<xstd/ints/memory/align_down.hpp>`

## `xstd::align_down`

```cpp
// (1)
template<alignable T>
  constexpr auto align_down(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> T;

// (2)
template<class T>
  auto align_down(T* pointer, size_t alignment) noexcept -> T*;
```

Rounds down to a multiple of a power-of-two alignment.

1. For an `alignable` value.
2. For a pointer, through its address.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `value` | the value to round down |
| `pointer` | the pointer whose address is rounded down |
| `alignment` | a power of two |

*Constraints:* (1) `T` models `alignable`.

*Preconditions:* (1) `alignment` is a power of two.

### Return value

1. *Returns:* The greatest multiple of `alignment` not greater than `value`, as a `T`, computed as
   `value - (value & m)` for `m = T(alignment - 1)`. A narrow type is returned as itself. It is a constant expression.
2. *Effects:* Equivalent to
   `return reinterpret_cast<T*>(align_down(reinterpret_cast<uintptr_t>(pointer), alignment));`.

### Exceptions

1. The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not for
   `absl::uint128`.
2. As for `align_up`'s pointer form.

### Remarks

(2) As for `align_up`'s pointer form.

### Notes

- (1) It is exact at the maximum of `size_t`.
- (2) It returns a `T*`.

### See also

| Entity | Relation |
| :----- | :------- |
| [`align_up`](align_up.md) | rounds up instead |
| [`is_aligned`](is_aligned.md) | holds of what it returns, and of `value` exactly when it leaves it unchanged |
| [`alignable`](../concepts/alignable.md) | what (1) asks of `T` |
| [`boost::alignment::align_down`](https://www.boost.org/doc/libs/latest/libs/align/) | the Boost.Align function it agrees with |
