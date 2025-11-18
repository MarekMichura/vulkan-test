#ifndef LIB_VULKAN_API_API
#define LIB_VULKAN_API_API

#include <memory>
#include <optional>
#include <vector>

#include "api_info.hpp"
#include "debugger/debugger.hpp"
#include "init_glfw/init.hpp"
#include "init_vulkan/init.hpp"
#include "window/window.hpp"

namespace vulkan {
class VulkanApi {
public:
  VulkanApi(const VulkanApi&) = delete;
  VulkanApi(VulkanApi&&) = delete;
  VulkanApi& operator=(const VulkanApi&) = delete;
  VulkanApi& operator=(VulkanApi&&) = delete;
  ~VulkanApi() = default;

  static std::shared_ptr<VulkanApi> createApi(const VulkanApiInfo& info = {});
  static std::shared_ptr<VulkanApi> getApi();

private:
  const std::shared_ptr<InitGlfw> _glfw;
  const std::shared_ptr<InitVulkan> _vulkan;
  std::optional<VulkanDebugger> _debugger;
  std::vector<Window> _windows;

  explicit VulkanApi(const VulkanApiInfo& info = {});

  static std::weak_ptr<VulkanApi>& ptr();
};
}  // namespace vulkan

#endif /* LIB_VULKAN_API_API */
