#include "descriptor.h"

namespace Magma {

    VkResult Descriptor::init() {
        vkDescriptorSetLayoutBinding.binding = pBinding;
        vkDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vkDescriptorSetLayoutBinding.descriptorCount = pCount;
        // which shader stages will use this resource
        vkDescriptorSetLayoutBinding.stageFlags = (VkShaderStageFlags)pStageFlags;
        // relevant for image sampling
        vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr; // optional

        return VK_SUCCESS;
    }

    VkDescriptorSetLayoutBinding& Descriptor::getLayoutBinding() {
        return vkDescriptorSetLayoutBinding;
    }

    // DescriptorSetLayout

    DescriptorSetLayout::~DescriptorSetLayout() {
        if (pVulkanDevice != nullptr) {
            vkDestroyDescriptorSetLayout(pVulkanDevice->getDevice(), vkDescriptorSetLayout,
                pAllocator);
        }
    }

    VkResult DescriptorSetLayout::init() {
        if (pVulkanDevice == nullptr) {
            Z_LOG_TXT("VulkanDescriptorSetLayout::init", "pVulkanDevice is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings{};
        for (auto& descriptor : pDescriptors) {
            descriptorSetLayoutBindings.push_back(descriptor.getLayoutBinding());
        }

        VkDescriptorSetLayoutCreateInfo descriptorSeyLayoutCreateInfo{};
        descriptorSeyLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSeyLayoutCreateInfo.bindingCount = 
            static_cast<uint32_t>(descriptorSetLayoutBindings.size());
        descriptorSeyLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

        auto createDescriptorSeyLayoutResult = vkCreateDescriptorSetLayout(pVulkanDevice->getDevice(),
            &descriptorSeyLayoutCreateInfo, pAllocator, &vkDescriptorSetLayout);
        Z_LOG_OBJ("VulkanDescriptorSetLayout::init::vkCreateDescriptorSetLayout", createDescriptorSeyLayoutResult);

        return createDescriptorSeyLayoutResult;
    }

    VkDescriptorSetLayout& DescriptorSetLayout::getDescriptorSetLayout() {
        return vkDescriptorSetLayout;
    }

    // DescriptorSet

    DescriptorSet::~DescriptorSet() {
        if (pVulkanDevice != nullptr) {
            vkDeviceWaitIdle(pVulkanDevice->getDevice());
            vkDestroyDescriptorPool(pVulkanDevice->getDevice(), vkDescriptorPool, nullptr);
        }
    }

    VkResult DescriptorSet::init() {
        if (pVulkanDevice == nullptr) {
            Z_LOG_TXT("VulkanDescriptorSet::init", "pVulkanDevice is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(pDescriptorSetLayouts.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;

        poolInfo.maxSets = pMaxSets;

        auto createDescriptorPoolResult = vkCreateDescriptorPool(pVulkanDevice->getDevice(),
            &poolInfo, nullptr, &vkDescriptorPool);
        Z_LOG_OBJ("VulkanDescriptorSet::init", createDescriptorPoolResult);

        if (createDescriptorPoolResult != VK_SUCCESS) {
            return createDescriptorPoolResult;
        }

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
        for (auto& descriptorSetLayout : pDescriptorSetLayouts) {
            descriptorSetLayouts.push_back(descriptorSetLayout.getDescriptorSetLayout());
        }

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = vkDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        allocInfo.pSetLayouts = descriptorSetLayouts.data();

        vkDescriptorSets.resize(1);

        auto allocateDescriptorSetsResult = vkAllocateDescriptorSets(pVulkanDevice->getDevice(),
            &allocInfo, vkDescriptorSets.data());
        Z_LOG_OBJ("VulkanDescriptorSet::init::vkAllocateDescriptorSets",
            allocateDescriptorSetsResult);

        if (allocateDescriptorSetsResult != VK_SUCCESS)
            return allocateDescriptorSetsResult;

        for (auto& descriptorSetLayout : pDescriptorSetLayouts) {
            for (auto& descriptor : descriptorSetLayout.pDescriptors) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = descriptor.pBuffer;
                bufferInfo.offset = 0;
                bufferInfo.range = descriptor.pSize;

                VkWriteDescriptorSet vkWriteDescriptorSet{};
                vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                vkWriteDescriptorSet.dstSet = vkDescriptorSets[0];
                vkWriteDescriptorSet.dstBinding = descriptor.pBinding;
                vkWriteDescriptorSet.dstArrayElement = 0;
                vkWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                vkWriteDescriptorSet.descriptorCount = descriptor.pCount;
                vkWriteDescriptorSet.pBufferInfo = &bufferInfo;
                vkWriteDescriptorSet.pImageInfo = nullptr; // optional
                vkWriteDescriptorSet.pTexelBufferView = nullptr; // optional

                vkUpdateDescriptorSets(pVulkanDevice->getDevice(),
                    1, &vkWriteDescriptorSet, 0, nullptr);
            }
        }

        return VK_SUCCESS;
    }

    std::vector<VkDescriptorSet>& DescriptorSet::getDescriptorSets() {
        return vkDescriptorSets;
    }

}