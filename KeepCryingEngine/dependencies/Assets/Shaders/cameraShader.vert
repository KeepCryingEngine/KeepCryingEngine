#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 model_view;
uniform mat4 transform;
uniform float width;

void main()
{
c_world = inverse(projection * view) * vec4(position, 1);
c_world = c_world*1.0/width;
	gl_Position = c_world;
}