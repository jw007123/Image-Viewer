#version 450

layout(location = 0) in  vec2 inTexCoord;
layout(location = 0) out vec4 outColour;

void main()
{
    outColour = vec4(inTexCoord.x, inTexCoord.y, 0.0f, 1.0f);
}
