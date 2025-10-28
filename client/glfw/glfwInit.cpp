#include "glfwInit.hpp"

#include <stdexcept>

#include "GLFW/glfw3.h"

namespace vul {
GlfwInit::GlfwInit()
{
  if (glfwInit() == 0) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
}

GlfwInit::~GlfwInit()
{
  glfwTerminate();
}
}  // namespace vul