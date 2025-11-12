#ifndef LIB_VULKAN_UTILS_GETFUNC
#define LIB_VULKAN_UTILS_GETFUNC

#include <stdexcept>
#include <type_traits>

#include <vulkan/vulkan_core.h>

#include "init_vulkan/init.hpp"

namespace vulkan {
template <typename T>
T getFunc()
{
  auto instance = InitVulkan::getInit();
  if constexpr (std::is_same_v<T, PFN_vkCreateDebugUtilsMessengerEXT>) {
    if (auto func = vkGetInstanceProcAddr(instance->getInstance(), "vkCreateDebugUtilsMessengerEXT"); func != nullptr) {
      return reinterpret_cast<T>(func);
    }
    throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT function");
  }
  else if constexpr (std::is_same_v<T, PFN_vkDestroyDebugUtilsMessengerEXT>) {
    if (auto func = vkGetInstanceProcAddr(instance->getInstance(), "vkDestroyDebugUtilsMessengerEXT"); func != nullptr) {
      return reinterpret_cast<T>(func);
    }
    throw std::runtime_error("Failed to load vkDestroyDebugUtilsMessengerEXT function");
  }
  else {
    static_assert(sizeof(T) == 0, "getFunc called with unsupported type");
  }
}
}  // namespace vulkan

#endif /* LIB_VULKAN_UTILS_GETFUNC */
