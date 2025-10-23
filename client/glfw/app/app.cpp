#include "app.hpp"

namespace vul {
void App::run()
{
  while (!_window.shouldClose()) {
    glfwPollEvents();
  }
}
}  // namespace vul