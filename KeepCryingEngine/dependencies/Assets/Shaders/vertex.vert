layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;

out vec4 ourColor;
out vec2 TexCoord;
out vec3 Normal;


#ifdef DEPTH

out vec4 relativeCameraPos;
uniform mat4 actualCameraModelView;

#endif


#ifdef LIGHTNING

out vec3 vertexPosition;

#endif


uniform mat4 projection;
uniform mat4 model_view;
uniform mat4 transform;

void main()
{
	gl_Position = projection * model_view * transform * vec4(position, 1.0f);
	ourColor = color;
	TexCoord = texCoord;
	mat3 normalMatrix = mat3(transform);
	normalMatrix = inverse(normalMatrix);
	Normal = normalize(normal * normalMatrix);


#ifdef DEPTH

	relativeCameraPos = actualCameraModelView * transform * vec4(position, 1.0f);

#endif
	

#ifdef LIGHTNING

	vertexPosition = position;

#endif
}