#define GLFW_INCLUDE_VULKAN
#include "surface.hpp"

#include <stdexcept>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

#include "init_vulkan/init.hpp"
#include "surface/surface_deleter.hpp"

namespace vulkan {
static VkSurfaceKHR createSurface(GLFWwindow* window)
{
  auto instance = InitVulkan::getInit();

  VkSurfaceKHR surface = nullptr;
  if (const VkResult status = glfwCreateWindowSurface(instance->getInstance(), window, nullptr, &surface); status != VK_SUCCESS) {
    throw std::runtime_error("Failed to create surface");
  }

  return surface;
}

Surface::Surface(GLFWwindow* window) : _surface(createSurface(window), SurfaceDeleter{}) {}

VkSurfaceKHR Surface::get() const
{
  return _surface.get();
}
}  // namespace vulkan