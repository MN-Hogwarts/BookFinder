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
	vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
};

Books::Books(string book_name) {
	name = book_name;
	img = cv::imread("Covers\\" + name + ".jpg");
	details = cv::imread("Details\\" + name + ".jpg");
	
	if (!img.data) {
		cout << " --(!) Error reading images " << endl; return;
	}
	cv::Mat blackWhite;
	cvtColor(img, blackWhite, CV_RGB2GRAY);
	int minHessian = 400;
	cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
	detector->detectAndCompute(blackWhite, cv::Mat(), keypoints, descriptors);
}

cv::Mat Books::getImage() {
	return img;
}

void Books::output() {
	cv::imshow(name, details);
	cv::waitKey(0);
}

class Matchable {
private:
	cv::Mat img;

public:
	Matchable(cv::Mat);
	cv::Mat getImage();
	vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
};

Matchable::Matchable(cv::Mat input) {
	img = input;

	if (!img.data) {
		cout << " --(!) Error reading images " << endl; return;
	}

	cv::Size newSize(input.cols * .3, input.rows * .3);
	cv::resize(input, img, newSize);

	cv::Mat blackWhite;
	cvtColor(img, blackWhite, CV_RGB2GRAY);
	int minHessian = 400;
	cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
	detector->detectAndCompute(blackWhite, cv::Mat(), keypoints, descriptors);
}

cv::Mat Matchable::getImage() {
	return img;
}