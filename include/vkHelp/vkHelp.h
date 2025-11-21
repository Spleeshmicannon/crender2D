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

#ifndef VKHELP_H
#define VKHELP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <vulkan/vulkan.h>

VkInstance createVkInstance(const char*const title)
{
    VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    VkInstance instance = NULL;
    
    // setup Application info
    appInfo.apiVersion = VK_API_VERSION_1_4;
    appInfo.pApplicationName = title;
    appInfo.pEngineName = "CRender2D";
    appInfo.engineVersion = VK_MAKE_VERSION(0,1,0);
    
    // setup instance create info
    createInfo.pApplicationInfo = &appInfo;


    
    // create instance
    vkCreateInstance(&createInfo, NULL, &instance);
    return instance;
}

#ifdef __cplusplus
}
#endif

#endif // VKHELP_H

