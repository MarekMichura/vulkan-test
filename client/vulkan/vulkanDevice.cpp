#include "vulkanDevice.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "printTable.hpp"

namespace vul {
VulkanDevice::VulkanDevice(const VkInstance& instance) : _instance(instance), _physicalDevice(selectPhysicalDevices())
{
}

VkPhysicalDevice VulkanDevice::selectPhysicalDevices()
{
  std::vector<DeviceData> devicesData = getDeviceData(getPhysicalDevices());

#ifdef DEBUG
  auto formatType = [](VkPhysicalDeviceType type) {
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
      return "\033[0;37mOther / Unspecified\033[0m";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      return "\033[1;34mIntegrated GPU\033[0m";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      return "\033[1;32mDiscrete GPU\033[0m";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      return "\033[1;35mVirtual GPU\033[0m";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      return "\033[1;33mCPU as GPU\033[0m";
    default:
      return "\033[1;31mUnknown\033[0m";
    }
  };

  auto formatVendor = [](uint32_t vendor) -> std::string {
    switch (vendor) {
    case 0x10DE:
      return "\033[1;32mNVIDIA\033[0m";
    case 0x1002:
      return "\033[1;31mAMD\033[0m";
    case 0x8086:
      return "\033[1;34mIntel\033[0m";
    case 0x13B5:
      return "\033[1;35mARM\033[0m";
    case 0x5143:
      return "\033[1;36mQualcomm\033[0m";
    case 0x106B:
      return "\033[1;33mApple\033[0m";
    case 0x1414:
      return "\033[1;37mMicrosoft (WARP)\033[0m";
    case 0x1AE0:
      return "\033[1;90mGoogle\033[0m";
    case 0x1010:
      return "\033[1;91mImgTec (PowerVR)\033[0m";
    case 0x1D05:
      return "\033[1;92mMesa / LLVMpipe\033[0m";
    default:
      return std::format("\033[1;93mUnknown 0x{:X}\033[0m", vendor);
    }
  };

