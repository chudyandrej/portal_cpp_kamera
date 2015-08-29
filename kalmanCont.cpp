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

void kalmanCont::kalmanSaveData(predict_position_t pos, cv::Mat res, double dT) {

    history_.push(pos);

    time_since_start_ += dT;
    //printf("%d \n",(int)history_.size());
    if (history_.size() >= 20) {

        time_since_start_ -= history_.front().t;
        history_.pop();
    }

    kalman_pred_x = history_.back().x;
    kalman_pred_y = history_.back().y;

    if (kalman_pred_x <= 0) {
        kalman_pred_x = 0;
    }
    if (kalman_pred_x >= frame_width) {
        kalman_pred_x = frame_width;
    }
   /* if (kalman_pred_y <= 0) {
        kalman_pred_y = 0;
    }
    if (kalman_pred_x >= frame_height) {
        kalman_pred_x = frame_height;
    }
*/

    cv::Point center;
    center.x = (int) kalman_pred_x;
    center.y = (int) kalman_pred_y;
    cv::circle(res, center, 5, CV_RGB(R,G,B), -1);
    cv::circle(res, center, 50, CV_RGB(R,G,B), 0);

}

int kalmanCont::kalmanMakeCalculate(cv::Mat res, cv::Rect bBox, double dT, cv::MatND hist) {
    hist_ = hist;
    objectsBoxCopy = bBox;

    using_rate_ = 0;
    last_x_pos_ = bBox.x + bBox.width / 2;
    last_y_pos_ = bBox.y + bBox.height / 2;

    predict_position_t pos;
    pos.x = last_x_pos_;
    pos.y = last_y_pos_;
    pos.t = dT;

    kalmanSaveData(pos, res, dT);
    return 0;
}

int kalmanCont::kalmanMakeCalculate(cv::Mat res, double dT) {
    double v_x = ((history_.back().x - history_.front().x) / time_since_start_);
    double v_y = ((history_.back().y - history_.front().y) / time_since_start_);

    predict_position_t pos;
    pos.x =  (history_.back().x + dT * v_x);
    pos.y =  (history_.back().y + dT * v_y);
    pos.t = dT;

    kalmanSaveData(pos, res, dT);
    return 0;
}

cv::MatND kalmanCont::hist() const {
    return hist_;
}

double kalmanCont::get_kalman_x_pos() const {
    return kalman_pred_x;
}

double kalmanCont::get_kalman_y_pos() const {
    return kalman_pred_y;
}

double kalmanCont::last_x_pos() const {
    return last_x_pos_;
}

double kalmanCont::last_y_pos() const {
    return last_y_pos_;
}

void kalmanCont::add_usingRate() {
    using_rate_++;
}

int kalmanCont::get_usingRate() const {
    return using_rate_;
}

double kalmanCont::starting_y_pos() const {
    return starting_y_pos_;
}
double kalmanCont::starting_x_pos() const {
    return starting_x_pos_;
}
void kalmanCont::set_startingYpos(double y_set) {
    starting_y_pos_ = y_set;
}
void kalmanCont::set_startingXpos(double x_set) {
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



void kalmanCont::push_selected_conture(int id, double distance) {

    selected_counture.push_back(conture_t(id,distance));

}

void kalmanCont::sort_conture_low_high() {
    std::sort(selected_counture.begin(), selected_counture.end(), less_than_distance_cont());
    printf("ID:%d",id_);
    for (int i = 0; i < selected_counture.size(); i++) {
        printf(" %f ", selected_counture[i].distance_cont);
    }
    printf("\n");
}

void kalmanCont::clear_history_frams() {
    selected_counture.clear();
}

double kalmanCont::distance_nearest_counture() const {
    return selected_counture[0].distance_cont;

}

double kalmanCont::distance_second_nearest_counture() const {
    if(selected_counture.size() >= 2) {
        return selected_counture[1].distance_cont;
    }
    else{
        return -1;
    }
}

int kalmanCont::index_object() const {
    return index_object_;
}

void kalmanCont::set_index_object(int index) {
    index_object_ = index;
}


void kalmanCont::set_change_startin_pos(bool value) {
    change_startin_pos_ = value;
}

bool kalmanCont::change_startin_pos() const {
    return change_startin_pos_;
}
