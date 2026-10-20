# `<xstd/ints/memory/is_aligned.hpp>`

## `xstd::is_aligned`

```cpp
// (1)
template<alignable T>
  constexpr auto is_aligned(T value, size_t alignment) noexcept(nothrow_alignable<T>) -> bool;

// (2)
template<class T>
  auto is_aligned(T* pointer, size_t alignment) noexcept -> bool;
```

Whether a value is a multiple of a power-of-two alignment.

1. For an `alignable` value.
2. For a pointer, through its address.

### Parameters

| Parameter | Meaning |
| :-------- | :------ |
| `value` | the value to test |
| `pointer` | the pointer whose address is tested |
| `alignment` | a power of two |

*Constraints:* (1) `T` models `alignable`.

*Preconditions:* (1) `alignment` is a power of two.

### Return value

1. *Returns:* `true` exactly when `value` is a multiple of `alignment`, computed as `T(value & m) == T(0UZ)` for
   `m = T(alignment - 1)`, as a `bool` whatever `T` is. It is a constant expression.
2. *Returns:* `is_aligned(reinterpret_cast<uintptr_t>(pointer), alignment)`.

### Exceptions

1. The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not for
   `absl::uint128`.
2. It is `noexcept`.

### Remarks

(2) It is not `constexpr`.

### Notes

(1) It holds of what `align_up` and `align_down` return, and of `value` exactly when `align_down` leaves it unchanged.

### See also

| Entity | Relation |
| :----- | :------- |
| [`align_up`](align_up.md), [`align_down`](align_down.md) | what they return satisfies it |
| [`alignable`](../concepts/alignable.md) | what (1) asks of `T` |
| [`boost::alignment::is_aligned`](https://www.boost.org/doc/libs/latest/libs/align/) | the Boost.Align function it agrees with |
