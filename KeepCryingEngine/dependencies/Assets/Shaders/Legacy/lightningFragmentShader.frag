#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 TexCoord;

out vec4 color;

uniform mat4 transform;
uniform mat4 rotation;
uniform sampler2D ourTexture;
uniform vec3 lightSourcePosition;
uniform vec3 cameraPosition;

void main()
{
	//Diffuse Lightning ---------
	vec4 worldPosition = transform * vec4(vertexPosition, 1.0f);
	vec3 lightDir = lightSourcePosition - worldPosition.xyz;
	float intensity = dot((rotation * vec4(vertexNormal, 0.0f)).xyz, normalize(lightDir));
	
	float diffuse = max(intensity, 0);
	// --------------------------
	
	//Specular Lightning --------
	float shininess = 48.0f;
	vec3 cameraDir = cameraPosition - worldPosition.xyz;
	vec3 halfVector = normalize(normalize(lightDir) + normalize(cameraDir));
	
	float specularIntensity = dot((rotation * vec4(vertexNormal, 0.0f)).xyz, normalize(halfVector));
	specularIntensity = max(specularIntensity, 0);		
	float specular = pow(specularIntensity, shininess);	
	//---------------------------
	
	//Ambient Lightning ---------
	vec4 ambientLight = vec4(0.0, 0.0, 0.0, 1.0); 
	// --------------------------
	
	color = ((diffuse * texture2D(ourTexture, TexCoord)) + specular) + ambientLight;
}