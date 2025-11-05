#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string_view>
#define GLFW_INCLUDE_VULKAN

#include <memory>

#include <GLFW/glfw3.h>

namespace vul {

class Window {
  struct WindowDef {
    int width = defaultWidth;
    int height = defaultHeight;
    const std::string_view title = "Vulkan window";
    int resize = GLFW_FALSE;

    constexpr static int defaultWidth = 800;
    constexpr static int defaultHeight = 600;
  };

public:
  Window(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) = delete;

  explicit Window(const WindowDef& def);
  ~Window() = default;

  [[nodiscard]] bool shouldClose() const;

private:
  const std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> _window;

  static GLFWwindow* createWindow(const WindowDef& def);
};
}  // namespace vul

#endif