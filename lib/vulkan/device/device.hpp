#ifndef LIB_VULKAN_DEVICE_DEVICE
#define LIB_VULKAN_DEVICE_DEVICE

#include <cstdint>
#include <memory>

#include <vulkan/vulkan_core.h>

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
  uint32_t _graphicsQueueIndex;
  std::unique_ptr<VkDevice_T, void (*)(VkDevice)> _device;
  VkQueue _graphicsQueue;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEVICE_DEVICE */
