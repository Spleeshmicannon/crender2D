/* zlib license
 * Copyright (C) 2025-11-29 09:34:31 J. Benson
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

#ifndef VKH_LOGICAL_DEVICE_H
#define VKH_LOGICAL_DEVICE_H

#include "../vkh_include.h"
#include "vkh_physical_device.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char* VKH_SWAPCHAIN_EXTENSION_NAME = "VK_KHR_swapchain";

static CP_INLINE bool VKH_createLogicalDevice(
    VkDevice*const device,
    const VkPhysicalDevice physicalDevice,
    const VKH_DevQueFamIndexes*const devQueFamIndexes,
    CR_Arena*const arena)
{
    void* oldHead = arena->head;
    uint32_t index = 0;
    uint32_t indices[4] = { 0 };

    if(devQueFamIndexes->graphicsIsValid)
    {
        indices[index++] = devQueFamIndexes->graphicsIndex;
    }

    if(devQueFamIndexes->presentIsValid)
    {
        if(devQueFamIndexes->presentIndex != indices[0])
        {
            indices[index++] = devQueFamIndexes->presentIndex;
        }
    }

    if(devQueFamIndexes->transferIsValid)
    {
        bool duplicate = false;
        for(uint32_t i = 0; i < index; ++i)
        {
            duplicate = duplicate || (devQueFamIndexes->transferIndex == indices[i]);
        }

        if(!duplicate)
        {
            indices[index++] = devQueFamIndexes->transferIndex;
        }
    }

    if(devQueFamIndexes->computeIsValid)
    {
        bool duplicate = false;
        for(uint32_t i = 0; i < index; ++i)
        {
            duplicate = duplicate || (devQueFamIndexes->computeIndex == indices[i]);
        }

        if(!duplicate)
        {
            indices[index++] = devQueFamIndexes->computeIndex;
        }
    }
    
    VkDeviceQueueCreateInfo* queCreateInfos = CR_arenaAllocate(arena, 
            sizeof(VkDeviceQueueCreateInfo) * index); // for each que index
    
    float quePriority = 1.0f;
    for(uint32_t i = 0; i < index; ++i)
    {
        queCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queCreateInfos[i].queueFamilyIndex = indices[i];
        queCreateInfos[i].queueCount = 1;

        if(indices[i] == devQueFamIndexes->graphicsIndex)
        {
            queCreateInfos[i].queueCount = 2;
        }

        queCreateInfos[i].flags = 0;
        queCreateInfos[i].pNext = 0;

        queCreateInfos[i].pQueuePriorities = &quePriority;
    }

    VkPhysicalDeviceFeatures deviceFeat = { 0 };
    deviceFeat.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo = { 0 };
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = index;
    deviceCreateInfo.pQueueCreateInfos = queCreateInfos;
    deviceCreateInfo.pEnabledFeatures = &deviceFeat;
    deviceCreateInfo.enabledExtensionCount = 1;

    const char* exts[] = { VKH_SWAPCHAIN_EXTENSION_NAME };
    deviceCreateInfo.ppEnabledExtensionNames = exts;

    VkResult res = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, device);

    if(VK_SUCCESS != res)
    {
        CP_log_error("Failed to create vulkan device with error: %s", vkResultToString(res));
        arena->head = oldHead;
        return false;
    }

    arena->head = oldHead;
    return true;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_LOGICAL_DEVICE_H

