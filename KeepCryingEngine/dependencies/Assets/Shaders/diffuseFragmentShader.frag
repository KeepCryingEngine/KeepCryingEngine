#version 330 core

in float intensity;
in float specular;
out vec4 color;

uniform sampler2D ourTexture;

void main()
{
	//color = vec4(intensity * vec3(1.0, 1.0, 1.0), 1.0f);
	
	color = vec4(specular * vec3(1.0, 1.0, 1.0), 1.0f);
}