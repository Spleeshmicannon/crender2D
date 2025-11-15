#version 430 core
layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform float aspectRatio;

in unsigned int gTexture[];
in vec2 gTexPos[];
in vec2 gTexSize[];
in vec2 gPos[];
in vec2 gSize[];

out vec2 texCoord;
out vec3 fragColor;
out vec4 texSrcCoord;

void main() {

    const vec4 center = gl_in[0].gl_Position;
    vec2 pointSize = gSize[0];
    const vec2 texPos = gTexPos[0];
    const vec2 texSize = gTexSize[0];
    const unsigned int texture = gTexture[0];
    const vec3 color = vec3(1,1,1);
    
    pointSize.x *= aspectRatio;

    // First triangle
    texCoord = vec2(0, 0);
    texSrcCoord = vec4(texPos, texSize);
    fragColor = color;
    gl_Position = center + vec4(-pointSize.x, -pointSize.y, 0.0, 0.0); // Bottom-left
    EmitVertex();
    
    texCoord = vec2(texSize.x, 0);
    texSrcCoord = vec4(texPos, texSize);
    fragColor = color;
    gl_Position = center + vec4(pointSize.x, -pointSize.y, 0.0, 0.0);  // Bottom-right
    EmitVertex();
    
    texCoord = vec2(0, texSize.y);
    texSrcCoord = vec4(texPos, texSize);
    fragColor = color;
    gl_Position = center + vec4(-pointSize.x, pointSize.y, 0.0, 0.0);  // Top-left
    EmitVertex();
    EndPrimitive();

    // Second triangle
    texCoord = vec2(texSize.x, 0);
    texSrcCoord = vec4(texPos, texSize);
    fragColor = color;
    gl_Position = center + vec4(pointSize.x, -pointSize.y, 0.0, 0.0);  // Bottom-right
    EmitVertex();
    
    texCoord = vec2(texSize.x, texSize.y);
    texSrcCoord = vec4(texPos, texSize);
    fragColor = color;
    gl_Position = center + vec4(pointSize.x, pointSize.y, 0.0, 0.0);   // Top-right
    EmitVertex();
    
    texCoord = vec2(0, texSize.y);
    texSrcCoord = vec4(texPos, texSize);
    fragColor = color;
    gl_Position = center + vec4(-pointSize.x, pointSize.y, 0.0, 0.0);  // Top-left (repeated)
    EmitVertex();
    EndPrimitive();
}
