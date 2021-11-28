#pragma once

void Clipping(std::vector<cv::DMatch>& good, std::vector<cv::KeyPoint>& leftKeypoints, std::vector<cv::KeyPoint>& rightKeypoints, cv::Mat& leftImg, cv::Mat& knnImg);
