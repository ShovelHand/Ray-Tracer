#include "C:\icg\raytrace\Sphere.h"



Sphere::Sphere(vec3 pos, float rad, Colour colour, int gloss)
{
	m_vPos = pos;
	float m_rRad = rad;
	m_cColour = colour;
	m_iGloss = gloss;

}
Sphere::~Sphere()
{

}