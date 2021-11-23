#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <cstdarg>


using namespace cv;
using namespace std;

//C'pas moi
void ShowManyImages(std::string title, int nArgs, ...) {
	int size;
	int i;
	int m, n;
	int x, y;

	// w - Maximum number of images in a row
	// h - Maximum number of images in a column
	int w, h;

	// scale - How much we have to resize the image
	float scale;
	int max;

	// If the number of arguments is lesser than 0 or greater than 12
	// return without displaying
	if (nArgs <= 0) {
		printf("Number of arguments too small....\n");
		return;
	}
	else if (nArgs > 14) {
		printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
		return;
	}
	// Determine the size of the image,
	// and the number of rows/cols
	// from number of arguments
	else if (nArgs == 1) {
		w = h = 1;
		size = 300;
	}
	else if (nArgs == 2) {
		w = 2; h = 1;
		size = 300;
	}
	else if (nArgs == 3 || nArgs == 4) {
		w = 2; h = 2;
		size = 300;
	}
	else if (nArgs == 5 || nArgs == 6) {
		w = 3; h = 2;
		size = 200;
	}
	else if (nArgs == 7 || nArgs == 8) {
		w = 4; h = 2;
		size = 200;
	}
	else {
		w = 4; h = 3;
		size = 150;
	}

	// Create a new 3 channel image
	cv::Mat DispImage = cv::Mat::zeros(cv::Size(100 + size * w, 60 + size * h), CV_8UC3);

	// Used to get the arguments passed
	va_list args;
	va_start(args, nArgs);

	// Loop for nArgs number of arguments
	for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {
		// Get the Pointer to the IplImage
		cv::Mat img = va_arg(args, cv::Mat);

		// Check whether it is NULL or not
		// If it is NULL, release the image, and return
		if (img.empty()) {
			printf("Invalid arguments");
			return;
		}

		// Find the width and height of the image
		x = img.cols;
		y = img.rows;

		// Find whether height or width is greater in order to resize the image
		max = (x > y) ? x : y;

		// Find the scaling factor to resize the image
		scale = (float)((float)max / size);

		// Used to Align the images
		if (i % w == 0 && m != 20) {
			m = 20;
			n += 20 + size;
		}

		// Set the image ROI to display the current image
		// Resize the input image and copy the it to the Single Big Image
		cv::Rect ROI(m, n, (int)(x / scale), (int)(y / scale));
		cv::Mat temp;
		resize(img, temp, cv::Size(ROI.width, ROI.height));
		temp.copyTo(DispImage(ROI));
	}

	// Create a new window, and show the Single Big Image
	cv::namedWindow(title, 1);
	imshow(title, DispImage);

	// End the number of arguments
	va_end(args);
}

int old() {
	//Read the image file
	cv::Mat queryImg = cv::imread("D:/Médias/Aedrine-Round.png");

	if (queryImg.empty()) // Check for failure
	{
		std::cout << "Could not open or find the image" << std::endl;
		system("pause"); //wait for any key press
		return -1;
	}
	// Create a VideoCapture object and open the input file
	// If the input is the web camera, pass 0 instead of the video file name
	cv::VideoCapture cap("./img/set1/video.mp4");

	cv::Mat frame;
	bool bSuccess = cap.read(frame); // read a new frame from video 

	if (cap.isOpened() == false)
	{
		std::cout << "Cannot open the video file" << std::endl;
		std::cin.get(); //wait for any key press
	}

	//Uncomment the following line if you want to start the video in the middle
	//cap.set(CAP_PROP_POS_MSEC, 300); 

	//get the frames rate of the video
	//double fps = cap.get(cv::CAP_PROP_FPS);
	//std::cout << "Frames per seconds : " << fps << std::endl;

	std::string windowName = "Vid";

	cv::namedWindow(windowName, cv::WINDOW_NORMAL); //create a window

	while (true)
	{
		cv::Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video 
		cv::Ptr<cv::FeatureDetector> orb = cv::ORB::create();

		// find the keypoints and descriptors with ORB
		//cv::detector->detect(queryImg, keypoints_object);
		//detector->detect(frame, keypoints_scene);

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}

		//show the frame in the created window
		cv::imshow(windowName, frame);
		ShowManyImages("image", 2, queryImg, frame);
		//wait for for 10 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
			break;
		}
	}
	cv::destroyWindow(windowName); //destroy the created window

	return 0;
}

int main(int argc, char** argv)
{
	cv::VideoCapture cap("./img/set1/video.mp4");

	if (cap.isOpened() == false)
	{
		std::cout << "Cannot open the video file" << std::endl;
		std::cin.get(); //wait for any key press
	}

	
	Mat descriptor_1, descriptor_2;

	vector<KeyPoint> keypoints_1, keypoints_2;

	Mat im_left = imread("./img/set1/bleach.jpg");

	Ptr<ORB> detector = ORB::create();
	vector<DMatch> matches; 
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
	Ptr<DescriptorExtractor> extractor;

	while (true)
	{
		Mat im_right;
		bool bSuccess = cap.read(im_right);
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}

		detector->detect(im_left, keypoints_2, descriptor_2);
		detector->detect(im_right, keypoints_1, descriptor_1);
		matcher->match(descriptor_1, descriptor_2, matches);

		Mat img_match;

		drawMatches(im_left, keypoints_1, im_right, keypoints_2, matches, img_match);

		imshow("Matches", img_match);
		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
			break;
		}
	}
	cv::waitKey(0);
}

