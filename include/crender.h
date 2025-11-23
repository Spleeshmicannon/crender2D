/* zlib license
 * Copyright (C) 2025 J. Benson
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

#ifndef CRENDER2D_H
#define CRENDER2D_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <cplat.h>

#include "vkHelp/vkh_include.h"
#include "vkHelp/vkh_memory.h"

typedef enum
{
    CR_ERROR_SUCCESS = 0,
    CR_ERROR_SHADER_COMPILATION_FAILED = 1,
    CR_ERROR_VULKAN_CALL_FAILED = 2,
    CR_ERROR_ALLOC_FAILED = 3
}
CR_ERROR;

typedef enum
{
    CR_RENDER_NO_FLAGS = 0x00
}
CR_RENDER_FLAGS;

typedef struct
{
    uint8_t flags;
}
CR_RendererConfig;

typedef struct
{
    VkInstance instance;
    VkSurfaceKHR surface;
    CR_Arena memArena;
}
CR_Renderer;

CR_ERROR CR_createRenderer(CR_Renderer*const renderer, const CR_RendererConfig*const config, const CP_Window*const window);

CR_ERROR CR_destroyRenderer(CR_Renderer*const renderer);

#ifdef __cplusplus
}
#endif

#endif // CRENDER2D_H
