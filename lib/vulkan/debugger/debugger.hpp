#ifndef LIB_VULKAN_DEBUGGER_DEBUGGER
#define LIB_VULKAN_DEBUGGER_DEBUGGER

#include <memory>

#include <vulkan/vulkan_core.h>

#include "debugger_deleter.hpp"
#include "init_vulkan/init_info.hpp"

namespace vulkan {
class VulkanDebugger {
public:
  VulkanDebugger(const VulkanDebugger&) = delete;
  VulkanDebugger(VulkanDebugger&&) = delete;
  VulkanDebugger& operator=(const VulkanDebugger&) = delete;
  VulkanDebugger& operator=(VulkanDebugger&&) = delete;

  explicit VulkanDebugger(const VulkanInfo& info);
  ~VulkanDebugger() = default;

private:
  const std::unique_ptr<VkDebugUtilsMessengerEXT_T, DebuggerDeleter> _debugger;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEBUGGER_DEBUGGER */
