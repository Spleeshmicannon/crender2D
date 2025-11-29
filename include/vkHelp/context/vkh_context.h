/* zlib license
 * Copyright (C) 2025-11-24 20:22:28 J. Benson
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

#ifndef VKH_CONTEXT_H
#define VKH_CONTEXT_H

#include <cplat.h>
#include <string.h>

#include "../vkh_include.h"

#ifdef CP_LINUX
#include "vkh_setup_linux.h"
#elif defined(CP_WIN32)
#include "vkh_setup_win32.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    VkInstance instance;
    VkSurfaceKHR surface;
} 
VKH_Context;

typedef struct
{
    const char* title;
    const char**const extensionNames;
    uint32_t extensionCount;
    uint32_t vkApiVersion;
    uint32_t appVersion;
    uint32_t engineVersion;
    const CP_Window*const window;
}
VKH_ContextConfig;

CP_INLINE VkInstance VKH_createVkInstance(
        const char*const title, 
        const char**const extensionNames,
        const uint32_t extensionCount,
        const uint32_t vkApiVersion,
        const uint32_t appVersion,
        const uint32_t engineVersion);


CP_INLINE bool VKH_createVkhContext(const VKH_ContextConfig*const config, VKH_Context *const context)
{
    context->instance = VKH_createVkInstance(
        config->title, 
        config->extensionNames, 
        config->extensionCount, 
        config->vkApiVersion, 
        config->appVersion, 
        config->engineVersion
    );

    if(VK_NULL_HANDLE == context->instance)
    {
        return false;
    }

    context->surface = VKH_createSurface(context->instance, config->window);

    return VK_NULL_HANDLE != context->surface;
}

CP_INLINE VkInstance VKH_createVkInstance(
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



#ifdef __cplusplus
}
#endif

#endif // VKH_CONTEXT_H

