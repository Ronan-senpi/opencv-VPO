#include <opencv2/opencv.hpp>
//#include <opencv2/features2d.hpp>

#include <string>
#include <cstdarg>
#include <fstream>
#include <iostream>

#include "src/Image.h"
const float DISTANCE_COEF = 0.74f;
const int MIN_MATCH_COUNT = 7;
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






int compareImgVid(int set = 1) {
	auto orb = cv::ORB::create(750);
	auto bfm = cv::BFMatcher::create();


	std::string path = "./img/set"+ std::to_string(set)+"/";
	std::vector<Image> leftImgs;
	Image::prepareImgs(path, leftImgs, orb);

	//return 1;
	if (leftImgs.empty()) // Check for failure
	{
		std::cout << "leftImgs is empty" << std::endl;
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

	while (true)
	{
		//Begin: detect points
		bool bSuccess = cap.read(rightImg);
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}
		cv::Mat matRightG;
		cv::cvtColor(rightImg, matRightG, cv::COLOR_RGB2GRAY);
		Image rightImgG(matRightG, orb);


		cv::Mat knnImg(rightImg);

		for (auto i : leftImgs) {
			i.knnMatch(bfm, rightImgG, DISTANCE_COEF);
			//cv:drawMatches(i.mat, i.keypoints, matRight, rightImgG.keypoints, i.matchesGood, knnImg);
			i.Clipping(rightImgG, knnImg, MIN_MATCH_COUNT);
		}

		//End: detect points
		//cv::imshow("Good Matches & Object detection", img_matches);
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
   //compare2Img();
	compareImgVid();
	return 1;
}

