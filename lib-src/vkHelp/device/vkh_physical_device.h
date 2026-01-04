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

#define VKH_PRESENT_MODE VK_PRESENT_MODE_FIFO_KHR

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    bool graphicsIsValid, computeIsValid, transferIsValid, presentIsValid;
    uint32_t graphicsIndex, computeIndex, transferIndex, presentIndex;
} 
VKH_DevQueFamIndexes;

typedef struct
{
    VkSurfaceFormatKHR surfaceFormat;
    VkSurfaceCapabilitiesKHR surfaceCapabilites;
    VkPresentModeKHR presentMode;
}
VKH_DeviceAttributes;

CP_INLINE uint32_t VKH_findBestPhysicalDevice(
    VkPhysicalDevice* physicalDevices, 
    uint32_t deviceCount,
    const VkSurfaceKHR surface,
    VKH_DeviceAttributes*const deviceAttributes,
    CR_Arena*const arena);

CP_INLINE bool VKH_QueryDeviceSwapchainSupport(
    const VkPhysicalDevice device, 
    const VkSurfaceKHR surface,
    VKH_DeviceAttributes*const deviceAttributes,
    CR_Arena*const arena);

CP_INLINE VkPhysicalDevice VKH_findPhysicalDevice(
        const VKH_Context*const context,
        VKH_DeviceAttributes*const deviceAttributes,
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
    
    physicalDevices = CR_arenaAllocate(arena, sizeof(VkPhysicalDevice) * deviceCount);
    res = vkEnumeratePhysicalDevices(context->instance, &deviceCount, physicalDevices);

    if(VK_SUCCESS != res)
    {
        arena->head = startHead;
        CP_log_error("Failed to read physical devices with error: %s", vkResultToString(res));
        return VK_NULL_HANDLE;
    }

    uint32_t bestDeviceIndex = VKH_findBestPhysicalDevice(
        physicalDevices, 
        deviceCount,
        context->surface,
        deviceAttributes,
        arena
    );

    arena->head = startHead;

    if(bestDeviceIndex == UINT32_MAX)
    {
        return VK_NULL_HANDLE;
    }
    
    return physicalDevices[bestDeviceIndex];
}

CP_INLINE uint32_t VKH_findBestPhysicalDevice(
    VkPhysicalDevice* physicalDevices, 
    uint32_t deviceCount,
    const VkSurfaceKHR surface,
    VKH_DeviceAttributes*const deviceAttributes,
    CR_Arena*const arena)
{
    uint32_t discreteGPUIndex = 0;
    uint32_t integratedGPUIndex = 0;
    bool foundDiscreteGPU = false;
    bool foundIntegratedGPU = false;
    VKH_DeviceAttributes tmpDeviceAttributes = {};

    for(uint32_t i = 0; i < deviceCount; ++i)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

        CP_log_info("Found Device: %s", props.deviceName);
        CP_log_info("          ID: %d", props.deviceID);
        CP_log_info("        Type: %s", vkPhysicalDeviceTypeToString(props.deviceType));
        
        if((props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) && 
            VKH_QueryDeviceSwapchainSupport(physicalDevices[i], surface, &tmpDeviceAttributes, arena))
        {
            foundDiscreteGPU = true;
            discreteGPUIndex = i;
            break;
        }
        else if((props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) && 
            VKH_QueryDeviceSwapchainSupport(physicalDevices[i], surface, &tmpDeviceAttributes, arena))
        {
            foundIntegratedGPU = true;
            integratedGPUIndex = i;
        }
    }

    if(!(foundIntegratedGPU || foundDiscreteGPU))
    {
        CP_log_error("Failed find a suitable GPU, quitting");
        return UINT32_MAX;
    }

    *deviceAttributes = tmpDeviceAttributes;

    if(foundDiscreteGPU)
    {
        return discreteGPUIndex;
    }
    
    return integratedGPUIndex;
}

CP_INLINE VKH_DevQueFamIndexes VKH_checkDeviceQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, CR_Arena*const arena)
{
    uint32_t queFamCount;
    void* oldHead = arena->head;
    VkQueueFamilyProperties* queFamProps;
    VKH_DevQueFamIndexes devQueFamIndexes = { 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queFamCount, VK_NULL_HANDLE);

    queFamProps = CR_arenaAllocate(arena, sizeof(VkQueueFamilyProperties) * queFamCount);

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
        (void)vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent);
        if(supportsPresent)
        {
            devQueFamIndexes.presentIndex = i;
            devQueFamIndexes.presentIsValid = true;
            CP_log_info("Present");
        }

        CP_log_info("----------------------")
    }

    arena->head = oldHead;

    return devQueFamIndexes;
}

CP_INLINE bool VKH_QueryDeviceSwapchainSupport(
    const VkPhysicalDevice device, 
    const VkSurfaceKHR surface,
    VKH_DeviceAttributes*const deviceAttributes,
    CR_Arena*const arena)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkSurfaceFormatKHR* surfaceFormats;
    VkPresentModeKHR* presentModes;
    void*const oldHead = arena->head;
    uint32_t surfaceFormatCount = 0, presentModeCount = 0;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
    
    CP_log_info("Device supports max image extent of %dx%d", 
        surfaceCapabilities.maxImageExtent.width,
        surfaceCapabilities.maxImageExtent.height);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount,VK_NULL_HANDLE);

    surfaceFormats = CR_arenaAllocate(arena, sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, surfaceFormats);
    
    bool foundValidSurfaceFormat = false;
    for(uint32_t i = 0; i < surfaceFormatCount; ++i)
    {
        // Non-linear format, may be an issue for some integrated graphics but should
        // be fine for most cases. TODO -> Revisit this decision
        if((surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB ||
            surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB) &&
            surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            deviceAttributes->surfaceFormat = surfaceFormats[i];
            foundValidSurfaceFormat = true;
            break;
        }
    }

    if(!foundValidSurfaceFormat)
    {
        arena->head = oldHead;
        return false;
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, VK_NULL_HANDLE);

    presentModes = CR_arenaAllocate(arena, sizeof(VkPresentModeKHR) * presentModeCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes);
    
    bool foundValidPresentMode = false;
    for(uint32_t i = 0; i < presentModeCount; ++i)
    {
        if(presentModes[i] == VKH_PRESENT_MODE)
        {
            deviceAttributes->presentMode = presentModes[i];
            foundValidPresentMode = true;
            break;
        }
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &deviceAttributes->surfaceCapabilites);

    arena->head = oldHead;

    return foundValidSurfaceFormat && foundValidPresentMode;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_PHYSICAL_DEVICE_H

