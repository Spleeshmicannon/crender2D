/* zlib license
 * Copyright (C) 2025-11-20 18:34:10 J. Benson
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

#ifndef VKH_SETUP_WIN32_H
#define VKH_SETUP_WIN32_H

#include <cplat.h>
#include "vkh_include.h"

#ifdef __cplusplus
extern "C" {
#endif

CP_INLINE VkSurfaceKHR createSurface(const VkInstance instance, const CP_Window*const window)
{
    VkWin32SurfaceCreateInfoKHR createInfo = { 0 };
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = window->hwnd;
    createInfo.hinstance = window->hinst;

    VkSurfaceKHR surface;

    VkResult res = vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface);
    if(VK_SUCCESS != res)
    {
        CP_log_error("Failed to create surface with error: %s", vkResultToString(res));
        return VK_NULL_HANDLE;
    }

    return surface;
}

#ifdef __cplusplus
}
#endif

#endif // VKH_SETUP_WIN32_H

