in vec4 ourColor;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

uniform sampler2D ourTexture;


#ifdef DEPTH

in vec4 relativeCameraPos;
uniform float actualCameraFar;

#endif


#ifdef LIGHTNING

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 transform;
uniform mat4 rotation;
uniform vec3 lightSourcePosition;
uniform vec3 cameraPosition;

#endif

#ifdef NORMALMAP

in vec3 FragmentPos;
in vec3 LightPos;
in vec3 CameraPos;

uniform sampler2D normalMap;

uniform mat4 rotation;

#endif

#ifdef CARTOON

uniform vec3 lightDir;

#endif

void main()
{
	color = texture2D(ourTexture,TexCoord);

#ifdef LIGHTNING

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
#endif

#ifdef CARTOON

	float intensity;
	intensity = dot(Normal,lightDir);

	if (intensity > 0.90)
		color = vec4(1.0,1.0,1.0,1.0);
	else if (intensity > 0.75)
		color = vec4(0.8,0.8,0.8,1.0);
	else if (intensity > 0.55)
		color = vec4(0.6,0.6,0.6,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.4,0.4,1.0);
	else
		color = vec4(0.2,0.2,0.2,1.0);

	color = color * texture2D(ourTexture,TexCoord);
#endif

#ifdef NORMALMAP

vec3 normal = texture2D(normalMap,TexCoord).rgb;
	normal = normalize(normal*2.0 - 1.0);
		
	vec3 lightDir =   normalize(LightPos - FragmentPos) ;
	float intensity = dot(normal, lightDir);
	
	float diffuse = max(intensity, 0);
	// --------------------------

	//Specular Lightning --------
	float shininess = 30.0f;
	vec3 cameraDir =  normalize(CameraPos -FragmentPos);
	vec3 halfVector = normalize(lightDir + cameraDir);
	
	float specularIntensity = dot(normal, halfVector);
	specularIntensity = max(specularIntensity, 0);		
	float specular = pow(specularIntensity, shininess);	
	//---------------------------

	//Ambient Lightning ---------
	vec4 ambientLight = vec4(0.0, 0.0, 0.0, 1.0); 
	// --------------------------
	
	color = ((diffuse * texture2D(ourTexture, TexCoord)) + specular) + ambientLight;

#endif

#ifdef DEPTH

	float depth = -relativeCameraPos.z/actualCameraFar;
	color = vec4(depth, depth, depth, 1.0);
	color = color * texture2D(ourTexture,TexCoord);

#endif

#ifdef COLOR

	color = ourColor;

#endif

}