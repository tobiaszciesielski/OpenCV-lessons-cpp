#include <opencv2/opencv.hpp>
#include <string>

const std::string winTitle_blured = "Blured";
const std::string winTitle_canny = "Canny edge detector";
const std::string winTitle_tracbars = "Trackbars";

const std::string tbTitle_1st = "1st Th.";
const std::string tbTitle_2nd = "2st Th.";
const std::string tbTitle_blur = "Blur val.";

const int maxValue = 500;
const int maxValue_blur = 19;

int firstThreshold = 0;
int secondThreshold = 0;
int blur = 1;

cv::Mat image;	// original img
cv::Mat src;	// CV_16SC1 image
cv::Mat blured;	// Blured image
cv::Mat detectedEdges;	// result of canny edge detector

static void trackbarCallback(int, void*)
{
	if (blur % 2 == 0) return;

	cv::GaussianBlur(src, blured, cv::Size(blur, blur), 0);
	cv::Canny(blured, blured, detectedEdges, firstThreshold, secondThreshold);

	detectedEdges.convertTo(detectedEdges, CV_8U);
	blured.convertTo(blured, CV_8U);

	cv::imshow(winTitle_blured, blured);
	cv::imshow(winTitle_canny, detectedEdges);
}

int main()
{
	image = cv::imread("lena.jpg", cv::IMREAD_GRAYSCALE);
	cv::namedWindow(winTitle_tracbars, cv::WINDOW_AUTOSIZE);

	image.convertTo(src, CV_16SC1);
	
	int key = 0;
	while (key != ' ')
	{
		cv::createTrackbar(tbTitle_1st, winTitle_tracbars, &firstThreshold, maxValue, trackbarCallback);
		cv::createTrackbar(tbTitle_2nd, winTitle_tracbars, &secondThreshold, maxValue, trackbarCallback);
		cv::createTrackbar(tbTitle_blur, winTitle_tracbars, &blur, maxValue_blur, trackbarCallback);

		trackbarCallback(0, NULL);

		key = cv::waitKey();
	}

	return 0;
}