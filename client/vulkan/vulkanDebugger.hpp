#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <vector>
#include "types.h"

namespace vul {
class VulkanDebugger {
public:
  VulkanDebugger(const VulkanDebugger&) = delete;
  VulkanDebugger(VulkanDebugger&&) = delete;
  VulkanDebugger& operator=(const VulkanDebugger&) = delete;
  VulkanDebugger& operator=(VulkanDebugger&&) = delete;

  VulkanDebugger(const VkInstance instance,
                 const std::vector<const char*>& extensions,
                 const std::vector<const char*>& layers);
  ~VulkanDebugger();

private:
  const VkInstance _instance;
  const VkDebugUtilsMessengerEXT _debugMessenger;

  static void checkIfExtensionIsAvailable(const std::vector<const char*>& extensions,
                                          const std::vector<const char*>& layers);
  VkDebugUtilsMessengerEXT createDebugMessenger(const std::vector<const char*>& extensions,
                                                const std::vector<const char*>& layers);
  static VKAPI_ATTR uint32 VKAPI_CALL debugCallback(  //
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData);
};
}  // namespace vul