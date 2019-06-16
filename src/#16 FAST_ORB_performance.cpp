#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <chrono>
#include "src/TimerBase.h"

std::ostream& operator<<(std::ostream& stream, const std::vector<cv::KeyPoint> vecOfVec)
{
	int i = 0;
	stream << "Keypoints: " << std::endl;
	for (auto& vec : vecOfVec)
		stream << ++i << ". [" << vec.pt.x << ", " << vec.pt.y << "]" << std::endl;

	return stream;
}

std::vector<cv::Mat> loadImages(std::string imagesPath, std::string imageName, const int imageCount, std::string imageExtension)
{
	std::vector<cv::Mat> images;
	for (int i = 1; i <= imageCount; i++)
		images.push_back(cv::imread(imagesPath + imageName + std::to_string(i) + imageExtension));

	return images;
}

void displayVectorImages(std::vector<cv::Mat> imagesVector, const std::string windowTitle)
{
	for (const auto& img : imagesVector)
	{
		cv::imshow(windowTitle, img);
		cv::waitKey(0);
	}
}

void compareVectorImages(std::vector<cv::Mat> firstVec, std::string fstTitle, std::vector<cv::Mat> secondVec, std::string secTitle)
{
	if (firstVec.size() != secondVec.size())
		return;

	for (int i = 0; i < firstVec.size(); i++)
	{
		cv::imshow(fstTitle, firstVec[i]);
		cv::imshow(secTitle, secondVec[i]);
		cv::waitKey(0);
	}
}

void displayFASTParam(cv::Ptr<cv::FastFeatureDetector> det)
{
	std::cout << "DefaultName: " << det->getDefaultName() << std::endl;
	std::cout << "Threshold: " << det->getThreshold() << std::endl;
	std::cout << "NonmaxSuppression: " << det->getNonmaxSuppression() << std::endl;
	std::cout << "FeatureDetectorType: " << det->getType() << std::endl;
}

void cloneImageVector(const std::vector<cv::Mat>& baseVector, std::vector<cv::Mat>& dstVector)
{
	if (baseVector.size() != dstVector.size())
		return;

	for (int i = 0; i < dstVector.size(); i++)
		dstVector.at(i) = baseVector.at(i).clone();
}

void drawKeypoints(std::vector<cv::Mat>& images,
	std::vector<std::vector<cv::KeyPoint>>& keypoints,
	std::vector<cv::Mat>& dstImages)
{
	if (images.size() != keypoints.size())
		return;

	for (int i = 0; i < images.size(); i++)
		cv::drawKeypoints(images.at(i), keypoints.at(i), dstImages.at(i));
}

std::vector<std::vector<cv::KeyPoint>> findFeaturesFAST(const std::vector<cv::Mat>& oryginalImages)
{
	auto FASTfeatureDetector = cv::FastFeatureDetector::create(90, true, cv::FastFeatureDetector::TYPE_9_16);

	std::vector< std::vector<cv::KeyPoint>> imagesKeypoints;

	cTimer timer;
	timer.start();

	FASTfeatureDetector->detect(oryginalImages, imagesKeypoints);

	std::cout << "FAST: " << timer.getTime(cTimer::timeUnit::Microseconds) << std::endl;
	timer.~cTimer();

	return imagesKeypoints;
}

std::vector<std::vector<cv::KeyPoint>> findFeaturesORB(std::vector<cv::Mat> oryginalImages)
{
	auto ORBfeatureDetector = cv::ORB::create(1000);

	std::vector< std::vector<cv::KeyPoint>> imagesKeypoints;

	cTimer timer;
	timer.start();

	ORBfeatureDetector->detect(oryginalImages, imagesKeypoints);

	std::cout << "ORB: " << timer.getTime(cTimer::timeUnit::Microseconds) << std::endl;
	timer.~cTimer();

	return imagesKeypoints;
}

