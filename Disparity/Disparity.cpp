
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

std::vector<cv::Point2f> points1, points2;
const int minPoint = 10;

std::vector<cv::Point2f> purgePoint(std::vector<cv::Point2f>& points, std::vector<uchar>& status) {
	std::vector<cv::Point2f> result;
	for (size_t i = 0; i < points.size(); i++)
		if (status[i] > 0)
			result.push_back(points[i]);
	return result;
}

int main(int argc, char** argv)
{
	std::vector<uchar> status;
	std::vector<float> err;
	// Read the image file
	cv::Mat image1 = cv::imread("./img/set2/image1.jpg");
	if (image1.empty()) // Check for failure
	{
		std::cout << "Could not open or find the image 1" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}
	cv::Mat image1G;
	cv::cvtColor(image1, image1G, cv::COLOR_RGB2GRAY);

	cv::Mat image2 = cv::imread("./img/set2/image2.jpg");
	if (image2.empty()) // Check for failure
	{
		std::cout << "Could not open or find the image 2" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}
	cv::Mat image2G;
	cv::cvtColor(image2, image2G, cv::COLOR_RGB2GRAY);

	cv::goodFeaturesToTrack(image1G, points1, 3000, 0.05, 5);

	if (points1.size() < minPoint)
		return 1;

	cv::calcOpticalFlowPyrLK(image1G, image2G, points1, points2, status, err);
	points1 = purgePoint(points1, status);
	points2 = purgePoint(points2, status);

	cv::Mat hImg;
	std::vector<cv::Mat> imgs{ image1, image2 };
	cv::hconcat(imgs, hImg);
	
	for (auto p : points1) {
		cv::circle(hImg, p, 2, cv::Scalar(0, 255, 0), 2);
	}

	for (auto p : points2) {
		cv::Point2f np(image1G.cols + p.x, p.y);
		cv::circle(hImg, np, 2, cv::Scalar(0, 255, 0), 2);
	}

	for (size_t i = 0; i < points1.size(); i++)
	{
		cv::Point2f p2(image1G.cols + points2[i].x, points2[i].y);
		cv::line(hImg, points1[i], p2, cv::Scalar(255, 0, 0,100));
	}

	std::cout << points1.size() << " " << points2.size() << std::endl;
	cv::imshow("shooooow", hImg); // Show our image inside the created window.

	cv::waitKey(0); 

	return 0;
}