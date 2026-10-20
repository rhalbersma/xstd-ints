# `<xstd/ints/memory/align_up.hpp>`

## `align_up`

- `align_up(T value, size_t alignment)`:
  - *Constraints:* `T` models `alignable`. A signed integer-class type is turned away rather than converted.
  - *Preconditions:* `alignment` is a power of two, and `value + (alignment - 1)` does not wrap in `T`, so the result is
    representable.
  - *Returns:* The least multiple of `alignment` not less than `value`, as a `T`, computed as `s - (s & m)` for
    `m = T(alignment - 1)` and `s = value + m`. A narrow type is returned as itself rather than promoted. It is a
    constant expression.
  - It is exact up to the greatest multiple of `alignment` that `size_t` represents.
  - It takes `uint128`.
  - *Remarks:* The exception specification is `nothrow_alignable<T>`: `noexcept` for `size_t` and `uint8_t`, and not for
    `absl::uint128`.
- `align_up(T* pointer, size_t alignment)`:
  - *Effects:* Equivalent to `return reinterpret_cast<T*>(align_up(reinterpret_cast<uintptr_t>(pointer), alignment));`
    so it answers as Boost.Align's pointer form does and has the integer form's preconditions.
  - It returns a `T*`, keeping the pointee type where Boost.Align returns `void*`.
  - *Remarks:* It is not `constexpr`, a `reinterpret_cast` being barred from constant expressions, and it is `noexcept`.
  - *Remarks:* The result is an address: it is not checked against the object `pointer` points into, and no object's
    lifetime begins there.
