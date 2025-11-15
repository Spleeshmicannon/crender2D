#version 460 core

in vec2 texCoord;
in vec4 texSrcCoord;
in vec3 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;

void main()
{
    vec2 spriteIndex = texSrcCoord.xy;
    vec2 spriteSize = texSrcCoord.zw;
    vec2 spriteStart = spriteIndex; 
    spriteStart *= spriteSize;
    vec2 adjustTexCoord = spriteStart + mod(texCoord, spriteSize);
    vec4 textureColor = texture(texture0, adjustTexCoord);
    finalColor = textureColor;
}
