#ifndef SPHERE_H
#define SPHERE_H
#include "icg_common.h"
#include <Eigen/Geometry>

typedef cv::Vec3b Colour;
class Sphere{
public:
	Sphere(vec3 pos, float rad, Colour colour, int gloss);
	~Sphere();

	vec3 GetPos() { return m_vPos; }
	float GetRad() { return m_fRadius; }
	Colour GetColour() { return m_cColour; }
	int GetGloss() { return m_iGloss; }

protected:

private:
	float m_fRadius;
	vec3 m_vPos;
	Colour m_cColour;
	int m_iGloss;
	
};
#endif