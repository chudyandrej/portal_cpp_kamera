//
// Created by andrej on 9.7.2015.
//



#ifndef PORTALS_OPENCV_H
#define PORTALS_OPENCV_H


#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video.hpp>
#include "kalmanCont.h"

using namespace cv;
using namespace std;

cv::VideoCapture init_cap_bg(const char *url);

void BgSubtractor(cv::Mat &frame , cv::Mat &rangeRes);

double CalcDistance(float x_1, float x_2, float y_1, float y_2);

int parsingContours(vector<kalmanCont>& KalObjects, int x,int y, double max);

void make_calculation(cv::Mat &frame, cv::Mat &rangeRes, double tick );

void send_transaction(const char *direction);

extern bool with_gui;



#endif
