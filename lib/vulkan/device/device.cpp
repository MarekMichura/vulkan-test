#include "device.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <optional>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "debug.hpp"
#include "device_data.hpp"
#include "format/string.hpp"
#include "format/table.hpp"
#include "init_vulkan/init.hpp"

namespace vulkan {
struct PrintData {  // NOLINT(altera-struct-pack-align)
  std::string_view property;
  std::function<std::string(const DeviceData&)> fun;
};

struct Print {  // NOLINT(altera-struct-pack-align)
  std::string_view property;
  std::vector<std::string> data;
};

static void showProperties(const std::vector<DeviceData>& devices)
{
  utils::table<DeviceData>(
      "Device Properties", devices,
      std::vector<utils::TableColumn<DeviceData>>{{
          {.title = "Name",
           .toString = [](const DeviceData& data) { return std::string(static_cast<const char*>(data.properties.deviceName)); }},
          {.title = "API version", .toString = [](const DeviceData& data) { return utils::version(data.properties.apiVersion); }},
          {.title = "Driver version", .toString = [](const DeviceData& data) { return utils::version(data.properties.deviceID); }},
          {.title = "vendor", .toString = [](const DeviceData& data) { return utils::vendor(data.properties.vendorID); }},
          {.title = "Type", .toString = [](const DeviceData& data) { return utils::deviceType(data.properties.deviceType); }},
          {.title = "Pipeline cache UUID",
           .toString = [](const DeviceData& data) { return utils::table(std::span(data.properties.pipelineCacheUUID)); }},
      }});
}

static void showLimits(const std::vector<DeviceData>& devices)
{
  // clang-format off
  static const std::vector<PrintData> printData{
      {"maxImageDimension1D", [](const DeviceData& data) { return utils::number(data.properties.limits.maxImageDimension1D); }},
      {"maxImageDimension2D", [](const DeviceData& data) { return utils::number(data.properties.limits.maxImageDimension2D); }},
      {"maxImageDimension3D", [](const DeviceData& data) { return utils::number(data.properties.limits.maxImageDimension3D); }},
      {"maxImageDimensionCube", [](const DeviceData& data) { return utils::number(data.properties.limits.maxImageDimensionCube); }},
      {"maxImageArrayLayers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxImageArrayLayers); }},
      {"maxTexelBufferElements", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTexelBufferElements); }},
      {"maxUniformBufferRange", [](const DeviceData& data) { return utils::number(data.properties.limits.maxUniformBufferRange); }},
      {"maxStorageBufferRange", [](const DeviceData& data) { return utils::number(data.properties.limits.maxStorageBufferRange); }},
      {"maxPushConstantsSize", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPushConstantsSize); }},
      {"maxMemoryAllocationCount", [](const DeviceData& data) { return utils::number(data.properties.limits.maxMemoryAllocationCount); }},
      {"maxSamplerAllocationCount", [](const DeviceData& data) { return utils::number(data.properties.limits.maxSamplerAllocationCount); }},
      {"bufferImageGranularity", [](const DeviceData& data) { return utils::number(data.properties.limits.bufferImageGranularity); }},
      {"sparseAddressSpaceSize", [](const DeviceData& data) { return utils::number(data.properties.limits.sparseAddressSpaceSize); }},
      {"maxBoundDescriptorSets", [](const DeviceData& data) { return utils::number(data.properties.limits.maxBoundDescriptorSets); }},
      {"maxPerStageDescriptorSamplers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPerStageDescriptorSamplers); }},
      {"maxPerStageDescriptorUniformBuffers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPerStageDescriptorUniformBuffers); }},
      {"maxPerStageDescriptorStorageBuffers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPerStageDescriptorStorageBuffers); }},
      {"maxPerStageDescriptorSampledImages", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPerStageDescriptorSampledImages); }},
      {"maxPerStageDescriptorStorageImages", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPerStageDescriptorStorageImages); }},
      {"maxPerStageDescriptorInputAttachments", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPerStageDescriptorInputAttachments); }},
      {"maxPerStageResources", [](const DeviceData& data) { return utils::number(data.properties.limits.maxPerStageResources); }},
      {"maxDescriptorSetSamplers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetSamplers); }},
      {"maxDescriptorSetUniformBuffers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetUniformBuffers); }},
      {"maxDescriptorSetUniformBuffersDynamic", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetUniformBuffersDynamic); }},
      {"maxDescriptorSetStorageBuffers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetStorageBuffers); }},
      {"maxDescriptorSetStorageBuffersDynamic", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetStorageBuffersDynamic); }},
      {"maxDescriptorSetSampledImages", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetSampledImages); }},
      {"maxDescriptorSetStorageImages", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetStorageImages); }},
      {"maxDescriptorSetInputAttachments", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDescriptorSetInputAttachments); }},
      {"maxVertexInputAttributes", [](const DeviceData& data) { return utils::number(data.properties.limits.maxVertexInputAttributes); }},
      {"maxVertexInputBindings", [](const DeviceData& data) { return utils::number(data.properties.limits.maxVertexInputBindings); }},
      {"maxVertexInputAttributeOffset", [](const DeviceData& data) { return utils::number(data.properties.limits.maxVertexInputAttributeOffset); }},
      {"maxVertexInputBindingStride", [](const DeviceData& data) { return utils::number(data.properties.limits.maxVertexInputBindingStride); }},
      {"maxVertexOutputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxVertexOutputComponents); }},
      {"maxTessellationGenerationLevel", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationGenerationLevel); }},
      {"maxTessellationPatchSize", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationPatchSize); }},
      {"maxTessellationControlPerVertexInputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationControlPerVertexInputComponents); }},
      {"maxTessellationControlPerVertexOutputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationControlPerVertexOutputComponents); }},
      {"maxTessellationControlPerPatchOutputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationControlPerPatchOutputComponents); }},
      {"maxTessellationControlTotalOutputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationControlTotalOutputComponents); }},
      {"maxTessellationEvaluationInputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationEvaluationInputComponents); }},
      {"maxTessellationEvaluationOutputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTessellationEvaluationOutputComponents); }},
      {"maxGeometryShaderInvocations", [](const DeviceData& data) { return utils::number(data.properties.limits.maxGeometryShaderInvocations); }},
      {"maxGeometryInputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxGeometryInputComponents); }},
      {"maxGeometryOutputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxGeometryOutputComponents); }},
      {"maxGeometryOutputVertices", [](const DeviceData& data) { return utils::number(data.properties.limits.maxGeometryOutputVertices); }},
      {"maxGeometryTotalOutputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxGeometryTotalOutputComponents); }},
      {"maxFragmentInputComponents", [](const DeviceData& data) { return utils::number(data.properties.limits.maxFragmentInputComponents); }},
      {"maxFragmentOutputAttachments", [](const DeviceData& data) { return utils::number(data.properties.limits.maxFragmentOutputAttachments); }},
      {"maxFragmentDualSrcAttachments", [](const DeviceData& data) { return utils::number(data.properties.limits.maxFragmentDualSrcAttachments); }},
      {"maxFragmentCombinedOutputResources", [](const DeviceData& data) { return utils::number(data.properties.limits.maxFragmentCombinedOutputResources); }},
      {"maxComputeSharedMemorySize", [](const DeviceData& data) { return utils::number(data.properties.limits.maxComputeSharedMemorySize); }},
      {"maxComputeWorkGroupCount", [](const DeviceData& data) { return utils::table(std::span(data.properties.limits.maxComputeWorkGroupCount)); }},
      {"maxComputeWorkGroupInvocations", [](const DeviceData& data) { return utils::number(data.properties.limits.maxComputeWorkGroupInvocations); }},
      {"maxComputeWorkGroupSize", [](const DeviceData& data) { return utils::table(std::span(data.properties.limits.maxComputeWorkGroupSize)); }},
      {"subPixelPrecisionBits", [](const DeviceData& data) { return utils::number(data.properties.limits.subPixelPrecisionBits); }},
      {"subTexelPrecisionBits", [](const DeviceData& data) { return utils::number(data.properties.limits.subTexelPrecisionBits); }},
      {"mipmapPrecisionBits", [](const DeviceData& data) { return utils::number(data.properties.limits.mipmapPrecisionBits); }},
      {"maxDrawIndexedIndexValue", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDrawIndexedIndexValue); }},
      {"maxDrawIndirectCount", [](const DeviceData& data) { return utils::number(data.properties.limits.maxDrawIndirectCount); }},
      {"maxSamplerLodBias", [](const DeviceData& data) { return utils::number(data.properties.limits.maxSamplerLodBias); }},
      {"maxSamplerAnisotropy", [](const DeviceData& data) { return utils::number(data.properties.limits.maxSamplerAnisotropy); }},
      {"maxViewports", [](const DeviceData& data) { return utils::number(data.properties.limits.maxViewports); }},
      {"maxViewportDimensions", [](const DeviceData& data) { return utils::table(std::span(data.properties.limits.maxViewportDimensions)); }},
      {"viewportBoundsRange", [](const DeviceData& data) { return utils::table(std::span(data.properties.limits.viewportBoundsRange)); }},
      {"viewportSubPixelBits", [](const DeviceData& data) { return utils::number(data.properties.limits.viewportSubPixelBits); }},
      {"minMemoryMapAlignment", [](const DeviceData& data) { return utils::number(data.properties.limits.minMemoryMapAlignment); }},
      {"minTexelBufferOffsetAlignment", [](const DeviceData& data) { return utils::number(data.properties.limits.minTexelBufferOffsetAlignment); }},
      {"minUniformBufferOffsetAlignment", [](const DeviceData& data) { return utils::number(data.properties.limits.minUniformBufferOffsetAlignment); }},
      {"minStorageBufferOffsetAlignment", [](const DeviceData& data) { return utils::number(data.properties.limits.minStorageBufferOffsetAlignment); }},
      {"minTexelOffset", [](const DeviceData& data) { return utils::number(data.properties.limits.minTexelOffset); }},
      {"maxTexelOffset", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTexelOffset); }},
      {"minTexelGatherOffset", [](const DeviceData& data) { return utils::number(data.properties.limits.minTexelGatherOffset); }},
      {"maxTexelGatherOffset", [](const DeviceData& data) { return utils::number(data.properties.limits.maxTexelGatherOffset); }},
      {"minInterpolationOffset", [](const DeviceData& data) { return utils::number(data.properties.limits.minInterpolationOffset); }},
      {"maxInterpolationOffset", [](const DeviceData& data) { return utils::number(data.properties.limits.maxInterpolationOffset); }},
      {"subPixelInterpolationOffsetBits", [](const DeviceData& data) { return utils::number(data.properties.limits.subPixelInterpolationOffsetBits); }},
      {"maxFramebufferWidth", [](const DeviceData& data) { return utils::number(data.properties.limits.maxFramebufferWidth); }},
      {"maxFramebufferHeight", [](const DeviceData& data) { return utils::number(data.properties.limits.maxFramebufferHeight); }},
      {"maxFramebufferLayers", [](const DeviceData& data) { return utils::number(data.properties.limits.maxFramebufferLayers); }},
      {"framebufferColorSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.framebufferColorSampleCounts); }},
      {"framebufferDepthSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.framebufferDepthSampleCounts); }},
      {"framebufferStencilSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.framebufferStencilSampleCounts); }},
      {"framebufferNoAttachmentsSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.framebufferNoAttachmentsSampleCounts); }},
      {"maxColorAttachments", [](const DeviceData& data) { return utils::number(data.properties.limits.maxColorAttachments); }},
      {"sampledImageColorSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.sampledImageColorSampleCounts); }},
      {"sampledImageIntegerSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.sampledImageIntegerSampleCounts); }},
      {"sampledImageDepthSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.sampledImageDepthSampleCounts); }},
      {"sampledImageStencilSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.sampledImageStencilSampleCounts); }},
      {"storageImageSampleCounts", [](const DeviceData& data) { return utils::number(data.properties.limits.storageImageSampleCounts); }},
      {"maxSampleMaskWords", [](const DeviceData& data) { return utils::number(data.properties.limits.maxSampleMaskWords); }},
      {"timestampComputeAndGraphics", [](const DeviceData& data) { return utils::string(data.properties.limits.timestampComputeAndGraphics); }},
      {"timestampPeriod", [](const DeviceData& data) { return utils::number(data.properties.limits.timestampPeriod); }},
      {"maxClipDistances", [](const DeviceData& data) { return utils::number(data.properties.limits.maxClipDistances); }},
      {"maxCullDistances", [](const DeviceData& data) { return utils::number(data.properties.limits.maxCullDistances); }},
      {"maxCombinedClipAndCullDistances", [](const DeviceData& data) { return utils::number(data.properties.limits.maxCombinedClipAndCullDistances); }},
      {"discreteQueuePriorities", [](const DeviceData& data) { return utils::number(data.properties.limits.discreteQueuePriorities); }},
      {"pointSizeRange", [](const DeviceData& data) { return utils::table(std::span(data.properties.limits.pointSizeRange)); }},
      {"lineWidthRange", [](const DeviceData& data) { return utils::table(std::span(data.properties.limits.lineWidthRange)); }},
      {"pointSizeGranularity", [](const DeviceData& data) { return utils::number(data.properties.limits.pointSizeGranularity); }},
      {"lineWidthGranularity", [](const DeviceData& data) { return utils::number(data.properties.limits.lineWidthGranularity); }},
      {"strictLines", [](const DeviceData& data) { return utils::string(data.properties.limits.strictLines); }},
      {"standardSampleLocations", [](const DeviceData& data) { return utils::string(data.properties.limits.standardSampleLocations); }},
      {"optimalBufferCopyOffsetAlignment", [](const DeviceData& data) { return utils::number(data.properties.limits.optimalBufferCopyOffsetAlignment); }},
      {"optimalBufferCopyRowPitchAlignment", [](const DeviceData& data) { return utils::number(data.properties.limits.optimalBufferCopyRowPitchAlignment); }},
      {"nonCoherentAtomSize", [](const DeviceData& data) { return utils::number(data.properties.limits.nonCoherentAtomSize); }},
  };
  // clang-format on

  std::vector<Print> prints;
  prints.reserve(printData.size());

  for (const auto& data : printData) {
    std::vector<std::string> values;
    values.reserve(devices.size());

    for (const auto& device : devices) {
      values.emplace_back(data.fun(device));
    }
    prints.emplace_back(data.property, std::move(values));
  }

  std::vector<utils::TableColumn<Print>> columns;
  columns.reserve(prints.size() + 1);
  columns.emplace_back("Var name", std::nullopt, utils::Align::left, [](const Print& print) { return std::string(print.property); });
  for (size_t i = 0; i < devices.size(); i++) {
    auto deviceName = std::string_view(static_cast<const char*>(devices.at(i).properties.deviceName));
    columns.emplace_back(deviceName, std::nullopt, utils::Align::left, [i](const Print& print) { return print.data.at(i); });
  }
  utils::table<Print>("Device Limits", prints, columns);
}

static void showSparseProperties(const std::vector<DeviceData>& devices)
{
  // clang-format off
  utils::table<DeviceData>(
      "Device sparse properties", devices,
      std::vector<utils::TableColumn<DeviceData>>{{
          {.title = "Name", .toString = [](const DeviceData& data) { return std::string(static_cast<const char*>(data.properties.deviceName)); }},
          {.title = "residencyStandard2DBlockShape",
           .toString = [](const DeviceData& data) { return utils::string(data.properties.sparseProperties.residencyStandard2DBlockShape); }},
          {.title = "residencyStandard2DMultisampleBlockShape",
           .toString = [](const DeviceData& data) { return utils::string(data.properties.sparseProperties.residencyStandard2DMultisampleBlockShape); }},
          {.title = "residencyStandard3DBlockShape",
           .toString = [](const DeviceData& data) { return utils::string(data.properties.sparseProperties.residencyStandard3DBlockShape); }},
          {.title = "residencyAlignedMipSize",
           .toString = [](const DeviceData& data) { return utils::string(data.properties.sparseProperties.residencyAlignedMipSize); }},
          {.title = "residencyNonResidentStrict",
           .toString = [](const DeviceData& data) { return utils::string(data.properties.sparseProperties.residencyNonResidentStrict); }},
      }});
  // clang-format on
}

static void showFeatures(const std::vector<DeviceData>& devices)
{
  // clang-format off
  static const std::vector<PrintData> printData{
      {"robustBufferAccess", [](const DeviceData& data) { return utils::string(data.features.robustBufferAccess); }},
      {"fullDrawIndexUint32", [](const DeviceData& data) { return utils::string(data.features.fullDrawIndexUint32); }},
      {"imageCubeArray", [](const DeviceData& data) { return utils::string(data.features.imageCubeArray); }},
      {"independentBlend", [](const DeviceData& data) { return utils::string(data.features.independentBlend); }},
      {"geometryShader", [](const DeviceData& data) { return utils::string(data.features.geometryShader); }},
      {"tessellationShader", [](const DeviceData& data) { return utils::string(data.features.tessellationShader); }},
      {"sampleRateShading", [](const DeviceData& data) { return utils::string(data.features.sampleRateShading); }},
      {"dualSrcBlend", [](const DeviceData& data) { return utils::string(data.features.dualSrcBlend); }},
      {"logicOp", [](const DeviceData& data) { return utils::string(data.features.logicOp); }},
      {"multiDrawIndirect", [](const DeviceData& data) { return utils::string(data.features.multiDrawIndirect); }},
      {"drawIndirectFirstInstance", [](const DeviceData& data) { return utils::string(data.features.drawIndirectFirstInstance); }},
      {"depthClamp", [](const DeviceData& data) { return utils::string(data.features.depthClamp); }},
      {"depthBiasClamp", [](const DeviceData& data) { return utils::string(data.features.depthBiasClamp); }},
      {"fillModeNonSolid", [](const DeviceData& data) { return utils::string(data.features.fillModeNonSolid); }},
      {"depthBounds", [](const DeviceData& data) { return utils::string(data.features.depthBounds); }},
      {"wideLines", [](const DeviceData& data) { return utils::string(data.features.wideLines); }},
      {"largePoints", [](const DeviceData& data) { return utils::string(data.features.largePoints); }},
      {"alphaToOne", [](const DeviceData& data) { return utils::string(data.features.alphaToOne); }},
      {"multiViewport", [](const DeviceData& data) { return utils::string(data.features.multiViewport); }},
      {"samplerAnisotropy", [](const DeviceData& data) { return utils::string(data.features.samplerAnisotropy); }},
      {"textureCompressionETC2", [](const DeviceData& data) { return utils::string(data.features.textureCompressionETC2); }},
      {"textureCompressionASTC_LDR", [](const DeviceData& data) { return utils::string(data.features.textureCompressionASTC_LDR); }},
      {"textureCompressionBC", [](const DeviceData& data) { return utils::string(data.features.textureCompressionBC); }},
      {"occlusionQueryPrecise", [](const DeviceData& data) { return utils::string(data.features.occlusionQueryPrecise); }},
      {"pipelineStatisticsQuery", [](const DeviceData& data) { return utils::string(data.features.pipelineStatisticsQuery); }},
      {"vertexPipelineStoresAndAtomics", [](const DeviceData& data) { return utils::string(data.features.vertexPipelineStoresAndAtomics); }},
      {"fragmentStoresAndAtomics", [](const DeviceData& data) { return utils::string(data.features.fragmentStoresAndAtomics); }},
      {"shaderTessellationAndGeometryPointSize", [](const DeviceData& data) { return utils::string(data.features.shaderTessellationAndGeometryPointSize); }},
      {"shaderImageGatherExtended", [](const DeviceData& data) { return utils::string(data.features.shaderImageGatherExtended); }},
      {"shaderStorageImageExtendedFormats", [](const DeviceData& data) { return utils::string(data.features.shaderStorageImageExtendedFormats); }},
      {"shaderStorageImageMultisample", [](const DeviceData& data) { return utils::string(data.features.shaderStorageImageMultisample); }},
      {"shaderStorageImageReadWithoutFormat", [](const DeviceData& data) { return utils::string(data.features.shaderStorageImageReadWithoutFormat); }},
      {"shaderStorageImageWriteWithoutFormat", [](const DeviceData& data) { return utils::string(data.features.shaderStorageImageWriteWithoutFormat); }},
      {"shaderUniformBufferArrayDynamicIndexing", [](const DeviceData& data) { return utils::string(data.features.shaderUniformBufferArrayDynamicIndexing); }},
      {"shaderSampledImageArrayDynamicIndexing", [](const DeviceData& data) { return utils::string(data.features.shaderSampledImageArrayDynamicIndexing); }},
      {"shaderStorageBufferArrayDynamicIndexing", [](const DeviceData& data) { return utils::string(data.features.shaderStorageBufferArrayDynamicIndexing); }},
      {"shaderStorageImageArrayDynamicIndexing", [](const DeviceData& data) { return utils::string(data.features.shaderStorageImageArrayDynamicIndexing); }},
      {"shaderClipDistance", [](const DeviceData& data) { return utils::string(data.features.shaderClipDistance); }},
      {"shaderCullDistance", [](const DeviceData& data) { return utils::string(data.features.shaderCullDistance); }},
      {"shaderFloat64", [](const DeviceData& data) { return utils::string(data.features.shaderFloat64); }},
      {"shaderInt64", [](const DeviceData& data) { return utils::string(data.features.shaderInt64); }},
      {"shaderInt16", [](const DeviceData& data) { return utils::string(data.features.shaderInt16); }},
      {"shaderResourceResidency", [](const DeviceData& data) { return utils::string(data.features.shaderResourceResidency); }},
      {"shaderResourceMinLod", [](const DeviceData& data) { return utils::string(data.features.shaderResourceMinLod); }},
      {"sparseBinding", [](const DeviceData& data) { return utils::string(data.features.sparseBinding); }},
      {"sparseResidencyBuffer", [](const DeviceData& data) { return utils::string(data.features.sparseResidencyBuffer); }},
      {"sparseResidencyImage2D", [](const DeviceData& data) { return utils::string(data.features.sparseResidencyImage2D); }},
      {"sparseResidencyImage3D", [](const DeviceData& data) { return utils::string(data.features.sparseResidencyImage3D); }},
      {"sparseResidency2Samples", [](const DeviceData& data) { return utils::string(data.features.sparseResidency2Samples); }},
      {"sparseResidency4Samples", [](const DeviceData& data) { return utils::string(data.features.sparseResidency4Samples); }},
      {"sparseResidency8Samples", [](const DeviceData& data) { return utils::string(data.features.sparseResidency8Samples); }},
      {"sparseResidency16Samples", [](const DeviceData& data) { return utils::string(data.features.sparseResidency16Samples); }},
      {"sparseResidencyAliased", [](const DeviceData& data) { return utils::string(data.features.sparseResidencyAliased); }},
      {"variableMultisampleRate", [](const DeviceData& data) { return utils::string(data.features.variableMultisampleRate); }},
      {"inheritedQueries", [](const DeviceData& data) { return utils::string(data.features.inheritedQueries); }},

  };
  // clang-format on

  std::vector<Print> prints;
  prints.reserve(printData.size());

  for (const auto& data : printData) {
    std::vector<std::string> values;
    values.reserve(devices.size());

    for (const auto& device : devices) {
      values.emplace_back(data.fun(device));
    }
    prints.emplace_back(data.property, std::move(values));
  }

  std::vector<utils::TableColumn<Print>> columns;
  columns.reserve(prints.size() + 1);
  columns.emplace_back("Var name", std::nullopt, utils::Align::left, [](const Print& print) { return std::string(print.property); });
  for (size_t i = 0; i < devices.size(); i++) {
    auto deviceName = std::string_view(static_cast<const char*>(devices.at(i).properties.deviceName));
    columns.emplace_back(deviceName, std::nullopt, utils::Align::left, [i](const Print& print) { return print.data.at(i); });
  }
  utils::table<Print>("Device features", prints, columns);
}

static void showMemory(const std::vector<DeviceData>& devices)
{
  // clang-format off
  utils::table<DeviceData>(
      "Device memory heap", devices,
      std::vector<utils::TableColumn<DeviceData>>{{
          {.title = "Name", .toString = [](const DeviceData& data) { return std::string(static_cast<const char*>(data.properties.deviceName)); }},
          {.title = "memory Heap size", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryHeaps | std::views::take(data.memory.memoryHeapCount) | std::views::transform([](const VkMemoryHeap& heap) { return heap.size; })); }},
          {.title = "memory Heap flag local", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryHeaps | std::views::take(data.memory.memoryHeapCount) | std::views::transform([](const VkMemoryHeap& heap) { return (heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0; })); }},
          {.title = "memory Heap flag multi", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryHeaps | std::views::take(data.memory.memoryHeapCount) | std::views::transform([](const VkMemoryHeap& heap) { return (heap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT) != 0; })); }},
  }});

  utils::table<DeviceData>(
      "Device memory type", devices,
      std::vector<utils::TableColumn<DeviceData>>{{
          {.title = "Name", .toString = [](const DeviceData& data) { return std::string(static_cast<const char*>(data.properties.deviceName)); }},
          {.title = "index", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryTypes | std::views::take(data.memory.memoryTypeCount) | std::views::transform([](const VkMemoryType& type) { return type.heapIndex; })); }},
          {.title = "Flag Device", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryTypes | std::views::take(data.memory.memoryTypeCount) | std::views::transform([](const VkMemoryType& type) { return utils::string((type.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0, true); })); }},
          {.title = "Flag CPU", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryTypes | std::views::take(data.memory.memoryTypeCount) | std::views::transform([](const VkMemoryType& type) { return utils::string((type.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0, true); })); }},
          {.title = "Flag Coherent", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryTypes | std::views::take(data.memory.memoryTypeCount) | std::views::transform([](const VkMemoryType& type) { return utils::string((type.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0, true); })); }},
          {.title = "Flag Cached", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryTypes | std::views::take(data.memory.memoryTypeCount) | std::views::transform([](const VkMemoryType& type) { return utils::string((type.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0, true); })); }},
          {.title = "Flag Lazy", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryTypes | std::views::take(data.memory.memoryTypeCount) | std::views::transform([](const VkMemoryType& type) { return utils::string((type.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0, true); })  ); }},
          {.title = "Flag Protected", .toString = [](const DeviceData& data) { return 
            utils::table(data.memory.memoryTypes | std::views::take(data.memory.memoryTypeCount) | std::views::transform([](const VkMemoryType& type) { return utils::string((type.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT) != 0, true); })); }},
  }});
  // clang-format on
}

