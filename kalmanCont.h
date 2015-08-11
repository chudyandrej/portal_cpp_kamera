//
// Created by andrej on 29.6.2015.
//

#ifndef PORTALS_KALMANCONT_H
#define PORTALS_KALMANCONT_H

#include <opencv2/video/tracking.hpp>
#include <iostream>


extern bool with_fps;

class kalmanCont {

public:

    kalmanCont();

    int kalmanMakeCalculate(cv::Mat res, cv::Rect objectsBox,bool Kalman_object,float dT);


    void add_usingRate();

    void set_startingYpos(int y_set);
    void set_startingXpos(int x_set);

    int get_usingRate() const ;

    float getKalmanXpos() const;

    float getKalmanYpos() const;

    float get_centerX() const ;

    float get_centerY() const ;

    int get_startingYpos() const ;

    int get_startingXpos() const ;

    void add_counter();

    int get_counter() const ;

    bool get_addCounture() const;

    void set_addCounture(bool status);

    void set_id(int id_new);

    int get_id() const;

    cv::Rect objectsBoxCopy;

    int R,G,B;



private:
    cv::KalmanFilter kf;
    cv::Mat meas;
    cv::Mat state;
    int usingRATE,startingYpso = 0 ,startingXpso = 0,counter=0,id;

    bool found,addCounture = false;
    float x,y,lastX,lastY;


};


#endif //PORTALS_KALMANCONT_H
