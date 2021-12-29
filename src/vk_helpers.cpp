#include <vulkan/vulkan.h>
#include "vk_types.h"

uint32_t findProperties(const VkPhysicalDeviceMemoryProperties *pMemoryProperties, uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties)
{
    const uint32_t memoryCount = pMemoryProperties->memoryTypeCount;
    for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex) {
        const uint32_t memoryTypeBits       = (1 << memoryIndex);
        const bool     isRequiredMemoryType = memoryTypeBitsRequirement & memoryTypeBits;

        const VkMemoryPropertyFlags properties            = pMemoryProperties->memoryTypes[memoryIndex].propertyFlags;
        const bool                  hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

        if (isRequiredMemoryType && hasRequiredProperties)
            return memoryIndex;
    }

    // failed to find memory type
    return -1;
}

bool allocateBufferMemory(VkDevice device, VkPhysicalDevice gpu, VkBuffer buffer, VkDeviceMemory *memory)
{
    VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryRequirements  buffer_memory_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &buffer_memory_requirements);

    VkPhysicalDeviceMemoryProperties gpu_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &gpu_memory_properties);

    uint32_t memory_type = findProperties(&gpu_memory_properties, buffer_memory_requirements.memoryTypeBits, flags);

    VkMemoryAllocateInfo memory_allocate_info = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, // VkStructureType                        sType
        nullptr, // const void                            *pNext
        buffer_memory_requirements.size, // VkDeviceSize                           allocationSize
        memory_type // uint32_t                               memoryTypeIndex
    };

    if (vkAllocateMemory(device, &memory_allocate_info, nullptr, memory) == VK_SUCCESS)
        return true;
    return false;
}
