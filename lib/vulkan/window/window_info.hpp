#ifndef LIB_VULKAN_WINDOW_WINDOW_INFO
#define LIB_VULKAN_WINDOW_WINDOW_INFO

#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

namespace vulkan {
struct WindowInfo {
  static constexpr int defaultWidth = 800;
  static constexpr int defaultHeight = 600;

  int width = defaultWidth;
  int height = defaultHeight;
  std::string title = "Vulkan window";
  int resize = GLFW_FALSE;

  std::vector<std::string> layers = {
#ifdef DEBUG
      "VK_LAYER_KHRONOS_validation"
#endif
  };
  std::vector<std::string> extensions;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_WINDOW_WINDOW_INFO */
