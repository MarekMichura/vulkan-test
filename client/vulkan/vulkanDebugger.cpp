#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "types.h"
#include <vulkan/vulkan_core.h>
#include "vulkanDebugger.hpp"

namespace vul {
VulkanDebugger::VulkanDebugger(const VkInstance instance,
                               const std::vector<const char*>& extensions,
                               const std::vector<const char*>& layers)
    : _instance(instance), _debugMessenger(createDebugMessenger(extensions, layers))
{
}

VulkanDebugger::~VulkanDebugger()
{
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT"));
  if (func == nullptr) {
    std::cerr << "debug messenger was not destroyed!, vkDestroyDebugUtilsMessengerEXT not found!";
  }
  else {
    func(_instance, _debugMessenger, nullptr);
  }
}

void VulkanDebugger::checkIfExtensionIsAvailable(const std::vector<const char*>& extensions,
                                                 const std::vector<const char*>& layers)
{
  auto extensionIt = std::find_if(  //
      extensions.begin(), extensions.end(),
      [](const char* str) { return std::strcmp(str, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0; });

  auto layerIt = std::find_if(  //
      layers.begin(), layers.end(),
      [](const char* str) { return std::strcmp(str, "VK_LAYER_KHRONOS_validation") == 0; });

  if (extensionIt == extensions.end() || layerIt == layers.end()) {
    throw std::runtime_error("Vulkan debuging extension is off");
  }
}

VkDebugUtilsMessengerEXT VulkanDebugger::createDebugMessenger(const std::vector<const char*>& extensions,
                                                              const std::vector<const char*>& layers)
{
  checkIfExtensionIsAvailable(extensions, layers);
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |  //
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |  //
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,     //
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |          //
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |       //
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
      .pUserData = nullptr,
  };

  VkDebugUtilsMessengerEXT debugMessenger{};
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT"));
  if (func == nullptr || func(_instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }

  return debugMessenger;
}

VKAPI_ATTR uint32 VKAPI_CALL VulkanDebugger::debugCallback(  //
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
{
  std::ostream& out = ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) ? std::cerr : std::cout;

  auto now = std::chrono::system_clock::now();
  out << "\t" << std::format("[{0:%H:%M:%S}] ", time_point_cast<std::chrono::seconds>(now));

  switch (messageType) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
      out << "[Validation] ";
      break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
      out << "[Performance] ";
      break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
      out << "[General] ";
      break;
    default:
      break;
  }

  switch (messageSeverity) {  // color select
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      out << "\033[31m";
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      out << "\033[33m";
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      out << "\033[34m";
      break;
    default:
      out << "\033[37m";
      break;
  }

  out << pCallbackData->pMessage << "\033[0m" << std::endl;
  return VK_FALSE;
}

}  // namespace vul