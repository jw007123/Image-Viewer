#version 450 core

in vec2 TexCoord;

out vec4 fColours;

uniform sampler2D textureSamp;
uniform bool	  onTop;

void main()
{
	fColours = texture(textureSamp, TexCoord);

	if (onTop)
	{
		gl_FragDepth = 0.0f;	
	}
	else
	{
		gl_FragDepth = 0.1f;	
	}
}
