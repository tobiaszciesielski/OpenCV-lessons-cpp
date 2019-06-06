#include <opencv2/opencv.hpp>
#include <string>
#include <cstdlib>

#include "TimerBase.h"

using std::string;
using namespace cv;

// window title
const string window_title = "Window";

void NegativeDirectMemoryAccess(Mat& img)
{
	if (img.isContinuous())
	{
		int numberOfChannels = img.channels();
		int numberOfPixels = img.rows * img.cols * numberOfChannels;
		uchar* p = img.data;
		for (int i = 0; i < numberOfPixels; i = i + numberOfChannels)
		{
			for (int j = 0; j < numberOfChannels; ++j)
			{
				p[i + j] = 255 - p[i + j];
			}
		}
	}
	else
	{
		std::cout << "Mat is not continous" << std::endl;
	}
}

void NegativeDirectMemoryAccess2(Mat& img)
{
		int numberOfChannels = img.channels();
		int numberOfPixels = img.rows * img.cols * numberOfChannels;
		uchar* p = img.data;
		for (int i = 0; i < numberOfPixels; i++)
		{
			p[i] = 255 - p[i];
		}
}

void NegativeIterator(Mat& img)
{
	int numberOfChannels = img.channels();

	if (numberOfChannels == 1)
	{
		MatIterator_<uchar> it, end;
		for (it = img.begin<uchar>(), end = img.end<uchar>(); it != end; ++it)
		{
			*it = 255 - *it;
		}
	}
	else if (numberOfChannels == 3)
	{
		MatIterator_<Vec<uchar, 3>> it, end;
		for (it = img.begin<Vec<uchar, 3>>(), end = img.end<Vec<uchar, 3>>(); it != end; ++it)
		{
			// there are two approaches to get Vec values:
			// through val filed
			(*it).val[0] = 255 - (*it).val[0];
			// or directly
			(*it)[1] = 255 - (*it)[1];

			(*it).val[2] = 255 - (*it).val[2];
		}
	}
	else
	{
		std::cout << "Unsuported number of channels" << std::endl;
	}
}

void NegativeRandomAccess(Mat& img)
{
	// this function works only for color images
	typedef Vec<uchar, 3> Pixel;

	int rows = img.rows;
	int cols = img.cols;

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			img.at<Pixel>(i, j).val[0] = 255 - img.at<Pixel>(i, j).val[0];
			img.at<Pixel>(i, j).val[1] = 255 - img.at<Pixel>(i, j).val[1];
			img.at<Pixel>(i, j).val[2] = 255 - img.at<Pixel>(i, j).val[2];
		}
	}
}

void NegativRawPtrAccess(Mat& img)
{
	// this function works only for color images
	typedef Vec<uchar, 3> Pixel;

	for (int r = 0; r < img.rows; ++r)
	{
		Pixel* ptr = img.ptr<Pixel>(r, 0);
		const Pixel* ptr_end = ptr + img.cols;
		for (; ptr != ptr_end; ++ptr)
		{
			ptr->val[0] = 255 - ptr->val[0];
			ptr->val[1] = 255 - ptr->val[1];
			ptr->val[2] = 255 - ptr->val[2];
		}
	}
}

void NegativeMatIterator(Mat& img)
{
	// this function works only for color images
	typedef Vec<uchar, 3> Pixel;

	for (Pixel &p : cv::Mat_<Pixel>(img))
	{
		p.val[0] = 255 - p.val[0];
		p.val[1] = 255 - p.val[1];
		p.val[2] = 255 - p.val[2];
	}
}

void NegativeLambda(Mat& img)
{
	// this function works only for color images
	typedef Vec<uchar, 3> Pixel;

	// using C++11 lambda.
	img.forEach<Pixel>([](Pixel &p, const int * position) -> void {
		p.val[0] = 255 - p.val[0];
		p.val[1] = 255 - p.val[1];
		p.val[2] = 255 - p.val[2];
	});
}

unsigned long check_time(cv::Mat mat, void(*func)(cv::Mat&), const int calls)
{
	cTimer timer;
	timer.start();

	for (int i = 0; i < calls; i++)
	{
		cv::Mat m = mat.clone();
		func(m);
	}

	return timer.getTime(cTimer::timeUnit::Microseconds) / calls;
}

int main()
{
	cv::Mat img = cv::imread("lena.jpg");
	cTimer timer;

	const int funcCalls = 1000;
	std::cout << "NegativeDirectMemoryAccess: " << check_time(img, NegativeDirectMemoryAccess, funcCalls) << "\n";
	std::cout << "NegativeDirectMemoryAccess2: " << check_time(img, NegativeDirectMemoryAccess2, funcCalls) << "\n";
	std::cout << "NegativeIterator: " << check_time(img, NegativeIterator, funcCalls) << "\n";
	std::cout << "NegativeRandomAccess: " << check_time(img, NegativeRandomAccess, funcCalls) << "\n";
	std::cout << "NegativRawPtrAccess: " << check_time(img, NegativRawPtrAccess, funcCalls) << "\n";
	std::cout << "NegativeMatIterator: " << check_time(img, NegativeMatIterator, funcCalls) << "\n";
	std::cout << "NegativeLambda: " << check_time(img, NegativeLambda, funcCalls) << "\n";

	std::cin.get();
	return 0;
}