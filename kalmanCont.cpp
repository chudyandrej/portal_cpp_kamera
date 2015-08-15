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
    usingRATE= 0;

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

int kalmanCont::kalmanMakeCalculate(cv::Mat res,cv::Rect objectsBox, bool object_frame, float dT) {
    objectsBoxCopy = objectsBox;
    object_on_frame = object_frame;

    if (object_on_frame) {
        usingRATE = 0;
        lastX = objectsBox.x + objectsBox.width / 2;
        lastY = objectsBox.y + objectsBox.height / 2;

        meas.at<float>(0) = lastX;                      //center x
        meas.at<float>(1) =  lastY ;                  //center y
        meas.at<float>(2) = (float) objectsBox.width;                   //dlzka
        meas.at<float>(3) = (float) objectsBox.height;                  //vyska
    }
    else {
        meas.at<float>(0) = x;                                          //center x
        meas.at<float>(1) = y;                                          //center y
        meas.at<float>(2) = 50;                   //dlzka
        meas.at<float>(3) = 50;                  //vyska
    }
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

    kf.transitionMatrix.at<float>(2) = dT;
    kf.transitionMatrix.at<float>(9) = dT;

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

    return id;
}

float kalmanCont::getKalmanXpos() const {
    return x;
}

float kalmanCont::getKalmanYpos() const {
    return y;
}

float kalmanCont::get_centerX() const {
    return lastX;
}

float kalmanCont::get_centerY() const {
    return lastY ;
}

void kalmanCont::add_usingRate() {
    usingRATE++;
}

int kalmanCont::get_usingRate() const {
    return usingRATE;
}

int kalmanCont::get_startingYpos() const {
    return startingYpso;
}
int kalmanCont::get_startingXpos() const {
    return startingXpso;
}
void kalmanCont::set_startingYpos(int y_set) {
    startingYpso = y_set;
}
void kalmanCont::set_startingXpos(int x_set) {
    startingXpso = x_set;
}

void kalmanCont::add_counter() {
    counter++;

}

int kalmanCont::get_counter() const {
    return counter;
}

bool kalmanCont::get_addCounture() const {
    return addCounture;
}

void kalmanCont::set_addCounture(bool status) {
    addCounture = status;
}

int kalmanCont::get_id() const {
    return id;
}

void kalmanCont::set_id(int id_new) {
    id = id_new;
}

bool kalmanCont::get_object_on_frame() const {
    return object_on_frame;
}
