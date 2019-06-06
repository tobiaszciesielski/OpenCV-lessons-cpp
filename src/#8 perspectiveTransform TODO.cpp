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
		stream << ++i << " [" << p.x << " , " << p.y << "]" << std::endl;

	return stream;
}

//460x290
cv::Mat getTransform(const std::vector<cv::Point2f>& markedPoints, const cv::Size &imageSize)
{
	auto imageSizef = cv::Size2f(imageSize);
	std::vector<cv::Point2f> imageArea = {
		{0, 0},
		{imageSizef.width, 0},
		{imageSizef.width, imageSizef.height},
		{0, imageSizef.height}
	};

	return cv::getPerspectiveTransform(markedPoints, imageArea);
}

static void mouseCallbackFunc(int mouseEvent, int x, int y, int flags, void*)
{
	static std::vector<cv::Point2f> markedPoints;
	static bool firstMark = false;
	static cv::Mat perspectiveTransform;

	if (mouseEvent == cv::MouseEventTypes::EVENT_LBUTTONDOWN)
	{ 
		markedPoints.push_back({ float(x), float(y) });

		system("cls");
		std::cout << "Vector contains:" << std::endl << markedPoints;
	}

	if (markedPoints.size() == 4)
	{
		if (firstMark == false)
		{
			perspectiveTransform = getTransform(markedPoints, srcImage.size());
			markedPoints.clear();

			cv::warpPerspective(srcImage, srcImage, perspectiveTransform, srcImage.size());
			cv::imshow(winTitle_source, srcImage);

			firstMark = true;
		}
		else if (firstMark = true)
		{
			//Here will be inserting marked area of srcImg into marked area of dstImg

			cv::imshow(winTitle_source, dstImage);
		}
	}
}

int main()
{
	srcImage = cv::imread("resources/PerspectiveTransform/pug.png");
	dstImage = cv::imread("resources/PerspectiveTransform/gallery.png");

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