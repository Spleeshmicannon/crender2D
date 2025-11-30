/* zlib license
 * Copyright (C) 2025-11-30 09:28:49 J. Benson
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

#ifndef VKH_SWAPCHAIN_H
#define VKH_SWAPCHAIN_H

#include <cplat.h>
#include "vkh_include.h"
#include "device/vkh_device.h"
#include "context/vkh_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    VkExtent2D resolution;
}
VKH_Swapchain;

CP_INLINE bool VKH_createSwapchain(const CP_Window*const window, const VKH_Context*const context, const VKH_Device*const device, VKH_Swapchain*const swapchain)
{
    int width, height;
    CP_getScreenWH(window, &width, &height);

    swapchain->resolution = (VkExtent2D){ (uint32_t)width, (uint32_t)height };
    
    //VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    (void)device;
    (void)context;

    return false;
}


#ifdef __cplusplus
}
#endif

#endif // VKH_SWAPCHAIN_H

