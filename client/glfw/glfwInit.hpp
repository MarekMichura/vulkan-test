#pragma once

namespace vul {
class GlfwInit {
public:
  GlfwInit(const GlfwInit&) = delete;
  GlfwInit(GlfwInit&&) = delete;
  GlfwInit& operator=(const GlfwInit&) = delete;
  GlfwInit& operator=(GlfwInit&&) = delete;

  GlfwInit();
  ~GlfwInit();
};
}  // namespace vul