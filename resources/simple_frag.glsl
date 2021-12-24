#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;
uniform vec3 lightPos;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 EPos;
in float d;

float a = 0.05;
float b = 0.1;
float c = 0.2;

void main()
{
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	float dC = max(0, dot(normal, light));
	vec3 halfV = normalize(-1*EPos) + normalize(light);
	float sC = pow(max(dot(normalize(halfV), normal), 0), MatShine);
	color = vec4(vec3(.7, .4, .2) * (MatAmb + dC*MatDif + sC*MatSpec) / (a + d * b + pow(d * c, 2)), 1.0);
	//color = vec4(MatAmb + dC*MatDif, 1.0);
}
