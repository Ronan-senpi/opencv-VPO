
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

cv::Mat prevInput, nextInput;
std::vector<cv::Point2f> prevPoints, nextPoints;

cv::Point2i roiTopLeft, roiBottomRight;
cv::Rect roi;
cv::Point start(-1, -1);
std::string windowName = "window";

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		start = cv::Point(x, y);
		roi = cv::Rect();
		prevPoints.clear();
		nextPoints.clear();
	}
	else if (event == cv::EVENT_MOUSEMOVE)
	{
		if (start.x >= 0) {
			cv::Point end(x, y);
			roi = cv::Rect(start, end);
		}
	}
	else if (event == cv::EVENT_LBUTTONUP) {
		cv::Point end(x, y);
		roi = cv::Rect(start, end);
		start = cv::Point(-1, -1);
	}
}
void detectPoints(const cv::Mat& nextImg, const cv::Mat& mask, const int maxCorners = 100, const double qualityLvl = 0.05, const double minDistance = 10)
{
	cv::goodFeaturesToTrack(nextImg, prevPoints, maxCorners, qualityLvl, minDistance, mask);
}

std::vector<cv::Point2f> purgePoint(std::vector<cv::Point2f>& points, std::vector<uchar>& status) {
	std::vector<cv::Point2f> result;
	for (size_t i = 0; i < points.size(); i++)
		if (status[i] > 0)
			result.push_back(points[i]);
	return result;
}

void trackPoints(const int minPoint = 3) {

	std::vector<uchar> status;
	std::vector<float> err;

	if (!prevInput.empty()) {

		cv::Mat mask = cv::Mat::zeros(nextInput.size(), CV_8U);
		cv::rectangle(mask, roi, cv::Scalar(255, 255, 255), -1);
		cv::imshow("Mask", mask);

		prevPoints = nextPoints;
		if (prevPoints.size() < minPoint) {
			detectPoints(nextInput, mask);

			if (prevPoints.size() < minPoint)
				return;
		}

		cv::calcOpticalFlowPyrLK(prevInput, nextInput, prevPoints, nextPoints, status, err);
		prevPoints = purgePoint(prevPoints, status);
		nextPoints = purgePoint(nextPoints, status);
	}
	prevInput = nextInput.clone();
}

void updateRoi() {
	
	int maxX = INT_MIN;
	int minY = INT_MAX;
	int minX = INT_MAX;
	int maxY = INT_MIN;

	for (auto np : nextPoints) {
		if (np.x > maxX)
			maxX = np.x;
		if (np.x < minX)
			minX = np.x;
		if (np.y > maxY)
			maxY = np.y;
		if (np.y < minY)
			minY = np.y;
	}

	roi = cv::Rect(cv::Point2f(minX, minY),
		cv::Point2f(maxX, maxY));

}

int vid(const std::string filename = "")
{
	cv::VideoCapture cap;
	if (filename != "")
		cap.open(filename);
	else
		cap.open(0);

	if (cap.isOpened() == false)
	{
		std::cout << "Cannot open the video file" << std::endl;
		std::cin.get(); //wait for any key press
		return -1;
	}

	cv::Mat drawImg, beurkImg;
	while (true)
	{
		if (start.x < 0) {
			if (!cap.read(drawImg))
				break;
			cv::cvtColor(drawImg, nextInput, cv::COLOR_RGB2GRAY);

			trackPoints();
			updateRoi();
		}

		beurkImg = drawImg.clone();
		for (auto np : nextPoints)
			cv::circle(beurkImg, np, 2, cv::Scalar(0, 0, 255), 2);
		cv::rectangle(beurkImg, roi, cv::Scalar(255, 0, 0), 1);
		cv::imshow(windowName, beurkImg);


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
	cv::namedWindow(windowName);
	cv::setMouseCallback(windowName, CallBackFunc, NULL);
	vid("./videos/vid2.mp4");

	return 0;
}