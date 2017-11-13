#pragma once
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Books {
private:
	string name;
	Mat image = imread(name + ".jpg");
	Mat details = imread(name + "1.jpg");

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
	imshow(name, details);
	waitKey(0);
}

