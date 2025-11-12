#define GLFW_INCLUDE_VULKAN
#include "window.hpp"

#include <memory>
#include <stdexcept>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

#include "device/device.hpp"
#include "init_vulkan/init.hpp"
#include "surface_deleter.hpp"
#include "window_info.hpp"

namespace vulkan {
static GLFWwindow* createWindow(const WindowInfo& info)
{
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, info.resize);

  const auto* title = info.title.data();
  auto width = info.width;
  auto height = info.height;

  if (GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr); window != nullptr) {  // NOLINT(misc-const-correctness)
    return window;
  }
  throw std::runtime_error("Failed to create GLFW window\n");
}

static void destroyWindow(GLFWwindow* window)
{
  glfwDestroyWindow(window);
}

static VkSurfaceKHR createSurface(GLFWwindow* window)
{
  auto instance = InitVulkan::getInit();

  VkSurfaceKHR surface = nullptr;
  glfwCreateWindowSurface(instance->getInstance(), window, nullptr, &surface);

  return surface;
}

Window::Window(const WindowInfo& info)
    : _window(createWindow(info), destroyWindow),  //
      _surface(createSurface(_window.get()), SurfaceDeleter{}),
      _device(std::make_unique<VulkanDevice>(_surface.get()))
{
}

GLFWwindow* Window::getWindow() const
{
  return _window.get();
}

VkSurfaceKHR Window::getSurface() const
{
  return _surface.get();
}

}  // namespace vulkan