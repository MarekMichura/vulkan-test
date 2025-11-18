#ifndef LIB_VULKAN_WINDOW_SURFACE_DELETER
#define LIB_VULKAN_WINDOW_SURFACE_DELETER

#include <memory>

#include <vulkan/vulkan_core.h>

#include "init_vulkan/init.hpp"

namespace vulkan {
struct SurfaceDeleter {
  std::shared_ptr<InitVulkan> instance = InitVulkan::getInit();

  void operator()(VkSurfaceKHR surface) const
  {
    if (surface) {
      vkDestroySurfaceKHR(instance->getInstance(), surface, nullptr);
    }
  }
};
}  // namespace vulkan

#endif /* LIB_VULKAN_WINDOW_SURFACE_DELETER */
