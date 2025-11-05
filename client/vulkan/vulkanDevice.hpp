#ifndef VULKAN_DEVICE_HPP
#define VULKAN_DEVICE_HPP

#include <array>
#include <functional>
#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace vul {
class VulkanDevice {
public:
  VulkanDevice(const VulkanDevice&) = delete;
  VulkanDevice(VulkanDevice&&) = delete;
  VulkanDevice& operator=(const VulkanDevice&) = delete;
  VulkanDevice& operator=(VulkanDevice&&) = delete;

  explicit VulkanDevice(const VkInstance& instance);
  ~VulkanDevice() = default;

private:
  struct DeviceData {
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
  };

  const VkInstance _instance;              // NOLINT(misc-misplaced-const)
  const VkPhysicalDevice _physicalDevice;  // NOLINT(misc-misplaced-const)

  VkPhysicalDevice selectPhysicalDevices();
  std::vector<VkPhysicalDevice> getPhysicalDevices();

  static std::vector<DeviceData> getDeviceData(const std::vector<VkPhysicalDevice>& devices);
  static std::string getDeviceTypeName(VkPhysicalDeviceType type);
  static DeviceData getBestDevice(const std::vector<DeviceData>& devicesData);
  static void debugInfo(const std::vector<DeviceData>& devicesData);

  using PropertiesT = std::tuple<std::string_view, std::function<std::string(const VulkanDevice::DeviceData&)>>;
  static const std::array<PropertiesT, 172> deviceProperty;
  static std::string deviceType(const VkPhysicalDeviceType type);
};
}  // namespace vul

#endif