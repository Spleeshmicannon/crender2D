#include <crender.h>
#include <cplat.h>
#include <stdlib.h>

CP_Window createWindow()
{
    CP_Window window;
    const CP_WindowConfig config = {
        .width = 1920,
        .height = 1080,
        .flags = 0,
        .major = 4,
        .minor = 3,
        .windowName = "test"
    };

    CP_ERROR error = CP_createWindow(&window, &config);
    if(error != CP_ERROR_SUCCESS)
    {
        CP_log_fatal("CP failed to create window");
        exit(-1);
    }

    CP_log_info("Created the window!");

    return window;
}

CR_Renderer createRenderer(const CP_Window* const window)
{
    CR_Renderer renderer;
    const CR_RendererConfig config = {
        .flags = 0
    };

    CR_ERROR error = CR_createRenderer(&renderer, &config, window);
    if(error != CR_ERROR_SUCCESS)
    {
        CP_log_fatal("CP failed to create renderer");
        exit(-1);
    }

    CP_log_info("Created the renderer!");

    return renderer;
}

int main()
{
    CP_Window window = createWindow();
    CR_Renderer renderer = createRenderer(&window);
    
    for(CP_WindowEvent event = CP_getNextEvent(&window); // zero initialise
        event.type != CP_EVENT_QUIT; // end loop if user quits
        event = CP_getNextEvent(&window)) // get next event
    {

    }
    
    CR_destroyRenderer(&renderer);
    CP_destroyWindow(&window);
}
