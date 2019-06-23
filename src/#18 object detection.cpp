#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <string>

std::vector<cv::KeyPoint> findFeaturesKAZE(const cv::Mat& oryginalImages) {
	auto KAZEfeatureDetector = cv::KAZE::create(300);
	std::vector<cv::KeyPoint> imagesKeypoints;

	KAZEfeatureDetector->detect(oryginalImages, imagesKeypoints);

	return imagesKeypoints;
}

cv::Mat computeDescriptors(const cv::Mat& image, std::vector<cv::KeyPoint>& features) {
	auto KAZEfeatureDescribtor = cv::KAZE::create();
	cv::Mat descriptors;

	KAZEfeatureDescribtor->compute(image, features, descriptors);

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
	double max = 0, min = 9999;
	for (const auto& match : matches)
	{
		if (match.distance < min)
			min = match.distance;

		if (match.distance > max)
			max = match.distance;
	}

	double mean1 = min * 1.6;
	double mean2 = (max + min) / 4;

	for (auto itr = matches.begin(); itr != matches.end(); )
		if (itr->distance > mean2)
			itr = matches.erase(itr);
		else
			itr++;
}

int main() {
	cv::Mat objectImg = cv::imread("resources/objectRecognition/obiekt4_s.png", cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::imshow("object", objectImg);

	cv::Mat sceneImg = cv::imread("resources/objectRecognition/scena1.png", cv::ImreadModes::IMREAD_GRAYSCALE);
	cv::imshow("scene", sceneImg);

	auto objImgFeatures = findFeaturesKAZE(objectImg);
	auto sceneImgFeatures = findFeaturesKAZE(sceneImg);

	auto objDescriptors = computeDescriptors(objectImg, objImgFeatures);
	auto sceneDescriptors = computeDescriptors(sceneImg, sceneImgFeatures);

	auto matches = findMatches(objDescriptors, sceneDescriptors);

	eraseBadMatches(matches);

	cv::Mat matchingResult;

	cv::drawMatches(
		objectImg, objImgFeatures,
		sceneImg, sceneImgFeatures,
		matches, matchingResult);

	//finding object

	std::vector<cv::Point2f> objectKeypoints;
	std::vector<cv::Point2f> objectKeypointsOnScene;

	for (size_t i = 0; i < matches.size(); i++) {
			objectKeypoints.push_back(
				objImgFeatures[matches[i].queryIdx].pt
			);

			objectKeypointsOnScene.push_back(
				sceneImgFeatures[matches[i].trainIdx].pt
			);
	}

	for (int i = 0; i < objectKeypoints.size(); i++)
		std::cout << i + 1 << ". " << objectKeypoints[i] << '\t' << objectKeypointsOnScene[i] << std::endl;


	std::vector<cv::Point2f> sceneCorners(4);
	std::vector<cv::Point2f> objectCorners(4);
	objectCorners[0] = cv::Point2f(0, 0);
	objectCorners[1] = cv::Point2f(objectImg.cols, 0);
	objectCorners[2] = cv::Point2f(objectImg.cols, objectImg.rows);
	objectCorners[3] = cv::Point2f(0, objectImg.rows);

	//std::cout << objectCorners;
//	std::cout << sceneCorners;

	cv::Mat H = cv::findHomography(objectKeypoints, objectKeypointsOnScene, cv::RANSAC);
	std::cout << std::endl << H;
	if (H.empty() == false)
	{
		cv::perspectiveTransform(objectCorners, sceneCorners, H);

		cv::line(matchingResult,
			sceneCorners[0] + cv::Point2f(objectImg.cols, 0),
			sceneCorners[1] + cv::Point2f(objectImg.cols, 0),
			cv::Scalar(0, 255, 0), 3);

		cv::line(matchingResult,
			sceneCorners[1] + cv::Point2f(objectImg.cols, 0),
			sceneCorners[2] + cv::Point2f(objectImg.cols, 0),
			cv::Scalar(0, 255, 0), 3);

		cv::line(matchingResult,
			sceneCorners[2] + cv::Point2f(objectImg.cols, 0),
			sceneCorners[3] + cv::Point2f(objectImg.cols, 0),
			cv::Scalar(0, 255, 0), 3);

		cv::line(matchingResult,
			sceneCorners[3] + cv::Point2f(objectImg.cols, 0), sceneCorners[0] + cv::Point2f(objectImg.cols, 0),
			cv::Scalar(0, 255, 0), 3);


		for (const auto& corner : sceneCorners)
			cv::circle(matchingResult, cv::Point2f(corner.x + objectImg.cols, corner.y), 3, cv::Scalar(0, 0, 255), 2);
	}

	cv::imshow("matches", matchingResult);

	cv::waitKey();
}

