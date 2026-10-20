# `<xstd/ints/memory/align_down.hpp>`

## `align_down`

- `align_down(T value, size_t alignment)`:
  - *Constraints:* `T` models `alignable`.
  - *Preconditions:* `alignment` is a power of two.
  - *Returns:* The greatest multiple of `alignment` not greater than `value`, as a `T`, computed as
    `value - (value & m)` for `m = T(alignment - 1)`. A narrow type is returned as itself. It is a constant expression.
  - It is exact at the maximum of `size_t`.
  - *Remarks:* The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not for
    `absl::uint128`.
- `align_down(T* pointer, size_t alignment)`:
  - *Effects:* Equivalent to
    `return reinterpret_cast<T*>(align_down(reinterpret_cast<uintptr_t>(pointer), alignment));`.
  - It returns a `T*`.
  - *Remarks:* As for `align_up`'s pointer form.
