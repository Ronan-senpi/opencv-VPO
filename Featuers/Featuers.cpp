#include <opencv2/opencv.hpp>
//#include <opencv2/features2d.hpp>

#include <string>
#include <cstdarg>
#include <fstream>
#include <iostream>

#include "src/Image.h"
const float DISTANCE_COEF = 0.6f;
const int MIN_MATCH_COUNT = 5;

int compareImgVid(int set = 2) {
	auto orbVid = cv::ORB::create(900);
	auto orbImg = cv::ORB::create(500);
	auto bfm = cv::BFMatcher::create();


	std::string path = "./img/set"+ std::to_string(set)+"/";
	std::vector<Image> leftImgs;
	Image::prepareImgs(path, leftImgs, orbImg);

	//return 1;
	if (leftImgs.empty()) // Check for failure
	{
		std::cout << "leftImgs is empty" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}

	cv::Mat rightImg;
	cv::VideoCapture cap("./img/set" + std::to_string(set) + "/video.mp4");
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
		Image rightImgG(matRightG, orbVid);


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

