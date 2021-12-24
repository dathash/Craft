#version 330 core
uniform sampler2D Texture0;
uniform int flip;
uniform float MatShine;

in vec2 vTexCoord;
in vec3 lightDir;
in vec3 fragNor;
in vec3 EPos;
in float d;
out vec4 Outcolor;

float a = 1.0;
float b = 0.0;
float c = 0.01;

void main() {
   vec3 normal = flip * normalize(fragNor);
   vec3 light = normalize(lightDir);
   vec3 eye = normalize(EPos);

  vec4 texColor0 = texture(Texture0, vTexCoord);

   float Dc = max(0, dot(normal, light));
   vec3 H = (eye + light) / 2.0;
   float Sc = pow(max(0, dot(normal, H)), MatShine);
  	//to set the out color as the texture color 
  	Outcolor = vec4(vec3(0.9, 0.8, 0.6) * (0.1 * texColor0 + Dc * texColor0 + 0.5 * Sc * texColor0).xyz / (a + d * b + pow(d * c, 2)), 1.0);
 
  	//to set the outcolor as the texture coordinate (for debugging)
	//Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}

