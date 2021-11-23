#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	// Read the image file
	cv::Mat image = cv::imread("D:/Médias/Aedrine-Round.png");

	if (image.empty()) // Check for failure
	{
		std::cout << "Could not open or find the image" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}

	std::string windowName = "Symmetry"; //Name of the window

	cv::namedWindow(windowName); // Create a window

	cv::imshow(windowName, image); // Show our image inside the created window.

	cv::waitKey(0); // Wait for any keystroke in the window

	cv::destroyWindow(windowName); //destroy the created window

	return 0;
}