in vec4 ourColor;
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D normalMapShader;

void main()
{
	color = texture2D(ourTexture,TexCoord);
}
