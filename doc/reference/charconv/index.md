# `<xstd/ints/charconv.hpp>`

```cpp
namespace xstd {
  template<class I>
    requires (same_as<remove_cv_t<I>, char> or integer<I>)
    constexpr size_t to_chars_max_size = see below;

  template<integral I>
    requires std-writes-chars<I>
    constexpr auto to_chars(char* first, char* last, I value, int base = 10) -> to_chars_result;
  auto to_chars(char* first, char* last, bool value, int base = 10) -> to_chars_result = delete;
  template<integer I>
    requires (not std-writes-chars<I>)
    constexpr auto to_chars(char* first, char* last, I value, int base = 10) -> to_chars_result;
}
```

| Header | Declares |
| :----- | :------- |
| [`<xstd/ints/charconv/to_chars.hpp>`](to_chars.md) | `to_chars`, `to_chars_max_size` |
