#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace vul {
class VulkanInit {
private:
  struct VulkanDef {
    const char* appName = "appName";
    const char* engineName = "engineName";

    unsigned int appVersion = VK_MAKE_VERSION(1, 0, 0);
    unsigned int engineVersion = VK_MAKE_VERSION(1, 0, 0);

    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
  };

public:
  VulkanInit(const VulkanInit&) = delete;
  VulkanInit(VulkanInit&&) = delete;
  VulkanInit& operator=(const VulkanInit&) = delete;
  VulkanInit& operator=(VulkanInit&&) = delete;

  VulkanInit(const VulkanDef& def);
  ~VulkanInit();

private:
  const VkInstance _instance;

  static VkInstance createInstance(const VulkanDef& def);
  static VkInstance checkExtensions();

#ifdef DEBUG
  static void vulcanExtensionInfo(const std::vector<VkExtensionProperties>& extensions);
#endif
};

}  // namespace vul