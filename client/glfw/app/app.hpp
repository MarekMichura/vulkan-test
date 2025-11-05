#ifndef APP_HPP
#define APP_HPP

#include <string_view>

#include "glfwInit.hpp"
#include "vulkanInit.hpp"
#include "window.hpp"

namespace vul {
class App {
public:
  constexpr static int width = 800;
  constexpr static int height = 600;
  constexpr static const std::string_view title = "Vulkan App";

  void run();

private:
  GlfwInit _glfw;
  VulkanInit _vulkan{{.appName = title, .engineName = title}};
  Window _window{{.width = width, .height = height, .title = title}};
};
}  // namespace vul

#endif