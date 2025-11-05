#include "vulkanDevice.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "printTable.hpp"

namespace vul {
VulkanDevice::VulkanDevice(const VkInstance& instance) : _instance(instance), _physicalDevice(selectPhysicalDevices()) {}

std::string VulkanDevice::deviceType(const VkPhysicalDeviceType type)
{
  switch (type) {
  case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    return "\033[0;37mOther / Unspecified\033[0m";
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    return "\033[1;34mIntegrated GPU\033[0m";
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    return "\033[1;32mDiscrete GPU\033[0m";
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    return "\033[1;35mVirtual GPU\033[0m";
  case VK_PHYSICAL_DEVICE_TYPE_CPU:
    return "\033[1;33mCPU as GPU\033[0m";
  default:
    return "\033[1;31mUnknown\033[0m";
  }
}

VkPhysicalDevice VulkanDevice::selectPhysicalDevices()
{
  const std::vector<DeviceData> devicesData = getDeviceData(getPhysicalDevices());

#ifdef DEBUG
  debugInfo(devicesData);
#endif

  return getBestDevice(devicesData).device;
}

std::vector<VkPhysicalDevice> VulkanDevice::getPhysicalDevices()
{
  uint32_t deviceCount = 0;
  if (const VkResult status = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr); deviceCount == 0 || status != VK_SUCCESS) {
    throw std::runtime_error(std::format("No GPUs with Vulkan support found! status: {}", std::to_string(status)));
  }

  std::vector<VkPhysicalDevice> devices;
  devices.resize(deviceCount);
  if (const VkResult status = vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data()); status != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get GPUs! status: {}", std::to_string(status)));
  }

  return devices;
}

std::vector<VulkanDevice::DeviceData> VulkanDevice::getDeviceData(const std::vector<VkPhysicalDevice>& devices)
{
  std::vector<VulkanDevice::DeviceData> deviceData;
  deviceData.reserve(devices.size());

  for (const auto& device : devices) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    deviceData.emplace_back(device, properties, features);
  }

  return deviceData;
}

void VulkanDevice::debugInfo(const std::vector<DeviceData>& devicesData)
{
  using printInfo = std::tuple<std::string, std::vector<std::string>>;
  std::vector<printInfo> deviceData;
  deviceData.reserve(deviceProperty.size());

  for (const PropertiesT& prop : deviceProperty) {
    std::vector<std::string> values;
    values.reserve(devicesData.size());
    for (const DeviceData& dev : devicesData) {
      values.push_back(std::get<1>(prop)(dev));
    }
    deviceData.emplace_back(std::get<0>(prop), std::move(values));
  }

  std::vector<printTable::TableColumn<printInfo>> columns = {{
      .header = "Var name",
      .align = printTable::Align::left,
      .toString = [](const printInfo& ele) { return std::get<0>(ele); },
  }};

  for (size_t i = 0; i < devicesData.size(); i++) {
    columns.push_back({
        .header = std::string_view(static_cast<const char*>(devicesData.at(i).properties.deviceName)),
        .align = printTable::Align::left,
        .toString = [i](const printInfo& ele) { return std::get<1>(ele).at(i); },
    });
  }

  printTable::print("Device data", deviceData, columns);
}

VulkanDevice::DeviceData VulkanDevice::getBestDevice(const std::vector<DeviceData>& devicesData)
{
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

  const int bestDeviceType = std::ranges::max(std::views::transform(devicesData, [&rateType](const DeviceData& ele) {  //
    return rateType(ele.properties);
  }));

  auto bestDeviceTypeDevices = std::views::filter(devicesData, [bestDeviceType, &rateType](const DeviceData& ele) {  //
    return rateType(ele.properties) == bestDeviceType;
  });

  const DeviceData bestDevice = *std::ranges::max_element(bestDeviceTypeDevices, [](const DeviceData& first, const DeviceData& second) {
    return first.properties.limits.maxImageDimension2D < second.properties.limits.maxImageDimension2D;
  });

  return bestDevice;
}

}  // namespace vul