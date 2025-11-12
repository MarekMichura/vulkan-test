#include "init.hpp"

#include <memory>
#include <stdexcept>

#include "GLFW/glfw3.h"

namespace vulkan {
InitGlfw::InitGlfw()
{
  if (glfwInit() == 0) {
    throw std::runtime_error("Failed to initialize GLFW");
  }
}

InitGlfw::~InitGlfw()
{
  glfwTerminate();
}

std::weak_ptr<InitGlfw>& InitGlfw::ptr()
{
  static std::weak_ptr<InitGlfw> ptr;
  return ptr;
}

std::shared_ptr<InitGlfw> InitGlfw::createInit()
{
  if (auto existing = ptr().lock()) {
    throw std::runtime_error("InitGlfw is initialized");
  }

  auto newInstance = std::shared_ptr<InitGlfw>(new InitGlfw());
  ptr() = newInstance;
  return newInstance;
}

std::shared_ptr<InitGlfw> InitGlfw::getInit()
{
  if (auto existing = ptr().lock()) {
    return existing;
  }

  throw std::runtime_error("InitGlfw is not initialized");
}
}  // namespace vulkan