#version 330 core

in vec2 TexCoord;
in vec3 VertexNormal;

uniform sampler2D texture_diffuse1;

uniform vec4 lightDirection;
uniform vec4 lightDiffuseColour;

out vec4 FragColor;

void main()
{
	vec4 L = normalize(lightDirection);

	vec3 N = normalize(VertexNormal);

	float lightIntensity = dot(L.xyz, N);
	float li = clamp(lightIntensity, 0.0, 1.0);

	vec4 LightData = lightDiffuseColour * li;

	vec4 textureColor = texture(texture_diffuse1, TexCoord);

	vec4 finalColour = LightData * textureColor;

	FragColor = finalColour;
}