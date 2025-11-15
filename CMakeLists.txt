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

file(GLOB_RECURSE GLAD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/src/*.c"
)
add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include" 
)

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

target_link_libraries(crender2D PRIVATE glad cplat)
target_include_directories(crender2D PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include" 
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/cplat/include"
)

if(WIN32)
    target_link_libraries(crender2D PUBLIC opengl32)
else()
    target_link_libraries(crender2D PUBLIC GL)
endif()

target_link_libraries(cplat PUBLIC)

# Crender2D test executable
add_executable(crenderTest ${SOURCES})
target_include_directories(crenderTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/cplat/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(crenderTest PRIVATE crender2D)

