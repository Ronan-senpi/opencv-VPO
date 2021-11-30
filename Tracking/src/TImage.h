#pragma once
#include <opencv2/opencv.hpp>
class TImage
{
public:
	cv::Mat mat;
	std::vector<cv::Point2f> prevPoints;
	TImage() {}
	TImage(cv::Mat m) :mat(m) {

	}
	void detectPoints(const int maxCorners = 100, const double qualityLvl = 0.05, const double minDistance = 10);
};

