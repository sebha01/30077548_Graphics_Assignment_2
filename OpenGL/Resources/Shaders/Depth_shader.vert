#version 330

uniform mat4 model;
uniform mat4 lightPosMatrix;


layout (location=0) in vec3 vertexPos;

void main(void) 
{
	gl_Position = lightPosMatrix * model * vec4(vertexPos, 1.0f);
}

