
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

void rectify(cv::Mat& img1,
	cv::Mat& img2,
	std::vector<cv::Point2f>& points1,
	std::vector<cv::Point2f>& points2,
	cv::Mat& rectifyImg1,
	cv::Mat& rectifyImg2) {
	cv::Mat fMat = cv::findFundamentalMat(points1, points2);
	cv::Mat h1, h2;
	cv::stereoRectifyUncalibrated(points1, points2, fMat, img1.size(), h1, h2);
	cv::warpPerspective(img1, rectifyImg1, h1, img1.size());
	cv::warpPerspective(img2, rectifyImg2, h2, img2.size());

}

void computeDisparity(cv::Mat& rImg1, cv::Mat& rImg2, cv::Mat& outDisp) {
	cv::Mat disp;
	auto sbm = cv::StereoBM::create();
	sbm->compute(rImg1, rImg2, disp);
	double min, max;
	cv::minMaxLoc(disp, &min, &max);
	double coef = 255. / (max - min);
	double offset = -min * 255. / (max - min);
	outDisp = offset + (disp * coef);
	outDisp.convertTo(outDisp, CV_8U);
}

const std::string SET_NUMBER = "1";
const int RESIZE_FACTOR = 1.5;

int main(int argc, char** argv)
{
	cv::Mat ri1, ri2, disp, dispShow, rect, rectShow, hImg, hImgShow;
	std::vector<uchar> status;
	std::vector<float> err;
	// Read the image file
	cv::Mat image1 = cv::imread("./img/set" + SET_NUMBER + "/image1.jpg");
	if (image1.empty()) // Check for failure
	{
		std::cout << "Could not open or find the image 1" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}
	cv::Mat image1G;
	cv::cvtColor(image1, image1G, cv::COLOR_RGB2GRAY);

	cv::Mat image2 = cv::imread("./img/set" + SET_NUMBER + "/image2.jpg");
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
		cv::line(hImg, points1[i], p2, cv::Scalar(255, 0, 0, 100));
	}

	cv::resize(hImg, hImgShow, cv::Size(hImg.cols / RESIZE_FACTOR, hImg.rows / RESIZE_FACTOR));
	cv::imshow("line", hImgShow);

	rectify(image1G, image2G, points1, points2, ri1, ri2);
	std::vector<cv::Mat> imgsRect{ ri1, ri2 };
	cv::hconcat(imgsRect, rect);
	cv::resize(rect, rectShow, cv::Size(rect.cols / RESIZE_FACTOR, rect.rows / RESIZE_FACTOR));
	cv::imshow("rect", rectShow);
	//cv::imshow("retify1", ri1);
	//cv::imshow("retify2", ri2);
	// Display

	computeDisparity(ri1, ri2, disp);
	cv::resize(disp, dispShow, cv::Size(disp.cols / RESIZE_FACTOR, disp.rows / RESIZE_FACTOR));
cv:imshow("DisparityMap", dispShow);

	cv::waitKey(0);

	return 0;
}