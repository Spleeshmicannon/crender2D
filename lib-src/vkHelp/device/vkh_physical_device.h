/* zlib license
 * Copyright (C) 2025-11-29 09:36:09 J. Benson
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution. 
*/

#ifndef VKH_PHYSICAL_DEVICE_H
#define VKH_PHYSICAL_DEVICE_H

#include "../vkh_include.h"
#include "../vkh_memory.h"
#include "../context/vkh_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    bool graphicsIsValid, computeIsValid, transferIsValid, presentIsValid;
    uint32_t graphicsIndex, computeIndex, transferIndex, presentIndex;
} 
VKH_DevQueFamIndexes;

CP_INLINE uint32_t findBestPhysicalDevice(VkPhysicalDevice* physicalDevices, uint32_t deviceCount);

CP_INLINE VkPhysicalDevice findPhysicalDevice(
        const VKH_Context*const context,
        CR_Arena*const arena
)
{
    uint32_t deviceCount = 0;
    VkResult res = !VK_SUCCESS;
    VkPhysicalDevice* physicalDevices = NULL;
    void* startHead = arena->head;

    res = vkEnumeratePhysicalDevices(context->instance, &deviceCount, VK_NULL_HANDLE);

    if(VK_SUCCESS != res)
    {
        CP_log_error("Failed to check physical devices with error: %s", vkResultToString(res));
        return VK_NULL_HANDLE;
    }
    else if(0 == deviceCount)
    {
        CP_log_error("No compatable physical devices found!");
        return VK_NULL_HANDLE;
    }
    
    physicalDevices = CR_ArenaAllocate(arena, sizeof(VkPhysicalDevice) * deviceCount);
    res = vkEnumeratePhysicalDevices(context->instance, &deviceCount, physicalDevices);

    if(VK_SUCCESS != res)
    {
        arena->head = startHead;
        CP_log_error("Failed to read physical devices with error: %s", vkResultToString(res));
        return VK_NULL_HANDLE;
    }

    uint32_t bestDeviceIndex = findBestPhysicalDevice(physicalDevices, deviceCount);
    
    return physicalDevices[bestDeviceIndex];
}

CP_INLINE uint32_t findBestPhysicalDevice(VkPhysicalDevice* physicalDevices, uint32_t deviceCount)
{
    uint32_t discreteGPUIndex = 0;
    uint32_t integratedGPUIndex = 0;
    bool foundDiscreteGPU = false;

    for(uint32_t i = 0; i < deviceCount; ++i)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

        CP_log_info("Found Device: %s", props.deviceName);
        CP_log_info("          ID: %d", props.deviceID);
        CP_log_info("        Type: %s", vkPhysicalDeviceTypeToString(props.deviceType));
        
        if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            foundDiscreteGPU = true;
            discreteGPUIndex = i;
        }
        else if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            integratedGPUIndex = i;
        }
    }

    if(foundDiscreteGPU)
    {
        return discreteGPUIndex;
    }

    return integratedGPUIndex;
}

CP_INLINE VKH_DevQueFamIndexes checkDeviceQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, CR_Arena*const arena)
{
    uint32_t queFamCount;
    VkQueueFamilyProperties* queFamProps;
    VKH_DevQueFamIndexes devQueFamIndexes = { 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queFamCount, VK_NULL_HANDLE);

    queFamProps = CR_ArenaAllocate(arena, sizeof(VkQueueFamilyProperties) * queFamCount);

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queFamCount, queFamProps);
    
    uint8_t minTransScore = 255;

    for(uint32_t i = 0; i < queFamCount; ++i)
    {
        uint8_t transScore = 0;

        CP_log_info("----- Queue Index %d ------", i)

        if(queFamProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            ++transScore;
            devQueFamIndexes.graphicsIndex = i;
            devQueFamIndexes.graphicsIsValid = true;
            CP_log_info("Graphics");
        }

        if(queFamProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            ++transScore;
            devQueFamIndexes.computeIndex = i;
            devQueFamIndexes.computeIsValid = true;
            CP_log_info("Compute");
        }

        if((queFamProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && transScore <= minTransScore)
        {
            minTransScore = transScore;
            devQueFamIndexes.transferIndex = i;
            devQueFamIndexes.transferIsValid = true;
            CP_log_info("Transfer");
        }

        VkBool32 supportsPresent = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent);
        if(supportsPresent)
        {
            devQueFamIndexes.presentIndex = i;
            devQueFamIndexes.presentIsValid = true;
            CP_log_info("Present");
        }

        CP_log_info("----------------------")
    }

    return devQueFamIndexes;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_PHYSICAL_DEVICE_H

