#pragma once
#include "window.hpp"
#include <string>

namespace vul {
class App {
public:
  static constexpr int width = 800;
  static constexpr int height = 600;
  static constexpr std::string title = "Vulkan App";

  void run();

private:
  vul::Window _window{width, height, title};
};
}  // namespace vul