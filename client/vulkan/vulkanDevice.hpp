#include <vulkan/vulkan_core.h>
#include <string>
#include <utility>
#include <vector>

namespace vul {
class VulkanDevice {
public:
  VulkanDevice(const VulkanDevice&) = delete;
  VulkanDevice(VulkanDevice&&) = delete;
  VulkanDevice& operator=(const VulkanDevice&) = delete;
  VulkanDevice& operator=(VulkanDevice&&) = delete;

  VulkanDevice(const VkInstance instance);
  ~VulkanDevice() = default;

private:
  const VkInstance _instance;
  const VkPhysicalDevice _physicalDevice;

  VkPhysicalDevice selectPhysicalDevices();
  std::vector<VkPhysicalDevice> getPhysicalDevices();

  static std::vector<std::pair<VkPhysicalDevice, VkPhysicalDeviceProperties>> getDeviceData(
      const std::vector<VkPhysicalDevice>& devices);
  static std::string getDeviceTypeName(VkPhysicalDeviceType type);
};
}  // namespace vul