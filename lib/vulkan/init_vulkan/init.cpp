#include "init.hpp"

#include <cstdint>
#include <format>
#include <memory>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "GLFW/glfw3.h"
#include <vulkan/vulkan_core.h>

#include "available/available.hpp"
#include "debug.hpp"
#include "format/string.hpp"
#include "format/table.hpp"
#include "init_info.hpp"

namespace vulkan {
static std::vector<std::string_view> getGlfwExtensions()
{
  uint32_t count = 0;
  auto* glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
  if (count == 0 || glfwExtensions == nullptr) {
    throw std::runtime_error("GLFW: Failed to get extensions");
  }

  return std::span(glfwExtensions, count) | std::ranges::to<std::vector<std::string_view>>();
}

static std::vector<VkExtensionProperties> getAvailableExtensions()
{
  uint32_t count = 0;
  if (const VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available extensions: {}", utils::result(result)));
  }
  std::vector<VkExtensionProperties> extensions(count);
  if (const VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data()); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available extensions: {}", utils::result(result)));
  }

  if constexpr (Debug) {
    // clang-format off
    utils::table<VkExtensionProperties>("AVAILABLE EXTENSIONS", extensions, std::vector<utils::TableColumn<VkExtensionProperties>>{{
      {.title = "layerName", .align = utils::Align::left, 
        .toString = [](const VkExtensionProperties& ele) { return std::string(static_cast<const char *>(ele.extensionName)); }},
      {.title = "spec ver", .toString = [](const VkExtensionProperties& ele) { return utils::version(ele.specVersion); }}}});
    // clang-format on
  }
  return extensions;
}

static std::vector<std::string_view> prepareExtensions(const VulkanInfo& def)
{
  static const auto compare = [](std::string_view name, const VkExtensionProperties& ext) {
    return std::string_view(static_cast<const char*>(ext.extensionName)) == name;
  };

  const std::vector<VkExtensionProperties> availableExtensions = getAvailableExtensions();
  const std::vector<std::string_view> glfwExtensions = getGlfwExtensions();

  if (!utils::checkPresent(glfwExtensions, availableExtensions, compare)) {
    throw std::runtime_error("GLFW requires an extension that is not available");
  }

  if (!utils::checkPresent(def.extensions, availableExtensions, compare)) {
    throw std::runtime_error("One or more requested extensions are not available");
  }

  std::vector<std::string_view> result;
  result.reserve(glfwExtensions.size() + def.extensions.size());

  result.insert(result.end(), glfwExtensions.begin(), glfwExtensions.end());
  result.insert(result.end(), def.extensions.begin(), def.extensions.end());

  return result;
}

static std::vector<VkLayerProperties> getAvailableLayers()
{
  uint32_t count = 0;
  if (const VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available layers: {}", utils::result(result)));
  }

  std::vector<VkLayerProperties> layers(count);
  if (const VkResult result = vkEnumerateInstanceLayerProperties(&count, layers.data()); result != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get vulkan available layers: {}", utils::result(result)));
  }

  if constexpr (Debug) {
    // clang-format off
    utils::table<VkLayerProperties>("AVAILABLE LAYERS", layers, std::vector<utils::TableColumn<VkLayerProperties>>{{
      {.title = "layerName",   .align = utils::Align::left, .toString = [](const VkLayerProperties& ele) { 
        return std::string(static_cast<const char*>(ele.layerName)); }},
      {.title = "description", .align = utils::Align::left, .toString = [](const VkLayerProperties& ele) { return std::string(static_cast<const char*>(ele.description)); }},
      {.title = "implementation ver", .toString = [](const VkLayerProperties& ele) { 
        return utils::version(ele.implementationVersion); }},
      {.title = "spec ver", .toString = [](const VkLayerProperties& ele) { return utils::version(ele.specVersion); }}}});
    // clang-format on
  }
  return layers;
}

static std::vector<std::string_view> prepareLayers(const VulkanInfo& def)
{
  static const auto compare = [](std::string_view name, const VkLayerProperties& ext) {  //
    return std::string_view(static_cast<const char*>(ext.layerName)) == name;
  };
  const std::vector<VkLayerProperties> availableLayers = getAvailableLayers();

  if (!utils::checkPresent(def.layers, availableLayers, compare)) {
    throw std::runtime_error("Layer is not available");
  }

  return def.layers;
}

static VkInstance createInstance(const VulkanInfo& def)
{
  auto extensions = prepareExtensions(def);
  auto layers = prepareLayers(def);

  auto extensionsTable = extensions |                                                              //
                         std::views::transform([](std::string_view str) { return str.data(); }) |  //
                         std::ranges::to<std::vector<const char*>>();
  auto layersTable = layers |                                                                  //
                     std::views::transform([](std::string_view str) { return str.data(); }) |  //
                     std::ranges::to<std::vector<const char*>>();

  const VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = def.appName.data(),  // NOLINT(bugprone-suspicious-stringview-data-usage)
      .applicationVersion = def.appVersion,
      .pEngineName = def.engineName.data(),  // NOLINT(bugprone-suspicious-stringview-data-usage)
      .engineVersion = def.engineVersion,
      .apiVersion = VK_API_VERSION_1_3,
  };

  const VkInstanceCreateInfo instanceInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(layers.size()),
      .ppEnabledLayerNames = layersTable.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensionsTable.data(),
  };

  VkInstance instance = nullptr;
  if (const VkResult status = vkCreateInstance(&instanceInfo, nullptr, &instance); status != VK_SUCCESS) {
    throw std::runtime_error(std::format("can not create vulkan instance. status: {}", utils::result(status)));
  }

  return instance;
}

static void destroyInstance(VkInstance instance)
{
  vkDestroyInstance(instance, nullptr);
}

InitVulkan::InitVulkan(const VulkanInfo& def) : _instance(createInstance(def), destroyInstance) {}

VkInstance InitVulkan::getInstance() const
{
  return _instance.get();
}

std::weak_ptr<InitVulkan>& InitVulkan::ptr()
{
  static std::weak_ptr<InitVulkan> ptr;
  return ptr;
}

std::shared_ptr<InitVulkan> InitVulkan::createInit(const VulkanInfo& info)
{
  if (auto existing = ptr().lock()) {
    throw std::runtime_error("InitVulkan is initialized");
  }

  auto newInstance = std::shared_ptr<InitVulkan>(new InitVulkan(info));
  ptr() = newInstance;
  return newInstance;
}

std::shared_ptr<InitVulkan> InitVulkan::getInit()
{
  if (auto existing = ptr().lock()) {
    return existing;
  }

  throw std::runtime_error("InitVulkan not initialized");
}

}  // namespace vulkan
