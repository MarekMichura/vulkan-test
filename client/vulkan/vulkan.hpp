#pragma once

#include <vulkan/vulkan_core.h>
#include <array>
#include <string>
#include <vector>
namespace vul {
class Vulkan {
public:
  Vulkan(const Vulkan&) = delete;
  Vulkan(Vulkan&&) = delete;
  Vulkan& operator=(const Vulkan&) = delete;
  Vulkan& operator=(Vulkan&&) = delete;

  Vulkan(const std::string name = "Vulkan");
  ~Vulkan();

private:
  const std::string _title;
  const VkApplicationInfo _appInfo;
  const std::vector<const char*> _glfwExtensions;
  const VkInstanceCreateInfo _instanceInfo;
  const VkInstance _instance;

  VkInstance createInstance();
  VkApplicationInfo createAppInfo();
  VkInstanceCreateInfo createInstanceInfo();

  static std::vector<const char*> getGlfwExtensions();

  static constexpr uint32_t _appVersion = VK_MAKE_VERSION(1, 0, 0);
  static constexpr uint32_t _engineVersion = VK_MAKE_VERSION(1, 0, 0);
  static constexpr const char* const _engineName = "No Engine";
  static constexpr std::array<const char*, 1> _vulkanLayers = {"VK_LAYER_KHRONOS_validation"};
};

}  // namespace vul