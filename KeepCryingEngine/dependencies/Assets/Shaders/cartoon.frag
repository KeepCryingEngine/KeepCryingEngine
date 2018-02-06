#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

uniform vec3 lightDir;
uniform sampler2D ourTexture;

void main()
{
float intensity;
	intensity = dot(Normal,lightDir);

	if (intensity > 0.90)
		color = vec4(1.0,1.0,1.0,1.0);
	else if (intensity > 0.75)
		color = vec4(0.8,0.8,0.8,1.0);
	else if (intensity > 0.55)
		color = vec4(0.6,0.6,0.6,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.4,0.4,1.0);
	else
		color = vec4(0.2,0.2,0.2,1.0);

	color = color * texture2D(ourTexture,TexCoord);
}