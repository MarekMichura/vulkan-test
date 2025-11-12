#ifndef LIB_VULKAN_INIT_GLFW_INIT
#define LIB_VULKAN_INIT_GLFW_INIT

#include <memory>

namespace vulkan {
class InitGlfw {
public:
  InitGlfw(const InitGlfw&) = delete;
  InitGlfw(InitGlfw&&) = delete;
  InitGlfw& operator=(const InitGlfw&) = delete;
  InitGlfw& operator=(InitGlfw&&) = delete;
  ~InitGlfw();

  static std::shared_ptr<InitGlfw> getInit();
  static std::shared_ptr<InitGlfw> createInit();

private:
  InitGlfw();

  static std::weak_ptr<InitGlfw>& ptr();
};
}  // namespace vulkan

#endif /* LIB_VULKAN_INIT_GLFW_INIT */
