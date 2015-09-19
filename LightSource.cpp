#include "C:\icg\raytrace\LightSource.h"

LightSource::LightSource(vec3 pos, Colour colour, float intensity)
	:m_vPos(pos)
	,m_cColour(colour)
	,m_fI(intensity)

{
	//empty constructor
}

LightSource::~LightSource()
{
	//empty destructor
}