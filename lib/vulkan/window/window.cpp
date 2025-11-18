#include "window.hpp"

#include <memory>
#include <stdexcept>

#include "GLFW/glfw3.h"

#include "device/device.hpp"
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

Window::Window(const WindowInfo& info)
    : _window(createWindow(info), destroyWindow),  //
      _surface(_window.get()),
      _device(info, _surface.get())
{
}

GLFWwindow* Window::getWindow() const
{
  return _window.get();
}

}  // namespace vulkan