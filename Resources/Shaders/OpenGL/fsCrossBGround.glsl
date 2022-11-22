#version 450 core

in vec2 fPoints;

out vec4 fColours;

uniform vec3  cameraPos;
uniform float aspectRatio;

void main()
{
	const float posScale = 0.1f;
	const float xPos	 = fPoints.x + cameraPos.x * posScale;
	const float yPos	 = fPoints.y + cameraPos.y * posScale;

	bool hasColour = false;

	// Larger grid
	{
		const float xSpacing = 0.05f;
		const float ySpacing = xSpacing * aspectRatio;
		const float smallNum = 0.001f;

		// Larger grid
		if (abs(mod(xPos, xSpacing)) < smallNum || abs(mod(yPos, ySpacing)) < smallNum)
		{
			fColours  = vec4(0.5f, 0.5f, 0.5f, 1.0f);
			hasColour = true;
		}
	}
	

	// Smaller grid
	{
		const float xSpacing = 0.01f;
		const float ySpacing = xSpacing * aspectRatio;
		const float smallNum = 0.0005f;

		if (abs(mod(xPos, xSpacing)) < smallNum || abs(mod(yPos, ySpacing)) < smallNum)
		{
			fColours  = vec4(0.25f, 0.25f, 0.25f, 1.0f);
			hasColour = true;
		}
	}
	 
	if (!hasColour)
	{
		discard;
	}
}
