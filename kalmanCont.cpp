//
// Created by andrej on 29.6.2015.
//


#include "kalmanCont.h"
#include "loaded_data.h"

kalmanCont::kalmanCont() {

    kf = cv::KalmanFilter(6, 4, 0, CV_32F);
    state =  cv::Mat(6, 1, CV_32F);  // [x,y,v_x,v_y,w,h]
    meas =  cv::Mat(4, 1, CV_32F);    // [z_x,z_y,z_w,z_h]

    first_start = false;
    using_rate_ = 0;

    R = rand() % 255;
    G = rand() % 255;
    B = rand() % 255;

    cv::setIdentity(kf.transitionMatrix);

    kf.measurementMatrix = cv::Mat::zeros(4, 6, CV_32F);
    kf.measurementMatrix.at<float>(0) = 1.0f;
    kf.measurementMatrix.at<float>(7) = 1.0f;
    kf.measurementMatrix.at<float>(16) = 1.0f;
    kf.measurementMatrix.at<float>(23) = 1.0f;


    kf.processNoiseCov.at<float>(0) = 1e-2;
    kf.processNoiseCov.at<float>(7) = 1e-2;
    kf.processNoiseCov.at<float>(14) = 5.0f;
    kf.processNoiseCov.at<float>(21) = 5.0f;
    kf.processNoiseCov.at<float>(28) = 1e-2;
    kf.processNoiseCov.at<float>(35) = 1e-2;


    cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));
}

void kalmanCont::kalmanSaveData(cv::Mat res, double dT) {
    if (!first_start){
        kf.errorCovPre.at<float>(0) = 1; // px
        kf.errorCovPre.at<float>(7) = 1; // px
        kf.errorCovPre.at<float>(14) = 1;
        kf.errorCovPre.at<float>(21) = 1;
        kf.errorCovPre.at<float>(28) = 1; // px
        kf.errorCovPre.at<float>(35) = 1; // px

        state.at<float>(0) = meas.at<float>(0);                         //center x
        state.at<float>(1) = meas.at<float>(1);                         //center y
        state.at<float>(2) = 0;
        state.at<float>(3) = 0;
        state.at<float>(4) = meas.at<float>(2);                         //dlzka
        state.at<float>(5) = meas.at<float>(3);
        first_start = true;
    }
   else {
        kf.correct(meas);
    }

    kf.transitionMatrix.at<float>(2) = (float) dT;
    kf.transitionMatrix.at<float>(9) = (float) dT;

    state = kf.predict();

    cv::Point center;
    center.x = (int) state.at<float>(0);
    center.y = (int) state.at<float>(1);

    if (center.x <= 0)
        center.x = 0;
    if (center.x >= frame_width)
        center.x = frame_width;
    if (center.y <= 0)
        center.y = 0;
    if (center.y >= frame_height)
        center.y = frame_height;
        cv::Rect predRect;
    predRect.width = (int) state.at<float>(4);
    predRect.height = (int) state.at<float>(5);
    predRect.x = state.at<float>(0) - predRect.width / 2;
    predRect.y = (int) (state.at<float>(1) - predRect.height / 2);

    cv::circle(res, center, 2, CV_RGB(255,0,0), -1);
    cv::rectangle(res, predRect, CV_RGB(255,0,0), 2);

    x = center.x;
    y = center.y;

}

int kalmanCont::kalmanMakeCalculate(cv::Mat res, cv::Rect objectsBox, double dT, cv::MatND hist) {
    hist_ = hist;
    objectsBoxCopy = objectsBox;

    object_area_ = objectsBox.area();


    using_rate_ = 0;
    last_x_pos_ = objectsBox.x + objectsBox.width / 2;
    last_y_pos_ = objectsBox.y + objectsBox.height / 2;


    meas.at<float>(0) = last_x_pos_;                      //center x
    meas.at<float>(1) = last_y_pos_;                  //center y
    meas.at<float>(2) = (float) objectsBox.width;                   //dlzka
    meas.at<float>(3) = (float) objectsBox.height;                  //vyska
    

    kalmanSaveData(res, dT);
    return 0;
}

int kalmanCont::kalmanMakeCalculate(cv::Mat res, double dT) {



    meas.at<float>(0) = x;                                          //center x
    meas.at<float>(1) = y;                                          //center y
    meas.at<float>(2) = 50;                   //dlzka
    meas.at<float>(3) = 50;                  //vyska
    

    kalmanSaveData(res, dT);
    return 0;
}

cv::MatND kalmanCont::hist() const {
    return hist_;
}

float kalmanCont::get_kalman_x_pos() const {
    return x;
}

float kalmanCont::get_kalman_y_pos() const {
    return y;
}

float kalmanCont::last_x_pos() const {
    return last_x_pos_;
}

float kalmanCont::last_y_pos() const {
    return last_y_pos_;
}

void kalmanCont::add_usingRate() {
    using_rate_++;
}

int kalmanCont::get_usingRate() const {
    return using_rate_;
}

int kalmanCont::starting_y_pos() const {
    return starting_y_pos_;
}
int kalmanCont::starting_x_pos() const {
    return starting_x_pos_;
}
void kalmanCont::set_startingYpos(int y_set) {
    starting_y_pos_ = y_set;
}
void kalmanCont::set_startingXpos(int x_set) {
    starting_x_pos_ = x_set;
}

void kalmanCont::set_counter() {
    counter_++;

}

int kalmanCont::counter() const {
    return counter_;
}


int kalmanCont::id() const {
    return id_;
}

void kalmanCont::set_id(int id_new) {
    id_ = id_new;
}

int kalmanCont::object_area() const {
    return object_area_;
}
