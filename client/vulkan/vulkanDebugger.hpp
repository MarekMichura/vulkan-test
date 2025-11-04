#ifndef VULKAN_DEBUGGER_HPP
#define VULKAN_DEBUGGER_HPP

#include <cstdint>
#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

namespace vul {
class VulkanDebugger {
public:
  VulkanDebugger(const VulkanDebugger&) = delete;
  VulkanDebugger(VulkanDebugger&&) = delete;
  VulkanDebugger& operator=(const VulkanDebugger&) = delete;
  VulkanDebugger& operator=(VulkanDebugger&&) = delete;

  VulkanDebugger(const VkInstance& instance, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
  ~VulkanDebugger();

private:
  const VkInstance _instance;                      // NOLINT(misc-misplaced-const)
  const VkDebugUtilsMessengerEXT _debugMessenger;  // NOLINT(misc-misplaced-const)

  static void checkIfExtensionIsAvailable(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
  VkDebugUtilsMessengerEXT createDebugMessenger(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
  static VKAPI_ATTR uint32_t VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData);
};
}  // namespace vul

#endif