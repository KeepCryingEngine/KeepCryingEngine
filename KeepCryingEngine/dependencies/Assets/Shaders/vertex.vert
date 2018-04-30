layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;

#ifdef RIGGING
layout (location = 4) in ivec4 bone_indices;
layout (location = 5) in vec4 bone_weights;

#define MAX_BONES 100

#endif


out vec4 ourColor;
out vec2 TexCoord;
out vec3 Normal;


#ifdef DEPTH

out vec4 relativeCameraPos;
uniform mat4 actualCameraModelView;

#endif


#ifdef LIGHTNING

out vec3 vertexPosition;
out vec3 vertexNormal;

#endif


uniform mat4 projection;
uniform mat4 model_view;
uniform mat4 transform;

#ifdef RIGGING
uniform mat4 palette[MAX_BONES];
#endif

void main()
{
	gl_Position = projection * model_view * transform * vec4(position, 1.0f);
	ourColor = color;
	TexCoord = texCoord;
	mat3 normalMatrix = mat3(transform);
	normalMatrix = inverse(normalMatrix);
	Normal = normalize(normal * normalMatrix);

#ifdef RIGGING

	mat4 skin_transform = 	palette[bone_indices[0]]*bone_weights[0]+
							palette[bone_indices[1]]*bone_weights[1]+ 
							palette[bone_indices[2]]*bone_weights[2]+
							palette[bone_indices[3]]*bone_weights[3];
							
	vec3 vertex_position = vec3(skin_transform*vec4(position, 1));
	vec3 vertex_normal = vec3(skin_transform*vec4(normal, 0));
//TODO: VERIFY
gl_Position = projection * model_view * vec4(vertex_position, 1.0f);

// Normal = normalize(vertex_normal * normalMatrix);
Normal = normalize(vec3(model_view * vec4(vertex_normal, 0.0f)));

#endif

#ifdef DEPTH

	relativeCameraPos = actualCameraModelView * transform * vec4(position, 1.0f);

#endif
	

#ifdef LIGHTNING

	vertexPosition = position;
	vertexNormal = normal;

#endif
}