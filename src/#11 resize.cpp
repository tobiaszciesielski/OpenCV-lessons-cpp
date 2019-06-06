#include <opencv2/opencv.hpp>
#include <string>
#include <array>

using std::string;

const string winTitle_source = "Source Image";
const string winTitle_destination = "Destination Destination";
const string winTitle_tracbars = "Trackbars";

const string tracbarTitle_scale = "Scale";
const int scaleTb_maxValue = 6;

const string tracbarTitle_interFlag = "Inter Type";
const int interFlagTb_maxValue = 4; 

cv::Mat src;
cv::Mat dst;

int interFlagTb_value = 0;
int scaleTb_value = 4;

const std::array<int, 5> interpolationMethods 
{
	cv::InterpolationFlags::INTER_NEAREST,	// - a nearest - neighbor interpolation
	cv::InterpolationFlags::INTER_LINEAR,	// - a bilinear interpolation(used by default)
	cv::InterpolationFlags::INTER_AREA,		// - resampling using pixel area relation.
											//   It may be a preferred method for image decimation, as it gives moire’ - free results.
											//   But when the image is zoomed, it is similar to the INTER_NEAREST method.
	cv::InterpolationFlags::INTER_CUBIC,	// - a bicubic interpolation over 4x4 pixel neighborhood
	cv::InterpolationFlags::INTER_LANCZOS4	// - a Lanczos interpolation over 8x8 pixel neighborhood

};

const std::array<float, 7> scales{ 0.25, 0.33, 0.5, 0.66, 1.5, 2.0, 2.15 };

void update()
{
	cv::resize(src, dst, cv::Size(), scales[scaleTb_value], scales[scaleTb_value], interpolationMethods[interFlagTb_value]);

	cv::imshow(winTitle_source, src);
	cv::imshow(winTitle_destination, dst);
}

void trackbarCallback_scale(int pos, void* pointer)
{
	update();
}

void trackbarCallback_interType(int pos, void* pointer)
{
	update();
}

int main()
{
	src = cv::imread("resources/PerspectiveTransform/pug.png");

	cv::namedWindow(winTitle_tracbars);

	cv::createTrackbar(tracbarTitle_scale, winTitle_tracbars, &scaleTb_value, scaleTb_maxValue, trackbarCallback_scale);
	cv::createTrackbar(tracbarTitle_interFlag, winTitle_tracbars, &interFlagTb_value, interFlagTb_maxValue, trackbarCallback_interType);

	int key = 0;
	while(key != ' ') {

		trackbarCallback_interType(NULL, nullptr);
		trackbarCallback_scale(NULL, nullptr);

		key = cv::waitKey();
	}

	return 0;
}

































