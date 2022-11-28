#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 viewToProj;
    mat4 worldToView;
    mat4 modelToWorld;
} uboMats;

void main()
{
	gl_Position	  = uboMats.viewToProj * uboMats.worldToView * uboMats.modelToWorld * vec4(inPosition, 1.0f);
	outTexCoord	  = inTexCoord;
}
