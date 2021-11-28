#include <opencv2/opencv.hpp>

#pragma once
class Image
{
private:

	//Calcule des angles de l'image d'origine
	void init();
public:
	Image() {}

	Image(cv::Mat m) : mat(m) {
		init();
	}
	Image(cv::Mat m, const cv::Ptr<cv::ORB>& orb) : mat(m) {
		init();
		detectAndCompute(orb);
	}
	std::vector<cv::Point2f> corners;
	cv::Mat mat;
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptor;
	cv::Scalar color;
	std::vector<std::vector<cv::DMatch>> matches;
	std::vector<cv::DMatch> matchesGood;

	void detectAndCompute(const cv::Ptr<cv::ORB>& orb);

	void knnMatch(const cv::Ptr<cv::BFMatcher>& bfm, const Image& img, const float distance);

	void Clipping(
		const Image& rightImg,
		cv::Mat& outImag,
		const int minGoodMatch = 10,
		const int thickness = 2);

	static void prepareImgs(const std::string& path,
		std::vector<Image>& leftImgs,
		const cv::Ptr<cv::ORB>& orb);
	static float getAngleBetween(const cv::Point2f a, const cv::Point2f b, const cv::Point2f c);

};

