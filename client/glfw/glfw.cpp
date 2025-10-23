#include "glfw/glfw.hpp"
#include <stdexcept>
#include "GLFW/glfw3.h"

namespace vul {
GLFW::GLFW()
{
  if (glfwInit() == 0) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
}

GLFW::~GLFW()
{
  glfwTerminate();
}
}  // namespace vul