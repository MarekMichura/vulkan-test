#include <vulkan/vulkan_core.h>
#include <algorithm>
#include <cstring>
#include <format>
#include <print>
#include <stdexcept>
#include <vector>
#include "GLFW/glfw3.h"
#include "types.h"
#include "vulkanInit.hpp"

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
  constexpr int nameWidth = 40;
  constexpr int verWidth = 7;
  constexpr int marginWidth = nameWidth + verWidth + 7;

  std::println("{:=^{}s}", " Vulkan Extensions ", marginWidth);
  std::println("| {:^{}} | {:^{}} |", "Name", nameWidth, "Version", verWidth);
  std::println("{:=^{}s}", "", marginWidth);
  for (const auto& ele : extensions) {
    std::println("|-{:-^{}}-|-{:-^{}}-|", ele.extensionName, nameWidth, ele.specVersion, verWidth);
  }
  std::println("{:=^{}s}\n", "", marginWidth);
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
  constexpr int nameWidth = 40;
  constexpr int verWidth = 7;
  constexpr int impWidth = 14;
  constexpr int marginWidth = nameWidth + verWidth + impWidth + 9;

  std::println("{:=^{}s}", " Vulkan Layers ", marginWidth);
  std::println("| {:^{}} | {:^{}} | {:^{}} |", "Name", nameWidth, "Version", verWidth, "Implementation", impWidth);
  std::println("{:=^{}s}", "", marginWidth);
  for (const auto& ele : layers) {
    std::println("|-{:-^{}}-|-{:-^{}}-|-{:-^{}}-|",  //
                 ele.layerName, nameWidth, ele.specVersion, verWidth, ele.implementationVersion, impWidth);
  }
  std::println("{:=^{}s}\n", "", marginWidth);
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
