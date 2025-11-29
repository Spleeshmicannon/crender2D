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

bool createLogicalDevice(
    VkDevice*const device, 
    const VKH_Context*const context, 
    const VKH_DevQueFamIndexes*const devQueFamIndexes,
    CR_Arena*const arena)
{
    uint32_t index = 0;
    uint32_t* indices = CR_ArenaAllocate(arena, 
        sizeof(uint32_t) * 
        (
            (1 & devQueFamIndexes->computeIsValid) +
            (1 & devQueFamIndexes->graphicsIsValid) +
            (1 & devQueFamIndexes->transferIsValid) +
            (1 & devQueFamIndexes->presentIsValid)
        )
    );

    if(devQueFamIndexes->graphicsIsValid)
    {
        indices[index++] = devQueFamIndexes->graphicsIndex;
    }

    if(devQueFamIndexes->presentIsValid)
    {
        indices[index++] = devQueFamIndexes->presentIndex;
    }

    if(devQueFamIndexes->transferIsValid)
    {
        indices[index++] = devQueFamIndexes->transferIndex;
    }

    if(devQueFamIndexes->computeIsValid)
    {
        indices[index++] = devQueFamIndexes->computeIndex;
    }

    VkDeviceQueueCreateInfo* queCreateInfos = CR_ArenaAllocate(arena, 
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

    return false;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_LOGICAL_DEVICE_H

