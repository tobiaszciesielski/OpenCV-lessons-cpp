#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <time.h>

using std::string;

const string thresholdTracbarTitle = "Thresholding";
const string tracbarsWinTitle = "Tracbars";
const string windowTitle_srcImage = "Source image";
const string windowTitle_dstImage = "Current image status";

cv::Mat sourceImage, destinationImage;

int thresholdValue = 128;

void displayImages()
{
	cv::imshow(windowTitle_srcImage, sourceImage);
	cv::imshow(windowTitle_dstImage, destinationImage);
}

void threshold(cv::Mat& src, int thresholdValue)
{
	cv::threshold(src, destinationImage, thresholdValue, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
}

void dilateAndErode(cv::Mat& src)
{
	cv::dilate(src, src, cv::Mat());
	cv::erode(src, src, cv::Mat());
}

void thresholdCallbackFunc(int, void*)
{
	threshold(sourceImage, thresholdValue);
	dilateAndErode(destinationImage);

	displayImages();
}

void preapSrcImage()
{
	sourceImage = cv::imread("resources/ShapeDetection/testImage.jpg", cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::resize(sourceImage, sourceImage, cv::Size(), 0.18, 0.18);
}

std::ostream& operator<<(std::ostream& stream, const std::vector<cv::Vec4i> vec)
{
	int i = 0;
	for (auto& p : vec)
		stream << ++i << ". [" << p[0] << ", " << p[1] << ", " << p[2] << ", " << p[3] << "]" << std::endl;

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::vector<std::vector<cv::Point>> vecOfVec)
{
	int i = 0, j = 0;
	for (auto& vec : vecOfVec)
	{
		stream << "Contour " << ++j << " contains: " << std::endl;

		for (auto& p : vec)
			stream << ++i << ". [" << p.x << ", " << p.y << "]" << std::endl;

		i = 0;
	}
	return stream;
}

int main()
{
	srand(time(NULL));

	////////////////////////////////////////////////////////////////////
	// This part is responsible for setting the appropriate threshold //
	////////////////////////////////////////////////////////////////////

	preapSrcImage();

	cv::namedWindow(tracbarsWinTitle);

	cv::createTrackbar(thresholdTracbarTitle, tracbarsWinTitle, &thresholdValue, 255, thresholdCallbackFunc);
	thresholdCallbackFunc(NULL, nullptr);

	cv::waitKey(0);
	cv::destroyWindow(tracbarsWinTitle);

	//////////////////////////////////////////////////
	// This part is responsible for detect contours // 
	//////////////////////////////////////////////////

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(destinationImage, contours, hierarchy,
		cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE,
		cv::RetrievalModes::RETR_TREE);

	for (auto& vec : contours)
	{
		for (auto& point : vec)
		{
			int r = rand() % 255;
			int g = rand() % 255;
			int b = rand() % 255;
			cv::Scalar color(b, g, r);

			cv::circle(destinationImage, point, 2, color, 2);
		}
	}

	const int contoursCount = contours.size();

	std::cout << "Contours: " << std::endl << contours << std::endl;
	std::cout << "Hierarchy: " << std::endl << hierarchy << std::endl;

	cv::cvtColor(destinationImage, destinationImage, cv::ColorConversionCodes::COLOR_GRAY2BGR);

	cv::Scalar colorArray[5];
	{
		colorArray[0] = cv::Scalar(0, 0, 255);		//red
		colorArray[1] = cv::Scalar(0, 255, 0);		//green
		colorArray[2] = cv::Scalar(255, 0, 0);		//blue
		colorArray[3] = cv::Scalar(255, 255, 0);	//cyan
		colorArray[4] = cv::Scalar(0, 255, 255);	//yellow
	}

	for (int i = 0; i < contoursCount; i++)
		cv::drawContours(destinationImage, contours, i, colorArray[i], 2, 8, hierarchy);

	// Get the moments
	std::vector<cv::Moments> moments(contoursCount);
	for (int i = 0; i < contoursCount; i++)
		moments[i] = cv::moments(contours[i], false);

	// Get thre mass centers
	std::vector<cv::Point2f> massCenters(contoursCount);
	std::cout << "Mass centeres:" << std::endl;
	for (int i = 0; i < contoursCount; i++)
	{
		massCenters[i] = cv::Point2f(
			static_cast<float>(moments[i].m10 / moments[i].m00),
			static_cast<float>(moments[i].m01 / moments[i].m00)
		);

		cv::circle(destinationImage, massCenters[i], 5, colorArray[i]);

		std::cout << i + 1 << ". " << massCenters[i] << std::endl;
	}

	cv::Point2f srcPointsPerspective[4];
	cv::Point2f dstPointsPerspective[4];

	dstPointsPerspective[0] = cv::Point2f(0.f, 0.f);
	dstPointsPerspective[1] = cv::Point2f(static_cast<float>(sourceImage.cols - 1), 0.f);
	dstPointsPerspective[2] = cv::Point2f(static_cast<float>(sourceImage.cols - 1), static_cast<float>(sourceImage.rows - 1));
	dstPointsPerspective[3] = cv::Point2f(0.f, static_cast<float>(sourceImage.rows - 1));

	srcPointsPerspective[0] = massCenters[3];
	srcPointsPerspective[1] = massCenters[2];
	srcPointsPerspective[2] = massCenters[0];
	srcPointsPerspective[3] = massCenters[1];

	cv::Mat perspectiveTransform = cv::getPerspectiveTransform(srcPointsPerspective, dstPointsPerspective);
	cv::Mat finalImage;

	cv::warpPerspective(sourceImage, finalImage, perspectiveTransform, sourceImage.size());

	cv::imshow("Final Image", finalImage);
	displayImages();

	cv::waitKey(0);

	return 0;
}