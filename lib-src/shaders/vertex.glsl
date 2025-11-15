#version 430 core
layout(location = 0) in unsigned int aTexture;
layout(location = 1) in vec2 aTexPos;
layout(location = 2) in vec2 aTexSize;
layout(location = 3) in vec2 aPos;
layout(location = 4) in vec2 aSize;

out unsigned int gTexture;
out vec2 gTexPos;
out vec2 gTexSize;
out vec2 gPos;
out vec2 gSize;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0f, 1.0f);
    gTexture = aTexture;
    gTexPos = aTexPos;
    gTexSize = aTexSize;
    gPos = aPos;
    gSize = aSize;
}
