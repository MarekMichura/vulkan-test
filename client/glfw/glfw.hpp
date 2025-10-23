#pragma once

namespace vul {
class GLFW {
public:
  GLFW(const GLFW&) = delete;
  GLFW(GLFW&&) = delete;
  GLFW& operator=(const GLFW&) = delete;
  GLFW& operator=(GLFW&&) = delete;

  GLFW();
  ~GLFW();
};
}  // namespace vul