# `<xstd/ints/ext/boost.hpp>`

```cpp
#include <boost/int128.hpp>
#include <boost/hash2/hash_append.hpp>

namespace xstd {
  template<> struct make_unsigned<boost::int128::int128>;
  template<> struct make_signed<boost::int128::uint128>;

  template<class Hash, class Flavor, integer T>
    constexpr auto hash_append_int(Hash& h, const Flavor& f, T v) -> void;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/ext/boost/int128.hpp>`](int128.md) | `make_signed`, `make_unsigned` |
| [`<xstd/ints/ext/boost/hash2.hpp>`](hash2.md) | `hash_append_int` |

The umbrella exports `<xstd/ints/ext/boost/int128.hpp>` and `<xstd/ints/ext/boost/hash2.hpp>`, and each exports the
Boost header it adapts, so including one is enough to name Boost's types and flavors. No header of the library includes
it: an adapted library is asked for by name. Through it, [Boost.Int128](https://github.com/cppalliance/int128)'s pair is paired and hashes as xstd's own 128-bit
pair does.
