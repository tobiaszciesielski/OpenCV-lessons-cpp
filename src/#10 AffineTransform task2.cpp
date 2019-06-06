#include <opencv2/opencv.hpp>
#include <string>

using std::string;

const string winTitle_source = "Source Image";
const string winTitle_destination = "Destination Image";

cv::Mat srcImage;
cv::Mat dstImage;

std::ostream& operator<<(std::ostream& stream, const std::vector<cv::Point> vec)
{
	int i = 0;
	for (auto& p : vec)
		stream  << ++i << " [" << p.x << " , " << p.y << "]" << std::endl;

	return stream;
}

//460x290
cv::Mat getTransform(const std::vector<cv::Point2f>& markedPoints, const cv::Size &imageSize)
{
	std::vector<cv::Point2f> imageArea = {
		{0,0},
		{float(imageSize.width), 0},
		{float(imageSize.width), float(imageSize.height)},
	};

	return cv::getAffineTransform(markedPoints, imageArea);
}

static void mouseCallbackFunc(int mouseEvent, int x, int y,int flags, void*)
{
	static std::vector<cv::Point2f> markedPoints;

	if (mouseEvent == cv::MouseEventTypes::EVENT_LBUTTONDOWN)
	{
		markedPoints.push_back({ float(x), float(y) });
		
		system("cls");
		std::cout << "Vector contains:" << std::endl << markedPoints;
	}
	
	if (markedPoints.size() == 3)
	{
		cv::Mat affineTransformMatrix = getTransform(markedPoints, srcImage.size());
		cv::warpAffine(srcImage, dstImage, affineTransformMatrix, srcImage.size());
		
		markedPoints.clear();
		
		cv::namedWindow(winTitle_destination);
		cv::imshow(winTitle_destination, dstImage);
	}
}

int main()
{
	srcImage = cv::imread("resources/PerspectiveTransform/pug.png");
	dstImage = srcImage.clone();

	cv::namedWindow(winTitle_source);

	cv::setMouseCallback(winTitle_source, mouseCallbackFunc);

	int key = 0;
	while (key != 'q')
	{
		cv::imshow(winTitle_source, srcImage);		
		key = cv::waitKey();
	}

	return 0;
}