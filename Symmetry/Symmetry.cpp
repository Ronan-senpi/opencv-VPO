#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	for (const auto& entry : std::filesystem::directory_iterator("./img/")) {

		std::string strPath = entry.path().string();
		if (strPath.substr(strPath.size() - 3) == "jpg")
		{

			// Read the image file
			cv::Mat image = cv::imread(strPath);

			if (image.empty()) // Check for failure
			{
				std::cout << "Could not open or find the image" << std::endl;
				system("pause"); //wait for any key press
				return -1;
			}
			cv::Mat fImage;
			cv::flip(image, fImage, 1);
			cv::Mat descriptor, fDescriptor;
			std::vector<cv::KeyPoint> keypoints, fKeypoints;
			auto orb = cv::ORB::create(10000);
			auto bfm = cv::BFMatcher::create();

			orb->detect(image, keypoints);
			orb->compute(image, keypoints, descriptor);

			orb->detect(fImage, fKeypoints);
			orb->compute(fImage, fKeypoints, fDescriptor);


			std::vector<std::vector<cv::DMatch>> matches;
			bfm->knnMatch(descriptor, fDescriptor, matches, 2);
			std::vector<cv::Point2f>  fGoodPoints, barrycenters;
			for (auto elem : matches) {
				if (elem[0].distance < 0.78 * elem[1].distance) {
					cv::Point2f p = keypoints[elem[0].queryIdx].pt;
					cv::Point2f fp = fKeypoints[elem[0].trainIdx].pt;
					fp.x = image.cols - fp.x;
					fGoodPoints.push_back(fp);
					barrycenters.push_back(cv::Point((p.x - fp.x) / 2 + fp.x, (p.y - fp.y) / 2 + fp.y));
				}
			}

			cv::Mat bari = cv::Mat::zeros(image.size(), CV_8UC1);
			for (auto p : barrycenters) {
				cv::circle(bari, p, 1, cv::Scalar(255, 255, 255));
			}


			std::vector<cv::Vec2f> lines;
			cv::HoughLines(bari, lines, 3, CV_PI / 180, barrycenters.size() / 3.5, 0, 0);
			if (!lines.empty()) {
				float rho = lines[0][0];
				float theta = lines[0][1];
				cv::Point pt1, pt2;

				double a = cos(theta);
				double b = sin(theta);
				double x0 = a * rho, y0 = b * rho;
				pt1.x = cvRound(x0 + 1000 * (-b));
				pt1.y = cvRound(y0 + 1000 * (a));
				pt2.x = cvRound(x0 - 1000 * (-b));
				pt2.y = cvRound(y0 - 1000 * (a));
				cv::line(image, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
			}

			cv::imshow("barrycenter " + strPath, bari);
			cv::imshow(strPath, image);
			
			cv::waitKey(0); // Wait for any keystroke in the window

			cv::destroyWindow("barrycenter " + strPath);
			cv::destroyWindow(strPath);
		}
	}
	return 0;
}