static void showQueue(const std::vector<DeviceData>& devices)
{
  for (const auto& device : devices) {
    const std::string title = std::format("Queue data for {}", std::string(static_cast<const char*>(device.properties.deviceName)));
    // clang-format off
    utils::table<DeviceDataQueue>(title, device.queues,
      std::vector<utils::TableColumn<DeviceDataQueue>>{{
        {.title = "Queue count", .toString = [](const DeviceDataQueue& data) { return utils::number(data.properties.queueCount); }},
        {.title = "GRAPHICS", .toString = [](const DeviceDataQueue& data) { return utils::string((data.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0); }},
        {.title = "COMPUTE", .toString = [](const DeviceDataQueue& data) { return utils::string((data.properties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0); }},
        {.title = "TRANSFER", .toString = [](const DeviceDataQueue& data) { return utils::string((data.properties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0); }},
        {.title = "SPARSE_BINDING", .toString = [](const DeviceDataQueue& data) { return utils::string((data.properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0); }},
        {.title = "PROTECTED", .toString = [](const DeviceDataQueue& data) { return utils::string((data.properties.queueFlags & VK_QUEUE_PROTECTED_BIT) != 0); }},
        {.title = "Valid bits", .toString = [](const DeviceDataQueue& data) { return utils::number(data.properties.timestampValidBits); }},
        {.title = "min image", .toString = [](const DeviceDataQueue& data) { return utils::string(data.properties.minImageTransferGranularity); }},
        {.title = "supportKHR", .toString = [](const DeviceDataQueue& data) { return utils::string(data.supportKHR); }},
    }});
    // clang-format on
  }
}

[[maybe_unused]]
static void show(const std::vector<DeviceData>& devices)
{
  showProperties(devices);
  showLimits(devices);
  showSparseProperties(devices);
  showFeatures(devices);
  showMemory(devices);
  showQueue(devices);
}

static std::vector<DeviceData> getDevicesData(const VkSurfaceKHR& surface)
{
  auto instance = InitVulkan::getInit();
  uint32_t count = 0;
  if (const VkResult status = vkEnumeratePhysicalDevices(instance->getInstance(), &count, nullptr); status != VK_SUCCESS) {
    throw std::runtime_error(std::format("No GPUs with Vulkan support found! status: {}", utils::result(status)));
  }

  std::vector<VkPhysicalDevice> devices(count);
  if (const VkResult status = vkEnumeratePhysicalDevices(instance->getInstance(), &count, devices.data()); status != VK_SUCCESS) {
    throw std::runtime_error(std::format("Failed to get GPUs! status: {}", utils::result(status)));
  }

  std::vector<DeviceData> devicesData;
  devicesData.reserve(count);
  for (const auto& device : devices) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> queues(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queues.data());

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    std::vector<DeviceDataQueue> queuesData;
    queuesData.reserve(count);
    for (uint32_t i = 0; i < queues.size(); ++i) {  // NOLINT(altera-id-dependent-backward-branch)
      VkBool32 supportKHR = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportKHR);

      queuesData.emplace_back(queues[i], supportKHR);
    }

    devicesData.emplace_back(device, properties, features, memProperties, std::move(queuesData));
  }

  if constexpr (Debug) {
    show(devicesData);
  }

  return devicesData;
}

static bool checkMinimalRequirements(const DeviceData& device)
{
  auto [id, properties, features, memory, queues] = device;
  if (properties.apiVersion < VK_API_VERSION_1_0) {
    return false;
  }

  bool queueGraphics = false;
  bool queueKHR = false;
  for (const auto& queue : queues) {
    if ((queue.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
      queueGraphics = true;
    }
    if (queue.supportKHR == VK_TRUE) {
      queueKHR = true;
    }
  }

  if (!queueGraphics || !queueKHR) {
    return false;
  }

  VkDeviceSize vram = 0;
  for (const VkMemoryHeap& mem : memory.memoryHeaps) {
    if ((mem.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0) {
      vram += mem.size;
    }
  }

  static constexpr uint64_t minMemory = 512ULL * 1024ULL * 1024ULL;  // 512MB
  return vram >= minMemory;
}

static bool compare(const DeviceData& deviceA, const DeviceData& deviceB)
{
  static constexpr auto typeScore = [](VkPhysicalDeviceType type) -> uint32_t {
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      return 3;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      return 2;
    default:
      return 1;
    }
  };
  static constexpr auto memorySize = [](const VkPhysicalDeviceMemoryProperties& memory) {
    VkDeviceSize size = 0;

    for (const VkMemoryHeap& mem : memory.memoryHeaps) {
      if (mem.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
        size += mem.size;
      }
    }
    return size;
  };
  static constexpr auto graphicQueue = [](const std::vector<DeviceDataQueue>& queues) {
    size_t size = 0;
    for (const auto& queue : queues) {
      if (queue.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        size++;
      }
    }
    return size;
  };

  const uint32_t typeScoreA = typeScore(deviceA.properties.deviceType);
  const uint32_t typeScoreB = typeScore(deviceB.properties.deviceType);
  if (typeScoreA != typeScoreB) {
    return typeScoreA < typeScoreB;
  }

  const uint32_t computeScoreA = deviceA.properties.limits.maxComputeWorkGroupInvocations;
  const uint32_t computeScoreB = deviceB.properties.limits.maxComputeWorkGroupInvocations;
  if (computeScoreA != computeScoreB) {
    return computeScoreA < computeScoreB;
  }

  const VkDeviceSize memoryScoreA = memorySize(deviceA.memory);
  const VkDeviceSize memoryScoreB = memorySize(deviceB.memory);
  if (memoryScoreA != memoryScoreB) {
    return memoryScoreA < memoryScoreB;
  }

  const VkDeviceSize queueScoreA = graphicQueue(deviceA.queues);
  const VkDeviceSize queueScoreB = graphicQueue(deviceB.queues);
  return queueScoreA < queueScoreB;
}

VulkanDevice::VulkanDevice(const VkSurfaceKHR& surface)
{
  auto bestDevice = std::ranges::max(getDevicesData(surface) | std::views::filter(checkMinimalRequirements), compare);
}
}  // namespace vulkan