//
// Created by andrej on 9.7.2015.
//



#ifndef PORTALS_OPENCV_H
#define PORTALS_OPENCV_H

#include <thread>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video.hpp>
#include "kalmanCont.h"
#include "communication.h"

using namespace cv;
using namespace std;

cv::VideoCapture init_cap_bg(const char *url);

void BgSubtractor(cv::Mat &frame , cv::Mat &rangeRes);

double CalcDistance(float x_1, float x_2, float y_1, float y_2);

int  parsingContours(vector<vector<cv::Point>> &objects,vector<cv::Rect> &objectsBox, float x,float y,  double max);

int make_calculation(cv::Mat &frame, cv::Mat &rangeRes, double tick );

void counter_person_flow(int object_index, bool direction);

extern bool with_gui;

extern bool person_flow;



#endif
