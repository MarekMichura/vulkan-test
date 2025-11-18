#ifndef LIB_VULKAN_INIT_VULKAN_INIT
#define LIB_VULKAN_INIT_VULKAN_INIT

#include <memory>

#include <vulkan/vulkan_core.h>

#include "init_info.hpp"

namespace vulkan {
class InitVulkan {
public:
  InitVulkan(const InitVulkan&) = delete;
  InitVulkan(InitVulkan&&) = delete;
  InitVulkan& operator=(const InitVulkan&) = delete;
  InitVulkan& operator=(InitVulkan&&) = delete;

  ~InitVulkan() = default;
  [[nodiscard]] VkInstance getInstance() const;

  static std::shared_ptr<InitVulkan> createInit(const VulkanInfo& info = {});
  static std::shared_ptr<InitVulkan> getInit();

private:
  std::unique_ptr<VkInstance_T, void (*)(VkInstance)> _instance;

  explicit InitVulkan(const VulkanInfo& def);

  static std::weak_ptr<InitVulkan>& ptr();
};
}  // namespace vulkan

#endif /* LIB_VULKAN_INIT_VULKAN_INIT */
