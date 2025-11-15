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
#include "crender.h"

#include <glad/gl.h>

#include <cplat.h>

#include <string.h>

static const char VERTEX_SHADER[] = {
#embed "shaders/vertex.glsl"
,'\0'
};

static const char GEOMETRY_SHADER[] = {
#embed "shaders/geometry.glsl"
,'\0'
};

static const char FRAGMENT_SHADER[] = {
#embed "shaders/fragment.glsl"
,'\0'
};

bool compile_shaders(CR_Renderer*const renderer);


CR_ERROR CR_createRenderer(CR_Renderer*const renderer, const CR_RendererConfig*const config)
{
    (void)renderer;
    (void)config;

    if(!compile_shaders(renderer))
    {
        return CR_ERROR_SHADER_COMPILATION_FAILED;
    }

    return CR_ERROR_SUCCESS;
}

CR_ERROR CR_destroyRenderer(CR_Renderer*const renderer)
{
    (void)renderer;
    return CR_ERROR_SUCCESS;
}

static bool compile_shader(const char* buffer, GLuint *shader, GLuint type)
{
    *shader = glCreateShader(type);
    
    size_t size = strlen(buffer);
    const char* shaders[1] = { buffer };

    glShaderSource(*shader, 1, shaders, (const GLint*)&size);
    glCompileShader(*shader);

    GLint success = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

    if(success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logSize);

        char* errorLog = (char*)CP_allocate((size_t)logSize * sizeof(char));
        glGetShaderInfoLog(*shader, logSize, &logSize, errorLog);

        CP_log_error("Failed to comile shader with error: %s\n", errorLog);

        CP_free(errorLog);

        return false;
    }

    return true;
}

bool compile_shaders(CR_Renderer*const renderer)
{
    GLuint vertexId, fragmentId, geometryId;

    bool shader_compile_status = true;

    CP_log_info("Compiling vertex shader");
    shader_compile_status &= compile_shader(VERTEX_SHADER, &vertexId, GL_VERTEX_SHADER);
    CP_log_info("Compiling fragment shader");
    shader_compile_status &= compile_shader(FRAGMENT_SHADER, &fragmentId, GL_FRAGMENT_SHADER);
    CP_log_info("Compiling geometry shader");
    shader_compile_status &= compile_shader(GEOMETRY_SHADER, &geometryId, GL_GEOMETRY_SHADER);

    if(shader_compile_status != true)
    {
        CP_log_error("Failed to compile shaders");
        return false;
    }

    renderer->shader_program = glCreateProgram();

    glAttachShader(renderer->shader_program, vertexId);
    glAttachShader(renderer->shader_program, fragmentId);
    glAttachShader(renderer->shader_program, geometryId);

    glLinkProgram(renderer->shader_program);

    GLint isLinked = 0;
    glGetProgramiv(renderer->shader_program, GL_LINK_STATUS, (int*)&isLinked);

    if (isLinked == GL_FALSE)
    {
        GLint logSize = 0;
        glGetProgramiv(renderer->shader_program, GL_INFO_LOG_LENGTH, &logSize);

        char* errorLog = (char*)CP_allocate((size_t)logSize * sizeof(char));
        glGetProgramInfoLog(renderer->shader_program, logSize, &logSize, errorLog);

        CP_log_error("Failed to link graphics shader program with error: %s\n", errorLog);

        glDeleteProgram(renderer->shader_program);
        CP_free(errorLog);
        return false;
    }

    return true;
}
