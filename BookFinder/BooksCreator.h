#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;


vector<Mat>objectvec()
{
	vector<Books>covers;

	Books b1("An Ember in the Ashes");
	Books b2("Divergent");
	Books b3("Insurgent");
	Books b4("Crown of Midnight");
	Books b5("Girl on the Plane");
	Books b6("Hollow City");
	Books b7("With Malice"); 
	Books b8("I Funny");
	Books b9("The Fire Wish");
	Books b10("Harry Potter and the Chamber of Secrets");
	Books b11("Homeland");

	cover.push_back(b1);
	cover.push_back(b2);
	cover.push_back(b3);
	cover.push_back(b4);
	cover.push_back(b5);
	cover.push_back(b6);
	cover.push_back(b7);
	cover.push_back(b8);
	cover.push_back(b9);
	cover.push_back(b10);
	cover.push_back(b11);

	return covers;
}



