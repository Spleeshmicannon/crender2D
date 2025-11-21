#include "crender.h"
#include "vkHelp/vkh_setup.h"

#ifdef CP_LINUX
#include "vkHelp/vkh_setup_linux.h"
#elif defined(CP_WIN32)
#include "vkHelp/vkh_setup_win32.h"
#endif

CR_ERROR CR_createRenderer(CR_Renderer*const renderer, const CR_RendererConfig*const config, const CP_Window*const window)
{
    (void)config;
    
    bool success;
    renderer->memArena = CR_createArena(&success);

    if(!success)
    {
        return CR_ERROR_ALLOC_FAILED;
    }

#ifdef CP_LINUX
#define EXTENSION_COUNT 2
    const char* extensions[EXTENSION_COUNT] = {
        "VK_KHR_surface",
        "VK_KHR_xcb_surface",
    };
#elif defined(CP_WIN32)
#define EXTENSION_COUNT 2
    const char* extensions[EXTENSION_COUNT] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };
#endif

    renderer->instance = createVkInstance(
        "CRenderGame",
        extensions,
        EXTENSION_COUNT,
        VK_API_VERSION_1_3,
        VK_MAKE_VERSION(0,1,0), // app version
        VK_MAKE_VERSION(0,1,0)  // engine version
    );

    if(VK_NULL_HANDLE == renderer->instance)
    {
        return CR_ERROR_VULKAN_CALL_FAILED;
    }

    renderer->surface = createSurface(renderer->instance, window);

    if(VK_NULL_HANDLE == renderer->surface)
    {
        return CR_ERROR_VULKAN_CALL_FAILED;
    }

    return CR_ERROR_SUCCESS;
}

CR_ERROR CR_destroyRenderer(CR_Renderer*const renderer)
{
    vkDestroyInstance(renderer->instance, NULL);
    CR_destoryArena(renderer->memArena);
    return CR_ERROR_SUCCESS;
}
