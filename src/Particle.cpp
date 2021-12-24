//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 start) :
	charge(1.0f),
	m(1.0f),
	d(0.0f),
	x(start),
	v(0.0f, 0.0f, 0.0f),
   a(0.0f, -0.001, 0.0),
	lifespan(1.0f),
	tEnd(3.0f),
   scale(1.0f),
	color(randFloat(0.0f, 1.0f), randFloat(0.0f, 1.0f), randFloat(0.0f, 1.0f), randFloat(0.0f, 1.0f))
{
}

Particle::~Particle()
{
}

void Particle::load(vec3 start)
{
	// Random initialization
	rebirth(0.0f, start);
}

/* all particles born at the origin */
void Particle::rebirth(float t, vec3 start)
{
	charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;	
	m = 1.0f;
  	d = randFloat(0.0f, 0.2f);
	x = start;
	v.x = randFloat(-0.27f, 0.3f);
	v.y = randFloat(-0.1f, 0.9f);
	v.z = randFloat(-0.3f, 0.27f);
	lifespan = randFloat(1.0f, 2.0f); 
	tEnd = t + lifespan;
	scale = randFloat(0.2, 1.0f);
   	color.r = randFloat(0.5f, 0.8f);
   	color.g = randFloat(0.0f, 0.5f);
   	color.b = randFloat(0.0f, 0.1f);
	color.a = 1.0f;
}

void Particle::update(float t, float h, const vec3 &g, const vec3 start)
{
	if(t > tEnd) {
		rebirth(t, start);
	}

	//very simple update
	x += 2 * h*v;
   v += a;
   
   
	//To do - how do you want to update the forces?
	color.a = (tEnd/t)/lifespan;
}
