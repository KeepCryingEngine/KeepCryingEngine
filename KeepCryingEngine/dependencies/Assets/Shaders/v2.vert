layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec4 tangent;

out vec4 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 LightPos;
out mat4 CameraView;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightSourcePosition;
uniform mat4 actualCameraModelView;

void main()
{
	ourColor = color;
	TexCoord = texCoord;
	mat3 normalMatrix = mat3(model);
	normalMatrix = inverse(normalMatrix);
	Normal = normalize(normal * normalMatrix);

	tangent = normalize(tangent * normalMatrix);
	vec3 bitangent = cross(tangent,Normal);
	mat3 matrix = mat3(tangent,bitangent,Normal);
	matrix = transpose(matrix);

	gl_Position = projection * view * matrix * model* vec4(position, 1.0f);
	LightPos = lightSourcePosition* matrix;
	CameraView = actualCameraModelView* matrix;
	
}
