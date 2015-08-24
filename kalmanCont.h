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

    int kalmanMakeCalculate(cv::Mat res, cv::Rect objectsBox,bool object_on_frame,double dT);


    void add_usingRate();

    void set_startingYpos(int y_set);
    void set_startingXpos(int x_set);

    int get_usingRate() const ;

    float get_kalman_x_pos() const;

    float get_kalman_y_pos() const;

    float last_x_pos() const ;

    float last_y_pos() const ;

    int starting_y_pos() const ;

    int starting_x_pos() const ;

    void set_counter();

    int counter() const ;
   

    void set_id(int id_new);

    int id() const;
    
    int object_area() const;
    
    cv::Rect objectsBoxCopy;

    int R,G,B;



private:
    cv::KalmanFilter kf;
    cv::Mat meas;
    cv::Mat state;
    int using_rate_, starting_y_pos_ = 0 , starting_x_pos_ = 0, counter_ = 0, id_;
    int object_area_;
    bool first_start;
    float x,y, last_x_pos_, last_y_pos_;


};


#endif //PORTALS_KALMANCONT_H
