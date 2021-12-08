#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	// Read the image file
	cv::Mat image = cv::imread("./img/f.jpg");

	if (image.empty()) // Check for failure
	{
		std::cout << "Could not open or find the image" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}
	cv::Mat fImage;
	cv::flip(image, fImage, cv::RotateFlags::ROTATE_180);

	cv::Mat descriptor, fDescriptor;
	std::vector<cv::KeyPoint> keypoints, fKeypoints;
	auto orb = cv::ORB::create(); 
	auto bfm = cv::BFMatcher::create();

	orb->detect(image, keypoints);
	orb->compute(image, keypoints, descriptor);

	orb->detect(fImage, fKeypoints);
	orb->compute(fImage, fKeypoints, fDescriptor);


	std::vector<std::vector<cv::DMatch>> matches;
	bfm->knnMatch(descriptor, fDescriptor, matches, 2);

	std::vector<cv::DMatch> good;
	for (auto elem : matches) {
		if (elem[0].distance < 0.75 * elem[1].distance) {
			good.push_back(elem[0]);
		}
	}
	cv::Mat knnImg;
cv:drawMatches(image, keypoints, fImage, fKeypoints, good, knnImg);

	cv::imshow("knnImg", knnImg);

#pragma region Keypoint

	cv::Mat featureImg;
	drawKeypoints(image, keypoints, featureImg, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
	cv::Mat fFeatureImg;
	drawKeypoints(fImage, fKeypoints, fFeatureImg, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

	cv::imshow("windowName", featureImg); // Show our image inside the created window.
	cv::imshow("fWindowName", fFeatureImg); // Show our image inside the created window.

#pragma endregion Keypoint

	cv::waitKey(0); // Wait for any keystroke in the window

	return 0;
}