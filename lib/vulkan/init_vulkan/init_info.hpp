#ifndef LIB_VULKAN_INIT_VULKAN_INITINFO
#define LIB_VULKAN_INIT_VULKAN_INITINFO

#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "debug.hpp"

namespace vulkan {
struct VulkanInfo {
  const std::string appName = "";
  const std::string engineName = "";

  const uint32_t appVersion = VK_MAKE_VERSION(1, 0, 1);
  const uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);

  const bool useDebugger = Debug;
  const std::vector<std::string> layers = {
#ifdef DEBUG
      "VK_LAYER_KHRONOS_validation"
#endif
  };
  const std::vector<std::string> extensions = {
#ifdef DEBUG
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
  };
};  // namespace vulkan
}  // namespace vulkan

#endif /* LIB_VULKAN_INIT_VULKAN_INITINFO */