  using deviceInfo = std::tuple<std::string, std::vector<std::string>>;
  std::vector<deviceInfo> deviceData{
      {"deviceID", std::vector<std::string>{}},                                         //
      {"vendorID", std::vector<std::string>{}},                                         //
      {"apiVersion", std::vector<std::string>{}},                                       //
      {"driverVersion", std::vector<std::string>{}},                                    //
      {"deviceType", std::vector<std::string>{}},                                       //
      {"pipelineCacheUUID", std::vector<std::string>{}},                                //
                                                                                        //
      {"maxImageDimension1D", std::vector<std::string>{}},                              //
      {"maxImageDimension2D", std::vector<std::string>{}},                              //
      {"maxImageDimension3D", std::vector<std::string>{}},                              //
      {"maxImageDimensionCube", std::vector<std::string>{}},                            //
      {"maxImageArrayLayers", std::vector<std::string>{}},                              //
      {"maxTexelBufferElements", std::vector<std::string>{}},                           //
      {"maxUniformBufferRange", std::vector<std::string>{}},                            //
      {"maxStorageBufferRange", std::vector<std::string>{}},                            //
      {"maxPushConstantsSize", std::vector<std::string>{}},                             //
      {"maxMemoryAllocationCount", std::vector<std::string>{}},                         //
      {"maxSamplerAllocationCount", std::vector<std::string>{}},                        //
      {"bufferImageGranularity", std::vector<std::string>{}},                           //
      {"sparseAddressSpaceSize", std::vector<std::string>{}},                           //
      {"maxBoundDescriptorSets", std::vector<std::string>{}},                           //
      {"maxPerStageDescriptorSamplers", std::vector<std::string>{}},                    //
      {"maxPerStageDescriptorUniformBuffers", std::vector<std::string>{}},              //
      {"maxPerStageDescriptorStorageBuffers", std::vector<std::string>{}},              //
      {"maxPerStageDescriptorSampledImages", std::vector<std::string>{}},               //
      {"maxPerStageDescriptorStorageImages", std::vector<std::string>{}},               //
      {"maxPerStageDescriptorInputAttachments", std::vector<std::string>{}},            //
      {"maxPerStageResources", std::vector<std::string>{}},                             //
      {"maxDescriptorSetSamplers", std::vector<std::string>{}},                         //
      {"maxDescriptorSetUniformBuffers", std::vector<std::string>{}},                   //
      {"maxDescriptorSetUniformBuffersDynamic", std::vector<std::string>{}},            //
      {"maxDescriptorSetStorageBuffers", std::vector<std::string>{}},                   //
      {"maxDescriptorSetStorageBuffersDynamic", std::vector<std::string>{}},            //
      {"maxDescriptorSetSampledImages", std::vector<std::string>{}},                    //
      {"maxDescriptorSetStorageImages", std::vector<std::string>{}},                    //
      {"maxDescriptorSetInputAttachments", std::vector<std::string>{}},                 //
      {"maxVertexInputAttributes", std::vector<std::string>{}},                         //
      {"maxVertexInputBindings", std::vector<std::string>{}},                           //
      {"maxVertexInputAttributeOffset", std::vector<std::string>{}},                    //
      {"maxVertexInputBindingStride", std::vector<std::string>{}},                      //
      {"maxVertexOutputComponents", std::vector<std::string>{}},                        //
      {"maxTessellationGenerationLevel", std::vector<std::string>{}},                   //
      {"maxTessellationPatchSize", std::vector<std::string>{}},                         //
      {"maxTessellationControlPerVertexInputComponents", std::vector<std::string>{}},   //
      {"maxTessellationControlPerVertexOutputComponents", std::vector<std::string>{}},  //
      {"maxTessellationControlPerPatchOutputComponents", std::vector<std::string>{}},   //
      {"maxTessellationControlTotalOutputComponents", std::vector<std::string>{}},      //
      {"maxTessellationEvaluationInputComponents", std::vector<std::string>{}},         //
      {"maxTessellationEvaluationOutputComponents", std::vector<std::string>{}},        //
      {"maxGeometryShaderInvocations", std::vector<std::string>{}},                     //
      {"maxGeometryInputComponents", std::vector<std::string>{}},                       //
      {"maxGeometryOutputComponents", std::vector<std::string>{}},                      //
      {"maxGeometryOutputVertices", std::vector<std::string>{}},                        //
      {"maxGeometryTotalOutputComponents", std::vector<std::string>{}},                 //
      {"maxFragmentInputComponents", std::vector<std::string>{}},                       //
      {"maxFragmentOutputAttachments", std::vector<std::string>{}},                     //
      {"maxFragmentDualSrcAttachments", std::vector<std::string>{}},                    //
      {"maxFragmentCombinedOutputResources", std::vector<std::string>{}},               //
      {"maxComputeSharedMemorySize", std::vector<std::string>{}},                       //
      {"maxComputeWorkGroupCount", std::vector<std::string>{}},                         //
      {"maxComputeWorkGroupInvocations", std::vector<std::string>{}},                   //
      {"maxComputeWorkGroupSize", std::vector<std::string>{}},                          //
      {"subPixelPrecisionBits", std::vector<std::string>{}},                            //
      {"subTexelPrecisionBits", std::vector<std::string>{}},                            //
      {"mipmapPrecisionBits", std::vector<std::string>{}},                              //
      {"maxDrawIndexedIndexValue", std::vector<std::string>{}},                         //
      {"maxDrawIndirectCount", std::vector<std::string>{}},                             //
      {"maxSamplerLodBias", std::vector<std::string>{}},                                //
      {"maxSamplerAnisotropy", std::vector<std::string>{}},                             //
      {"maxViewports", std::vector<std::string>{}},                                     //
      {"maxViewportDimensions", std::vector<std::string>{}},                            //
      {"viewportBoundsRange", std::vector<std::string>{}},                              //
      {"viewportSubPixelBits", std::vector<std::string>{}},                             //
      {"minMemoryMapAlignment", std::vector<std::string>{}},                            //
      {"minTexelBufferOffsetAlignment", std::vector<std::string>{}},                    //
      {"minUniformBufferOffsetAlignment", std::vector<std::string>{}},                  //
      {"minStorageBufferOffsetAlignment", std::vector<std::string>{}},                  //
      {"minTexelOffset", std::vector<std::string>{}},                                   //
      {"maxTexelOffset", std::vector<std::string>{}},                                   //
      {"minTexelGatherOffset", std::vector<std::string>{}},                             //
      {"maxTexelGatherOffset", std::vector<std::string>{}},                             //
      {"minInterpolationOffset", std::vector<std::string>{}},                           //
      {"maxInterpolationOffset", std::vector<std::string>{}},                           //
      {"subPixelInterpolationOffsetBits", std::vector<std::string>{}},                  //
      {"maxFramebufferWidth", std::vector<std::string>{}},                              //
      {"maxFramebufferHeight", std::vector<std::string>{}},                             //
      {"maxFramebufferLayers", std::vector<std::string>{}},                             //
      {"framebufferColorSampleCounts", std::vector<std::string>{}},                     //
      {"framebufferDepthSampleCounts", std::vector<std::string>{}},                     //
      {"framebufferStencilSampleCounts", std::vector<std::string>{}},                   //
      {"framebufferNoAttachmentsSampleCounts", std::vector<std::string>{}},             //
      {"maxColorAttachments", std::vector<std::string>{}},                              //
      {"sampledImageColorSampleCounts", std::vector<std::string>{}},                    //
      {"sampledImageIntegerSampleCounts", std::vector<std::string>{}},                  //
      {"sampledImageDepthSampleCounts", std::vector<std::string>{}},                    //
      {"sampledImageStencilSampleCounts", std::vector<std::string>{}},                  //
      {"storageImageSampleCounts", std::vector<std::string>{}},                         //
      {"maxSampleMaskWords", std::vector<std::string>{}},                               //
      {"timestampComputeAndGraphics", std::vector<std::string>{}},                      //
      {"timestampPeriod", std::vector<std::string>{}},                                  //
      {"maxClipDistances", std::vector<std::string>{}},                                 //
      {"maxCullDistances", std::vector<std::string>{}},                                 //
      {"maxCombinedClipAndCullDistances", std::vector<std::string>{}},                  //
      {"discreteQueuePriorities", std::vector<std::string>{}},                          //
      {"pointSizeRange", std::vector<std::string>{}},                                   //
      {"lineWidthRange", std::vector<std::string>{}},                                   //
      {"pointSizeGranularity", std::vector<std::string>{}},                             //
      {"lineWidthGranularity", std::vector<std::string>{}},                             //
      {"strictLines", std::vector<std::string>{}},                                      //
      {"standardSampleLocations", std::vector<std::string>{}},                          //
      {"optimalBufferCopyOffsetAlignment", std::vector<std::string>{}},                 //
      {"optimalBufferCopyRowPitchAlignment", std::vector<std::string>{}},               //
      {"nonCoherentAtomSize", std::vector<std::string>{}},                              //
                                                                                        //
      {"residencyStandard2DBlockShape", std::vector<std::string>{}},                    //
      {"residencyStandard2DMultisampleBlockShape", std::vector<std::string>{}},         //
      {"residencyStandard3DBlockShape", std::vector<std::string>{}},                    //
      {"residencyAlignedMipSize", std::vector<std::string>{}},                          //
      {"residencyNonResidentStrict", std::vector<std::string>{}},                       //
                                                                                        //
      {"robustBufferAccess", std::vector<std::string>{}},                               //
      {"fullDrawIndexUint32", std::vector<std::string>{}},                              //
      {"imageCubeArray", std::vector<std::string>{}},                                   //
      {"independentBlend", std::vector<std::string>{}},                                 //
      {"geometryShader", std::vector<std::string>{}},                                   //
      {"tessellationShader", std::vector<std::string>{}},                               //
      {"sampleRateShading", std::vector<std::string>{}},                                //
      {"dualSrcBlend", std::vector<std::string>{}},                                     //
      {"logicOp", std::vector<std::string>{}},                                          //
      {"multiDrawIndirect", std::vector<std::string>{}},                                //
      {"drawIndirectFirstInstance", std::vector<std::string>{}},                        //
      {"depthClamp", std::vector<std::string>{}},                                       //
      {"depthBiasClamp", std::vector<std::string>{}},                                   //
      {"fillModeNonSolid", std::vector<std::string>{}},                                 //
      {"depthBounds", std::vector<std::string>{}},                                      //
      {"wideLines", std::vector<std::string>{}},                                        //
      {"largePoints", std::vector<std::string>{}},                                      //
      {"alphaToOne", std::vector<std::string>{}},                                       //
      {"multiViewport", std::vector<std::string>{}},                                    //
      {"samplerAnisotropy", std::vector<std::string>{}},                                //
      {"textureCompressionETC2", std::vector<std::string>{}},                           //
      {"textureCompressionASTC_LDR", std::vector<std::string>{}},                       //
      {"textureCompressionBC", std::vector<std::string>{}},                             //
      {"occlusionQueryPrecise", std::vector<std::string>{}},                            //
      {"pipelineStatisticsQuery", std::vector<std::string>{}},                          //
      {"vertexPipelineStoresAndAtomics", std::vector<std::string>{}},                   //
      {"fragmentStoresAndAtomics", std::vector<std::string>{}},                         //
      {"shaderTessellationAndGeometryPointSize", std::vector<std::string>{}},           //
      {"shaderImageGatherExtended", std::vector<std::string>{}},                        //
      {"shaderStorageImageExtendedFormats", std::vector<std::string>{}},                //
      {"shaderStorageImageMultisample", std::vector<std::string>{}},                    //
      {"shaderStorageImageReadWithoutFormat", std::vector<std::string>{}},              //
      {"shaderStorageImageWriteWithoutFormat", std::vector<std::string>{}},             //
      {"shaderUniformBufferArrayDynamicIndexing", std::vector<std::string>{}},          //
      {"shaderSampledImageArrayDynamicIndexing", std::vector<std::string>{}},           //
      {"shaderStorageBufferArrayDynamicIndexing", std::vector<std::string>{}},          //
      {"shaderStorageImageArrayDynamicIndexing", std::vector<std::string>{}},           //
      {"shaderClipDistance", std::vector<std::string>{}},                               //
      {"shaderCullDistance", std::vector<std::string>{}},                               //
      {"shaderFloat64", std::vector<std::string>{}},                                    //
      {"shaderInt64", std::vector<std::string>{}},                                      //
      {"shaderInt16", std::vector<std::string>{}},                                      //
      {"shaderResourceResidency", std::vector<std::string>{}},                          //
      {"shaderResourceMinLod", std::vector<std::string>{}},                             //
      {"sparseBinding", std::vector<std::string>{}},                                    //
      {"sparseResidencyBuffer", std::vector<std::string>{}},                            //
      {"sparseResidencyImage2D", std::vector<std::string>{}},                           //
      {"sparseResidencyImage3D", std::vector<std::string>{}},                           //
      {"sparseResidency2Samples", std::vector<std::string>{}},                          //
      {"sparseResidency4Samples", std::vector<std::string>{}},                          //
      {"sparseResidency8Samples", std::vector<std::string>{}},                          //
      {"sparseResidency16Samples", std::vector<std::string>{}},                         //
      {"sparseResidencyAliased", std::vector<std::string>{}},                           //
      {"variableMultisampleRate", std::vector<std::string>{}},                          //
      {"inheritedQueries", std::vector<std::string>{}},                                 //
  };

