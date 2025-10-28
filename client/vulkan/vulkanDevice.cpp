#include "vulkanDevice.hpp"

#include <algorithm>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "printTable.hpp"
#include "types.h"

namespace vul {
VulkanDevice::VulkanDevice(const VkInstance& instance) : _instance(instance), _physicalDevice(selectPhysicalDevices())
{
}

VkPhysicalDevice VulkanDevice::selectPhysicalDevices()
{
  auto devicesData = getDeviceData(getPhysicalDevices());

#ifdef DEBUG

  auto formatType = [](VkPhysicalDeviceType type) {
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
  };

  auto formatVendor = [](uint32_t vendor) -> std::string {
    switch (vendor) {
    case 0x10DE:
      return "\033[1;32mNVIDIA\033[0m";
    case 0x1002:
      return "\033[1;31mAMD\033[0m";
    case 0x8086:
      return "\033[1;34mIntel\033[0m";
    case 0x13B5:
      return "\033[1;35mARM\033[0m";
    case 0x5143:
      return "\033[1;36mQualcomm\033[0m";
    case 0x106B:
      return "\033[1;33mApple\033[0m";
    case 0x1414:
      return "\033[1;37mMicrosoft (WARP)\033[0m";
    case 0x1AE0:
      return "\033[1;90mGoogle\033[0m";
    case 0x1010:
      return "\033[1;91mImgTec (PowerVR)\033[0m";
    case 0x1D05:
      return "\033[1;92mMesa / LLVMpipe\033[0m";
    default:
      return std::format("\033[1;93mUnknown 0x{:X}\033[0m", vendor);
    }
  };

  printTable::print(
      "Vulkan Devices", devicesData,
      {{.header = "ID", .toString = [](const auto& d) { return std::to_string(d.properties.deviceID); }},
       {.header = "Name", .toString = [](const auto& d) { return std::string(d.properties.deviceName); }},
       {.header = "Type", .toString = [&](const auto& d) { return formatType(d.properties.deviceType); }},
       {.header = "Vendor", .toString = [&](const auto& d) { return formatVendor(d.properties.vendorID); }},
       {.header = "API", .toString = [](const auto& d) { return printTable::version(d.properties.apiVersion); }},
       {.header = "Driver", .toString = [](const auto& d) { return printTable::version(d.properties.driverVersion); }},
       {.header = "Max Image 2D",
        .toString = [](const auto& d) { return printTable::number(d.properties.limits.maxImageDimension2D); }},
       {.header = "Max Image 3D",
        .toString = [](const auto& d) { return printTable::number(d.properties.limits.maxImageDimension3D); }},
       {.header = "Max Image Cube",
        .toString = [](const auto& d) { return printTable::number(d.properties.limits.maxImageDimensionCube); }},
       {.header = "Max Memory Allo.",
        .toString = [](const auto& d) { return printTable::number(d.properties.limits.maxMemoryAllocationCount); }},
       {.header = "Max PushConst",
        .toString = [](const auto& d) { return printTable::number(d.properties.limits.maxPushConstantsSize); }},
       {.header = "Max UniformBuf",
        .toString = [](const auto& d) { return printTable::number(d.properties.limits.maxUniformBufferRange); }},
       {.header = "Sampler Anisotropy",
        .toString = [](const auto& d) { return printTable::boolean(d.features.samplerAnisotropy); }},
       {.header = "Geometry Shader",
        .toString = [](const auto& d) { return printTable::boolean(d.features.geometryShader); }},
       {.header = "Tessellation Shader",
        .toString = [](const auto& d) { return printTable::boolean(d.features.tessellationShader); }},
       {.header = "MultiViewport",
        .toString = [](const auto& d) { return printTable::boolean(d.features.multiViewport); }},
       {.header = "Compute Shader",
        .toString = [](const auto& d) { return printTable::boolean(d.features.shaderInt64); }},
       {.header = "FillMode NonSolid",
        .toString = [](const auto& d) { return printTable::boolean(d.features.fillModeNonSolid); }},
       {.header = "Wide Lines", .toString = [](const auto& d) { return printTable::boolean(d.features.wideLines); }},
       {.header = "MultiDrawIndirect",
        .toString = [](const auto& d) { return printTable::boolean(d.features.multiDrawIndirect); }},
       {.header = "Texture Compression BC",
        .toString = [](const auto& d) { return printTable::boolean(d.features.textureCompressionBC); }}});
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
        return rateType(ele.properties);
      }));

  auto bestDeviceTypeDevices = std::views::filter(                //
      devicesData, [bestDeviceType, rateType](const auto& ele) {  //
        return rateType(ele.properties) == bestDeviceType;
      });

  auto bestDevice = *std::ranges::max_element(  //
      bestDeviceTypeDevices, [](const auto& first, const auto& second) {
        return first.properties.limits.maxImageDimension2D < second.properties.limits.maxImageDimension2D;
      });

  return bestDevice.device;
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

}  // namespace vul