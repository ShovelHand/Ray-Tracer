#include "icg_common.h"
#include <Eigen/Geometry>

#include "C:\icg\raytrace\Sphere.h"
#include "C:\icg\raytrace\LightSource.h"

#ifndef WITH_OPENCV
#error OpenCV required for this exercise
#endif

//#define INFINITY 10000  //just some arbitrarily large number to help find closest intersection
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

vec3 castRay(vec3 o, vec3 d)
{
	vec3 ray(d.x() - o.x(), d.y() - o.y(), d.z() -o.z()); d.normalize();
	ray.normalize();
	return ray;
}

Colour Shade(vec3 origin, Sphere* sphere, vec3 p)
{
	Colour colour = sphere->GetColour()/10;
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
		vec3 h = (view + l); h.normalize();
		float I = (*itr)->GetI(); //Intensity
	
		//is the colour of the light source hitting it, which is probably a sloppy way of doing this.
		vec3 shadowRay = castRay(l, p);
		bool inShadow = false;
		colour += sphere->GetColour()*I*fmax(0.2f, n.dot(l));
		colour += (*itr)->GetColour()*(I*pow(fmax(0.2f, n.dot(h)), sphere->GetGloss())); //using GetColour() means that the specular highligh colour 
		//for (std::vector<Sphere*>::iterator itr = Spheres.begin(); itr != Spheres.end(); ++itr)
		//{
		//	float b = (shadowRay.dot(p - (*itr)->GetPos()));
		//	float c = (p - (*itr)->GetPos()).dot(p - (*itr)->GetPos()) - pow((*itr)->GetRad(), 2);
		//	float discriminant = sqrt(pow(b, 2) - c);
		//	float t = fmin(((-1)*b - discriminant), ((-1)*b + discriminant));
		//	if (discriminant >= 0 ) 
		//	{	
		//		inShadow = true;
		//	}
		//	if (!inShadow)
		//	{
		//		colour += sphere->GetColour()*I*fmax(0.2f, n.dot(l));
		//		colour += (*itr)->GetColour()*(I*pow(fmax(0.2f, n.dot(h)), sphere->GetGloss())); //using GetColour() means that the specular highligh colour 
		//	}
		//}

	}

	return colour;
}

int main(int, char**){
	/// Rays and vectors represented with Eigen
	typedef Eigen::Vector3f vec3;
	typedef Eigen::ParametrizedLine<float, 3> ray3;

	MyImage image;

	//build collection of spheres					b	g	r
	Sphere sphere1(vec3(0, 0, 1.5), 0.40f, Colour(255, 0, 0), 50);
	Spheres.push_back(&sphere1);
	Sphere sphere2(vec3(-0.75, 0.5, 0.5), 0.4f, Colour(0, 0, 255), 1000);
	Spheres.push_back(&sphere2);
	Sphere sphere3(vec3(0.75, 0.5, 0.5), 0.4f, Colour(0, 255, 0), 100000);
	Spheres.push_back(&sphere3);
	Sphere sphere4(vec3(-0.4f, -0.75f, 0.5f), 0.4f, Colour(150, 0, 150), 10000);
	Spheres.push_back(&sphere4);
	Sphere sphere5(vec3(-0.99f, -1.7f, 0.5f), 0.4f, Colour(150, 155, 150), 10000);
	Spheres.push_back(&sphere5);

	//build collection of light sources
	LightSource light1(vec3(-0.5f, -1.7f, 1.75f), Colour(150, 150, 150) ,10);
	LightSources.push_back(&light1);
	LightSource light2(vec3(-1.0f, 0.0f, -0.5f), Colour(150, 150, 150), 50);
	LightSources.push_back(&light2);
	LightSource light3(vec3(0.99f, 0.7f, 5.0f), Colour(255, 15, 15), 30.0f);
	LightSources.push_back(&light3);

	vec3 eye(0, 0, -2);  //if we assume image pane has origin at 0,0,0, then eye is 10 units in front of it, and 'd' = 10
	float dist = eye.z();

	//finding pixel coords
	float left, right, top, bottom;
	left = -(float(image.cols) / float(image.rows));
	bottom = -1;
	top = 1;
	right = (float(image.cols) / float(image.rows));

	for (int row = 0; row < image.rows; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			//construct ray
			float u = (left + (right - left)*(col + 0.5) / image.cols);
			float v = (bottom + (top - bottom)*(row + 0.5) / image.rows);

			vec3 d(u, v, 0);
			vec3 r = castRay(eye, d);
			float tmin = 1000;//arbitrarily large instantiation for infinity.
			float t;
			bool intersection = false;
			//check for ground plane intersection
			vec3 p0(0, 1, 2);
			vec3 n(0, 1, 0); //normal vector to plane on the x-z plane
			Colour groundColour(50, 50, 50);
			if (r.dot(n) != 0 && row >= image.rows/2) //the inequality is here until I can figure out why the plane appears in the sky.
			{
		
				t = ((p0 - eye).dot(n)) / r.dot(n);

				tmin = t;
				
					intersection = true;
					Colour colour(10, 10, 10);  //only one ground, so its colour is declared here
					vec3 p(eye.x() + t*r.x(), eye.y() + t*r.y(), eye.z() + t*r.z()); //point of intersection

					for (std::vector<LightSource*>::iterator itr = LightSources.begin(); itr != LightSources.end(); ++itr)
					{
						//get unit vector towards light source
						vec3 l = p - (*itr)->GetPos(); l.normalize();
						//get unit vector towards eye
						vec3 view = p - eye; view.normalize();
						vec3 h = view + l; h.normalize();
						float I = (*itr)->GetI(); //Intensity
						colour += groundColour*I*fmax(0.0f, n.dot(l)); // only one ground, so colour can be handled here.
						colour += groundColour*(I*pow(fmax(0.0f, n.dot(h)), 10));
						vec3 shadowRay = castRay(p, l);

						for (std::vector<Sphere*>::iterator itr = Spheres.begin(); itr != Spheres.end(); ++itr)
						{
							float b = (shadowRay.dot(eye - (*itr)->GetPos()));
							float c = (p - (*itr)->GetPos()).dot(p - (*itr)->GetPos()) - pow((*itr)->GetRad(), 2);
							float discriminant = sqrt(pow(b, 2) - c);
							if (discriminant >= 0) //don't waste computation time if no intersection
							{
									colour -= Colour(10, 10, 10);
							}
						}

						//is the colour of the light source hitting it, which is probably a sloppy way of doing this.
					}

					image(row, col) = colour;
				
			}
			std::vector<Sphere*>::iterator closestSphere;

			for (std::vector<Sphere*>::iterator itr = Spheres.begin(); itr != Spheres.end(); ++itr)
			{
				float b = (r.dot(eye - (*itr)->GetPos()));
				float c = (eye - (*itr)->GetPos()).dot(eye - (*itr)->GetPos()) - pow((*itr)->GetRad(), 2);
				float discriminant = sqrt(pow(b, 2) - c);
				if (discriminant >= 0) //don't waste computation time if no intersection
				{
					t = fmin(((-1)*b - discriminant), ((-1)*b + discriminant));
					intersection = true;
					if (t < tmin)
					{
						vec3 p(eye.x() + t*r.x(), eye.y() + t*r.y(), eye.z() + t*r.z());
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

