#include "printTable.hpp"

#include <cstddef>
#include <cstdint>
#include <format>
#include <regex>
#include <string>

#include <vulkan/vulkan_core.h>

namespace vul {

std::string printTable::stripAnsi(const std::string& str)
{
  static const std::regex ansi(R"(\x1B\[[0-9;?]*[A-Za-z])");
  return std::regex_replace(str, ansi, "");
}

size_t printTable::visibleLength(const std::string& str)
{
  return stripAnsi(str).size();
}

std::string printTable::version(const uint32_t ver)
{
  return std::format("{}.{}.{}", VK_VERSION_MAJOR(ver), VK_VERSION_MINOR(ver), VK_VERSION_PATCH(ver));
}

std::string printTable::boolean(bool ele)
{
  return ele ? "\033[32mYes\033[0m" : "\033[31mNo\033[0m";
}

std::string printTable::vendor(uint32_t vendorID)
{
  constexpr uint32_t nvidia = 0x10de;
  constexpr uint32_t amd = 0x1002;
  constexpr uint32_t intel = 0x8086;
  constexpr uint32_t arm = 0x13B5;
  constexpr uint32_t qualcomm = 0x5143;
  constexpr uint32_t apple = 0x106B;
  constexpr uint32_t microsoft = 0x1414;
  constexpr uint32_t google = 0x1AE0;
  constexpr uint32_t power = 0x1010;
  constexpr uint32_t mesa = 0x1D05;

  switch (vendorID) {
  case nvidia:
    return "\033[1;32mNVIDIA\033[0m";
  case amd:
    return "\033[1;31mAMD\033[0m";
  case intel:
    return "\033[1;34mIntel\033[0m";
  case arm:
    return "\033[1;35mARM\033[0m";
  case qualcomm:
    return "\033[1;36mQualcomm\033[0m";
  case apple:
    return "\033[1;33mApple\033[0m";
  case microsoft:
    return "\033[1;37mMicrosoft (WARP)\033[0m";
  case google:
    return "\033[1;90mGoogle\033[0m";
  case power:
    return "\033[1;91mImgTec (PowerVR)\033[0m";
  case mesa:
    return "\033[1;92mMesa / LLVMpipe\033[0m";
  default:
    return std::format("\033[1;93mUnknown vendor 0x{:X}\033[0m", vendorID);
  }
}
}  // namespace vul