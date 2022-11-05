#version 450 core

in vec2 TexCoord;

out vec4 fColours;

uniform sampler2D textureSamp;

void main()
{
	fColours = texture(textureSamp, TexCoord);
}
