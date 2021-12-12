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
	cv::flip(image, fImage, 1);
	//cv::imshow("flip", fImage);
	//cv::imshow("normal", image);
	cv::Mat descriptor, fDescriptor;
	std::vector<cv::KeyPoint> keypoints, fKeypoints;
	auto orb = cv::ORB::create(1500);
	auto bfm = cv::BFMatcher::create();

	orb->detect(image, keypoints);
	orb->compute(image, keypoints, descriptor);

	orb->detect(fImage, fKeypoints);
	orb->compute(fImage, fKeypoints, fDescriptor);


	std::vector<std::vector<cv::DMatch>> matches;
	bfm->knnMatch(descriptor, fDescriptor, matches, 2);
	std::vector<cv::Point2f> goodPoints, fGoodPoints, hBarrycenter;
	for (auto elem : matches) {
		if (elem[0].distance < 0.8 * elem[1].distance) {
			cv::Point2f p = keypoints[elem[0].queryIdx].pt;
			goodPoints.push_back(p);
			cv::Point2f fp = fKeypoints[elem[0].trainIdx].pt;
			fp.x = image.cols - fp.x;
			fGoodPoints.push_back(fp);
			hBarrycenter.push_back(cv::Point((p.x - fp.x) / 2 + fp.x, (p.y - fp.y) / 2 + fp.y));
		}
	}

	//cv::Mat knnImg;
//cv:drawMatches(image, keypoints, image, fKeypoints, good, knnImg);
	//cv::imshow("knnImg", knnImg);
	//for (size_t i = 0; i < goodPoints.size(); i++) {
	//	cv::line(image, goodPoints[i], fGoodPoints[i], cv::Scalar(255, 0, 0, 100));
	//}
	cv::imshow("image", image);
	cv::Mat bari = cv::Mat::zeros(image.size(), image.type());
	for (auto p : hBarrycenter) {
		cv::circle(image, p, 2, cv::Scalar(0, 255, 0),2);
		cv::circle(bari, p, 2, cv::Scalar(255, 255, 255), 2);
	}


	cv::imshow("barrycenterImage", image);
	cv::imshow("barrycenter", bari);


#pragma region Keypoint

	//cv::Mat featureImg;
	//drawKeypoints(image, keypoints, featureImg, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
	//cv::Mat fFeatureImg;
	//drawKeypoints(fImage, fKeypoints, fFeatureImg, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

	//cv::imshow("windowName", featureImg); // Show our image inside the created window.
	//cv::imshow("fWindowName", fFeatureImg); // Show our image inside the created window.

#pragma endregion Keypoint

	cv::waitKey(0); // Wait for any keystroke in the window

	return 0;
}