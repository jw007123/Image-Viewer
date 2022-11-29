#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 viewToProj;
    mat4 worldToView;
} uboMats;

layout(push_constant) uniform PushConstants
{
    mat4 modelToWorld;
} pcsMats;

void main()
{
	gl_Position	  = uboMats.viewToProj * uboMats.worldToView * pcsMats.modelToWorld * vec4(inPosition, 1.0f);
	outTexCoord	  = inTexCoord;
}
