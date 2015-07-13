//
// Created by andrej on 29.6.2015.
//

#ifndef PORTALS_KALMANCONT_H
#define PORTALS_KALMANCONT_H

#include <opencv2/video/tracking.hpp>
#include <iostream>



class kalmanCont {

public:

    kalmanCont(float lastY);

    void kalmanMakeCalculate(cv::Mat res, cv::Rect objectsBox,cv::Moments mu ,bool Kalman_object);


    void add_usingRate();

    void set_startingYpos(int y);

    int get_usingRate() const ;

    float getKalmanXpos() const;

    float getKalmanYpos() const;

    float get_centerX() const ;

    float get_centerY() const ;

    int get_startingYpos() const ;

    void add_counter();

    int get_counter() const ;

    bool get_addCounture() const;

    void set_addCounture(bool status);


    cv::Rect objectsBoxCopy;

    int R,G,B,id;



private:
    cv::KalmanFilter kf;
    cv::Mat meas;
    cv::Mat state;
    int usingRATE,startingYpso,counter=0;
    double ticks = 0, precTick=0;
    bool found,addCounture = false;
    float dT,x,y,lastX,lastY;


};


#endif //PORTALS_KALMANCONT_H
