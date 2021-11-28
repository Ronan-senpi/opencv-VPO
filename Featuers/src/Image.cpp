#include "Image.h"
#include <filesystem>

void Image::init()
{
	color = cv::Scalar(
		(double(rand()) / double(RAND_MAX)) * (255),
		(double(rand()) / double(RAND_MAX)) * (255),
		(double(rand()) / double(RAND_MAX)) * (255));
	if (corners.size() != 4) {
		corners = std::vector<cv::Point2f>(4);
	}
	if (!mat.empty() && mat.cols > 0 && mat.rows > 0)
	{
		corners[0] = cv::Point2f(0, 0);
		corners[1] = cv::Point2f((float)mat.cols, 0);
		corners[2] = cv::Point2f((float)mat.cols, (float)mat.rows);
		corners[3] = cv::Point2f(0, (float)mat.rows);
	}
}

void Image::detectAndCompute(const cv::Ptr<cv::ORB>& orb)
{
	orb->detect(mat, keypoints);
	orb->compute(mat, keypoints, descriptor);
}

void Image::knnMatch(const cv::Ptr<cv::BFMatcher>& bfm, const Image& img, const float distance = 0.75f)
{
	matches.clear();
	matchesGood.clear();
	bfm->knnMatch(descriptor, img.descriptor, matches, 2);

	for (auto elem : matches) {
		if (elem[0].distance < distance * elem[1].distance) {
			matchesGood.push_back(elem[0]);
		}
	}
}

void Image::Clipping(const Image& rightImg,
						cv::Mat& outImag,
						const int minGoodMatch,
						const int thickness)
{
	//Begin: Clipping
	if (matchesGood.size() > minGoodMatch)
	{
		std::vector<cv::Point2f> obj, scene;

		for (auto i : matchesGood) {
			obj.push_back(keypoints[i.queryIdx].pt);
			scene.push_back(rightImg.keypoints[i.trainIdx].pt);
		}
		cv::Mat H = cv::findHomography(obj, scene, cv::RANSAC);

		//transforme les angle de l'image dans le plan de l'autre image
		std::vector<cv::Point2f> scene_corners(4);
		cv::perspectiveTransform(corners, scene_corners, H);

		//Dessine sur le flux vid�o
		cv::line(outImag, scene_corners[0] ,
			scene_corners[1] , color, thickness);
		cv::line(outImag, scene_corners[1] ,
			scene_corners[2] , color, thickness);
		cv::line(outImag, scene_corners[2] ,
			scene_corners[3] , color, thickness);
		cv::line(outImag, scene_corners[3] ,
			scene_corners[0] , color, thickness);
	}
	//End: Clipping
}

void Image::prepareImgs(const std::string& path, std::vector<Image>& leftImgs, const cv::Ptr<cv::ORB>& orb)
{
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		std::cout << entry.path() << std::endl;
		std::string strPath = entry.path().string();
		if (strPath.substr(strPath.size() - 3) == "jpg")
		{

			auto mat = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
			if (!mat.empty()) {
				leftImgs.push_back(Image(mat, orb));
			}
			else
				std::cout << "Could not open or find the image : " << entry.path().string() << std::endl;
		}
	}
}