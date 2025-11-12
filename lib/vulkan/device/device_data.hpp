#ifndef LIB_VULKAN_DEVICE_DEVICE_DATA
#define LIB_VULKAN_DEVICE_DEVICE_DATA

#include <vector>

#include <vulkan/vulkan_core.h>

namespace vulkan {
struct DeviceDataQueue {
  VkQueueFamilyProperties properties;
  VkBool32 supportKHR;
};

struct DeviceData {
  VkPhysicalDevice device;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  VkPhysicalDeviceMemoryProperties memory;
  std::vector<DeviceDataQueue> queues;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEVICE_DEVICE_DATA */
