#ifndef VULKAN_DEVICE_HPP
#define VULKAN_DEVICE_HPP

#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace vul {
class VulkanDevice {
private:
  struct DeviceData {
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
  };

public:
  VulkanDevice(const VulkanDevice&) = delete;
  VulkanDevice(VulkanDevice&&) = delete;
  VulkanDevice& operator=(const VulkanDevice&) = delete;
  VulkanDevice& operator=(VulkanDevice&&) = delete;

  explicit VulkanDevice(const VkInstance& instance);
  ~VulkanDevice() = default;

private:
  const VkInstance _instance;              // NOLINT(misc-misplaced-const)
  const VkPhysicalDevice _physicalDevice;  // NOLINT(misc-misplaced-const)

  VkPhysicalDevice selectPhysicalDevices();
  std::vector<VkPhysicalDevice> getPhysicalDevices();

  static std::vector<DeviceData> getDeviceData(const std::vector<VkPhysicalDevice>& devices);
  static std::string getDeviceTypeName(VkPhysicalDeviceType type);
};
}  // namespace vul

#endif