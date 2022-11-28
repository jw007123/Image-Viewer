#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColour;

void main()
{
    gl_Position = vec4(inPosition, 1.0f);
    fragColour  = vec3(inTexCoord, 1.0f);
}
