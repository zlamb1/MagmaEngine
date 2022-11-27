#include "device_enums.h"

// QueueFamily

uint32_t QueueFamily::getQueue() {
    return queue.value();
}

uint32_t QueueFamily::getPresentation() {
    return presentation.value();
}

void QueueFamily::setQueue() {
    this->presentation = {};
}

void QueueFamily::setQueue(uint32_t queue) {
    this->queue = queue;
}

void QueueFamily::setPresentation() {
    this->presentation = {};
}

void QueueFamily::setPresentation(uint32_t presentation) {
    this->presentation = presentation;
}

bool QueueFamily::hasQueue() {
    return queue.has_value();
}

bool QueueFamily::hasPresentation() {
    return presentation.has_value();
}

// VulkanQueueType

VulkanQueueType operator|(VulkanQueueType& a, VulkanQueueType& b) {
    return (VulkanQueueType)((uint32_t)a | (uint32_t)b);
}