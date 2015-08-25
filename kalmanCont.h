//
// Created by andrej on 29.6.2015.
//

#ifndef PORTALS_KALMANCONT_H
#define PORTALS_KALMANCONT_H
#include <queue>
#include <opencv2/video/tracking.hpp>
#include <iostream>

extern bool with_fps;

typedef struct {
    double x;
    double y;
    double t;
} position_t;

class kalmanCont {

public:

    kalmanCont();

    int kalmanMakeCalculate(cv::Mat res, cv::Rect objectsBox, double dT, cv::MatND hist);

    int kalmanMakeCalculate(cv::Mat res, double dT);

    void kalmanSaveData(position_t pos, cv::Mat res, double dT);

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

    double distance_from_conture() const;

    void set_distance_from_conture(double value);



    cv::Rect objectsBoxCopy;

    int R,G,B;

    cv::MatND hist() const;

private:

    int using_rate_, starting_y_pos_ = 0 , starting_x_pos_ = 0, counter_ = 0, id_;
    int object_area_;
    float x,y, last_x_pos_, last_y_pos_;
    cv::MatND hist_;
    std::queue<position_t> history_;
    double time_since_start_, distance_from_conture_;


};
#endif //PORTALS_KALMANCONT_H
