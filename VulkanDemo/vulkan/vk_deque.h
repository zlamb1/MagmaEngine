#pragma once

#include <deque>
#include <iostream>

#include "vk_obj.h"

class VulkanDeque {

public:
	VulkanDeque();

	void addWrapper(VulkanWrapper* wrapper);

	void unwrap();

private:
	std::deque<VulkanWrapper*> wrappers{};

};