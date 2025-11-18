#ifndef LIB_VULKAN_DEBUGGER_DEBUGGER_DELETER
#define LIB_VULKAN_DEBUGGER_DEBUGGER_DELETER
#include <exception>
#include <iostream>
#include <memory>

#include <vulkan/vulkan_core.h>

#include "init_vulkan/init.hpp"
#include "utils/getFunc.hpp"

namespace vulkan {
struct DebuggerDeleter {
  void operator()(VkDebugUtilsMessengerEXT debugger) const
  {
    try {
      auto func = getFunc<PFN_vkDestroyDebugUtilsMessengerEXT>();
      func(instance->getInstance(), debugger, nullptr);
    }
    catch (const std::format_error& e) {
      std::cerr << "Failed to destroy debugger:\n\t" << e.what() << "\n";
      std::terminate();
    }
    catch (...) {
      std::cerr << "Unknown error destroying debugger\n";
      std::terminate();
    }
  }

private:
  std::shared_ptr<InitVulkan> instance = InitVulkan::getInit();
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEBUGGER_DEBUGGER_DELETER */
