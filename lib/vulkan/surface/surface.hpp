#ifndef LIB_VULKAN_SURFACE_SURFACE
#define LIB_VULKAN_SURFACE_SURFACE

#include <memory>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

#include "surface_deleter.hpp"

namespace vulkan {
class Surface {
public:
  Surface(const Surface&) = delete;
  Surface(Surface&&) = default;
  Surface& operator=(const Surface&) = delete;
  Surface& operator=(Surface&&) = default;

  Surface(GLFWwindow* window);
  ~Surface() = default;

  VkSurfaceKHR get() const;

private:
  std::unique_ptr<VkSurfaceKHR_T, SurfaceDeleter> _surface;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_SURFACE_SURFACE */
