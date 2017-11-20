#include <stdio.h>
#include <iostream>
#include <cmath>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "Books.h"
#include "GUI.h"
#include "BooksCreator.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

bool findBook(Books, Matchable, bool = false);
bool ptsTooClose(Point2f, Point2f);
bool checkConvex(vector<Point2f>);

int main()
{
	imshow("BookFinder", imread("UIOutputs//Startup.jpg"));
	waitKey(500);
	vector<Books> books = objectvec();
	destroyWindow("BookFinder");

	while (1) {
		string filename = input();
		Matchable input(imread(filename));

		//imshow("Resized", input.getImage());

		bool found = false;
		imshow("Searching", imread("UIOutputs//Search.jpg"));
		waitKey(500);
		///*
		for (int i = 0; i < books.size(); i++) {
			if (findBook(books.at(i), input)) {
				//destroyWindow("Searching");
				destroyAllWindows();
				books.at(i).output();
				found = true;
				break;
			}
		}
		if (!found) {
			destroyAllWindows();
			imshow("Not found!", imread("UIOutputs//NotFoundPage.jpg"));
			waitKey(0);
		}
		//*/
		destroyAllWindows();
	}

	/*
	bool found = findBook();
	if (found)
	cout << "true\n";
	else cout << "false\n";
	*/
	return 0;
}

bool findBook(Books cover, Matchable input, bool displayInternal)
{
	if (!cover.getImage().data || !input.getImage().data)
	{
		cout << " --(!) Error reading images " << endl; return 0;
	}
	Mat img_object;
	cvtColor(cover.getImage(), img_object, CV_RGB2GRAY);
	Mat img_scene;
	cvtColor(input.getImage(), img_scene, CV_RGB2GRAY);
	/*
	Mat resized;
	Size newSize(img_scene.cols * .3, img_scene.rows * .3);
	resize(img_scene, resized, newSize);
	*/
	//imshow("Resized scene", resized);
	//-- Step 1: Detect the keypoints and extract descriptors using SURF
	//int minHessian = 400;
	//Ptr<SURF> detector = SURF::create(minHessian);
	vector<KeyPoint> keypoints_object = cover.keypoints;
	vector<KeyPoint> keypoints_scene = input.keypoints;
	Mat descriptors_object = cover.descriptors;
	Mat descriptors_scene = input.descriptors;
	/*
	detector->detectAndCompute(img_object, Mat(), keypoints_object, descriptors_object);
	//detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);
	detector->detectAndCompute(resized, Mat(), keypoints_scene, descriptors_scene);
	*/
	//-- Step 2: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);
	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	/*
	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	*/
	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance <= 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	//-- Localize the object
	vector<Point2f> obj;
	vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj, scene, RANSAC);
	//-- Get the corners from the image_1 ( the object to be "detected" )
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
	vector<Point2f> scene_corners(4);
	if (H.rows != H.cols
		|| H.cols != 3 && H.cols != 4
		|| H.rows != 3 && H.rows != 4) {
		return false;
	}
	perspectiveTransform(obj_corners, scene_corners, H);
	/*
	for (int i = 0; i < scene_corners.size(); i++) {
	cout << scene_corners.at(i) << endl;
	}
	*/
	bool validShape = true;
	for (int i = 0; i < scene_corners.size(); i++) { // checks if any points are too close to each other to be a match
		if (ptsTooClose(scene_corners.at((i + 1) % 4), scene_corners.at(i % 4))) {
			validShape = false;
		}
	}
	for (int i = 0; i < scene_corners.size(); i++) { // checks if any coordinates are more than 1/3 out of the frams
		if (scene_corners.at(i).x < 0 - input.getImage().cols / 3 || scene_corners.at(i).y < 0 - input.getImage().rows / 3
			|| scene_corners.at(i).x > input.getImage().cols + input.getImage().cols / 3
			|| scene_corners.at(i).y > input.getImage().rows + input.getImage().rows / 3) {
			validShape = false;
		}
	}
	/*
	if (copysignf(1, scene_corners.at(0).x - scene_corners.at(1).x) != copysignf(1, scene_corners.at(3).x - scene_corners.at(2).x)
		|| copysignf(1, scene_corners.at(0).y - scene_corners.at(3).y) != copysignf(1, scene_corners.at(1).y - scene_corners.at(2).y)) {
		validShape = false;
	}
	*/
	if (!checkConvex(scene_corners)) {
		validShape = false;
	}
	if (displayInternal){// && validShape) {
		Mat img_matches;
		drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		//drawMatches(img_object, keypoints_object, resized, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		//-- Draw lines between the corners (the mapped object in the scene - image_2 )
		line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		//-- Show detected matches
		destroyAllWindows();
		namedWindow("Good Matches & Object detection", WINDOW_AUTOSIZE);
		imshow("Good Matches & Object detection", img_matches);
		waitKey(0);
	}
	return validShape;
}

bool ptsTooClose(Point2f a, Point2f b)
{
	return (abs(a.x - b.x) < 30 && abs(a.y - b.y) < 30);
}

bool checkConvex(vector<Point2f> corners) {
	// vector size must be 4
	bool convex = true;
	for (int i = 0; i < 2; i++) {
		// finds coordinates of median of diagonals
		float medx = (corners.at(i).x + corners.at(i + 2).x) / 2;
		float medy = (corners.at(i).y + corners.at(i + 2).y) / 2;
		if (copysignf(1, medx - corners.at(i + 1).x) == copysignf(1, medx - corners.at((i + 3) % 4).x) // checks if x-velues are on same side of median
			&& (copysignf(1, corners.at(i).y - corners.at(i + 1).y) == copysignf(1, corners.at(i).y - corners.at((i + 3) % 4).y) // checks if y-values are on same side of points
			|| copysignf(1, corners.at(i + 2).y - corners.at(i + 1).y) == copysignf(1, corners.at(i + 2).y - corners.at((i + 3) % 4).y))
			|| copysignf(1, medy - corners.at(i + 1).y) == copysignf(1, medy - corners.at((i + 3) % 4).y) // checks if y-velues are on same side of median
			&& (copysignf(1, corners.at(i).x - corners.at(i + 1).x) == copysignf(1, corners.at(i).x - corners.at((i + 3) % 4).x) // checks if x-values are on same side of points
			|| copysignf(1, corners.at(i + 2).x - corners.at(i + 1).x) == copysignf(1, corners.at(i + 2).x - corners.at((i + 3) % 4).x))) {

			convex = false;
		}
	}
	return convex;
}
