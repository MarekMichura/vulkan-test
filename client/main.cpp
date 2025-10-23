#include <cstdlib>
#include <exception>
#include <iostream>
#include "app.hpp"
#include "glfw.hpp"
#include "vulkan.hpp"

int main()
{
  try {
    vul::GLFW glfw;
    vul::Vulkan vulkan;
    vul::App app;

    app.run();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
