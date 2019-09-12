#ifndef IMAGE_CLASSIFIER
#define IMAGE_CLASSIFIER

#include "image_classifier.hpp"
#include <string>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class image_classifier
{
public:
	static string classify(const Mat &img);

	private:

};


#endif