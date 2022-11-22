#version 450 core

in vec2 fPoints;

out vec4 fColours;

uniform vec3 cameraPos;

void main()
{
	const float borderSpacing = 0.01f;
	const float borderCol	  = 0.5f;

	if (abs(fPoints.x) > (0.5f - borderSpacing) || abs(fPoints.y) > (0.5f - borderSpacing))
	{
		fColours	 = vec4(borderCol, borderCol, borderCol, 1.0f);
		gl_FragDepth = 0.0f;
	}
	else
	{
		discard;
	}
}
