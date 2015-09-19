#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H
#include "icg_common.h"
#include <Eigen/Geometry>

typedef cv::Vec3b Colour;
class LightSource{
public:
	LightSource(vec3 pos, Colour colour, float intensity); //TODO: Intensity should be a property of the lightsource
	~LightSource();

	vec3 GetPos() { return m_vPos; }
	Colour GetColour() { return m_cColour; }
	float GetI() { return m_fI; }

protected:

private:
	vec3 m_vPos;
	Colour m_cColour;
	float m_fI; //light intensity

};
#endif