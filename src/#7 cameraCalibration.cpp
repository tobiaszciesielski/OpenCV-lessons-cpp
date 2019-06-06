#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const std::string winTitle_original = "Original";
const std::string winTitle_calibrated = "Calibrated";

std::vector<cv::Mat> loadInput()
{
	std::vector<cv::Mat> inputPhotos;
	for (size_t i = 0; i < 5; i++)
	{
		std::string imgName;

		if (i < 10)
			imgName = "resources/Calibration/img_21130751_000" + std::to_string(i) + ".bmp";
		else if (i < 100)
			imgName = "resources/Calibration/img_21130751_00" + std::to_string(i) + ".bmp";
		else 
			imgName = "resources/Calibration/img_21130751_0" + std::to_string(i) + ".bmp";

		inputPhotos.emplace_back(cv::imread(imgName));
	}

	return inputPhotos;
}

void cornerDetection(std::vector<cv::Mat> input)
{
	std::vector<std::vector<cv::Point2f>> cornersFromAllImages;
	
	auto flags = cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK;
	auto chessboardSize = cv::Size(8, 5);

	for (auto& image : input)
	{
		std::vector<cv::Point2f> corners;

		if (cv::findChessboardCorners(image, chessboardSize, corners, flags))
		{
			cv::Mat grayScale;
			cv::cvtColor(image, grayScale, cv::COLOR_RGB2GRAY);

			cv::TermCriteria terms(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1);
			cv::cornerSubPix(grayScale, corners, cv::Size(11, 11), cv::Size(-1, -1), terms);

			cornersFromAllImages.push_back(corners);
		}
	}

	std::vector<std::vector<cv::Point3f>> cornersModelForAllImages;
	for (int i = 0; i < cornersFromAllImages.size(); ++i)
	{
		std::vector<cv::Point3f> cornersModel;

		for (int j = 0; j < chessboardSize.height; ++j)
			for (int k = 0; k < chessboardSize.width; ++k)
				cornersModel.push_back(cv::Point3f(k, j, 0.f));

		cornersModelForAllImages.push_back(cornersModel);
	}

	std::cout << cornersFromAllImages.size() << std::endl;
	std::cout << cornersModelForAllImages.size() << std::endl;

	cv::Mat cameraMatrix, distCoeffs;
	std::vector<cv::Mat> rvec, tvecs;

	double rms = cv::calibrateCamera(
		cornersModelForAllImages,
		cornersFromAllImages,
		input[0].size(),
		cameraMatrix, 
		distCoeffs, 
		rvec, 
		tvecs
	);

	std::cout << cameraMatrix << std::endl;
	std::cout << distCoeffs << std::endl;
	std::cout << rms << std::endl;


	cv::namedWindow(winTitle_original, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(winTitle_calibrated, cv::WINDOW_AUTOSIZE);

	for (size_t i = 0; i < input.size(); i++)
	{
		cv::Mat undistortImage = input[i].clone();
		cv::undistort(input[i], undistortImage, cameraMatrix, distCoeffs);
		
		cv::imshow(winTitle_original, input[i]);
		cv::imshow(winTitle_calibrated, undistortImage);
		cv::waitKey();
	}
}

int main()
{
	std::vector<cv::Mat> calibrationSet = loadInput();

	cornerDetection(calibrationSet);

	cv::waitKey(20);

	return 0;
}
