#ifndef LIB_VULKAN_DEVICE_DEVICE
#define LIB_VULKAN_DEVICE_DEVICE

#include <vulkan/vulkan_core.h>

namespace vulkan {
class VulkanDevice {
public:
  VulkanDevice(const VulkanDevice&) = delete;
  VulkanDevice(VulkanDevice&&) = delete;
  VulkanDevice& operator=(const VulkanDevice&) = delete;
  VulkanDevice& operator=(VulkanDevice&&) = delete;

  explicit VulkanDevice(const VkSurfaceKHR& surface);
  ~VulkanDevice() = default;

private:
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEVICE_DEVICE */
