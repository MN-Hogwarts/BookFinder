#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

class Books {
private:
	string name;
	Mat img;
	Mat details;

public:
	Books(string);
	Mat getImage();
	void output();
};

Books::Books(string book_name) {
	name = book_name;
	img = imread(name + ".jpg");
	details = imread(name + "1.jpg");
}

Mat Books::getImage() {
	return img;
}

void Books::output() {
	imshow(name, details);
	waitKey(0);
}

