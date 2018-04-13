#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
in vec4 relativeCameraPos;

out vec4 color;

uniform sampler2D ourTexture;
uniform float actualCameraFar;

void main()
{
	float depth = -relativeCameraPos.z/actualCameraFar;
	color = vec4(depth, depth, depth, 1.0);
	color = color * texture2D(ourTexture,TexCoord);
}