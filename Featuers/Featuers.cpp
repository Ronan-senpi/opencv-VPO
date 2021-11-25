#include <opencv2/opencv.hpp>
//#include <opencv2/features2d.hpp>

#include <iostream>
#include <string>
#include <cstdarg>

const float DISTANCE_COEF = 0.75f;
const int MIN_MATCH_COUNT = 8;
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
	cv::Mat leftImgG;
	leftImg.copyTo(leftImgG);
	cv::Mat rightImg;
	cv::VideoCapture cap("./img/set1/video.mp4");
	if (cap.isOpened() == false)
	{
		std::cout << "Cannot open the video file" << std::endl;
		std::cin.get(); //wait for any key press
		return -1;
	}

	auto orb = cv::ORB::create(750);

	orb->detect(leftImgG, leftKeypoints);
	orb->compute(leftImgG, leftKeypoints, leftDescriptor);
	while (true)
	{
		//Begin: detect points
		bool bSuccess = cap.read(rightImg);
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}

		cv::Mat rightImgG;
		rightImg.copyTo(rightImgG);
		orb->detect(rightImgG, rightKeypoints);
		orb->compute(rightImgG, rightKeypoints, rightDescriptor);

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
		//End: detect points
		//Begin: Clipping
		if (good.size() > MIN_MATCH_COUNT)
		{
			std::vector<cv::Point2f> obj, scene;

			for (auto i : good) {
				obj.push_back(leftKeypoints[i.queryIdx].pt);
				scene.push_back(rightKeypoints[i.trainIdx].pt);
			}
			cv::Mat H = cv::findHomography(obj, scene, cv::RANSAC);
			

			std::vector<cv::Point2f> obj_corners(4);
			obj_corners[0] = cv::Point2f(0, 0);
			obj_corners[1] = cv::Point2f((float)leftImg.cols, 0);
			obj_corners[2] = cv::Point2f((float)leftImg.cols, (float)leftImg.rows);
			obj_corners[3] = cv::Point2f(0, (float)leftImg.rows);
			std::vector<cv::Point2f> scene_corners(4);

			cv::perspectiveTransform(obj_corners, scene_corners, H);

			cv::line(knnImg, scene_corners[0] + cv::Point2f((float)leftImg.cols, 0),
				scene_corners[1] + cv::Point2f((float)leftImg.cols, 0), cv::Scalar(0, 255, 0), 4);
			cv::line(knnImg, scene_corners[1] + cv::Point2f((float)leftImg.cols, 0),
				scene_corners[2] + cv::Point2f((float)leftImg.cols, 0), cv::Scalar(0, 255, 0), 4);
			cv::line(knnImg, scene_corners[2] + cv::Point2f((float)leftImg.cols, 0),
				scene_corners[3] + cv::Point2f((float)leftImg.cols, 0), cv::Scalar(0, 255, 0), 4);
			cv::line(knnImg, scene_corners[3] + cv::Point2f((float)leftImg.cols, 0),
				scene_corners[0] + cv::Point2f((float)leftImg.cols, 0), cv::Scalar(0, 255, 0), 4);
		}
		//End: Clipping

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
	compare2Img();
	compareImgVid();
	return 1;
}

