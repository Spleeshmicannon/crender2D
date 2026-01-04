#include "crender.h"

#define VULKAN_HEAP_SIZE 200 // in bytes

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

    if(!VKH_createDevice(&renderer->context, &renderer->device, &renderer->memArena))
    {
        return CR_ERROR_VULKAN_CALL_FAILED;
    }

    if(!VKH_createSwapchain(window, &renderer->context, &renderer->device, &renderer->swapchain))
    {
        return CR_ERROR_VULKAN_CALL_FAILED;
    }

    return CR_ERROR_SUCCESS;
}

void CR_drawScreen(CR_Renderer*const renderer)
{
    vkWaitForFences(
        renderer->device.device,
        1,
        VK_NULL_HANDLE, // fences in flight
        VK_TRUE,
        UINT64_MAX
    );

    vkResetFences(renderer->device.device, 1, VK_NULL_HANDLE);

    VkResult res = vkAcquireNextImageKHR(
        renderer->device.device,
        renderer->swapchain.swapchain,
        UINT64_MAX,
        VK_NULL_HANDLE, // image available
        VK_NULL_HANDLE,
        VK_NULL_HANDLE // image index
    );

    if(VK_TRUE != res)
    {
        CP_log_error("Failed to acquire next image for swapchain");
        return;
    }
}

CR_ERROR CR_destroyRenderer(CR_Renderer*const renderer)
{
    vkDestroySwapchainKHR(renderer->device.device, renderer->swapchain.swapchain, VK_NULL_HANDLE);
    vkDestroyDevice(renderer->device.device, VK_NULL_HANDLE);
    vkDestroySurfaceKHR(renderer->context.instance, renderer->context.surface, VK_NULL_HANDLE);
    vkDestroyInstance(renderer->context.instance, VK_NULL_HANDLE);
    CR_destoryArena(renderer->memArena);
    return CR_ERROR_SUCCESS;
}
