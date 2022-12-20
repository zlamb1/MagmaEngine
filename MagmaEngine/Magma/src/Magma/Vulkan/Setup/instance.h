#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "vulkan_object.h"
#include "Magma/Vulkan/Setup/validater.h"

namespace Magma {

	class Instance : public VulkanObject {

	public:
		Instance(std::shared_ptr<Validater> validater);
		~Instance() override;

		std::vector<const char*> m_RequiredExtensions{};

		VkResult init() override;

		VkInstance& getInstance();

	public:
		VkDebugUtilsMessengerCreateInfoEXT m_DebugCreateInfo{};
		std::shared_ptr<Validater> m_Validater = nullptr;

	private:
		VkInstance m_Instance{};

	};

}

