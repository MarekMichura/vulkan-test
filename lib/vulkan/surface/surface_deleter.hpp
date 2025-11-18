#ifndef LIB_VULKAN_WINDOW_SURFACE_DELETER
#define LIB_VULKAN_WINDOW_SURFACE_DELETER

#include <memory>

#include <vulkan/vulkan_core.h>

#include "init_vulkan/init.hpp"

namespace vulkan {
struct SurfaceDeleter {
  void operator()(VkSurfaceKHR surface) const
  {
    if (surface != nullptr) {
      vkDestroySurfaceKHR(instance->getInstance(), surface, nullptr);
    }
  }

private:
  std::shared_ptr<InitVulkan> instance = InitVulkan::getInit();
};
}  // namespace vulkan

#endif /* LIB_VULKAN_WINDOW_SURFACE_DELETER */
