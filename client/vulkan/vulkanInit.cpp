#include "vulkanInit.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <format>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

#include "printTable.hpp"
#include "vulkanDebugger.hpp"
#include "vulkanDevice.hpp"

namespace vul {
VulkanInit::VulkanInit(const VulkanDef& def)
    : _instance(createInstance(def), [](VkInstance inst) { vkDestroyInstance(inst, nullptr); }),
      _vulkanDebugger(def.enableDebugger ? std::make_optional<VulkanDebugger>(_instance.get(), def.extensions, def.layers) : std::nullopt),
      _vulkanDevice(std::make_unique<VulkanDevice>(_instance.get()))
{
}

VkInstance VulkanInit::createInstance(const VulkanDef& def)
{
  assert(*def.appName.end() == '\0');
  assert(*def.engineName.end() == '\0');

  const std::vector<VkExtensionProperties> availableExtensions = getAllAvailableExtensions();
  const std::vector<const char*> extensions = combineExtensionsWithGlfwExtensions(def.extensions);
  const std::vector<VkLayerProperties> availableLayers = getAllAvailableLayers();

  checkExtensions(extensions, availableExtensions);
  checkLayers(def.layers, availableLayers);

  const auto* applicationName = def.appName.data();
  const auto* engineName = def.engineName.data();

  const VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = applicationName,
      .applicationVersion = def.appVersion,
      .pEngineName = engineName,
      .engineVersion = def.engineVersion,
      .apiVersion = VK_API_VERSION_1_3,
  };

  const VkInstanceCreateInfo instanceInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(def.layers.size()),
      .ppEnabledLayerNames = def.layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

  VkInstance instance = nullptr;
  if (const VkResult status = vkCreateInstance(&instanceInfo, nullptr, &instance); status != VK_SUCCESS) {
    throw std::runtime_error(std::format("can not create vulkan instance. status: {}", std::to_string(status)));
  }

  return instance;
}

std::vector<const char*> VulkanInit::combineExtensionsWithGlfwExtensions(const std::vector<const char*>& def)
{
  uint32_t glfwCountInstanceExtensions = 0;
  const char** glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwCountInstanceExtensions);
  if (glfwInstanceExtensions == nullptr) {
    throw std::runtime_error("GLFW: Failed to get extensions");
  }

  std::vector<const char*> extensions;
  extensions.reserve(glfwCountInstanceExtensions + def.size());

  const std::span<const char* const> glfwInstanceExtensionsSpan(glfwInstanceExtensions, glfwCountInstanceExtensions);
  for (const char* const ele : glfwInstanceExtensionsSpan) {
    extensions.emplace_back(ele);
  }
  extensions.insert(extensions.begin(), def.begin(), def.end());

  return extensions;
}

std::vector<VkExtensionProperties> VulkanInit::getAllAvailableExtensions()
{
  uint32_t count = 0;
  if (const VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available extensions: {}", std::to_string(result)));
  }

  std::vector<VkExtensionProperties> extensions(count);
  if (const VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data()); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available extensions: {}", std::to_string(result)));
  }

#ifdef DEBUG
  // clang-format off
  printTable::print( "Vulkan Extensions", extensions,
      {{.header = "Name",.toString = [](const VkExtensionProperties& ele) { return std::string(static_cast<const char*>(ele.extensionName)); }},
       {.header = "Version", .toString = [](const VkExtensionProperties& ele) { return printTable::version(ele.specVersion); }}});
  // clang-format on
#endif

  return extensions;
}

std::vector<VkLayerProperties> VulkanInit::getAllAvailableLayers()
{
  uint32_t count = 0;
  if (const VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available layers: {}", std::to_string(result)));
  }

  std::vector<VkLayerProperties> layers(count);
  if (const VkResult result = vkEnumerateInstanceLayerProperties(&count, layers.data()); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available layers: {}", std::to_string(result)));
  }

#ifdef DEBUG
  // clang-format off
  printTable::print(
      "Vulkan layers", layers,
      {{.header = "Name", .toString = [](const auto& ele) { return std::string(static_cast<const char*>(ele.layerName)); }},
       {.header = "Version", .toString = [](const auto& ele) { return printTable::version(ele.specVersion); }},
       {.header = "Implementation", .toString = [](const auto& ele) { return printTable::version(ele.implementationVersion); }}});
  // clang-format on
#endif

  return layers;
}

void VulkanInit::checkExtensions(const std::vector<const char*>& extensions, const std::vector<VkExtensionProperties>& availableExtensions)
{
  for (const char* extension : extensions) {
    const bool found = std::ranges::any_of(availableExtensions, [&extension](const VkExtensionProperties& ext) {
      return std::string_view(static_cast<const char*>(ext.extensionName)) == extension;
    });

    if (!found) {
      throw std::runtime_error(std::format("Extension '{}' is not available", extension));
    }
  }
}

void VulkanInit::checkLayers(const std::vector<const char*>& layers, const std::vector<VkLayerProperties>& availableLayers)
{
  for (const auto& layer : layers) {
    const bool found = std::ranges::any_of(availableLayers, [&layer](const VkLayerProperties& lay) {  //
      return std::string_view(static_cast<const char*>(lay.layerName)) == layer;
    });

    if (!found) {
      throw std::runtime_error(std::format("Layer: {} is not available", layer));
    }
  }
}

}  // namespace vul
