#include "icg_common.h"
#include <Eigen/Geometry>

#include "C:\icg\raytrace\Sphere.h"
#ifndef WITH_OPENCV
#error OpenCV required for this exercise
#endif

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

int main(int, char**){
	/// Rays and vectors represented with Eigen
	typedef Eigen::Vector3f vec3;
	typedef Eigen::ParametrizedLine<float, 3> ray3;

	MyImage image;

	/// TODO: define camera position and sphere position here
	vec3 center(0, 0, -1); //the sphere center
	float radius = 0.40;
	vec3 eye(0, 0, 1);  //if we assume image pane has origin at 0,0,0, then eye is 10 units in front of it, and 'd' = 10
	float dist = eye.z();
	vec3 light(-0.5, -0.25,0);
	//finding pixel coords
	float left, right, top, bottom;
	left = bottom = -1;
	top = right = 1;

	for (int row = 0; row < image.rows; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			//////get normalized device coords
			float u = (left + (right - left)*(col + 0.5) / image.cols);
			float v = bottom + (top - bottom)*(row + 0.5) / image.rows;
			//construct ray
			vec3 s(col, row, 0);  //point on viewframe //was col, row, 0
			vec3 d(u -eye.x(), v-eye.x(), -dist); d.normalize();
			/*if (row == 240 && col == 320)
			{
				cout << d;
			}*/
			float b = (d.dot(eye - center));
			float c = (eye - center).dot(eye - center) - pow(radius, 2);
			float discriminant = sqrt(pow(b, 2) - c);
			if (discriminant >= 0) //don't waste computation time if no intersection
			{
				float t = fmin(((-1)*b - discriminant), ((-1)*b + discriminant));

				//get n based on c and p(t)
				vec3 p(eye.x() + t*d.x(), eye.y() + t*d.y(), eye.z() + t*d.z()); //coords of point on sphere
				//pixel coords;
				float x = (2 * (p.x()) - 1)*(image.cols / image.rows)* tan(1 / 2 * M_PI / 180);
				float y = (1 - 2 * (p.y()))* tan(1 / 2 * M_PI / 180);
				vec3 n((center.x() - p.x()) / radius, (center.y() - p.y()) / radius, (center.z() -p.z() ) / radius);
				//get unit vector towards light source
				vec3 l = p - light; l.normalize();
				//get unit vector towards eye
				vec3 view = p - eye; view.normalize();
				vec3 h = view + l; h.normalize();
				float factor = n.dot(l); //todo: this is lambertian shading. Use Phong.
				float I = 35; //Intensity

		
				image(row, col) = Colour(fmax(0, factor) * 200 + I, fmax(0, factor) * 200 + I, fmax(0, factor) * 100 + I);
				//		image(u, v) = Colour(200, 200, 100);
			}
			else
			{//set background colour
				image(row, col) = Colour(120, 120, 120);
			}
		}
	}

	image.show();
	// image.save("output.png"); ///< Does not work on Windows!

	return EXIT_SUCCESS;
}
