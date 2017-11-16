#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
//using namespace cv;

class Books {
private:
	string name;
	cv::Mat img;
	cv::Mat details;

public:
	Books(string);
	cv::Mat getImage();
	void output();
};

Books::Books(string book_name) {
	name = book_name;
	img = cv::imread("Covers\\" + name + ".jpg");
	details = cv::imread("Details\\" + name + ".jpg");
}

cv::Mat Books::getImage() {
	return img;
}

void Books::output() {
	cv::imshow(name, details);
	cv::waitKey(0);
}

