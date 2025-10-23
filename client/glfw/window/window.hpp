#pragma once
#define GLFW_INCLUDE_VULKAN

#include <memory>
#include <GLFW/glfw3.h>
#include <string>

namespace vul {
class Window {
public:
  Window(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) = delete;
  ~Window() = default;

  Window(int width, int height, const std::string& title);

  bool shouldClose() const;

private:
  const int _width;
  const int _height;
  const std::string _title;
  std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> _window;
};
}  // namespace vul