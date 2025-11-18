#ifndef LIB_VULKAN_DEVICE_DEVICE
#define LIB_VULKAN_DEVICE_DEVICE

#include <memory>

#include <vulkan/vulkan_core.h>

#include "queue.hpp"
#include "window/window_info.hpp"

namespace vulkan {
class VulkanDevice {
public:
  VulkanDevice(const VulkanDevice&) = delete;
  VulkanDevice(VulkanDevice&&) = default;
  VulkanDevice& operator=(const VulkanDevice&) = delete;
  VulkanDevice& operator=(VulkanDevice&&) = default;

  explicit VulkanDevice(const WindowInfo& info, const VkSurfaceKHR& surface);
  ~VulkanDevice() = default;

private:
  std::unique_ptr<VkDevice_T, void (*)(VkDevice)> _device;
  std::unique_ptr<Queue> _queue = nullptr;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEVICE_DEVICE */