void precessImages(const std::string name, std::vector<cv::Mat>& originalImages)
{
	// create new vector and clone images
	std::vector<cv::Mat> imagesWithFASTfeatures(originalImages.size());
	cloneImageVector(originalImages, imagesWithFASTfeatures);

	cv::Mat templateImage = originalImages.at(0).clone();
	
	std::vector<cv::Mat> imagesWithORBfeatures(originalImages.size());
	cloneImageVector(originalImages, imagesWithORBfeatures);

	// find features and draw keypoints
	auto foundFeaturesWithFAST = findFeaturesFAST(originalImages);
	drawKeypoints(originalImages, foundFeaturesWithFAST, imagesWithFASTfeatures);
	
	//int pos1 = 1;
	//for (auto& i : foundFeaturesWithFAST.at(0))
	//{
	//	std::cout << pos1++ << ". keypoint: " << std::endl;
	//	std::cout << ". angle:" << '\t' << i.angle << std::endl;
	//	std::cout << ". class_id:" << '\t' << i.class_id << std::endl;
	//	std::cout << ". octave:" << '\t' << i.octave << std::endl;
	//	std::cout << ". pt:" << '\t' << i.pt << std::endl;
	//	std::cout << ". response:" << '\t' << i.response << std::endl;
	//	std::cout << ". size:" << '\t' << i.size << std::endl << std::endl;
	//}

	auto foundFeaturesWithORB = findFeaturesORB(originalImages);
	drawKeypoints(originalImages, foundFeaturesWithORB, imagesWithORBfeatures);
	
	/*int pos2 = 1;
	for (auto& i : foundFeaturesWithORB.at(0))
	{
		if (i.pt.x == 570)
		{
			
			std::cout << pos2++ << ". keypoint: " << std::endl;
			std::cout  << ". angle:"		<<	'\t' << i.angle		<< std::endl;
			std::cout  << ". class_id:"		<<	'\t' << i.class_id	<< std::endl;
			std::cout  << ". octave:"		<<	'\t' << i.octave	<< std::endl;
			std::cout  << ". pt:"			<<	'\t' << i.pt		<< std::endl;
			std::cout  << ". response:"		<<	'\t' << i.response	<< std::endl;
			std::cout  << ". size:"			<<	'\t' << i.size		<< std::endl << std::endl;
		}
	}*/

	// lets see results
	compareVectorImages(imagesWithORBfeatures, "ORB", imagesWithFASTfeatures, "FAST");
	
	/*
	auto ORBfeatureDescribtor = cv::ORB::create();
	std::vector<cv::Mat> descriptors;

	ORBfeatureDescribtor->compute(originalImages, foundFeaturesWithORB, descriptors);
		
	cv::Mat descriptorScrap(descriptors[0], cv::Rect(0, 0, 10, 10));
	std::cout << descriptorScrap;

	auto matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
		
	try
	{
		for (int i = 0; i < originalImages.size(); i++)
		{
			std::vector<cv::DMatch> matches;
			matcher->match(descriptors[i], descriptors[0], matches);

			std::cout << "matches count: "	<< '\t' << matches.size()		<< std::endl;
			std::cout << "distance: "		<< '\t' << matches[0].distance	<< std::endl;
			std::cout << "imgIdx: "			<< '\t' << matches[0].imgIdx	<< std::endl;
			std::cout << "queryIdx: "		<< '\t' << matches[0].queryIdx	<< std::endl;
			std::cout << "trainIdx: "		<< '\t' << matches[0].trainIdx	<< std::endl << std::endl;

			cv::Mat imageWithMatches;

			cv::drawMatches(templateImage, foundFeaturesWithFAST[0], originalImages[i], foundFeaturesWithFAST[i], matches, imageWithMatches);
			cv::imshow("Matching result", imageWithMatches);

			cv::waitKey(0);
		}

	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	*/
}

int main()
{
	using std::string;

	const string imagesPath = "resources/detection_description_match/";
	const string imageExtension = ".bmp";

	auto scaledImages = loadImages(imagesPath, "forward-", 6, imageExtension);
	auto rotatedImages = loadImages(imagesPath, "perspective-", 5, imageExtension);
	auto distortedImages = loadImages(imagesPath, "rotate-", 8, imageExtension);

	precessImages("Scaled images", scaledImages);
	cv::waitKey(0);

	precessImages("Rotated images", rotatedImages);
	cv::waitKey(0);

	precessImages("Distorted images", distortedImages);
	cv::waitKey(0);

	//Two images FAST detector
	{
		//{
			//auto fastFeatureDetector = cv::FastFeatureDetector::create(90, true, cv::FastFeatureDetector::TYPE_9_16);

			//cv::Mat firstScaledImage = scaledImages[0].clone();
			//cv::Mat secondScaledImage;

			//try
			//{
			//	secondScaledImage = scaledImages.at(6).clone();
			//}
			//catch (const std::exception& e)
			//{
			//	std::cout << "We caught exception! Here it is: " << e.what() << std::endl;
			//	secondScaledImage = scaledImages.at(5).clone();
			//}

			//std::vector<cv::KeyPoint> firstImageKeypoints;
			//std::vector<cv::KeyPoint> secondImageKeypoints;

			//displayFASTParam(fastFeatureDetector);

			//fastFeatureDetector->detect(firstScaledImage, firstImageKeypoints);
			//fastFeatureDetector->detect(secondScaledImage, secondImageKeypoints);

			////std::cout << firstImageKeypoints.size() << std::endl;
			////std::cout << firstImageKeypoints;

			////std::cout << secondImageKeypoints.size() << std::endl;
			////std::cout << secondImageKeypoints;

			//cv::Mat firstScaledImageWithKeypoints = firstScaledImage.clone();
			//cv::Mat secondScaledImageWithKeypoints = secondScaledImage.clone();

			//cv::drawKeypoints(firstScaledImage, firstImageKeypoints, firstScaledImageWithKeypoints);
			//cv::drawKeypoints(secondScaledImage, secondImageKeypoints, secondScaledImageWithKeypoints);

			//cv::imshow("1st Image Keypoints", firstScaledImageWithKeypoints);
			//cv::imshow("2nd Image Keypoints", secondScaledImageWithKeypoints);
		//}
		//cv::waitKey(0);
		//cv::destroyAllWindows();
	}
}
