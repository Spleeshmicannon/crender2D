/* zlib license
 * Copyright (C) 2025-11-16 16:18:44 J. Benson
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

#ifndef VKH_SETUP_H
#define VKH_SETUP_H

#include <cplat.h>
#include <string.h>

#include "vkh_include.h"
#include "vkh_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

CP_INLINE VkInstance createVkInstance(
        const char*const title, 
        const char**const extensionNames,
        const uint32_t extensionCount,
        const uint32_t vkApiVersion,
        const uint32_t appVersion,
        const uint32_t engineVersion)
{
    VkApplicationInfo appInfo = { 0 };
    VkInstanceCreateInfo createInfo = { 0 };
    VkInstance instance = VK_NULL_HANDLE;
    
    // setup Application info
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = vkApiVersion;
    appInfo.pApplicationName = title;
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = "CRender2D";
    appInfo.engineVersion = engineVersion;
    
    // setup instance create info
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensionNames;
    createInfo.flags = 0;

#ifdef CRENDER_DEBUG
    const char* layer = "VK_LAYER_KHRONOS_validation";
    createInfo.ppEnabledLayerNames = &layer;
    createInfo.enabledLayerCount = 1;
#endif
    
    // create instance
    VkResult res = vkCreateInstance(&createInfo, NULL, &instance);
    if(VK_SUCCESS != res)
    {
        CP_log_error("Failed to create vulkan instance with error: %s", vkResultToString(res))
        return VK_NULL_HANDLE;
    }

    return instance;
}

CP_INLINE VkDevice findPhysicalDevice(VkInstance instance, CR_Arena*const arena)
{
    uint32_t deviceCount = 0;
    VkResult res = !VK_SUCCESS;
    VkPhysicalDevice* physicalDevice = NULL;
    void* startHead = arena->head;
    void* endDeviceLocation = NULL;

    res = vkEnumeratePhysicalDevices(instance, &deviceCount, VK_NULL_HANDLE);

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
    
    endDeviceLocation = CR_ArenaAllocate(arena, sizeof(VkPhysicalDevice));

    physicalDevice = CR_ArenaAllocate(arena, sizeof(VkPhysicalDevice) * deviceCount);
    res = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevice);

    if(VK_SUCCESS != res)
    {
        arena->head = startHead;
        CP_log_error("Failed to read physical devices with error: %s", vkResultToString(res));
        return VK_NULL_HANDLE;
    }

    bool deviceMeetsRequirements = false;
    uint32_t i = 0;
    for(; i < deviceCount; ++i)
    {
        VkPhysicalDeviceProperties props;
        VkPhysicalDeviceFeatures feats;
        VkPhysicalDeviceMemoryProperties memProps;
        
        vkGetPhysicalDeviceProperties(physicalDevice[i], &props);
        vkGetPhysicalDeviceFeatures(physicalDevice[i], &feats);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice[i], &memProps);

        CP_log_info("Found Device: %s", props.deviceName);
        CP_log_info("          ID: %d", props.deviceID);
        CP_log_info("        Type: %s", vkPhysicalDeviceTypeToString(props.deviceType));
        
        deviceMeetsRequirements = 
            props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
            props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
            props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;

        if(deviceMeetsRequirements)
        {
            break;
        }
    }

    if(deviceMeetsRequirements)
    {
        memcpy(endDeviceLocation, (void*)physicalDevice[i - 1], sizeof(VkPhysicalDevice));
        arena->head = physicalDevice;
        return endDeviceLocation;
    }

    arena->head = startHead;

    return VK_NULL_HANDLE;
}



#ifdef __cplusplus
}
#endif

#endif // VKH_SETUP_H

