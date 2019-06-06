#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using std::string;

const string winTitle_bgImg = "background image";
const string winTitle_currentImg = "current image";
const string winTitle_diffImg = "diffrential image";

const int tbMaxValue = 255;
int tbValue = 0;

cv::Mat frame;
cv::Mat backgroundImage;
cv::Mat previousImg, currentImage, nextImg;
cv::Mat diffrentialImage;

void trackbarCallback(int position, void* pointer)
{
	cv::threshold(diffrentialImage, diffrentialImage,
		position, 255, cv::ThresholdTypes::THRESH_BINARY);

	cv::imshow(winTitle_diffImg, diffrentialImage);
}

void saveBackground(const cv::Mat& frame)
{
	backgroundImage = frame.clone();
	cv::imshow(winTitle_bgImg, backgroundImage);
}

void saveFrame(const cv::Mat& frame)
{
	static int x = 0, y = 0, z = 0;
	static bool firstIteration = true;

	if (firstIteration)
	{
		nextImg = frame.clone();
		currentImage = frame.clone();
		previousImg = frame.clone(); 

		firstIteration = false;
	}
	else
	{
		previousImg = currentImage.clone();
		currentImage = nextImg.clone();
		nextImg = frame.clone();
	}
}

void prepOutputImage()
{
	cv::Mat fstDiff, scndDiff, outputImage;

	cv::absdiff(previousImg, currentImage, fstDiff);
	cv::absdiff(currentImage, nextImg, scndDiff);

	cv::bitwise_and(fstDiff, scndDiff, diffrentialImage);
	
	cv::dilate(diffrentialImage, diffrentialImage, cv::Mat());
	cv::erode(diffrentialImage, diffrentialImage, cv::Mat());
	
	//cv::absdiff(backgroundImage, diffrentialImage, diffrentialImage);

	//cv::imshow(winTitle_diffImg, diffrentialImage);
}

int main()
{
	cv::VideoCapture camera(0);

	cv::namedWindow(winTitle_bgImg);
	cv::namedWindow(winTitle_currentImg);
	cv::namedWindow(winTitle_diffImg);

	cv::createTrackbar("0-255", winTitle_diffImg, &tbValue, tbMaxValue, trackbarCallback);

	bool backgroundSaved = false;
	
	while (true)
	{
		camera >> frame;
		cv::cvtColor(frame, frame, cv::ColorConversionCodes::COLOR_RGBA2GRAY);

		if (cv::waitKey(1) == 'b')
		{
			saveBackground(frame);
			backgroundSaved = true;
		}

		saveFrame(frame);
		cv::imshow(winTitle_currentImg, currentImage);
		
		if (backgroundSaved)
		{
			prepOutputImage();
			trackbarCallback(tbValue, nullptr);
		}
	}

	return 0;
}