  for (const DeviceData& device : devicesData) {
    std::get<1>(deviceData.at(0)).emplace_back(printTable::number(device.properties.deviceID));
    std::get<1>(deviceData.at(1)).emplace_back(formatVendor(device.properties.vendorID));
    std::get<1>(deviceData.at(2)).emplace_back(printTable::version(device.properties.apiVersion));
    std::get<1>(deviceData.at(3)).emplace_back(printTable::version(device.properties.driverVersion));
    std::get<1>(deviceData.at(4)).emplace_back(formatType(device.properties.deviceType));
    std::get<1>(deviceData.at(5)).emplace_back(printTable::number(device.properties.pipelineCacheUUID));

    std::get<1>(deviceData.at(6)).emplace_back(printTable::number(device.properties.limits.maxImageDimension1D));
    std::get<1>(deviceData.at(7)).emplace_back(printTable::number(device.properties.limits.maxImageDimension2D));
    std::get<1>(deviceData.at(8)).emplace_back(printTable::number(device.properties.limits.maxImageDimension3D));
    std::get<1>(deviceData.at(9)).emplace_back(printTable::number(device.properties.limits.maxImageDimensionCube));
    std::get<1>(deviceData.at(10)).emplace_back(printTable::number(device.properties.limits.maxImageArrayLayers));
    std::get<1>(deviceData.at(11)).emplace_back(printTable::number(device.properties.limits.maxTexelBufferElements));
    std::get<1>(deviceData.at(12)).emplace_back(printTable::number(device.properties.limits.maxUniformBufferRange));
    std::get<1>(deviceData.at(13)).emplace_back(printTable::number(device.properties.limits.maxStorageBufferRange));
    std::get<1>(deviceData.at(14)).emplace_back(printTable::number(device.properties.limits.maxPushConstantsSize));
    std::get<1>(deviceData.at(15)).emplace_back(printTable::number(device.properties.limits.maxMemoryAllocationCount));
    std::get<1>(deviceData.at(16)).emplace_back(printTable::number(device.properties.limits.maxSamplerAllocationCount));
    std::get<1>(deviceData.at(17)).emplace_back(printTable::number(device.properties.limits.bufferImageGranularity));
    std::get<1>(deviceData.at(18)).emplace_back(printTable::number(device.properties.limits.sparseAddressSpaceSize));
    std::get<1>(deviceData.at(19)).emplace_back(printTable::number(device.properties.limits.maxBoundDescriptorSets));
    std::get<1>(deviceData.at(20))
        .emplace_back(printTable::number(device.properties.limits.maxPerStageDescriptorSamplers));
    std::get<1>(deviceData.at(21))
        .emplace_back(printTable::number(device.properties.limits.maxPerStageDescriptorUniformBuffers));
    std::get<1>(deviceData.at(22))
        .emplace_back(printTable::number(device.properties.limits.maxPerStageDescriptorStorageBuffers));
    std::get<1>(deviceData.at(23))
        .emplace_back(printTable::number(device.properties.limits.maxPerStageDescriptorSampledImages));
    std::get<1>(deviceData.at(24))
        .emplace_back(printTable::number(device.properties.limits.maxPerStageDescriptorStorageImages));
    std::get<1>(deviceData.at(25))
        .emplace_back(printTable::number(device.properties.limits.maxPerStageDescriptorInputAttachments));
    std::get<1>(deviceData.at(26)).emplace_back(printTable::number(device.properties.limits.maxPerStageResources));
    std::get<1>(deviceData.at(27)).emplace_back(printTable::number(device.properties.limits.maxDescriptorSetSamplers));
    std::get<1>(deviceData.at(28))
        .emplace_back(printTable::number(device.properties.limits.maxDescriptorSetUniformBuffers));
    std::get<1>(deviceData.at(29))
        .emplace_back(printTable::number(device.properties.limits.maxDescriptorSetUniformBuffersDynamic));
    std::get<1>(deviceData.at(30))
        .emplace_back(printTable::number(device.properties.limits.maxDescriptorSetStorageBuffers));
    std::get<1>(deviceData.at(31))
        .emplace_back(printTable::number(device.properties.limits.maxDescriptorSetStorageBuffersDynamic));
    std::get<1>(deviceData.at(32))
        .emplace_back(printTable::number(device.properties.limits.maxDescriptorSetSampledImages));
    std::get<1>(deviceData.at(33))
        .emplace_back(printTable::number(device.properties.limits.maxDescriptorSetStorageImages));
    std::get<1>(deviceData.at(34))
        .emplace_back(printTable::number(device.properties.limits.maxDescriptorSetInputAttachments));
    std::get<1>(deviceData.at(35)).emplace_back(printTable::number(device.properties.limits.maxVertexInputAttributes));
    std::get<1>(deviceData.at(36)).emplace_back(printTable::number(device.properties.limits.maxVertexInputBindings));
    std::get<1>(deviceData.at(37))
        .emplace_back(printTable::number(device.properties.limits.maxVertexInputAttributeOffset));
    std::get<1>(deviceData.at(38))
        .emplace_back(printTable::number(device.properties.limits.maxVertexInputBindingStride));
    std::get<1>(deviceData.at(39)).emplace_back(printTable::number(device.properties.limits.maxVertexOutputComponents));
    std::get<1>(deviceData.at(40))
        .emplace_back(printTable::number(device.properties.limits.maxTessellationGenerationLevel));
    std::get<1>(deviceData.at(41)).emplace_back(printTable::number(device.properties.limits.maxTessellationPatchSize));
    std::get<1>(deviceData.at(42))
        .emplace_back(printTable::number(device.properties.limits.maxTessellationControlPerVertexInputComponents));
    std::get<1>(deviceData.at(43))
        .emplace_back(printTable::number(device.properties.limits.maxTessellationControlPerVertexOutputComponents));
    std::get<1>(deviceData.at(44))
        .emplace_back(printTable::number(device.properties.limits.maxTessellationControlPerPatchOutputComponents));
    std::get<1>(deviceData.at(45))
        .emplace_back(printTable::number(device.properties.limits.maxTessellationControlTotalOutputComponents));
    std::get<1>(deviceData.at(46))
        .emplace_back(printTable::number(device.properties.limits.maxTessellationEvaluationInputComponents));
    std::get<1>(deviceData.at(47))
        .emplace_back(printTable::number(device.properties.limits.maxTessellationEvaluationOutputComponents));
    std::get<1>(deviceData.at(48))
        .emplace_back(printTable::number(device.properties.limits.maxGeometryShaderInvocations));
    std::get<1>(deviceData.at(49))
        .emplace_back(printTable::number(device.properties.limits.maxGeometryInputComponents));
    std::get<1>(deviceData.at(50))
        .emplace_back(printTable::number(device.properties.limits.maxGeometryOutputComponents));
    std::get<1>(deviceData.at(51)).emplace_back(printTable::number(device.properties.limits.maxGeometryOutputVertices));
    std::get<1>(deviceData.at(52))
        .emplace_back(printTable::number(device.properties.limits.maxGeometryTotalOutputComponents));
    std::get<1>(deviceData.at(53))
        .emplace_back(printTable::number(device.properties.limits.maxFragmentInputComponents));
    std::get<1>(deviceData.at(54))
        .emplace_back(printTable::number(device.properties.limits.maxFragmentOutputAttachments));
    std::get<1>(deviceData.at(55))
        .emplace_back(printTable::number(device.properties.limits.maxFragmentDualSrcAttachments));
    std::get<1>(deviceData.at(56))
        .emplace_back(printTable::number(device.properties.limits.maxFragmentCombinedOutputResources));
    std::get<1>(deviceData.at(57))
        .emplace_back(printTable::number(device.properties.limits.maxComputeSharedMemorySize));
    std::get<1>(deviceData.at(58)).emplace_back(printTable::number(device.properties.limits.maxComputeWorkGroupCount));
    std::get<1>(deviceData.at(59))
        .emplace_back(printTable::number(device.properties.limits.maxComputeWorkGroupInvocations));
    std::get<1>(deviceData.at(60)).emplace_back(printTable::number(device.properties.limits.maxComputeWorkGroupSize));
    std::get<1>(deviceData.at(61)).emplace_back(printTable::number(device.properties.limits.subPixelPrecisionBits));
    std::get<1>(deviceData.at(62)).emplace_back(printTable::number(device.properties.limits.subTexelPrecisionBits));
    std::get<1>(deviceData.at(63)).emplace_back(printTable::number(device.properties.limits.mipmapPrecisionBits));
    std::get<1>(deviceData.at(64)).emplace_back(printTable::number(device.properties.limits.maxDrawIndexedIndexValue));
    std::get<1>(deviceData.at(65)).emplace_back(printTable::number(device.properties.limits.maxDrawIndirectCount));
    std::get<1>(deviceData.at(66)).emplace_back(printTable::number(device.properties.limits.maxSamplerLodBias));
    std::get<1>(deviceData.at(67)).emplace_back(printTable::number(device.properties.limits.maxSamplerAnisotropy));
    std::get<1>(deviceData.at(68)).emplace_back(printTable::number(device.properties.limits.maxViewports));
    std::get<1>(deviceData.at(69)).emplace_back(printTable::number(device.properties.limits.maxViewportDimensions));
    std::get<1>(deviceData.at(70)).emplace_back(printTable::number(device.properties.limits.viewportBoundsRange));
    std::get<1>(deviceData.at(71)).emplace_back(printTable::number(device.properties.limits.viewportSubPixelBits));
    std::get<1>(deviceData.at(72)).emplace_back(printTable::number(device.properties.limits.minMemoryMapAlignment));
    std::get<1>(deviceData.at(73))
        .emplace_back(printTable::number(device.properties.limits.minTexelBufferOffsetAlignment));
    std::get<1>(deviceData.at(74))
        .emplace_back(printTable::number(device.properties.limits.minUniformBufferOffsetAlignment));
    std::get<1>(deviceData.at(75))
        .emplace_back(printTable::number(device.properties.limits.minStorageBufferOffsetAlignment));
    std::get<1>(deviceData.at(76)).emplace_back(printTable::number(device.properties.limits.minTexelOffset));
    std::get<1>(deviceData.at(77)).emplace_back(printTable::number(device.properties.limits.maxTexelOffset));
    std::get<1>(deviceData.at(78)).emplace_back(printTable::number(device.properties.limits.minTexelGatherOffset));
    std::get<1>(deviceData.at(79)).emplace_back(printTable::number(device.properties.limits.maxTexelGatherOffset));
    std::get<1>(deviceData.at(80)).emplace_back(printTable::number(device.properties.limits.minInterpolationOffset));
    std::get<1>(deviceData.at(81)).emplace_back(printTable::number(device.properties.limits.maxInterpolationOffset));
    std::get<1>(deviceData.at(82))
        .emplace_back(printTable::number(device.properties.limits.subPixelInterpolationOffsetBits));
    std::get<1>(deviceData.at(83)).emplace_back(printTable::number(device.properties.limits.maxFramebufferWidth));
    std::get<1>(deviceData.at(84)).emplace_back(printTable::number(device.properties.limits.maxFramebufferHeight));
    std::get<1>(deviceData.at(85)).emplace_back(printTable::number(device.properties.limits.maxFramebufferLayers));
    std::get<1>(deviceData.at(86))
        .emplace_back(printTable::number(device.properties.limits.framebufferColorSampleCounts));
    std::get<1>(deviceData.at(87))
        .emplace_back(printTable::number(device.properties.limits.framebufferDepthSampleCounts));
    std::get<1>(deviceData.at(88))
        .emplace_back(printTable::number(device.properties.limits.framebufferStencilSampleCounts));
    std::get<1>(deviceData.at(89))
        .emplace_back(printTable::number(device.properties.limits.framebufferNoAttachmentsSampleCounts));
    std::get<1>(deviceData.at(90)).emplace_back(printTable::number(device.properties.limits.maxColorAttachments));
    std::get<1>(deviceData.at(91))
        .emplace_back(printTable::number(device.properties.limits.sampledImageColorSampleCounts));
    std::get<1>(deviceData.at(92))
        .emplace_back(printTable::number(device.properties.limits.sampledImageIntegerSampleCounts));
    std::get<1>(deviceData.at(93))
        .emplace_back(printTable::number(device.properties.limits.sampledImageDepthSampleCounts));
    std::get<1>(deviceData.at(94))
        .emplace_back(printTable::number(device.properties.limits.sampledImageStencilSampleCounts));
    std::get<1>(deviceData.at(95)).emplace_back(printTable::number(device.properties.limits.storageImageSampleCounts));
    std::get<1>(deviceData.at(96)).emplace_back(printTable::number(device.properties.limits.maxSampleMaskWords));
    std::get<1>(deviceData.at(97))
        .emplace_back(printTable::boolean(device.properties.limits.timestampComputeAndGraphics != 0U));
    std::get<1>(deviceData.at(98)).emplace_back(printTable::number(device.properties.limits.timestampPeriod));
    std::get<1>(deviceData.at(99)).emplace_back(printTable::number(device.properties.limits.maxClipDistances));
    std::get<1>(deviceData.at(100)).emplace_back(printTable::number(device.properties.limits.maxCullDistances));
    std::get<1>(deviceData.at(101))
        .emplace_back(printTable::number(device.properties.limits.maxCombinedClipAndCullDistances));
    std::get<1>(deviceData.at(102)).emplace_back(printTable::number(device.properties.limits.discreteQueuePriorities));
    std::get<1>(deviceData.at(103)).emplace_back(printTable::number(device.properties.limits.pointSizeRange));
    std::get<1>(deviceData.at(104)).emplace_back(printTable::number(device.properties.limits.lineWidthRange));
    std::get<1>(deviceData.at(105)).emplace_back(printTable::number(device.properties.limits.pointSizeGranularity));
    std::get<1>(deviceData.at(106)).emplace_back(printTable::number(device.properties.limits.lineWidthGranularity));
    std::get<1>(deviceData.at(107)).emplace_back(printTable::boolean(device.properties.limits.strictLines != 0U));
    std::get<1>(deviceData.at(108))
        .emplace_back(printTable::boolean(device.properties.limits.standardSampleLocations != 0U));
    std::get<1>(deviceData.at(109))
        .emplace_back(printTable::number(device.properties.limits.optimalBufferCopyOffsetAlignment));
    std::get<1>(deviceData.at(110))
        .emplace_back(printTable::number(device.properties.limits.optimalBufferCopyRowPitchAlignment));
    std::get<1>(deviceData.at(111)).emplace_back(printTable::number(device.properties.limits.nonCoherentAtomSize));

    std::get<1>(deviceData.at(112))
        .emplace_back(printTable::boolean(device.properties.sparseProperties.residencyStandard2DBlockShape != 0U));
    std::get<1>(deviceData.at(113))
        .emplace_back(
            printTable::boolean(device.properties.sparseProperties.residencyStandard2DMultisampleBlockShape != 0U));
    std::get<1>(deviceData.at(114))
        .emplace_back(printTable::boolean(device.properties.sparseProperties.residencyStandard3DBlockShape != 0U));
    std::get<1>(deviceData.at(115))
        .emplace_back(printTable::boolean(device.properties.sparseProperties.residencyAlignedMipSize != 0U));
    std::get<1>(deviceData.at(116))
        .emplace_back(printTable::boolean(device.properties.sparseProperties.residencyNonResidentStrict != 0U));

    std::get<1>(deviceData.at(117)).emplace_back(printTable::boolean(device.features.robustBufferAccess != 0U));
    std::get<1>(deviceData.at(118)).emplace_back(printTable::boolean(device.features.fullDrawIndexUint32 != 0U));
    std::get<1>(deviceData.at(119)).emplace_back(printTable::boolean(device.features.imageCubeArray != 0U));
    std::get<1>(deviceData.at(120)).emplace_back(printTable::boolean(device.features.independentBlend != 0U));
    std::get<1>(deviceData.at(121)).emplace_back(printTable::boolean(device.features.geometryShader != 0U));
    std::get<1>(deviceData.at(122)).emplace_back(printTable::boolean(device.features.tessellationShader != 0U));
    std::get<1>(deviceData.at(123)).emplace_back(printTable::boolean(device.features.sampleRateShading != 0U));
    std::get<1>(deviceData.at(124)).emplace_back(printTable::boolean(device.features.dualSrcBlend != 0U));
    std::get<1>(deviceData.at(125)).emplace_back(printTable::boolean(device.features.logicOp != 0U));
    std::get<1>(deviceData.at(126)).emplace_back(printTable::boolean(device.features.multiDrawIndirect != 0U));
    std::get<1>(deviceData.at(127)).emplace_back(printTable::boolean(device.features.drawIndirectFirstInstance != 0U));
    std::get<1>(deviceData.at(128)).emplace_back(printTable::boolean(device.features.depthClamp != 0U));
    std::get<1>(deviceData.at(129)).emplace_back(printTable::boolean(device.features.depthBiasClamp != 0U));
    std::get<1>(deviceData.at(130)).emplace_back(printTable::boolean(device.features.fillModeNonSolid != 0U));
    std::get<1>(deviceData.at(131)).emplace_back(printTable::boolean(device.features.depthBounds != 0U));
    std::get<1>(deviceData.at(132)).emplace_back(printTable::boolean(device.features.wideLines != 0U));
    std::get<1>(deviceData.at(133)).emplace_back(printTable::boolean(device.features.largePoints != 0U));
    std::get<1>(deviceData.at(134)).emplace_back(printTable::boolean(device.features.alphaToOne != 0U));
    std::get<1>(deviceData.at(135)).emplace_back(printTable::boolean(device.features.multiViewport != 0U));
    std::get<1>(deviceData.at(136)).emplace_back(printTable::boolean(device.features.samplerAnisotropy != 0U));
    std::get<1>(deviceData.at(137)).emplace_back(printTable::boolean(device.features.textureCompressionETC2 != 0U));
    std::get<1>(deviceData.at(138)).emplace_back(printTable::boolean(device.features.textureCompressionASTC_LDR != 0U));
    std::get<1>(deviceData.at(139)).emplace_back(printTable::boolean(device.features.textureCompressionBC != 0U));
    std::get<1>(deviceData.at(140)).emplace_back(printTable::boolean(device.features.occlusionQueryPrecise != 0U));
    std::get<1>(deviceData.at(141)).emplace_back(printTable::boolean(device.features.pipelineStatisticsQuery != 0U));
    std::get<1>(deviceData.at(142))
        .emplace_back(printTable::boolean(device.features.vertexPipelineStoresAndAtomics != 0U));
    std::get<1>(deviceData.at(143)).emplace_back(printTable::boolean(device.features.fragmentStoresAndAtomics != 0U));
    std::get<1>(deviceData.at(144))
        .emplace_back(printTable::boolean(device.features.shaderTessellationAndGeometryPointSize != 0U));
    std::get<1>(deviceData.at(145)).emplace_back(printTable::boolean(device.features.shaderImageGatherExtended != 0U));
    std::get<1>(deviceData.at(146))
        .emplace_back(printTable::boolean(device.features.shaderStorageImageExtendedFormats != 0U));
    std::get<1>(deviceData.at(147))
        .emplace_back(printTable::boolean(device.features.shaderStorageImageMultisample != 0U));
    std::get<1>(deviceData.at(148))
        .emplace_back(printTable::boolean(device.features.shaderStorageImageReadWithoutFormat != 0U));
    std::get<1>(deviceData.at(149))
        .emplace_back(printTable::boolean(device.features.shaderStorageImageWriteWithoutFormat != 0U));
    std::get<1>(deviceData.at(150))
        .emplace_back(printTable::boolean(device.features.shaderUniformBufferArrayDynamicIndexing != 0U));
    std::get<1>(deviceData.at(151))
        .emplace_back(printTable::boolean(device.features.shaderSampledImageArrayDynamicIndexing != 0U));
    std::get<1>(deviceData.at(152))
        .emplace_back(printTable::boolean(device.features.shaderStorageBufferArrayDynamicIndexing != 0U));
    std::get<1>(deviceData.at(153))
        .emplace_back(printTable::boolean(device.features.shaderStorageImageArrayDynamicIndexing != 0U));
    std::get<1>(deviceData.at(154)).emplace_back(printTable::boolean(device.features.shaderClipDistance != 0U));
    std::get<1>(deviceData.at(155)).emplace_back(printTable::boolean(device.features.shaderCullDistance != 0U));
    std::get<1>(deviceData.at(156)).emplace_back(printTable::boolean(device.features.shaderFloat64 != 0U));
    std::get<1>(deviceData.at(157)).emplace_back(printTable::boolean(device.features.shaderInt64 != 0U));
    std::get<1>(deviceData.at(158)).emplace_back(printTable::boolean(device.features.shaderInt16 != 0U));
    std::get<1>(deviceData.at(159)).emplace_back(printTable::boolean(device.features.shaderResourceResidency != 0U));
    std::get<1>(deviceData.at(160)).emplace_back(printTable::boolean(device.features.shaderResourceMinLod != 0U));
    std::get<1>(deviceData.at(161)).emplace_back(printTable::boolean(device.features.sparseBinding != 0U));
    std::get<1>(deviceData.at(162)).emplace_back(printTable::boolean(device.features.sparseResidencyBuffer != 0U));
    std::get<1>(deviceData.at(163)).emplace_back(printTable::boolean(device.features.sparseResidencyImage2D != 0U));
    std::get<1>(deviceData.at(164)).emplace_back(printTable::boolean(device.features.sparseResidencyImage3D != 0U));
    std::get<1>(deviceData.at(165)).emplace_back(printTable::boolean(device.features.sparseResidency2Samples != 0U));
    std::get<1>(deviceData.at(166)).emplace_back(printTable::boolean(device.features.sparseResidency4Samples != 0U));
    std::get<1>(deviceData.at(167)).emplace_back(printTable::boolean(device.features.sparseResidency8Samples != 0U));
    std::get<1>(deviceData.at(168)).emplace_back(printTable::boolean(device.features.sparseResidency16Samples != 0U));
    std::get<1>(deviceData.at(169)).emplace_back(printTable::boolean(device.features.sparseResidencyAliased != 0U));
    std::get<1>(deviceData.at(170)).emplace_back(printTable::boolean(device.features.variableMultisampleRate != 0U));
    std::get<1>(deviceData.at(171)).emplace_back(printTable::boolean(device.features.inheritedQueries != 0U));
  }
  std::vector<printTable::TableColumn<deviceInfo>> columns = {
      {.header = "Var name",
       .toString = [](const auto& d) { return std::get<0>(d); },
       .align = printTable::Align::right},
  };

