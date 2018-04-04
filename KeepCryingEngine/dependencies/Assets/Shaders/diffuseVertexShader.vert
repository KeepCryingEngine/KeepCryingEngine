#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;

uniform mat4 projection;
uniform mat4 model_view;
uniform mat4 rotation;
uniform mat4 transform;
uniform vec3 cameraPosition;
uniform vec3 lightSourcePosition;

out float intensity;
out float specular;

void main()
{
	gl_Position = projection * model_view * transform * vec4(position, 1.0f);	

	vec4 worldPosition = transform * vec4(position, 1.0f);
	vec3 lightDir = lightSourcePosition - worldPosition.xyz;
	intensity = dot((rotation * vec4(normal, 0.0f)).xyz, normalize(lightDir));
	intensity = max(intensity, 0);
	
	vec3 cameraDir = cameraPosition - worldPosition.xyz;
	vec3 halfVector = normalize(normalize(lightDir) + normalize(cameraDir));
	
	specular = dot((rotation * vec4(normal, 0.0f)).xyz, normalize(halfVector));
	specular = max(specular, 0);
	
	float shininess = 64.0f;
	
	specular = pow(specular, shininess);	
}