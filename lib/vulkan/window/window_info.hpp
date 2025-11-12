#ifndef LIB_VULKAN_WINDOW_WINDOWINFO
#define LIB_VULKAN_WINDOW_WINDOWINFO

#include <string_view>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

namespace vulkan {
struct WindowInfo {
  int width = defaultWidth;
  int height = defaultHeight;
  const std::string_view title = "Vulkan window";
  int resize = GLFW_FALSE;

  static constexpr int defaultWidth = 800;
  static constexpr int defaultHeight = 600;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_WINDOW_WINDOWINFO */
