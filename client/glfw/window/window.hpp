#pragma once
#define GLFW_INCLUDE_VULKAN

#include <memory>
#include <string>
#include <string_view>

#include <GLFW/glfw3.h>

namespace vul {
class Window {
private:
  struct windowDef {
    int width = defaultWidth;
    int height = defaultHeight;
    std::string_view title = "Vulkan window";
    int resize = GLFW_FALSE;

    constexpr static int defaultWidth = 800;
    constexpr static int defaultHeight = 600;
  };

public:
  Window(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) = delete;

  explicit Window(const windowDef& def);
  ~Window() = default;

  [[nodiscard]] bool shouldClose() const;

private:
  const int _width;
  const int _height;
  const std::string _title;
  std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> _window;
};
}  // namespace vul