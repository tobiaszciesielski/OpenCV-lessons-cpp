//This program maintains the threshold of the captured video with the same number of light and dark pixels

#include <opencv2/opencv.hpp>
#include <string>

using std::string;

//Window titles
const string winTitle_frame = "Capturing";
const string winTitle_threshold = "Threshold";

//Global Variables
const int maxLevel = 255;
int thresholdSlider = 255 / 2;

//Functions
void thresholding(cv::Mat& image, int level);
void keep_equality(cv::Mat &image);

int main()
{
	cv::VideoCapture cap(0); 
	if (!cap.isOpened())
		return -1;
	
	while (true)
	{
		cv::Mat frame;
		cap >> frame;
	
		cv::Mat grayFrame;
		cv::cvtColor(frame, grayFrame, CV_RGB2GRAY);

		cv::Mat thresholded;
		thresholded = grayFrame.clone();

		thresholding(thresholded, thresholdSlider);
		keep_equality(thresholded);

		cv::imshow(winTitle_frame, grayFrame);
		cv::imshow(winTitle_threshold, thresholded);

		if (cv::waitKey(30) >= 0)
			break;
	}
	return 0;
}

void thresholding(cv::Mat& image, int level)
{
	for (auto& p : cv::Mat_<uchar>(image))
		if (p < level)
			p = 0;
		else
			p = 255;
}

void keep_equality(cv::Mat &image)
{
	int less = 0, more = 0; // less or more than current value of thresholdSlider

	for (auto& p : cv::Mat_<uchar>(image))
		if (p < thresholdSlider)
			less++;
		else
			more++;

	if (less < more)
		thresholdSlider++;
	else
		thresholdSlider--;
}