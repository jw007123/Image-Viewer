#version 450 core

in vec2 TexCoord;

out vec4 fColours;

uniform sampler2D textureSamp;

void main()
{
	gl_FragDepth = 0.0f;
	fColours	 = texture(textureSamp, TexCoord);
}
