#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;

out vec4 ourColor;
out vec2 TexCoord;

uniform mat4 model_view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * model_view * vec4(position, 1.0f);
	ourColor = color;
	TexCoord = texCoord;
}