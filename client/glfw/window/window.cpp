#include "window.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>

#include "GLFW/glfw3.h"

namespace vul {
Window::Window(const WindowDef& def) : _window(createWindow(def), glfwDestroyWindow) {}

bool Window::shouldClose() const
{
  return glfwWindowShouldClose(_window.get()) != 0;
}

GLFWwindow* Window::createWindow(const WindowDef& def)
{
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, def.resize);

  const auto* title = def.title.data();
  assert(*def.title.end() == '\0');
  auto width = def.width;
  auto height = def.height;

  if (GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr); window != nullptr) {  // NOLINT(misc-const-correctness)
    return window;
  }
  throw std::runtime_error("Failed to create GLFW window\n");
}
}  // namespace vul