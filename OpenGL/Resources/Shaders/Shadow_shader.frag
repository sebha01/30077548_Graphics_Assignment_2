#version 330 core

in vec3 Vertex;
in vec3 Normal; 
in vec2 TexCoord;
in vec4 FragLightPos;

//Texture sampler
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_shadow;

//Camera location
uniform vec3 eyePos;

//Light information
uniform vec4		lightPosition;
uniform vec4		lightAmbient;
uniform vec4		lightDiffuse;

//Material iformation
uniform vec4		matAmbient;
uniform vec4		matDiffuse;
uniform vec4        matSpecularColour;
uniform float       matSpecularExponent;

out vec4 FragColour;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    //Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //Normalise coordicates to  [0 - 1] range
    projCoords = projCoords * 0.5 + 0.5;
    //Get closest depth value from light's point of view
    float closestDepth = texture(texture_shadow, projCoords.xy).r; 
    //Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    //Check if current fragment posisition is in shadow
    float shadow = currentDepth - 0.005  > closestDepth  ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	//Ambient light value
	vec4 texColour = texture(texture_diffuse1, TexCoord);
	vec4 ambient = lightAmbient * matAmbient * texColour;

	//Diffuse light value
	vec3 N = normalize(Normal);	
	vec3 L = normalize(lightPosition.xyz - Vertex);
	float lambertTerm = clamp(dot(N, L), 0.0, 1.0);
	vec4 diffuse = lightDiffuse * matDiffuse * lambertTerm * texColour;

	//Specular light value
	vec3 E = normalize(eyePos - Vertex);
	vec3 R = reflect(-L, N); // reflected light vector about normal N
	float specularIntensity = pow(max(dot(E, R), 0.0), matSpecularExponent);
	vec4 specular = matSpecularColour * specularIntensity * texColour;

	//Calculate shadow
    float shadow = ShadowCalculation(FragLightPos); 

	//Final colour is the combinatin of all components
	FragColour = ambient + ((1.0 - shadow) * diffuse + specular);
}