#include "printTable.hpp"

#include <cstddef>
#include <cstdint>
#include <format>
#include <regex>
#include <string>

#include <vulkan/vulkan_core.h>

namespace vul {

std::string printTable::stripAnsi(const std::string& s)
{
  static const std::regex ansi_pattern(R"(\x1B\[[0-9;]*[A-Za-z])");
  return std::regex_replace(s, ansi_pattern, "");
}

size_t printTable::visibleLength(const std::string& s)
{
  return stripAnsi(s).size();
}

std::string printTable::version(const uint32_t ver)
{
  return std::format("{}.{}.{}", VK_VERSION_MAJOR(ver), VK_VERSION_MINOR(ver), VK_VERSION_PATCH(ver));
}

std::string printTable::boolean(bool ele)
{
  return ele ? "\033[32mYes\033[0m" : "\033[31mNo\033[0m";
}

}  // namespace vul