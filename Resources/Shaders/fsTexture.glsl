#version 450 core

in vec2 TexCoord;

out vec4 fColours;

uniform sampler2D textureSamp;

void main()
{
	// Use texture colours and set always on top
	fColours	 = texture(textureSamp, TexCoord);
	gl_FragDepth = 0.0f;	
}
