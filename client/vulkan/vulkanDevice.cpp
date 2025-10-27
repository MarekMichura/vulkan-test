#include <algorithm>
#include <print>
#include <stdexcept>
#include <string>
#include <ranges>
#include <vector>
#include "types.h"
#include "vulkanDevice.hpp"
#include <vulkan/vulkan_core.h>

namespace vul {
VulkanDevice::VulkanDevice(const VkInstance instance) : _instance(instance), _physicalDevice(selectPhysicalDevices()) {}

VkPhysicalDevice VulkanDevice::selectPhysicalDevices()
{
  auto devicesData = getDeviceData(getPhysicalDevices());

#ifdef DEBUG
  constexpr int width = 10;
  constexpr int nameWidth = 50;
  constexpr int typeWidth = 12;
  constexpr int marginWidth = (width * 5) + nameWidth + typeWidth + 27;

  std::println("{:=^{}s}", " Vulkan devices ", marginWidth);
  std::println("| {:^{}} | {:^{}} | {:^{}} | {:^{}} | {:^{}} | {:^{}} | ",  //
               "ID", width, "Name", nameWidth, "Type", typeWidth, "Api", width, "driver", width, "vendor", width

  );
  std::println("{:=^{}s}", "", marginWidth);
  for (const auto& element : devicesData) {
    auto ele = element.second;
    std::println("| {:^{}} | {:^{}} | {:^{}} | {:^{}} | {:^{}} | {:^{}} |",                          //
                 ele.deviceID, width, ele.deviceName, nameWidth, getDeviceTypeName(ele.deviceType),  //
                 typeWidth, ele.apiVersion, width, ele.driverVersion, width, ele.vendorID, width);
  }
  std::println("{:=^{}s}\n", "", marginWidth);
#endif

  auto rateType = [](const VkPhysicalDeviceProperties& data) -> int {
    switch (data.deviceType) {
      case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return 3;
      case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return 2;
      case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return 1;
      default:
        return 0;
    }
  };

  auto bestDeviceType = std::ranges::max(                               //
      std::views::transform(devicesData, [rateType](const auto& ele) {  //
        return rateType(ele.second);
      }));

  auto bestDeviceTypeDevices = std::views::filter(                //
      devicesData, [bestDeviceType, rateType](const auto& ele) {  //
        return rateType(ele.second) == bestDeviceType;
      });

  auto bestDevice = std::ranges::max_element(  //
      bestDeviceTypeDevices, [](const auto& first, const auto& second) {
        return first.second.limits.maxImageDimension2D < second.second.limits.maxImageDimension2D;
      });
  return bestDevice->first;
}

std::vector<VkPhysicalDevice> VulkanDevice::getPhysicalDevices()
{
  uint32 deviceCount = 0;
  auto status = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
  if (deviceCount == 0 || status != VK_SUCCESS) {
    throw std::runtime_error("No GPUs with Vulkan support found!");
  }

  std::vector<VkPhysicalDevice> devices;
  devices.resize(deviceCount);
  status = vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
  if (status != VK_SUCCESS) {
    throw std::runtime_error("Failed to get GPUs!");
  }

  return devices;
}

std::vector<std::pair<VkPhysicalDevice, VkPhysicalDeviceProperties>> getDeviceData(
    const std::vector<VkPhysicalDevice>& devices)
{
  std::vector<std::pair<VkPhysicalDevice, VkPhysicalDeviceProperties>> deviceData;
  deviceData.resize(devices.size());

  std::transform(devices.begin(), devices.end(), deviceData.begin(), [](VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    return std::pair<VkPhysicalDevice, VkPhysicalDeviceProperties>(device, properties);
  });

  return deviceData;
}

std::string VulkanDevice::getDeviceTypeName(VkPhysicalDeviceType type)
{
  switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
      return "Other / Unspecified";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      return "Integrated GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      return "Discrete GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      return "Virtual GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      return "CPU as GPU";
    default:
      return "Unknown";
  }
}
}  // namespace vul