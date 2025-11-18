#ifndef LIB_VULKAN_DEVICE_QUEUE
#define LIB_VULKAN_DEVICE_QUEUE

#include <vector>

#include "device/device_data.hpp"
#include "vulkan/vulkan_core.h"

namespace vulkan {
class Queue {
public:
  Queue(const Queue&) = delete;
  Queue(Queue&&) = default;
  Queue& operator=(const Queue&) = delete;
  Queue& operator=(Queue&&) = default;

  explicit Queue(VkDevice device, std::vector<DeviceDataQueue> queues);
  ~Queue() = default;

private:
  std::vector<std::vector<VkQueue>> _queues;

  std::vector<uint32_t> _graphics;
  std::vector<uint32_t> _compute;
  std::vector<uint32_t> _transfer;
  std::vector<uint32_t> _sparse;
};
}  // namespace vulkan

#endif /* LIB_VULKAN_DEVICE_QUEUE */
