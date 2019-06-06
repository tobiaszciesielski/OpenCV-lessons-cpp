#include <opencv2/opencv.hpp>
#include <string>

using std::string;

const string winTitle = "Window";
cv::Mat image; 
cv::Point vec1(0,0), vec2(0,0);

void thresholdArea(cv::Mat& image, cv::Rect area, int channel)
{
	if (channel > 3 || channel < 0) 
	{
		std::cout << "Error! Unidentified number of channel!\n";
			return;
	}

	typedef cv::Vec<uchar, 3> Pixel;

	int dxStart = area.x, dxEnd = area.x + area.width;
	int dyStart = area.y, dyEnd = area.y + area.height;

	for (int i = dyStart; i < dyEnd; ++i)
	{
		for (int j = dxStart; j < dxEnd; ++j)
		{	
			auto& pixelValue = image.at<Pixel>(i, j).val[channel];

			if (pixelValue < 128)
				pixelValue = 0;
			else 
				pixelValue = 255;
		}
	}

	//Drawing rectangle around thresholded area
	cv::rectangle(image, area, cv::Scalar(0, 0, 255), 1);
}

static void mouseCallback(int event, int x, int y, int flags, void*)
{
	if (event == cv::MouseEventTypes::EVENT_LBUTTONDOWN)
		vec1 = cv::Point2i(x, y);

	if (event == cv::MouseEventTypes::EVENT_LBUTTONUP)
	{
		vec2 = cv::Point2i(x, y);
		thresholdArea(image, cv::Rect(vec1, vec2), 1);
	}
}

int main()
{
	image = cv::imread("lena.jpg");

	int key = 0;
	while (key != ' ')
	{
		cv::setMouseCallback(winTitle, mouseCallback);
		cv::imshow(winTitle, image);

		key = cv::waitKey(20);
	}

	return 0;
}