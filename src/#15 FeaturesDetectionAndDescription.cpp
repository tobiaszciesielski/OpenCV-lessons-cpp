#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <chrono>

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

void drawKeypoints(	const std::vector<cv::Mat>& images, 
					const std::vector<std::vector<cv::KeyPoint>>& keypoints,
					std::vector<cv::Mat>& dstImages) 
{
	if (images.size() != keypoints.size())
		return;

	for (int i = 0; i < images.size(); i++)
		cv::drawKeypoints(images.at(i), keypoints.at(i), dstImages.at(i));
}

std::vector<cv::Mat> findFeaturesFAST(const std::vector<cv::Mat>& oryginalImages)
{
	auto FASTfeatureDetector = cv::FastFeatureDetector::create(90, true, cv::FastFeatureDetector::TYPE_9_16);
	std::vector< std::vector<cv::KeyPoint>> imagesKeypoints;

	std::vector<cv::Mat> scaledImagesWithKeypoints(oryginalImages.size());

	auto start = std::chrono::system_clock::now();

	cloneImageVector(oryginalImages, scaledImagesWithKeypoints);

	auto end = std::chrono::system_clock::now();
	auto elapsed = end - start;
	std::cout << "FAST: " << elapsed.count() << '\n';
	
	FASTfeatureDetector->detect(oryginalImages, imagesKeypoints);

	drawKeypoints(oryginalImages, imagesKeypoints, scaledImagesWithKeypoints);

	return scaledImagesWithKeypoints;
}

std::vector<cv::Mat> findFeaturesORB(std::vector<cv::Mat> oryginalImages)
{
	auto ORBfeatureDetector = cv::ORB::create(1000);

	std::vector< std::vector<cv::KeyPoint>> allImagesKeypoints;
	std::vector<cv::Mat> scaledImagesWithKeypoints(oryginalImages.size());

	for (int i = 0; i < oryginalImages.size(); i++)
		scaledImagesWithKeypoints.at(i) = oryginalImages.at(i).clone();

	auto start = std::chrono::system_clock::now();

	ORBfeatureDetector->detect(oryginalImages, allImagesKeypoints);
	
	auto end = std::chrono::system_clock::now();
	auto elapsed = end - start;
	std::cout << "ORB: " << elapsed.count() << '\n';

	for (int i = 0; i < oryginalImages.size(); i++)
		cv::drawKeypoints(scaledImagesWithKeypoints.at(i), allImagesKeypoints.at(i), scaledImagesWithKeypoints.at(i));

	return scaledImagesWithKeypoints;
}

void findAndCompareFeatures(const std::string name, const std::vector<cv::Mat>& collection)
{
	std::cout << name << " : " << std::endl;
	auto foundFeaturesWithFAST = findFeaturesFAST(collection);
	auto foundFeaturesWithORB = findFeaturesORB(collection);
	compareVectorImages(foundFeaturesWithORB, "ORB", foundFeaturesWithFAST, "FAST");
}

int main()
{
	using std::string;

	const string imagesPath = "resources/detection_description_match/";
	const string imageExtension = ".bmp";

	auto scaledImages = loadImages(imagesPath, "forward-", 6, imageExtension);
	auto rotatedImages = loadImages(imagesPath, "perspective-", 5, imageExtension);
	auto distortedImages = loadImages(imagesPath, "rotate-", 8, imageExtension);

	findAndCompareFeatures("Scaled images", scaledImages);
	cv::waitKey(0);

	findAndCompareFeatures("Rotated images", scaledImages);
	cv::waitKey(0);

	findAndCompareFeatures("Distorted images", distortedImages);
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

//TODO:
//	Detekcja cech algorytmem FAST		v/
//	Znalezione cechy nanieœæ na obraz	v/
//
//	Detekcja cech algorytmem ORB		v/
//	Znalezione cechy nanieœæ na obraz	v/
//
//	Porównaæ dok³adnoœæ algorytmów znajdywania cech		v/
//	Porównaæ szybkoœæ algorytmów znajdywania cech		v/
//	Porównaæ szybkoœæ algorytmów znajdywania cech		v/
//
//	Dokonaæ deskrypcji cech algorytmem OR	-
//	(metoda compute -> wynik w Mat)
//
//	Wykonaæ operacjê dopasowania cech		-
//	(metoda match)