in vec4 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 LightPos;
in vec3 CameraPos;


out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D normalMap;

uniform mat4 rotation;

void main()
{
	vec3 normal = texture2D(normalMap,TexCoord).rgb;
	
	float intensity = dot((rotation * vec4(normal , 0.0f)).xyz, normalize(LightPos));
	
	float diffuse = max(intensity, 0);
	// --------------------------
	
	//Specular Lightning --------
	float shininess = 48.0f;
	vec3 halfVector = normalize(normalize(LightPos) + normalize(CameraPos));
	
	float specularIntensity = dot((rotation * vec4(normal , 0.0f)).xyz, normalize(halfVector));
	specularIntensity = max(specularIntensity, 0);		
	float specular = pow(specularIntensity, shininess);	
	//---------------------------
	
	//Ambient Lightning ---------
	vec4 ambientLight = vec4(0.0, 0.0, 0.0, 1.0); 
	// --------------------------
	
	color = ((diffuse * texture2D(ourTexture, TexCoord)) + specular) + ambientLight;

}
