#include "app.hpp"

#include "GLFW/glfw3.h"

namespace vul {
void App::run()
{
  while (!_window.shouldClose()) {
    glfwPollEvents();
  }
}
}  // namespace vul