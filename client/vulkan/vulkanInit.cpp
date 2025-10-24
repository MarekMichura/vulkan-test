#include <vulkan/vulkan_core.h>
#include <format>
#include <print>
#include <stdexcept>
#include "GLFW/glfw3.h"
#include "vulkanInit.hpp"

namespace vul {
VulkanInit::VulkanInit(const VulkanDef& def) : _instance(createInstance(def)) {}

VulkanInit::~VulkanInit()
{
  vkDestroyInstance(_instance, nullptr);
}

VkInstance VulkanInit::createInstance(const VulkanDef& def)
{
  unsigned int glfwCountInstanceExtensions = 0;
  const char** glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwCountInstanceExtensions);

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
      .enabledLayerCount = static_cast<unsigned int>(def.layers.size()),
      .ppEnabledLayerNames = def.layers.data(),
      .enabledExtensionCount = glfwCountInstanceExtensions,
      .ppEnabledExtensionNames = glfwInstanceExtensions,
  };

  VkInstance instance{};
  if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("can not create vulkan instance");
  }
  unsigned int extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

#ifdef DEBUG
  vulcanExtensionInfo(extensions);
#endif
  return instance;
}

#ifdef DEBUG
void VulkanInit::vulcanExtensionInfo(const std::vector<VkExtensionProperties>& extensions)
{
  constexpr int extensionWidth = 40;
  constexpr int versionWidth = 7;
  constexpr int marginWidth = extensionWidth + versionWidth + 7;

  std::println("{:=^{}s}", " Vulkan Extensions ", marginWidth);
  std::println("| {:^{}} | {:^{}} |", "Name", extensionWidth, "Version", versionWidth);
  std::println("{:=^{}s}", "", marginWidth);
  for (const auto& ele : extensions) {
    std::println("|-{:->{}}-|-{:-^{}}-|", ele.extensionName, extensionWidth, ele.specVersion, versionWidth);
  }
  std::println("{:=^{}s}", "", marginWidth);
}
#endif

}  // namespace vul