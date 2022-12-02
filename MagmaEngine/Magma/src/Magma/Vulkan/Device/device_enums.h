#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

namespace Magma {

    class QueueFamily {

    public:
        uint32_t getQueue();
        uint32_t getPresentation();

        void setQueue();
        void setQueue(uint32_t queue);

        void setPresentation();
        void setPresentation(uint32_t presentation);

        bool hasQueue();
        bool hasPresentation();

    private:
        std::optional<uint32_t> queue, presentation;

    };

    class SwapchainSupportDetails {

    public:
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats{};
        std::vector<VkPresentModeKHR> presentModes{};

    };

    enum class VulkanPriority {
        NONE = 0,
        LOW = 1,
        MEDIUM = 2,
        HIGH = 3,
        REQUIRED = -1
    };

    enum class VulkanDeviceType {
        CPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU,
        DISCRETE_GPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
        INTEGRATED_GPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
        MAX = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM,
        OTHER = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER,
        VIRTUAL_GPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
    };

    enum class VulkanQueueType {
        COMPUTE = VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT,
        GRAPHICS = VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT,
        TRANSFER = VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT
    };

    VulkanQueueType operator|(VulkanQueueType& a, VulkanQueueType& b);

    enum class VulkanDeviceFeature {
        robustBufferAccess,
        fullDrawIndexUint32,
        imageCubeArray,
        independentBlend,
        geometryShader,
        tessellationShader,
        sampleRateShading,
        dualSrcBlend,
        logicOp,
        multiDrawIndirect,
        drawIndirectFirstInstance,
        depthClamp,
        depthBiasClamp,
        fillModeNonSolid,
        depthBounds,
        wideLines,
        largePoints,
        alphaToOne,
        multiViewport,
        samplerAnisotropy,
        textureCompressionETC2,
        textureCompressionASTC_LDR,
        textureCompressionBC,
        occlusionQueryPrecise,
        pipelineStatisticsQuery,
        vertexPipelineStoresAndAtomics,
        fragmentStoresAndAtomics,
        shaderTessellationAndGeometryPointSize,
        shaderImageGatherExtended,
        shaderStorageImageExtendedFormats,
        shaderStorageImageMultisample,
        shaderStorageImageReadWithoutFormat,
        shaderStorageImageWriteWithoutFormat,
        shaderUniformBufferArrayDynamicIndexing,
        shaderSampledImageArrayDynamicIndexing,
        shaderStorageBufferArrayDynamicIndexing,
        shaderStorageImageArrayDynamicIndexing,
        shaderClipDistance,
        shaderCullDistance,
        shaderFloat64,
        shaderInt64,
        shaderInt16,
        shaderResourceResidency,
        shaderResourceMinLod,
        sparseBinding,
        sparseResidencyBuffer,
        sparseResidencyImage2D,
        sparseResidencyImage3D,
        sparseResidency2Samples,
        sparseResidency4Samples,
        sparseResidency8Samples,
        sparseResidency16Samples,
        sparseResidencyAliased,
        variableMultisampleRate,
        inheritedQueries
    };

}