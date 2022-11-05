#version 450 core

layout (location = 0) in vec3 inPoints;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 viewToProj;
uniform mat4 worldToView;
uniform mat4 modelToWorld;

void main()
{
	gl_Position = viewToProj * worldToView * modelToWorld * vec4(inPoints, 1.0f);
	TexCoord	= aTexCoord;
}
