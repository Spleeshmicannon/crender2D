cmake_minimum_required(VERSION 3.22)
Project(crender2D_project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib-src/*.c"
)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib-src")
set(DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE HEADER_FILES RELATIVE 
    "${SOURCE_DIR}" "${SOURCE_DIR}/*.h"
)

foreach(header IN LISTS HEADER_FILES)
    set(src "${SOURCE_DIR}/${header}")
    set(dst "${DEST_DIR}/${header}")
    get_filename_component(dst_dir "${dst}" DIRECTORY)
    file(MAKE_DIRECTORY "${dst_dir}")
    file(COPY "${src}" DESTINATION "${dst_dir}")
endforeach()

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -static")
endif()

add_subdirectory("deps/cplat")

add_library(crender2D STATIC ${LIB_SOURCES})

if(MSVC)
    target_compile_options(crender2D PRIVATE /W4 /WX /O2)
else()
    target_compile_options(crender2D PRIVATE
        -Wall 
        -Wextra 
        -Werror 
        -Wstrict-overflow=3 
        -Wimplicit 
        -Wconversion 
        -Wformat 
        -O2
        -std=c23
    )
endif()

target_include_directories(crender2D PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/cplat/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/Vulkan-Headers/include"
)

if(WIN32)
    if(DEFINED ENV{VULKAN_HOME})
        set(VULKAN_HOME $ENV{VULKAN_HOME})
        message(STATUS "Using vulkan sdk: ${VULKAN_HOME}")
        target_link_libraries(crender2D PRIVATE cplat "${VULKAN_HOME}/Lib/vulkan-1.lib")
    else()
        message(FATAL_ERROR "Required environment variable: VULKAN_HOME")
    endif()
else()
    target_compile_definitions(crender2D PRIVATE CRENDER_DEBUG)
    target_link_libraries(crender2D PRIVATE cplat vulkan)
endif()

target_link_libraries(cplat PUBLIC)

# Crender2D test executable
add_executable(crenderTest ${SOURCES})
target_include_directories(crenderTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/cplat/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(crenderTest PRIVATE crender2D)

