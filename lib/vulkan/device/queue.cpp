#include "queue.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <ranges>
#include <utility>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "device_data.hpp"

namespace vulkan {
static std::vector<std::vector<VkQueue>> createQueues(VkDevice device, const std::vector<DeviceDataQueue>& queues)
{
  std::vector<std::vector<VkQueue>> result;
  result.reserve(queues.size());
  for (const auto& queue : queues) {
    result.emplace_back(queue.properties.queueCount);
    auto& elements = result.back();
    uint32_t seq = 0;

    for (auto& ele : elements) {
      vkGetDeviceQueue(device, queue.queueIndex, seq++, &ele);
    }
  }
  return result;
}

struct GenerateQueueData_T {
  std::vector<uint32_t> graphics;
  std::vector<uint32_t> compute;
  std::vector<uint32_t> transfer;
  std::vector<uint32_t> sparse;
};

static GenerateQueueData_T generateQueueData(const std::vector<DeviceDataQueue>& queues)
{
  static auto getGraphics = [](const DeviceDataQueue& queue) {
    return (queue.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && queue.supportKHR;
  };
  static auto getCompute = [](const DeviceDataQueue& queue) { return (queue.properties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0; };
  static auto getTransfer = [](const DeviceDataQueue& queue) { return (queue.properties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0; };
  static auto getSparse = [](const DeviceDataQueue& queue) { return (queue.properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0; };

  static auto priority = [](const DeviceDataQueue& queue) {
    if ((queue.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && VK_QUEUE_GRAPHICS_BIT) {
      return 4;
    }
    if ((queue.properties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) {
      return 3;
    }
    if ((queue.properties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
      return 2;
    }
    if ((queue.properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) {
      return 1;
    }
    return 0;
  };
  static auto sort = [](const DeviceDataQueue& queueA, const DeviceDataQueue& queueB) { return priority(queueA) < priority(queueB); };
  static auto transform = [](const DeviceDataQueue& queue) { return queue.queueIndex; };

  auto dataGraphics = queues | std::views::filter(getGraphics) | std::ranges::to<std::vector>();
  auto dataCompute = queues | std::views::filter(getCompute) | std::ranges::to<std::vector>();
  auto dataTransfer = queues | std::views::filter(getTransfer) | std::ranges::to<std::vector>();
  auto dataSparse = queues | std::views::filter(getSparse) | std::ranges::to<std::vector>();

  std::ranges::sort(dataCompute, sort);
  std::ranges::sort(dataTransfer, sort);
  std::ranges::sort(dataSparse, sort);

  return {.graphics = dataGraphics | std::views::transform(transform) | std::ranges::to<std::vector>(),
          .compute = dataCompute | std::views::transform(transform) | std::ranges::to<std::vector>(),
          .transfer = dataTransfer | std::views::transform(transform) | std::ranges::to<std::vector>(),
          .sparse = dataSparse | std::views::transform(transform) | std::ranges::to<std::vector>()};
}

Queue::Queue(VkDevice device, const std::vector<DeviceDataQueue>& queues)  //
    : _queues(createQueues(device, queues))
{
  auto queueData = generateQueueData(queues);

  _graphics = std::move(queueData.graphics);
  _compute = std::move(queueData.compute);
  _transfer = std::move(queueData.transfer);
  _sparse = std::move(queueData.sparse);
}
}  // namespace vulkan