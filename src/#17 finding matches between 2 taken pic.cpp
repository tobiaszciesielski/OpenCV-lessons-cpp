#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

cv::Mat takePhoto() {
	cv::VideoCapture camera(0);
	cv::Mat capturedImage;
	const std::string cameraWindowTitle = "Press 'SPACE' to take a photo";
	const std::string agreementWinTitle = "Do you like this image? yes('y')/no('another')";
	
	int agreeKey = 0;
	do {
		int takingPicKey = 0;
		while (takingPicKey != ' ') {
			camera >> capturedImage;
			cv::imshow(cameraWindowTitle, capturedImage);
			takingPicKey = cv::waitKey(1);
		}
		cv::destroyWindow(cameraWindowTitle);

		cv::imshow(agreementWinTitle, capturedImage);
		agreeKey = cv::waitKey(0);
		cv::destroyWindow(agreementWinTitle);

	} while (agreeKey != 'y');

	camera.release();
	return capturedImage;
}

std::vector<cv::KeyPoint> findFeaturesORB(const cv::Mat& oryginalImages) {
	auto ORBfeatureDetector = cv::ORB::create(300);
	std::vector<cv::KeyPoint> imagesKeypoints;

	ORBfeatureDetector->detect(oryginalImages, imagesKeypoints);

	return imagesKeypoints;
}

cv::Mat computeDescriptors(const cv::Mat& image, std::vector<cv::KeyPoint>& features) {
	auto ORBfeatureDescribtor = cv::ORB::create();
	cv::Mat descriptors;

	ORBfeatureDescribtor->compute(image, features, descriptors);

	return descriptors;
}

std::vector<cv::DMatch> findMatches(const cv::Mat& descriptors1, const cv::Mat& descriptors2) {
	auto matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
	std::vector<cv::DMatch> matches;
	
	matcher->match(descriptors1, descriptors2, matches);
	
	return matches;
}

void eraseBadMatches(std::vector<cv::DMatch>& matches)
{
	double meanOfDistance = 0;
	for (const auto& m : matches)
		meanOfDistance += m.distance;

	meanOfDistance = meanOfDistance / matches.size();

	for (auto itr = matches.begin(); itr != matches.end(); itr++)
		if (itr->distance > meanOfDistance)
			itr = matches.erase(itr);
		else
			itr++;
}

int main() {
	const std::string tmpImgWinTitle = "Template image";
	const std::string compImgWinTitle = "Comparative image";
	
	cv::Mat capturedImage = takePhoto();
	cv::imshow(tmpImgWinTitle, capturedImage);

	cv::Mat comparativeImage = takePhoto();
	cv::imshow(compImgWinTitle, comparativeImage);
	
	cv::waitKey(0);

	auto capImgFeatures = findFeaturesORB(capturedImage);
	auto compImgFeatures = findFeaturesORB(comparativeImage);

	auto capImgDescriptors = computeDescriptors(capturedImage, capImgFeatures);
	auto compImgDescriptors = computeDescriptors(comparativeImage, compImgFeatures);

	cv::Mat matchingResult;
	auto matches = findMatches(capImgDescriptors, compImgDescriptors);

	std::cout << "Before: " << matches.size() << std::endl;
	eraseBadMatches(matches);
	std::cout << "After: " << matches.size() << std::endl;
q
	cv::drawMatches(
		capturedImage, capImgFeatures, 
		comparativeImage, compImgFeatures, 
		matches, matchingResult);

	cv::imshow("Final effect", matchingResult);

	cv::waitKey(0);
}