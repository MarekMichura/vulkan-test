#ifndef LIB_UTILS_FORMAT_STRING
#define LIB_UTILS_FORMAT_STRING

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <format>
#include <iostream>
#include <locale>
#include <ranges>
#include <string>

#include <vulkan/vulkan_core.h>

#include "format/ansi.hpp"

namespace utils {
std::string deviceType(VkPhysicalDeviceType type);

std::string string(bool data, bool compact = false);

std::string string(VkExtent3D data);

std::string result(VkResult result);

std::string vendor(uint32_t vendor);

std::string version(uint32_t version);

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
std::string number(const T ele, const char floor = '_')
{
  std::string str;
  try {
    str = std::format(std::locale("en_US.UTF-8"), "{:L}", ele);
  }
  catch (...) {
    str = std::to_string(ele);
  }
  std::ranges::replace(str, ',', floor);
  return str;
}

template <std::ranges::input_range Range>
std::string table(const Range& table, size_t n = 0)
{
  using T = std::ranges::range_value_t<Range>;
  assert(table.size() > 0);
  static constexpr auto separatorA = utils::ansi<{.bold = true, .color = COLOR::RED}>(", ");
  static constexpr auto separatorB = utils::ansi<{.bold = true, .color = COLOR::RED}>("|");
  static constexpr auto bracketOpen = utils::ansi<{.bold = true, .color = COLOR::GREEN}>("[");
  static constexpr auto bracketClose = utils::ansi<{.bold = true, .color = COLOR::GREEN}>("]");

  auto toString = [n](T ele) -> std::string {
    std::string result;
    if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, VkExtent3D>) {
      result = string(ele);
    }
    else if constexpr (Number<T>) {
      result = number(ele);
    }
    else {
      result = std::move(ele);
    }
    const size_t length = strLen(result);
    if (length <= n) {
      try {
        return std::format(" {:{}s} ", result, n + (result.length() - length));
      }
      catch (const std::format_error& e) {
        std::cerr << "Weird error with formatter:\n\t" << e.what() << "\n";
        std::terminate();
      }
    }
    return result;
  };

  auto data = table |                                                                              //
              std::views::transform(toString) |                                                    //
              std::views::join_with(std::string(n > 0 ? separatorB.data() : separatorA.data())) |  //
              std::ranges::to<std::string>();

  auto result = std::format("{}{}{}", std::string(bracketOpen.data()), data, std::string(bracketClose.data()));
  return result;
}
}  // namespace utils
#endif /* LIB_UTILS_FORMAT_STRING */
