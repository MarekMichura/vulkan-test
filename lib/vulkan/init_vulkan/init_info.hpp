#ifndef LIB_VULKAN_INIT_VULKAN_INITINFO
#define LIB_VULKAN_INIT_VULKAN_INITINFO

#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "debug.hpp"

namespace vulkan {
struct VulkanInfo {
  std::string appName;
  std::string engineName;

  uint32_t appVersion = VK_MAKE_VERSION(1, 0, 1);
  uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);

  bool useDebugger = Debug;
  std::vector<std::string> layers = {
#ifdef DEBUG
      "VK_LAYER_KHRONOS_validation"
#endif
  };
  std::vector<std::string> extensions = {
#ifdef DEBUG
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
  };
};  // namespace vulkan
}  // namespace vulkan

#endif /* LIB_VULKAN_INIT_VULKAN_INITINFO */
