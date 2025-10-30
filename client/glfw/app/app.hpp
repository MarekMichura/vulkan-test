#ifndef APP_HPP
#define APP_HPP

#include <string>

#include "glfwInit.hpp"
#include "vulkanInit.hpp"
#include "window.hpp"

namespace vul {
class App {
public:
  static constexpr int width = 800;
  static constexpr int height = 600;
  constexpr static const std::string title = "Vulkan App";

  void run();

private:
  vul::GlfwInit _glfw;
  vul::VulkanInit _vulkan{{.appName = title, .engineName = title}};
  vul::Window _window{{.width = width, .height = height, .title = title}};
};
}  // namespace vul

#endif