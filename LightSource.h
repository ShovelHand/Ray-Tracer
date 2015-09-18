#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H
#include "icg_common.h"
#include <Eigen/Geometry>

typedef cv::Vec3b Colour;
class LightSource{
public:
	LightSource(vec3 pos, Colour colour);
	~LightSource();

	vec3 GetPos() { return m_vPos; }
	Colour GetColour() { return m_cColour; }

protected:

private:
	vec3 m_vPos;
	Colour m_cColour;

};
#endif