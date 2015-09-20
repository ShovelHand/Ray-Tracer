#include "icg_common.h"
#include <Eigen/Geometry>

#include "C:\icg\raytrace\Sphere.h"
#include "C:\icg\raytrace\LightSource.h"

#ifndef WITH_OPENCV
#error OpenCV required for this exercise
#endif

#define INFINITY 10000  //just some arbitrarily large number to help find closest intersection
typedef cv::Vec3b Colour;
Colour red() { return Colour(255, 0, 0); }
Colour white() { return Colour(255, 255, 255); }
Colour black() { return Colour(0, 0, 0); }

struct MyImage{
	/// Data (not private for convenience)
	int cols = 620;
	int rows = 480;
	///  Channel with [0..255] range image (aka uchar)
	cv::Mat image = cv::Mat(rows, cols, CV_8UC3, cv::Scalar(255, 255, 255));

	Colour& operator()(int row, int col)
	{
		assert(row >= 0 && row < rows);
		assert(col >= 0 && col < cols);
		return image.at<cv::Vec3b>(cv::Point(col, row));
	}

	/// Displays the image and pauses program execution (until key pressed)
	void show(){
		/// Display the image
		cv::imshow("image", image);
		cv::waitKey(0);

	}

	/// Use files with "*.png" extension
	void save(const std::string& filename){
		cv::imwrite(filename, image);
	}
};

//TODO: in a perfect world, these would be declared less slopily.
std::vector<Sphere*> Spheres;
std::vector<LightSource*> LightSources;

Colour Shade(vec3 origin, Sphere* sphere, vec3 p)
{
	Colour colour(0, 0, 0);
	//get normal n based on the point of interesection
	vec3 n((sphere->GetPos().x() - p.x()) / sphere->GetRad(),
		(sphere->GetPos().y() - p.y()) / sphere->GetRad(),
		(sphere->GetPos().z() - p.z()) / sphere->GetRad());
	
	for (std::vector<LightSource*>::iterator itr = LightSources.begin(); itr != LightSources.end(); ++itr)
	{
		//get unit vector towards light source
		vec3 l = p - (*itr)->GetPos(); l.normalize();
		//get unit vector towards eye
		vec3 view = p - origin; view.normalize();
		vec3 h = view + l; h.normalize();
		float I = (*itr)->GetI(); //Intensity
		colour +=  sphere->GetColour()*I*fmax(0.0f, n.dot(l));
		colour += (*itr)->GetColour()*(I*pow(fmax(0.0f, n.dot(h)), sphere->GetGloss())); //using GetColour() means that the specular highligh colour 
		//is the colour of the light source hitting it, which is probably a sloppy way of doing this.
	}

	return colour;
}

int main(int, char**){
	/// Rays and vectors represented with Eigen
	typedef Eigen::Vector3f vec3;
	typedef Eigen::ParametrizedLine<float, 3> ray3;

	MyImage image;

	//build collection of spheres					b	g	r
	Sphere sphere1(vec3(0, 0, -1.5), 0.40f, Colour(255, 0, 0), 50);
	Spheres.push_back(&sphere1);
	Sphere sphere2(vec3(-0.5, 0.5, -0.5), 0.5f, Colour(0, 0, 255), 1000);
	Spheres.push_back(&sphere2);
	Sphere sphere3(vec3(0.75, -0.5, -1), 0.7f, Colour(0, 255, 0), 100000);
	Spheres.push_back(&sphere3);
	Sphere sphere4(vec3(-0.4, -0.75, 0), 0.2f, Colour(150, 0, 150), 10000);
	Spheres.push_back(&sphere4);

	//build collection of light sources
	LightSource light1(vec3(-0.5f, -0.25f, 1.0f), Colour(150, 150, 150) ,10);
	LightSources.push_back(&light1);
	LightSource light2(vec3(-0.25f, -1.0f, -3.0f), Colour(150, 150, 150), 50);
	LightSources.push_back(&light2);
	LightSource light3(vec3(0.75f, 0.7f, -0.75f), Colour(255, 15, 15), 30.0f);
	LightSources.push_back(&light3);

	vec3 eye(0, -0.75, 1.5);  //if we assume image pane has origin at 0,0,0, then eye is 10 units in front of it, and 'd' = 10
	float dist = eye.z();
	vec3 light(-0.5, -0.25,0);
	//finding pixel coords
	float left, right, top, bottom;
	left = bottom = -1;
	top = right = 1;

	for (int row = 0; row < image.rows; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			//construct ray
			float u = (left + (right - left)*(col + 0.5) / image.cols);
			float v = bottom + (top - bottom)*(row + 0.5) / image.rows;
			vec3 d(u -eye.x(), v-eye.y(), -dist); d.normalize();
		
			//check each sphere for intersection with ray, and if it is the closest intersection
			float tmin = INFINITY;
			float t;
			bool intersection = false;
			//check for ground plane intersection
			vec3 p0(0, 1, 0); //vec3 p1(-1, -2, 2); //two points on our ground plane. x-z plane, two units down.
			vec3 n(0, 1, 0); //normal vector to plane on the x-z plane
			n.normalize();   //not really necessary unless I want to see the plane at a weird tilt
			if (d.dot(n) != 0)
			{
				t = ((p0 - eye).dot(n)) / d.dot(n);
			
				tmin = t;
				if ((eye + t*d - p0).dot(n) == 0 && !intersection )// the t < 1 may become un-necessary once shading is handled better, or if INFINITY is set to a better level
				{
					intersection = true;
					Colour colour(0, 0, 0);  //only one ground, so its colour is declared here
					vec3 p(eye.x() + t*d.x(), eye.y() + t*d.y(), eye.z() + t*d.z()); //point of intersection

					for (std::vector<LightSource*>::iterator itr = LightSources.begin(); itr != LightSources.end(); ++itr)
					{
						//get unit vector towards light source
						vec3 l = p - (*itr)->GetPos(); l.normalize();
						//get unit vector towards eye
						vec3 view = p - eye; view.normalize();
						vec3 h = view + l; h.normalize();
						float I = (*itr)->GetI(); //Intensity
						colour += Colour(50,30,50)*I*fmax(0.0f, n.dot(l));
						colour += Colour(50, 0, 50)*(I*pow(fmax(0.0f, n.dot(h)), 0)); //using GetColour() means that the specular highligh colour 
						//is the colour of the light source hitting it, which is probably a sloppy way of doing this.
					}

					image(row, col) = colour;
				}
			}
			std::vector<Sphere*>::iterator closestSphere;

			for (std::vector<Sphere*>::iterator itr = Spheres.begin(); itr != Spheres.end(); ++itr)
			{
				float b = (d.dot(eye - (*itr)->GetPos()));
				float c = (eye - (*itr)->GetPos()).dot(eye - (*itr)->GetPos()) - pow((*itr)->GetRad(), 2);
				float discriminant = sqrt(pow(b, 2) - c);
				if (discriminant >= 0) //don't waste computation time if no intersection
				{
					t = fmin(((-1)*b - discriminant), ((-1)*b + discriminant));
					intersection = true;
					if (t < tmin)
					{
						vec3 p(eye.x() + t*d.x(), eye.y() + t*d.y(), eye.z() + t*d.z());
						tmin = t;
						image(row, col) = Shade(eye, (*itr), p);
					}
				}
			}
		
			
			if (!intersection)
			{
				image(row, col) = Colour(0, 0, 0);
			}
		}
	}

	image.show();
	// image.save("output.png"); ///< Does not work on Windows!

	return EXIT_SUCCESS;
}
