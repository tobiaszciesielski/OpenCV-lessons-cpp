#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const std::string winTitle_original = "Original";
const std::string winTitle_hough = "Hough";
const std::string winTitle_houghP = "HoughP";
const std::string winTitle_houghC = "HoughC";

void prepareImage(cv::Mat& input)
{
	cv::GaussianBlur(input, input, cv::Size(5, 5), 0.5);
	cv::Canny(input, input, 50, 200);
}

void houghTransform(cv::Mat& input)
{
	std::vector<cv::Vec2f> lines;
	
	prepareImage(input);
	cv::HoughLines(input, lines, 1, CV_PI / 180, 150);

	cv::cvtColor(input, input, cv::COLOR_GRAY2BGR);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));

		cv::line(input, pt1, pt2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	}
}

void probabilisticHoughTransform(cv::Mat& input)
{
	std::vector<cv::Vec4i> lines;

	prepareImage(input);
	cv::HoughLinesP(input, lines, 1, CV_PI / 180, 50, 50, 10); // runs the actual detection

	cv::cvtColor(input, input, cv::COLOR_GRAY2BGR);

	for (size_t i = 0; i < lines.size(); i++)
	{
		cv::Vec4i l = lines[i];
		cv::line(input, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
	}
}

void houghCircles(cv::Mat& input)
{
	std::vector<cv::Vec3f> circles;

	prepareImage(input);
	cv::HoughCircles(input, circles, cv::HOUGH_GRADIENT, 2,
		input.rows / 2,  // minimum distance between the centers of the detected circles
		50, 200, // two thresholds
		5, 80 // min and max radius to detect larger circles
	);

	cv::cvtColor(input, input, cv::COLOR_GRAY2BGR);

	for (size_t i = 0; i < circles.size(); i++)
	{
		cv::Vec3f c = circles[i];
		cv::Point center = cv::Point(c[0], c[1]);

		// circle center
		circle(input, center, 1, cv::Scalar(100, 100, 100), 3, cv::LINE_AA);

		// circle outline
		int radius = c[2];
		circle(input, center, radius, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
	}
}

int main()
{
	cv::VideoCapture capture(0);
	if (!capture.isOpened())
		return 0;

	cv::Mat edgesHugh;
	cv::Mat edgesHughP;
	cv::Mat edgesHughC;

	cv::namedWindow(winTitle_original, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(winTitle_hough, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(winTitle_houghP, CV_WINDOW_AUTOSIZE);

	int key = 0;
	while (key != ' ')
	{
		cv::Mat frame;
		capture >> frame;

		cv::cvtColor(frame, edgesHugh, CV_RGB2GRAY);
		cv::cvtColor(frame, edgesHughP, CV_RGB2GRAY);
		cv::cvtColor(frame, edgesHughC, CV_RGB2GRAY);

		houghTransform(edgesHugh);
		probabilisticHoughTransform(edgesHughP);
		houghCircles(edgesHughC);

		cv::imshow(winTitle_original, frame);
		cv::imshow(winTitle_hough, edgesHugh);
		cv::imshow(winTitle_houghP, edgesHughP);
		cv::imshow(winTitle_houghC, edgesHughC);

		key = cv::waitKey(20);
	}
	return 0;
}