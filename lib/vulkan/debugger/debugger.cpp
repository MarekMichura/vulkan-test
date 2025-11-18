#include "debugger.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include "available/available.hpp"
#include "debugger/debugger_deleter.hpp"
#include "format/ansi.hpp"
#include "format/string.hpp"
#include "init_vulkan/init.hpp"
#include "init_vulkan/init_info.hpp"
#include "utils/getFunc.hpp"

namespace vulkan {
static VKAPI_ATTR uint32_t VKAPI_CALL debugCallback(  //
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
{
  static constexpr auto end = utils::ansi();
  auto getMessageType = [](VkDebugUtilsMessageTypeFlagsEXT type) {
    static constexpr auto bold = utils::ansi<{.bold = true, .underline = true}>();

    switch (type) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
      return std::format("{}[{:^11}]{}", bold, "GENERAL", end);
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
      return std::format("{}[{:^11}]{}", bold, "PERFORMANCE", end);
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
      return std::format("{}[{:^11}]{}", bold, "VALIDATION", end);
    case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
      return std::format("{}[{:^11}]{}", bold, "ADDRESS", end);
    default:
      return std::format("{}[{:^11}]{}", bold, "UNKNOWN", end);
    }
  };
  auto getMessageColor = [](VkDebugUtilsMessageSeverityFlagBitsEXT severity) -> std::string_view {
    static constexpr auto info = utils::ansi<{.bold = true, .color = utils::COLOR::BRIGHT_WHITE}>();
    static constexpr auto warning = utils::ansi<{.bold = true, .color = utils::COLOR::YELLOW}>();
    static constexpr auto error = utils::ansi<{.bold = true, .color = utils::COLOR::RED}>();
    static constexpr auto verbose = utils::ansi<{.bold = true, .color = utils::COLOR::CYAN}>();

    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      return {info};
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      return {warning};
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      return {error};
    default:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      return {verbose};
    }
  };
  auto time = []() {
    static constexpr auto negative = utils::ansi<{.bold = true, .negative = true}>();
    static const std::chrono::time_zone* zone = std::chrono::current_zone();
    std::chrono::zoned_time now = {zone, floor<std::chrono::seconds>(std::chrono::system_clock::now())};

    return std::format("{} {:%H:%M:%S} {}", negative, now, end);
  };

  std::ostream& out = ((messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) ? std::cerr : std::cout;
  static constexpr std::string_view format = "   {} {} {}{}{}\n";
  out << std::format(format, time(), getMessageType(messageType), getMessageColor(messageSeverity), pCallbackData->pMessage, end);
  return VK_FALSE;
}

static VkDebugUtilsMessengerEXT createDebugger(const VulkanInfo& info)
{
  static constexpr const char* neededExtension = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  static constexpr const char* neededLayer = "VK_LAYER_KHRONOS_validation";
  auto instance = InitVulkan::getInit();

  if (!utils::checkPresent(neededExtension, info.extensions)) {
    throw std::runtime_error("Required Vulkan extension VK_EXT_debug_utils is not present");
  }
  if (!utils::checkPresent(neededLayer, info.layers)) {
    throw std::runtime_error("Required Vulkan validation layer VK_LAYER_KHRONOS_validation is not present");
  }

  VkDebugUtilsMessengerEXT debugMessenger{};
  const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity =  //
      static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) |
      static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) |
      static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) |
      static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
      .messageType =  //
      static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) |
      static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) |
      static_cast<uint32_t>(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
      .pfnUserCallback = debugCallback,
      .pUserData = nullptr,
  };
  auto func = getFunc<PFN_vkCreateDebugUtilsMessengerEXT>();
  if (const VkResult status = func(instance->getInstance(), &debugCreateInfo, nullptr, &debugMessenger); status != VK_SUCCESS) {
    throw std::runtime_error(std::format("failed to set up debug messenger! status: {}", utils::result(status)));
  }

  return debugMessenger;
}

VulkanDebugger::VulkanDebugger(const VulkanInfo& info) : _debugger(createDebugger(info), DebuggerDeleter{}) {}
}  // namespace vulkan