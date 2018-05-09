layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

out vec4 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 LightPos;
out vec3 CameraPos;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightSourcePosition;
uniform vec3 cameraPosition;

void main()
{
	ourColor = color;
	TexCoord = texCoord;
	mat3 normalMatrix = mat3(model);
	normalMatrix = inverse(normalMatrix);
	Normal = normalize(normal * normalMatrix);

	vec3 tangent2 = normalize(tangent * normalMatrix);
	vec3 bitangent = cross(tangent2,Normal);
	mat3 matrix = mat3(tangent2,bitangent,Normal);
	matrix = transpose(matrix);

	gl_Position = projection * view * mat4(matrix) * model* vec4(position, 1.0f);
	LightPos = matrix * lightSourcePosition;
	CameraPos=  matrix * cameraPosition;
	
}
