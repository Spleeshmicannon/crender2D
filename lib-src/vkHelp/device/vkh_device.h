/* zlib license
 * Copyright (C) 2025-11-29 09:28:53 J. Benson
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

#ifndef VKH_DEVICE_H
#define VKH_DEVICE_H

#include "../vkh_include.h"
#include "../context/vkh_context.h"
#include "vkh_logical_device.h"
#include "vkh_physical_device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    VkQueue computeQueue;
}
VKH_Device;

CP_INLINE bool createDevice(const VKH_Context*const context, VKH_Device*const device, CR_Arena*const arena)
{
    device->physicalDevice = findPhysicalDevice(context, arena);

    if(VK_NULL_HANDLE == device->physicalDevice)
    {
        CP_log_error("Failed to find physical device");
        return false;
    }

    VKH_DevQueFamIndexes deviceQueueFamilies = checkDeviceQueueFamilies(
        device->physicalDevice, 
        context->surface, 
        arena
    );

    if(!deviceQueueFamilies.graphicsIsValid)
    {
        CP_log_error("Couldn't find a graphics queue");
        return false;
    }

    if(!deviceQueueFamilies.presentIsValid)
    {
        CP_log_error("Couldn't find a present queue");
        return false;
    }

    if(!deviceQueueFamilies.transferIsValid)
    {
        CP_log_error("Couldn't find a transfer queue");
        return false;
    }

    if(!deviceQueueFamilies.computeIndex)
    {
        CP_log_error("Couldn't find a compute queue");
        return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_DEVICE_H

