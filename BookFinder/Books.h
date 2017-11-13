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
	Mat image = imread(name + ".jpg");
	ifstream details;

public:
	Books(string);
	Mat getImage();
	void output();
};

Books::Books(string book_name) {
	name = book_name;
}

Mat Books::getImage() {
	return image;
}

void Books::output() {
	details.open(name + ".txt");
	if (details.is_open())
		cout << details.rdbuf();
}

