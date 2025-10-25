#pragma once

#include <vulkan/vulkan_core.h>
#include <memory>
#include <vector>
#include "VulkanDebugger.hpp"
#include "types.h"

namespace vul {
class VulkanInit {
private:
  struct VulkanDef {
    const char* appName = "appName";
    const char* engineName = "engineName";

    uint32 appVersion = VK_MAKE_VERSION(1, 0, 0);
    uint32 engineVersion = VK_MAKE_VERSION(1, 0, 0);

#ifdef DEBUG
    bool enableDebugger = true;
    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> extensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
#else
    bool enableDebugger = false;
    std::vector<const char*> layers{};
    std::vector<const char*> extensions{};
#endif
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
  std::unique_ptr<VulkanDebugger> _vulkanDebugger = nullptr;

  static VkInstance createInstance(const VulkanDef& def);

  static std::vector<const char*> combineExtensionsWithGlfwExtensions(const std::vector<const char*>& def);
  static std::vector<VkExtensionProperties> getAllAvailableExtensions();
  static std::vector<VkLayerProperties> getAllAvailableLayers();

  static void createDebugValidation();
  static void checkExtensions(const std::vector<const char*>& extensions,
                              const std::vector<VkExtensionProperties>& availableExtensions);
  static void checkLayers(const std::vector<const char*>& layers,
                          const std::vector<VkLayerProperties>& availableLayers);
};

}  // namespace vul