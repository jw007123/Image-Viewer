#version 450 core

layout (location = 0) in vec2 inPoints;

out vec2 fPoints;

void main()
{
	fPoints     = inPoints;
	gl_Position = vec4(inPoints.x * 2.0f, inPoints.y * 2.0f, 0.0f, 1.0f);
}
