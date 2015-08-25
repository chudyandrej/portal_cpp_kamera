//
// Created by andrej on 29.6.2015.
//


#include "kalmanCont.h"
#include "loaded_data.h"

kalmanCont::kalmanCont() {


    using_rate_ = 0;

    R = rand() % 255;
    G = rand() % 255;
    B = rand() % 255;
}

void kalmanCont::kalmanSaveData(position_t pos, cv::Mat res, double dT) {

    history_.push(pos);

    time_since_start_ += dT;
    if (history_.size() >= 100) {
        //printf("%d, %d\n",history_.back().y,history_.front().y);
        time_since_start_ -= history_.front().t;
        history_.pop();
    }

    cv::Point center;
    center.x = (int) history_.back().x;
    center.y = (int) history_.back().y;

    if (center.x <= 0)
        center.x = 0;
    if (center.x >= frame_width)
        center.x = frame_width;
    if (center.y <= 0)
        center.y = 0;
    if (center.y >= frame_height)
        center.y = frame_height;

    cv::circle(res, center, 10, CV_RGB(R,G,B), -1);
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

    position_t pos;
    pos.x =  last_x_pos_;
    pos.y = last_y_pos_;
    pos.t = dT;

    kalmanSaveData(pos, res, dT);
    return 0;
}

int kalmanCont::kalmanMakeCalculate(cv::Mat res, double dT) {
    double v_x = ((history_.back().x - history_.front().x) / time_since_start_);
    double v_y = ((history_.back().y - history_.front().y) / time_since_start_);

    position_t pos;
    pos.x =  (history_.back().x + dT * v_x);
    pos.y =  (history_.back().y + dT * v_y);
    pos.t = dT;

    kalmanSaveData(pos, res, dT);
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


double kalmanCont::distance_from_conture() const {
    return distance_from_conture_;
}

void kalmanCont::set_distance_from_conture(double value) {
    distance_from_conture_ = value;

}
