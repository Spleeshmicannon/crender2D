#include "crender.h"

#define VULKAN_HEAP_SIZE 10000 // 10kb

CR_ERROR CR_createRenderer(CR_Renderer*const renderer, const CR_RendererConfig*const config, const CP_Window*const window)
{
    (void)config;
    (void)window;
    (void)renderer;
    
    if(!CR_createArena(&renderer->memArena, VULKAN_HEAP_SIZE))
    {
        CP_log_error("failed to create a memory arena");
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

    const VKH_ContextConfig contextConfig = {
        .title = "CRenderGame",
        .extensionNames = extensions,
        .extensionCount = EXTENSION_COUNT,
        .vkApiVersion = VK_API_VERSION_1_3,
        .appVersion = VK_MAKE_VERSION(0,1,0),
        .engineVersion = VK_MAKE_VERSION(0,1,0),
        .window = window
    };

    if(!VKH_createVkhContext(&contextConfig, &renderer->context))
    {
        return CR_ERROR_VULKAN_CALL_FAILED;
    }

    if(!createDevice(&renderer->context, &renderer->device, &renderer->memArena))
    {
        return CR_ERROR_VULKAN_CALL_FAILED;
    }

    CP_log_info("Using %f%% of memory allocation", 
        (float)(renderer->memArena.head - renderer->memArena.arenaStart) / 
        (float)renderer->memArena.arenaSize * 100.0
    );

    return CR_ERROR_SUCCESS;
}

CR_ERROR CR_destroyRenderer(CR_Renderer*const renderer)
{
    vkDestroySurfaceKHR(renderer->context.instance, renderer->context.surface, NULL);
    vkDestroyInstance(renderer->context.instance, NULL);
    CR_destoryArena(renderer->memArena);
    return CR_ERROR_SUCCESS;
}
