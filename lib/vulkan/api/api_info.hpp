#ifndef LIB_VULKAN_API_API_INFO
#define LIB_VULKAN_API_API_INFO

#include <vector>

#include <vulkan/vulkan_core.h>

#include "init_vulkan/init_info.hpp"
#include "window/window_info.hpp"

namespace vulkan {
struct VulkanApiInfo {
  VulkanInfo vulkanInitInfo = {};

  WindowInfo mainWindowInfo = {};
  std::vector<WindowInfo> windowsInfo;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_API_API_INFO */
