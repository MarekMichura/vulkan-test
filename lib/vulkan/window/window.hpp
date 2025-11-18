#ifndef LIB_VULKAN_WINDOW_WINDOW
#define LIB_VULKAN_WINDOW_WINDOW

#include <memory>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

#include "device/device.hpp"
#include "surface/surface.hpp"
#include "window_info.hpp"

namespace vulkan {
class Window {
public:
  Window(const Window&) = delete;
  Window(Window&&) = default;
  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) = default;

  explicit Window(const WindowInfo& info = {});
  ~Window() = default;

  [[nodiscard]] bool shouldClose() const;
  [[nodiscard]] GLFWwindow* getWindow() const;
  [[nodiscard]] VkSurfaceKHR getSurface() const;

private:
  std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> _window;
  Surface _surface;
  VulkanDevice _device;
};

}  // namespace vulkan

#endif /* LIB_VULKAN_WINDOW_WINDOW */
