# `<xstd/ints/ext/absl.hpp>`

```cpp
#include <absl/numeric/int128.h>

namespace xstd {
  template<> struct make_unsigned<absl::int128>;
  template<> struct make_signed<absl::uint128>;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/ext/absl/int128.hpp>`](int128.md) | `make_signed`, `make_unsigned` |

The umbrella exports `<xstd/ints/ext/absl/int128.hpp>`, which exports `<absl/numeric/int128.h>`, so including either is
enough to name Abseil's pair. No header of the library includes it.
