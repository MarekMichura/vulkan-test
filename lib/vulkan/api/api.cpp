#include "api.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

#include "api_info.hpp"
#include "debugger/debugger.hpp"
#include "init_glfw/init.hpp"
#include "init_vulkan/init.hpp"
#include "init_vulkan/init_info.hpp"
#include "window/window.hpp"

namespace vulkan {
static std::vector<Window> createWindows(const VulkanApiInfo& info)
{
  auto instance = InitVulkan::getInit();
  std::vector<Window> windows = {};
  windows.reserve(info.windowsInfo.size() + 1);

  windows.emplace_back(info.mainWindowInfo);
  for (const auto& windowInfo : info.windowsInfo) {
    windows.emplace_back(windowInfo);
  }

  return windows;
}

static VulkanDebugger createDebugger(const VulkanInfo& info)
{
  return VulkanDebugger(info);
}

VulkanApi::VulkanApi(const VulkanApiInfo& info)
    : _glfw(InitGlfw::createInit()),
      _vulkan(InitVulkan::createInit(info.vulkanInitInfo)),
#ifdef DEBUG
      _debugger(createDebugger(info.vulkanInitInfo)),
#endif
      _windows(createWindows(info))
{
}

std::weak_ptr<VulkanApi>& VulkanApi::ptr()
{
  static std::weak_ptr<VulkanApi> ptr;
  return ptr;
}

std::shared_ptr<VulkanApi> VulkanApi::createApi(const VulkanApiInfo& info)
{
  if (auto existing = ptr().lock()) {
    throw std::runtime_error("VulkanApi is initialized");
  }

  auto newInstance = std::shared_ptr<VulkanApi>(new VulkanApi(info));
  ptr() = newInstance;
  return newInstance;
}

std::shared_ptr<VulkanApi> VulkanApi::getApi()
{
  if (auto existing = ptr().lock()) {
    return existing;
  }

  throw std::runtime_error("VulkanInit not initialized");
}

}  // namespace vulkan