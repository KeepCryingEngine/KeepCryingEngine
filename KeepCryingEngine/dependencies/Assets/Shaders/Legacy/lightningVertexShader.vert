#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;

uniform mat4 projection;
uniform mat4 model_view;
uniform mat4 transform;

out vec2 TexCoord;
out vec3 vertexPosition;
out vec3 vertexNormal;

void main()
{	
	vertexPosition = position;
	vertexNormal = normal;
	TexCoord = texCoord;
	
	gl_Position = projection * model_view * transform * vec4(position, 1.0f);	
}