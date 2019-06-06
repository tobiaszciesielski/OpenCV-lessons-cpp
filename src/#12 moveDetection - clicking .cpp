/*

“background image”, “current image”, “foreground image”. 
Do okna “foreground image” podpi¹æ trackbar (wartoœci 0-255).

*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using std::string;

const string winTitle_bgImg = "background image";
const string winTitle_currentImg = "current image";
const string winTitle_diffImg = "diffrential image";

const int tbMaxValue = 255;

cv::Mat frame;
cv::Mat backgroundImage;
cv::Mat currentImage;
cv::Mat diffrentialImage;

void trackbarCallback(int position, void* pointer)
{
	cv::Mat thresholdImg = diffrentialImage.clone();

	cv::threshold(diffrentialImage, thresholdImg,
		position, 255, cv::ThresholdTypes::THRESH_BINARY);

	cv::imshow(winTitle_diffImg, thresholdImg);
}

int main()
{
	cv::VideoCapture camera(0);

	cv::namedWindow(winTitle_bgImg);
	cv::namedWindow(winTitle_currentImg);
	cv::namedWindow(winTitle_diffImg);

	cv::createTrackbar("0-255", winTitle_diffImg, NULL, tbMaxValue, trackbarCallback);

	bool backgroundSaved = false;

	while (true)
	{
		camera >> frame;
		cv::cvtColor(frame, frame, cv::ColorConversionCodes::COLOR_RGBA2GRAY);

		//cv::imshow(winTitle_currentImg, frame);
	
		switch (cv::waitKey(1))
		{
			case('b'):
			{
				backgroundSaved = true;
				std::cout << "'backgroud saved!";
				backgroundImage = frame.clone();
				cv::imshow(winTitle_bgImg, backgroundImage);
			} break;

			case('f'):
			{
				std::cout << "current frame saved";
				currentImage = frame.clone();
				cv::imshow(winTitle_currentImg, currentImage);

				if (backgroundSaved == true)
				{
					cv::absdiff(backgroundImage, currentImage, diffrentialImage);
					trackbarCallback(NULL, nullptr);
				}


			} break;

			default:
				break;
		}		
	}
	return 0;
}