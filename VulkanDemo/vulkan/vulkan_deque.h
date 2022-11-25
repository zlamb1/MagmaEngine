#pragma once

#include <deque>
#include <iostream>

#include "vulkan_object.h"

class VulkanDeque {

public:
	VulkanDeque() = default;

	void addObject(VulkanObject* object);

	void unwrap();

private:
	std::deque<VulkanObject*> objects{};

};