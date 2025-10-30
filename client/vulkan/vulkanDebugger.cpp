#include "vulkanDebugger.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

namespace vul {
VulkanDebugger::VulkanDebugger(const VkInstance& instance,
                               const std::vector<const char*>& extensions,
                               const std::vector<const char*>& layers)
    : _instance(instance), _debugMessenger(createDebugMessenger(extensions, layers))
{
}

VulkanDebugger::~VulkanDebugger()
{
  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(  // NOLINT
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
  auto extensionIt = std::ranges::find_if(
      extensions, [](const char* str) { return std::strcmp(str, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0; });

  auto layerIt = std::ranges::find_if(
      layers, [](const char* str) { return std::strcmp(str, "VK_LAYER_KHRONOS_validation") == 0; });

  if (extensionIt == extensions.end() || layerIt == layers.end()) {
    throw std::runtime_error("Vulkan debuging extension is off");
  }
}

VkDebugUtilsMessengerEXT VulkanDebugger::createDebugMessenger(const std::vector<const char*>& extensions,
                                                              const std::vector<const char*>& layers)
{
  checkIfExtensionIsAvailable(extensions, layers);
  const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) |
                         static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) |
                         static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
      .messageType = static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) |
                     static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) |
                     static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
      .pfnUserCallback = debugCallback,
      .pUserData = nullptr,
  };

  VkDebugUtilsMessengerEXT debugMessenger{};
  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(  // NOLINT
      vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT"));
  if (func == nullptr || func(_instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }

  return debugMessenger;
}

VKAPI_ATTR uint32_t VKAPI_CALL VulkanDebugger::debugCallback(  //
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
{
  std::ostream& out = ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) ? std::cerr : std::cout;

  const std::chrono::time_point now = std::chrono::system_clock::now();
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
    out << "\033[1;31m";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    out << "\033[1;33m";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    out << "\033[1;34m";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    out << "\033[1;37m";
    break;
  default:
    out << "\033[37m";
    break;
  }

  out << pCallbackData->pMessage << "\033[0m\n";
  return VK_FALSE;
}

}  // namespace vul