# `<xstd/ints.hpp>`

```cpp
#include <xstd/ints/bit.hpp>
#include <xstd/ints/charconv.hpp>
#include <xstd/ints/concepts.hpp>
#include <xstd/ints/cstdint.hpp>
#include <xstd/ints/cstdlib.hpp>
#include <xstd/ints/format.hpp>
#include <xstd/ints/limits.hpp>
#include <xstd/ints/memory.hpp>
#include <xstd/ints/type_traits.hpp>
```

- It exports every domain umbrella, and through it `int128` reaches the concepts, `numeric_limits`, `abs`, `div`,
  `to_chars` with `to_chars_max_size`, and the formatting of `div_result`.
- Through it the alignment functions take `size_t` and `uint128`, which model `alignable`, and turn away `int128`, which
  does not.
- It does not export `<xstd/ints/ext/absl.hpp>` or `<xstd/ints/ext/boost.hpp>`, each of which hard-includes the library
  it adapts, nor any header under `<xstd/ints/ext/>`.
