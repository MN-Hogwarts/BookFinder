#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "Books.h"

using namespace std;
using namespace cv;


vector<Books>objectvec()
{
	vector<Books> covers;

	Books b1("An Ember in the Ashes");
	Books b2("Divergent");
	Books b3("Insurgent");
	Books b4("Crown of Midnight");
	Books b5("Girl on a Plane");
	Books b6("Hollow City");
	Books b7("With Malice"); 
	Books b8("I Funny");
	Books b9("The Fire Wish");
	Books b10("Harry Potter and the Chamber of Secrets");
	Books b11("Homeland");

	covers.push_back(b1);
	covers.push_back(b2);
	covers.push_back(b3);
	covers.push_back(b4);
	covers.push_back(b5);
	covers.push_back(b6);
	covers.push_back(b7);
	covers.push_back(b8);
	covers.push_back(b9);
	covers.push_back(b10);
	covers.push_back(b11);

	return covers;
}



