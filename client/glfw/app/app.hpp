#pragma once
#include <string>

#include "glfwInit.hpp"
#include "vulkanInit.hpp"
#include "window.hpp"

namespace vul {
class App {
public:
  static constexpr int width = 800;
  static constexpr int height = 600;
  static constexpr std::string title = "Vulkan App";
  static constexpr const char* name = title.c_str();

  void run();

private:
  vul::GlfwInit _glfw;
  vul::VulkanInit _vulkan{{.appName = name, .engineName = name}};
  vul::Window _window{{.width = width, .height = height, .title = title}};
};
}  // namespace vul