#include <opencv2/opencv.hpp>
//#include <opencv2/features2d.hpp>

#include <iostream>
#include <string>
#include <cstdarg>

const float DISTANCE_COEF = 0.6f;
int compare2Img() {

	cv::Mat leftDescriptor, rightDescriptor;
	std::vector<cv::KeyPoint> leftKeypoints, rightKeypoints;

	cv::Mat leftImg = cv::imread("./img/set1/bleach.jpg");
	cv::Mat rightImg = cv::imread("./img/set1/bleach.png");

	auto orb = cv::ORB::create();

	orb->detect(leftImg, leftKeypoints);
	orb->compute(leftImg, leftKeypoints, leftDescriptor);

	orb->detect(rightImg, rightKeypoints);
	orb->compute(rightImg, rightKeypoints, rightDescriptor);

	auto bfm = cv::BFMatcher::create();
	std::vector<std::vector<cv::DMatch>> matches;
	bfm->knnMatch(leftDescriptor, rightDescriptor, matches, 2);

	std::vector<cv::DMatch> good;
	for (auto elem : matches) {
		if (elem[0].distance < DISTANCE_COEF * elem[1].distance) {
			good.push_back(elem[0]);
		}
	}

	cv::Mat knnImg;
cv:drawMatches(leftImg, leftKeypoints, rightImg, rightKeypoints, good, knnImg);

	//cv::Mat kleftImg;
	//cv::drawKeypoints(leftImg, leftKeypoints, kleftImg);

	//cv::Mat krightImg;
	//cv::drawKeypoints(rightImg, rightKeypoints, krightImg);

	//cv::imshow("leftImg", kleftImg);
	cv::imshow("knnImg", knnImg);
	cv::waitKey(0);
	return 0;
}

int compareImgVid() {

	cv::Mat leftDescriptor, rightDescriptor;
	std::vector<cv::KeyPoint> leftKeypoints, rightKeypoints;

	cv::Mat leftImg = cv::imread("./img/set1/naruto.jpg");
	if (leftImg.empty()) // Check for failure
	{
		std::cout << "Could not open or find the image" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}

	cv::Mat rightImg;
	cv::VideoCapture cap("./img/set1/video.mp4");
	if (cap.isOpened() == false)
	{
		std::cout << "Cannot open the video file" << std::endl;
		std::cin.get(); //wait for any key press
		return -1;
	}

	auto orb = cv::ORB::create(1000);

	orb->detect(leftImg, leftKeypoints);
	orb->compute(leftImg, leftKeypoints, leftDescriptor);
	while (true)
	{
		bool bSuccess = cap.read(rightImg);
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}
		orb->detect(rightImg, rightKeypoints);
		orb->compute(rightImg, rightKeypoints, rightDescriptor);

		auto bfm = cv::BFMatcher::create();
		std::vector<std::vector<cv::DMatch>> matches;
		bfm->knnMatch(leftDescriptor, rightDescriptor, matches, 2);

		std::vector<cv::DMatch> good;
		for (auto elem : matches) {
			if (elem[0].distance < DISTANCE_COEF * elem[1].distance) {
				good.push_back(elem[0]);
			}
		}

		cv::Mat knnImg;
		cv:drawMatches(leftImg, leftKeypoints, rightImg, rightKeypoints, good, knnImg);

		cv::imshow("knnImg", knnImg);
		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
			break;
		}
	}
	return 0;
}

int main(int argc, char** argv)
{
	compare2Img();
	compareImgVid();
	return 1;
}

