#include "ansi.hpp"

#include <cstddef>
#include <string_view>

namespace utils {
size_t strLen(const std::string_view str)
{
  size_t len = 0;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '\033') {
      i += 2;
      while (i < str.size() && str[i] != 'm') {
        ++i;
      }
    }
    else {
      ++len;
    }
  }

  return len;
}
}  // namespace utils