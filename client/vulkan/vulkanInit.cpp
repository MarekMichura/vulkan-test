#include <vulkan/vulkan_core.h>
#include <algorithm>
#include <cstring>
#include <format>
#include <print>
#include <stdexcept>
#include <string>
#include <vector>
#include "GLFW/glfw3.h"
#include "types.h"
#include "vulkanInit.hpp"
#include "printTable.hpp"

namespace vul {
VulkanInit::VulkanInit(const VulkanDef& def)
    : _instance(createInstance(def), [](VkInstance inst) { vkDestroyInstance(inst, nullptr); }),
      _vulkanDebugger(def.enableDebugger
                          ? std::make_optional<VulkanDebugger>(_instance.get(), def.extensions, def.layers)
                          : std::nullopt),
      _vulkanDevice(std::make_unique<VulkanDevice>(_instance.get()))
{
}

VkInstance VulkanInit::createInstance(const VulkanDef& def)
{
  auto availableExtensions = getAllAvailableExtensions();
  auto extensions = combineExtensionsWithGlfwExtensions(def.extensions);
  auto availableLayers = getAllAvailableLayers();

  checkExtensions(extensions, availableExtensions);
  checkLayers(def.layers, availableLayers);

  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = def.appName,
      .applicationVersion = def.appVersion,
      .pEngineName = def.engineName,
      .engineVersion = def.engineVersion,
      .apiVersion = VK_API_VERSION_1_3,
  };

  VkInstanceCreateInfo instanceInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32>(def.layers.size()),
      .ppEnabledLayerNames = def.layers.data(),
      .enabledExtensionCount = static_cast<uint32>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

  VkInstance instance = nullptr;
  if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("can not create vulkan instance");
  }

  return instance;
}

std::vector<const char*> VulkanInit::combineExtensionsWithGlfwExtensions(const std::vector<const char*>& def)
{
  uint32 glfwCountInstanceExtensions = 0;
  const char** glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwCountInstanceExtensions);
  if (glfwInstanceExtensions == nullptr) {
    throw std::runtime_error("GLFW: Failed to get extensions");
  }

  std::vector<const char*> extensions;
  extensions.reserve(glfwCountInstanceExtensions + def.size());

  extensions.insert(extensions.end(), glfwInstanceExtensions, glfwInstanceExtensions + glfwCountInstanceExtensions);
  extensions.insert(extensions.begin(), def.begin(), def.end());

  return extensions;
}

std::vector<VkExtensionProperties> VulkanInit::getAllAvailableExtensions()
{
  uint32 count = 0;
  VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available extensions: {}", std::to_string(result)));
  }

  std::vector<VkExtensionProperties> extensions(count);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
  if (result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available extensions: {}", std::to_string(result)));
  }

#ifdef DEBUG
  printTable("Vulkan Extensions", extensions,
             {{.header = "Name", .toString = [](const auto& ele) { return ele.extensionName; }},
              {.header = "Version", .toString = [](const auto& ele) { return std::to_string(ele.specVersion); }}});
#endif

  return extensions;
}

std::vector<VkLayerProperties> VulkanInit::getAllAvailableLayers()
{
  uint32 count = 0;
  VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available layers: {}", std::to_string(result)));
  }

  std::vector<VkLayerProperties> layers(count);
  result = vkEnumerateInstanceLayerProperties(&count, layers.data());
  if (result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available layers: {}", std::to_string(result)));
  }

#ifdef DEBUG
  printTable("Vulkan layers", layers,
             {{.header = "Name", .toString = [](const auto& ele) { return ele.layerName; }},
              {.header = "Version", .toString = [](const auto& ele) { return std::to_string(ele.specVersion); }},
              {.header = "Implementation",
               .toString = [](const auto& ele) { return std::to_string(ele.implementationVersion); }}});
#endif

  return layers;
}

void VulkanInit::checkExtensions(const std::vector<const char*>& extensions,
                                 const std::vector<VkExtensionProperties>& availableExtensions)
{
  for (const auto* const extension : extensions) {
    auto it = std::find_if(
        availableExtensions.begin(), availableExtensions.end(),
        [&extension](const VkExtensionProperties& ext) { return std::strcmp(extension, ext.extensionName) == 0; });

    if (it == availableExtensions.end()) {
      throw std::runtime_error(std::format("Extension: {} is not available", extension));
    }
  }
}

void VulkanInit::checkLayers(const std::vector<const char*>& layers,
                             const std::vector<VkLayerProperties>& availableLayers)
{
  for (const auto* const layer : layers) {
    auto it = std::find_if(availableLayers.begin(), availableLayers.end(),
                           [&layer](const VkLayerProperties& ext) { return std::strcmp(layer, ext.layerName) == 0; });

    if (it == availableLayers.end()) {
      throw std::runtime_error(std::format("Layer: {} is not available", layer));
    }
  }
}

}  // namespace vul
