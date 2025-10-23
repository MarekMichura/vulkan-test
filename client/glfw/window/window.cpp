#include "window.hpp"
#include "GLFW/glfw3.h"

namespace vul {
Window::Window(int width, int height, const std::string& title)
    : _width(width), _height(height), _title(title), _window(nullptr, glfwDestroyWindow)
{
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow* window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);

  if (window == nullptr) {
    throw std::runtime_error("Failed to create GLFW window\n");
  }
  _window.reset(window);
}

bool Window::shouldClose() const
{
  return glfwWindowShouldClose(_window.get()) != 0;
}
}  // namespace vul