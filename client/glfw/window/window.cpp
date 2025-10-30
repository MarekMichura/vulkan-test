#include "window.hpp"

#include <memory>
#include <stdexcept>
#include <string>

#include "GLFW/glfw3.h"

namespace vul {
Window::Window(const windowDef& def) : _window(createWindow(def), glfwDestroyWindow) {}

bool Window::shouldClose() const
{
  return glfwWindowShouldClose(_window.get()) != 0;
}

GLFWwindow* Window::createWindow(const windowDef& def)
{
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, def.resize);

  GLFWwindow* window =  // NOLINT(misc-const-correctness)
      glfwCreateWindow(def.width, def.height, def.title.c_str(), nullptr, nullptr);

  if (window == nullptr) {
    throw std::runtime_error("Failed to create GLFW window\n");
  }

  return window;
}
}  // namespace vul