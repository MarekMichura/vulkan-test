#include "vulkan.hpp"
#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <vector>
#include "GLFW/glfw3.h"

namespace vul {
std::vector<const char*> Vulkan::getGlfwExtensions()
{
  unsigned int count = 0;
  const char** list = glfwGetRequiredInstanceExtensions(&count);

  return std::vector<const char*>(list, list + count);
}

Vulkan::Vulkan(const std::string name)
    : _title(name),
      _appInfo(createAppInfo()),
      _glfwExtensions(getGlfwExtensions()),
      _instanceInfo(createInstanceInfo()),
      _instance(createInstance())
{
}

Vulkan::~Vulkan()
{
  vkDestroyInstance(_instance, nullptr);
}

VkApplicationInfo Vulkan::createAppInfo()
{
  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = _title.c_str(),
      .applicationVersion = _appVersion,
      .pEngineName = _engineName,
      .engineVersion = _engineVersion,
      .apiVersion = VK_API_VERSION_1_0,
  };

  return appInfo;
}

VkInstanceCreateInfo Vulkan::createInstanceInfo()
{
  VkInstanceCreateInfo instanceInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &_appInfo,
      .enabledLayerCount = _vulkanLayers.size(),
      .ppEnabledLayerNames = _vulkanLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(_glfwExtensions.size()),
      .ppEnabledExtensionNames = _glfwExtensions.data(),
  };

  return instanceInfo;
}

VkInstance Vulkan::createInstance()
{
  VkInstance instance{};
  if (vkCreateInstance(&_instanceInfo, NULL, &instance) != VK_SUCCESS) {
    throw std::runtime_error("can not create vulkan instance");
  }

  return instance;
}

}  // namespace vul