  for (size_t i = 0; i < devicesData.size(); i++) {
    columns.push_back({
        .header = devicesData.at(i).properties.deviceName,                          //
        .toString = [i](const deviceInfo& ele) { return std::get<1>(ele).at(i); },  //
        .align = printTable::Align::left,
    });
  }

  printTable::print("Device data", deviceData, columns);
#endif

  auto rateType = [](const VkPhysicalDeviceProperties& data) -> int {
    switch (data.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      return 3;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      return 2;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      return 1;
    default:
      return 0;
    }
  };

  auto bestDeviceType = std::ranges::max(                               //
      std::views::transform(devicesData, [rateType](const auto& ele) {  //
        return rateType(ele.properties);
      }));

  auto bestDeviceTypeDevices = std::views::filter(                //
      devicesData, [bestDeviceType, rateType](const auto& ele) {  //
        return rateType(ele.properties) == bestDeviceType;
      });

  auto bestDevice = *std::ranges::max_element(  //
      bestDeviceTypeDevices, [](const auto& first, const auto& second) {
        return first.properties.limits.maxImageDimension2D < second.properties.limits.maxImageDimension2D;
      });

  return bestDevice.device;
}

std::vector<VkPhysicalDevice> VulkanDevice::getPhysicalDevices()
{
  uint32_t deviceCount = 0;
  auto status = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
  if (deviceCount == 0 || status != VK_SUCCESS) {
    throw std::runtime_error("No GPUs with Vulkan support found!");
  }

  std::vector<VkPhysicalDevice> devices;
  devices.resize(deviceCount);
  status = vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
  if (status != VK_SUCCESS) {
    throw std::runtime_error("Failed to get GPUs!");
  }

  return devices;
}

std::vector<VulkanDevice::DeviceData> VulkanDevice::getDeviceData(const std::vector<VkPhysicalDevice>& devices)
{
  std::vector<VulkanDevice::DeviceData> deviceData;
  deviceData.reserve(devices.size());

  for (const auto& device : devices) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    deviceData.emplace_back(device, properties, features);
  }

  return deviceData;
}

}  // namespace vul