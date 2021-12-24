#version 330 core

uniform sampler2D alphaTexture;

in vec4 partCol;

out vec4 outColor;


void main()
{
	float alpha = texture(alphaTexture, gl_PointCoord).r;
   if (alpha < 0.1) {
      discard;
   }
	outColor = vec4(partCol.xyz, partCol.a * alpha);
   
}
