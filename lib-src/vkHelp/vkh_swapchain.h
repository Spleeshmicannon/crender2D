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
    VkSwapchainKHR swapchain;
    VkSwapchainCreateInfoKHR createInfo;
}
VKH_Swapchain;

#define VKH_MIN_IMAGE_COUNT 3
#define VKH_MAX_FRAMES_IN_FLIGHT ((uint32_t)2)
#define VKH_CLAMP(x,min,max) (x < min ? min : (x > max ? max : x))

CP_INLINE bool VKH_calculateExtents(
    const CP_Window*const window, 
    const VKH_Device*const device, 
    VkExtent2D*const extents);

CP_INLINE bool VKH_createSwapchain(
    const CP_Window*const window, 
    const VKH_Context*const context, 
    const VKH_Device*const device, 
    VKH_Swapchain*const swapchain)
{
    if(!VKH_calculateExtents(window, device, &swapchain->resolution))
    {
        return false;
    }
    
    const uint32_t minImageCount = VKH_CLAMP(
        VKH_MIN_IMAGE_COUNT, 
        device->deviceAttributes.surfaceCapabilites.minImageCount, 
        device->deviceAttributes.surfaceCapabilites.maxImageCount);

    swapchain->createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain->createInfo.flags = 0;
    swapchain->createInfo.surface = context->surface;
    swapchain->createInfo.minImageCount = minImageCount;
    swapchain->createInfo.imageFormat = device->deviceAttributes.surfaceFormat.format;
    swapchain->createInfo.imageColorSpace = device->deviceAttributes.surfaceFormat.colorSpace;
    swapchain->createInfo.imageExtent = swapchain->resolution;
    swapchain->createInfo.imageArrayLayers = 1;
    swapchain->createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain->createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO support concurrent if necessary
    swapchain->createInfo.queueFamilyIndexCount = 0;
    swapchain->createInfo.pQueueFamilyIndices = 0;
    swapchain->createInfo.preTransform = device->deviceAttributes.surfaceCapabilites.currentTransform;
    swapchain->createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain->createInfo.presentMode = device->deviceAttributes.presentMode;
    swapchain->createInfo.clipped = VK_TRUE;

    VkResult res = vkCreateSwapchainKHR(device->device, &swapchain->createInfo, VK_NULL_HANDLE, &swapchain->swapchain);

    if(res != VK_TRUE)
    {
        return false;
    }

    return true;
}

CP_INLINE bool VKH_resizeSwapchain(
    VKH_Swapchain*const swapchain, 
    const VKH_Device*const device,
    uint32_t width, 
    uint32_t height)
{
    if(device->deviceAttributes.surfaceCapabilites.currentExtent.width != UINT32_MAX)
    {
        width = device->deviceAttributes.surfaceCapabilites.currentExtent.width;
        height = device->deviceAttributes.surfaceCapabilites.currentExtent.height;
    }
    else
    {
        width = VKH_CLAMP(
            width, 
            device->deviceAttributes.surfaceCapabilites.minImageExtent.width,
            device->deviceAttributes.surfaceCapabilites.maxImageExtent.width
        );

        height = VKH_CLAMP(
            height, 
            device->deviceAttributes.surfaceCapabilites.minImageExtent.height,
            device->deviceAttributes.surfaceCapabilites.maxImageExtent.height
        );
    }

    swapchain->resolution = (VkExtent2D){width, height};
    swapchain->createInfo.imageExtent = swapchain->resolution;
    swapchain->createInfo.oldSwapchain = swapchain->swapchain;

    vkDeviceWaitIdle(device->device); // heavy wait, TODO do smarter pre frame waits

    VkResult res = vkCreateSwapchainKHR(device->device, &swapchain->createInfo, VK_NULL_HANDLE, &swapchain->swapchain);

    swapchain->createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(res != VK_TRUE)
    {
        return false;
    }

    vkDestroySwapchainKHR(device->device, swapchain->swapchain, VK_NULL_HANDLE);

    return true;
}

CP_INLINE bool VKH_calculateExtents(
    const CP_Window*const window, 
    const VKH_Device*const device, 
    VkExtent2D*const extents)
{
    if(device->deviceAttributes.surfaceCapabilites.currentExtent.width != UINT32_MAX)
    {
        *extents = device->deviceAttributes.surfaceCapabilites.currentExtent;
        return true;
    }

    int osWidth, osHeight;
    CP_getScreenWH(window, &osWidth, &osHeight);

    if(osWidth < 0 || osHeight < 0)
    {
        CP_log_error("Failed to retrieve valid screen dimensions, dimensions are: %dx%d", 
                osWidth, osHeight);
        return false;
    }

    const uint32_t width = VKH_CLAMP(
        (uint32_t)osWidth, 
        device->deviceAttributes.surfaceCapabilites.minImageExtent.width,
        device->deviceAttributes.surfaceCapabilites.maxImageExtent.width
    );

    const uint32_t height = VKH_CLAMP(
        (uint32_t)osHeight, 
        device->deviceAttributes.surfaceCapabilites.minImageExtent.height,
        device->deviceAttributes.surfaceCapabilites.maxImageExtent.height
    );

    *extents = (VkExtent2D){width, height};

    return true;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_SWAPCHAIN_H

