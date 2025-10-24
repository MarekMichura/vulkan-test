#pragma once
#include <string>
#include "vulkanInit.hpp"
#include "glfwInit.hpp"
#include "window.hpp"

namespace vul {
class App {
public:
  static constexpr int width = 800;
  static constexpr int height = 600;
  static constexpr std::string title = "Vulkan App";

  void run();

private:
  vul::GlfwInit _glfw;
  vul::VulkanInit _vulkan{{}};
  vul::Window _window{{}};
};
}  // namespace vul