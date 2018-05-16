layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;
layout (location = 5) in ivec4 bone_indices;
layout (location = 6) in vec4 bone_weights;

#define MAX_BONES 100

out vec4 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragmentPos;
out vec3 LightPos;
out vec3 CameraPos;


//Uniforms
layout(std140) uniform camera{
	uniform mat4 projection;
	uniform mat4 view;
};
uniform mat4 model;
uniform vec3 lightSourcePosition;
uniform vec3 cameraPosition;
uniform mat4 palette[MAX_BONES];

void main()
{
	vec4 worldPos =model* vec4(position, 1.0f);
	gl_Position = projection * view  * worldPos;
	ourColor = color;
	TexCoord = texCoord;
	mat3 normalMatrix = mat3(model);
	normalMatrix = inverse(normalMatrix);
	Normal = normalize(normal * normalMatrix);

	mat4 skin_transform = 	palette[bone_indices[0]]*bone_weights[0]+
							palette[bone_indices[1]]*bone_weights[1]+ 
							palette[bone_indices[2]]*bone_weights[2]+
							palette[bone_indices[3]]*bone_weights[3];
							
	vec3 vertex_position = vec3(skin_transform*vec4(position, 1));
	vec3 vertex_normal = vec3(skin_transform*vec4(normal, 0));
	vec3 vertex_tangents = vec3(skin_transform * vec4(tangent,0));

	worldPos = model* vec4(vertex_position, 1.0f);
	gl_Position = projection * view * worldPos;

	vertex_tangents = normalize(vertex_tangents - vertex_normal * dot(vertex_normal,vertex_tangents));

	vec3 tangent2 = normalize(vertex_tangents * normalMatrix);
	Normal = normalize(vertex_normal * normalMatrix);
	vec3 bitangent = cross(Normal,tangent2);
	mat3 tangentSpaceMatrix = mat3(tangent2,bitangent,Normal);
	tangentSpaceMatrix = transpose(tangentSpaceMatrix);

	FragmentPos = tangentSpaceMatrix * vec3(worldPos);
	LightPos = tangentSpaceMatrix * lightSourcePosition;
	CameraPos= tangentSpaceMatrix * cameraPosition;
}
