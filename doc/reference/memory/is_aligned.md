# `<xstd/ints/memory/is_aligned.hpp>`

## `is_aligned`

- `is_aligned(T value, size_t alignment)`:
  - *Constraints:* `T` models `alignable`.
  - *Preconditions:* `alignment` is a power of two.
  - *Returns:* `true` exactly when `value` is a multiple of `alignment`, computed as `T(value & m) == T(0UZ)` for
    `m = T(alignment - 1)`, as a `bool` whatever `T` is. It is a constant expression.
  - It holds of what `align_up` and `align_down` return, and of `value` exactly when `align_down` leaves it unchanged.
  - *Remarks:* The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not for
    `absl::uint128`.
- `is_aligned(T* pointer, size_t alignment)`:
  - *Returns:* `is_aligned(reinterpret_cast<uintptr_t>(pointer), alignment)`.
  - *Remarks:* It is not `constexpr`, and it is `noexcept`.
