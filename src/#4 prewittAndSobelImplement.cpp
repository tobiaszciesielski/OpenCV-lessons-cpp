#include <opencv2/opencv.hpp>
#include <string>

const std::string winTitle_original = "Original";
const std::string winTitle_corners_prewitt = "Prewitt Gradient";
const std::string winTitle_corners_sobel = "Sobel Gradient";

cv::Mat cornerDetection_Prewitt(const cv::Mat& input)
{
	//Prewitt mask
	cv::Mat Py = (cv::Mat_<char>(3, 3) <<
		-1, -1, -1,
		0, 0, 0,
		1, 1, 1);

	cv::Mat Px = (cv::Mat_<char>(3, 3) <<
		-1, 0, 1,
		-1, 0, 1,
		-1, 0, 1);

	cv::Mat derivDx;	// directional derivative herizontal
	cv::filter2D(input, derivDx, CV_32F, Px);
	derivDx /= 3;
	cv::pow(derivDx, 2, derivDx); // overloaded pow func by openCV

	cv::Mat derivDy;	// directional derivative vertical
	cv::filter2D(input, derivDy, CV_32F, Py);
	derivDy /= 3;
	cv::pow(derivDy, 2, derivDy);

	cv::Mat output_prewitt;
	cv::sqrt(derivDx + derivDy, output_prewitt); // overloaded sqrt func by openCV

	output_prewitt.convertTo(output_prewitt, CV_8U);

	return output_prewitt;
}

cv::Mat cornerDetection_Sobel(const cv::Mat& input)
{
	//Sobel mask
	cv::Mat Py = (cv::Mat_<char>(3, 3) <<
		-1, -2, -1,
		0, 0, 0,
		1, 2, 1);

	cv::Mat Px = (cv::Mat_<char>(3, 3) <<
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1);

	cv::Mat derivDx;	// directional derivative herizontal
	cv::filter2D(input, derivDx, CV_32F, Px);
	derivDx /= 4;
	cv::pow(derivDx, 2, derivDx); // overloaded pow func by openCV

	cv::Mat derivDy;	// directional derivative vertical
	cv::filter2D(input, derivDy, CV_32F, Py);
	derivDy /= 4;
	cv::pow(derivDy, 2, derivDy);

	cv::Mat output_sobel;
	cv::sqrt(derivDx + derivDy, output_sobel); // overloaded sqrt func by openCV

	output_sobel.convertTo(output_sobel, CV_8U);

	return output_sobel;
}

int main()
{
	//TODO:
	// itaration version of sobel and prewitt

	cv::Mat image(cv::imread("lena.jpg", cv::IMREAD_GRAYSCALE));//, cv::Rect(100, 100, 10, 10));

	cv::Mat image_prewitt = cornerDetection_Prewitt(image).clone();
	cv::Mat image_sobel = cornerDetection_Sobel(image).clone();

	cv::imshow(winTitle_original, image);
	cv::imshow(winTitle_corners_prewitt, image_prewitt);
	cv::imshow(winTitle_corners_sobel, image_sobel);

	cv::waitKey();
	return 0;
}