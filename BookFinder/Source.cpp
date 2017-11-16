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
#include "BooksCreator.h"
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

bool findBook(Mat img_object, Mat img_scene, bool = false);
bool ptsTooClose(Point2f, Point2f);

int main()
{
	/*
	Books test("Homeland");
	if (findBook(test.getImage(), imread("HomelandSide.jpg"), true)) {
		//test.output();
	}
	*/
	Mat input = imread("GirlPlane.jpg");

	vector<Books> books = objectvec();
	for (int i = 0; i < books.size(); i++) {
		if (findBook(books.at(i).getImage(), input)) {
			books.at(i).output();
			break;
		}
	}
	
	//imshow("Image", test.getImage());
	//test.output();
	/*
	bool found = findBook();
	if (found)
	cout << "true\n";
	else cout << "false\n";
	*/
	return 0;
}

bool findBook(Mat cover, Mat input, bool displayInternal)
{
	if (!cover.data || !input.data)
	{
		cout << " --(!) Error reading images " << endl; return 0;
	}
	Mat img_object;
	cvtColor(cover, img_object, CV_RGB2GRAY);
	Mat img_scene;
	cvtColor(input, img_scene, CV_RGB2GRAY);
	Mat resized;
	Size newSize(img_scene.cols * .3, img_scene.rows * .3);
	resize(img_scene, resized, newSize);
	//imshow("Resized scene", resized);
	//-- Step 1: Detect the keypoints and extract descriptors using SURF
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create(minHessian);
	vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	detector->detectAndCompute(img_object, Mat(), keypoints_object, descriptors_object);
	//detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);
	detector->detectAndCompute(resized, Mat(), keypoints_scene, descriptors_scene);
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
	perspectiveTransform(obj_corners, scene_corners, H);
	/*
	for (int i = 0; i < scene_corners.size(); i++) {
	cout << scene_corners.at(i) << endl;
	}
	*/
	bool ptsFarEnough = true;
	for (int i = 1; i < scene_corners.size(); i++) {
		if (ptsTooClose(scene_corners.at(i), scene_corners.at(i - 1))) {
			ptsFarEnough = false;
		}
	}
	if (copysignf(1, scene_corners.at(0).x - scene_corners.at(1).x) != copysignf(1, scene_corners.at(3).x - scene_corners.at(2).x)
		|| copysignf(1, scene_corners.at(0).y - scene_corners.at(3).y) != copysignf(1, scene_corners.at(1).y - scene_corners.at(2).y)) {
		ptsFarEnough = false;
	}
	if (displayInternal && ptsFarEnough) {
		Mat img_matches;
		//drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		drawMatches(img_object, keypoints_object, resized, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		//-- Draw lines between the corners (the mapped object in the scene - image_2 )
		line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
		//-- Show detected matches
		namedWindow("Good Matches & Object detection", WINDOW_AUTOSIZE);
		imshow("Good Matches & Object detection", img_matches);
		waitKey(0);
	}
	return ptsFarEnough;
}

bool ptsTooClose(Point2f a, Point2f b)
{
	return (abs(a.x - b.x) < 10 && abs(a.y - b.y) < 10);
}

