#include "string.hpp"

#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>

#include <vulkan/vulkan_core.h>

#include "format/ansi.hpp"

namespace utils {

std::string deviceType(const VkPhysicalDeviceType type)
{
  static constexpr auto other = utils::ansi<{.color = utils::COLOR::WHITE}>("Other / Unspecified");
  static constexpr auto integrated = utils::ansi<{.color = utils::COLOR::GREEN}>("Integrated GPU");
  static constexpr auto discrete = utils::ansi<{.color = utils::COLOR::BLUE}>("Discrete GPU");
  static constexpr auto virtualGpu = utils::ansi<{.color = utils::COLOR::CYAN}>("Virtual GPU");
  static constexpr auto cpu = utils::ansi<{.color = utils::COLOR::YELLOW}>("CPU as GPU");
  static constexpr auto unknown = utils::ansi<{.color = utils::COLOR::BRIGHT_RED}>("Unknown");

  switch (type) {
  case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    return {other.data()};
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    return {integrated.data()};
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    return {discrete.data()};
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    return {virtualGpu.data()};
  case VK_PHYSICAL_DEVICE_TYPE_CPU:
    return {cpu.data()};
  default:
    return {unknown.data()};
  }
}

std::string result(VkResult result)
{
  switch (result) {
  case VK_SUCCESS:
    return "VK_SUCCESS";
  case VK_NOT_READY:
    return "VK_NOT_READY";
  case VK_TIMEOUT:
    return "VK_TIMEOUT";
  case VK_EVENT_SET:
    return "VK_EVENT_SET";
  case VK_EVENT_RESET:
    return "VK_EVENT_RESET";
  case VK_INCOMPLETE:
    return "VK_INCOMPLETE";
  case VK_ERROR_OUT_OF_HOST_MEMORY:
    return "VK_ERROR_OUT_OF_HOST_MEMORY";
  case VK_ERROR_OUT_OF_DEVICE_MEMORY:
    return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
  case VK_ERROR_INITIALIZATION_FAILED:
    return "VK_ERROR_INITIALIZATION_FAILED";
  case VK_ERROR_DEVICE_LOST:
    return "VK_ERROR_DEVICE_LOST";
  case VK_ERROR_MEMORY_MAP_FAILED:
    return "VK_ERROR_MEMORY_MAP_FAILED";
  case VK_ERROR_LAYER_NOT_PRESENT:
    return "VK_ERROR_LAYER_NOT_PRESENT";
  case VK_ERROR_EXTENSION_NOT_PRESENT:
    return "VK_ERROR_EXTENSION_NOT_PRESENT";
  case VK_ERROR_FEATURE_NOT_PRESENT:
    return "VK_ERROR_FEATURE_NOT_PRESENT";
  case VK_ERROR_INCOMPATIBLE_DRIVER:
    return "VK_ERROR_INCOMPATIBLE_DRIVER";
  case VK_ERROR_TOO_MANY_OBJECTS:
    return "VK_ERROR_TOO_MANY_OBJECTS";
  case VK_ERROR_FORMAT_NOT_SUPPORTED:
    return "VK_ERROR_FORMAT_NOT_SUPPORTED";
  case VK_ERROR_FRAGMENTED_POOL:
    return "VK_ERROR_FRAGMENTED_POOL";
  case VK_ERROR_UNKNOWN:
    return "VK_ERROR_UNKNOWN";
  case VK_ERROR_OUT_OF_POOL_MEMORY:
    return "VK_ERROR_OUT_OF_POOL_MEMORY";
  case VK_ERROR_INVALID_EXTERNAL_HANDLE:
    return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
  case VK_ERROR_FRAGMENTATION:
    return "VK_ERROR_FRAGMENTATION";
  case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
    return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
  case VK_PIPELINE_COMPILE_REQUIRED:
    return "VK_PIPELINE_COMPILE_REQUIRED";
  case VK_ERROR_SURFACE_LOST_KHR:
    return "VK_ERROR_SURFACE_LOST_KHR";
  case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
    return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
  case VK_SUBOPTIMAL_KHR:
    return "VK_SUBOPTIMAL_KHR";
  case VK_ERROR_OUT_OF_DATE_KHR:
    return "VK_ERROR_OUT_OF_DATE_KHR";
  case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
    return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
  case VK_ERROR_VALIDATION_FAILED_EXT:
    return "VK_ERROR_VALIDATION_FAILED_EXT";
  case VK_ERROR_INVALID_SHADER_NV:
    return "VK_ERROR_INVALID_SHADER_NV";
  case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
    return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
  case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
    return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
  case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
    return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
  case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
    return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
  case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
    return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
  case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
    return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
  case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
    return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
  case VK_ERROR_NOT_PERMITTED_KHR:
    return "VK_ERROR_NOT_PERMITTED_KHR";
  case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
    return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
  case VK_THREAD_IDLE_KHR:
    return "VK_THREAD_IDLE_KHR";
  case VK_THREAD_DONE_KHR:
    return "VK_THREAD_DONE_KHR";
  case VK_OPERATION_DEFERRED_KHR:
    return "VK_OPERATION_DEFERRED_KHR";
  case VK_OPERATION_NOT_DEFERRED_KHR:
    return "VK_OPERATION_NOT_DEFERRED_KHR";
  case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
    return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
  case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
    return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
  case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
    return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
  case VK_RESULT_MAX_ENUM:
    return "VK_RESULT_MAX_ENUM";
  default:
    throw std::runtime_error(std::format("VkResult {} not found and can not be converted to string", static_cast<uint32_t>(result)));
  }
}

std::string vendor(uint32_t vendor)
{
  constexpr uint32_t NVIDIA = 0x10de;
  constexpr uint32_t AMD = 0x1002;
  constexpr uint32_t INTEL = 0x8086;
  constexpr uint32_t ARM = 0x13B5;
  constexpr uint32_t QUALCOMM = 0x5143;
  constexpr uint32_t APPLE = 0x106B;
  constexpr uint32_t MICROSOFT = 0x1414;
  constexpr uint32_t GOOGLE = 0x1AE0;
  constexpr uint32_t POWER = 0x1010;
  constexpr uint32_t MESA = 0x1D05;

  static constexpr auto nvidia = ansi<{.color = COLOR::GREEN}>("Nvidia");
  static constexpr auto amd = ansi<{.color = COLOR::RED}>("AMD");
  static constexpr auto intel = ansi<{.color = COLOR::BLUE}>("Intel");
  static constexpr auto arm = ansi<{.color = COLOR::CYAN}>("ARM");
  static constexpr auto qualcomm = ansi<{.color = COLOR::MAGENTA}>("Qualcomm");
  static constexpr auto apple = ansi<{.color = COLOR::BRIGHT_RED}>("Apple");
  static constexpr auto microsoft = ansi<{.color = COLOR::YELLOW}>("Microsoft");
  static constexpr auto google = ansi<{.color = COLOR::BRIGHT_BLUE}>("Google");
  static constexpr auto power = ansi<{.color = COLOR::BRIGHT_MAGENTA}>("Power");
  static constexpr auto mesa = ansi<{.color = COLOR::BRIGHT_GREEN}>("Mesa");

  // clang-format off
  switch (vendor) {
    case NVIDIA:    return {nvidia.data()};
    case AMD:       return {amd.data()};
    case INTEL:     return {intel.data()};
    case ARM:       return {arm.data()};
    case QUALCOMM:  return {qualcomm.data()};
    case APPLE:     return {apple.data()};
    case MICROSOFT: return {microsoft.data()};
    case GOOGLE:    return {google.data()};
    case POWER:     return {power.data()};
    case MESA:      return {mesa.data()};
    default:        return "Unknown";
  }
  // clang-format on
}

std::string version(uint32_t version)
{
  static constexpr auto green = utils::ansi<{.color = COLOR::GREEN}>("-");

  return std::format("ver: {}{}{}{}{}",                        //
                     VK_VERSION_MAJOR(version), green.data(),  //
                     VK_VERSION_MINOR(version), green.data(),  //
                     VK_VERSION_PATCH(version));
}

std::string string(bool data, bool compact)
{
  static constexpr auto TRUE = ansi<{.color = COLOR::GREEN}>("true");
  static constexpr auto FALSE = ansi<{.color = COLOR::RED}>("false");
  static constexpr auto TRUE_COMPACT = ansi<{.bold = true, .color = COLOR::GREEN}>("T");
  static constexpr auto FALSE_COMPACT = ansi<{.bold = true, .color = COLOR::RED}>("F");

  if (compact) {
    if (data) {
      return {TRUE_COMPACT.data()};
    }
    return {FALSE_COMPACT.data()};
  }
  if (data) {
    return {TRUE.data()};
  }
  return {FALSE.data()};
}

std::string string(VkExtent3D data)
{
  static constexpr auto green = utils::ansi<{.bold = true, .color = COLOR::BRIGHT_GREEN}>("-");

  return std::format("{}{}{}{}{}",               //
                     data.width, green.data(),   //
                     data.height, green.data(),  //
                     data.depth);
}

}  // namespace utils