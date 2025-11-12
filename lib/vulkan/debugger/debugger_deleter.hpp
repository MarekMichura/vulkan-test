#ifndef LIB_VULKAN_DEBUGGER_DEBUGGER_DELETER
#define LIB_VULKAN_DEBUGGER_DEBUGGER_DELETER
#include <memory>

#include <vulkan/vulkan_core.h>

#include "init_vulkan/init.hpp"
#include "utils/getFunc.hpp"

namespace vulkan {
struct DebuggerDeleter {
  std::shared_ptr<InitVulkan> instance = InitVulkan::getInit();

  void operator()(VkDebugUtilsMessengerEXT debugger) const
  {
    auto func = getFunc<PFN_vkDestroyDebugUtilsMessengerEXT>();
    func(instance->getInstance(), debugger, nullptr);
  }
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEBUGGER_DEBUGGER_DELETER */
