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

typedef struct {
        int id;
        double distance;
        vector<cv::Point> contours;
        cv::Moments mu;
        vector<kalmanCont> candidate_object;
        int selected_object;
        bool contour_use;
} contour_t;

cv::VideoCapture init_cap_bg(const char *url);

void BgSubtractor(cv::Mat *frame, cv::Mat *fg_mask);

double CalcDistance(double x_1, double x_2, double y_1, double y_2);

int parsingContours(vector<contour_t> &found_contures, kalmanCont &tracked_object);



int ProcessFrame(cv::Mat *frame, cv::Mat *fg_mask, double tick);

int counterAbsPersonFlow(int object_index);

void loadValidCounureToObject(vector<contour_t> &found_contures, vector<kalmanCont> &tracked_object);

void selectContourObject (vector<contour_t> &found_contures, vector<kalmanCont> &tracked_object);

bool comp(kalmanCont a,kalmanCont b);
extern bool with_gui;





#endif
