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

//class ImagePlane{
//public:
//	ImagePlane(vec2 const& llCorner, // The lower-left corner (l, b, 0)
//		vec2 const& urCorner, // The upper-right corner (r, t, 0)
//		int const& xRes, int const& yRes) : // The dimensions of the image
//		l(llCorner.x()),
//		b(llCorner.y()),
//		r(urCorner.x()),
//		t(urCorner.y()),
//		mRows(xRes),
//		mCols(yRes)
//	{ }
//
//	inline vec3 generatePixelPos(int i, int j){
//		float u = l + (r - l) * (i + 0.5f) / mRows;
//		float v = b + (t - b) * (j + 0.5f) / mCols;
//		return vec2(u, v, 0);
//	}
//
//private:
//	int mRows, mCols;
//	float l, r, b, t;
//};
#endif