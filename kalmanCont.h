//
// Created by andrej on 29.6.2015.
//

#ifndef PORTALS_KALMANCONT_H
#define PORTALS_KALMANCONT_H
#include <queue>
#include <opencv2/video/tracking.hpp>
#include <iostream>

typedef struct {
    double x;
    double y;
    double t;
} predict_position_t;

struct conture_t{
    int ID;
    double distance_cont;
    conture_t(int k, double s) : ID(k), distance_cont(s) {}
};

struct less_than_distance_cont {
    inline bool operator() (const conture_t & struct1, const conture_t & struct2) {
        return (struct1.distance_cont < struct2.distance_cont);
    }
};

extern bool with_fps;

class kalmanCont {

public:

    kalmanCont();

    int kalmanMakeCalculate(cv::Mat res, cv::Rect bBox, double dT, cv::MatND hist);

    int kalmanMakeCalculate(cv::Mat res, double dT);

    void kalmanSaveData(predict_position_t pos, cv::Mat res, double dT);

    void add_usingRate();

    void set_startingYpos(double y_set);

    void set_startingXpos(double x_set);

    int get_usingRate() const ;

    double get_kalman_x_pos() const;

    double get_kalman_y_pos() const;

    double last_x_pos() const ;

    double last_y_pos() const ;

    double starting_y_pos() const ;

    double starting_x_pos() const ;

    void set_counter();

    int counter() const ;

    void set_id(int id_new);

    int id() const;

    void push_selected_conture(int id, double distance);

    void sort_conture_low_high();

    void clear_history_frams();

    double distance_nearest_counture() const;

    double distance_second_nearest_counture() const;

    void set_change_startin_pos(bool value) ;

    bool change_startin_pos() const;

    int index_object() const;

    void set_index_object(int index);


    cv::Rect objectsBoxCopy;

    int R,G,B;

    cv::MatND hist() const;

    std::vector<conture_t> selected_counture;


private:
    bool change_startin_pos_ = false;
    int using_rate_, counter_ = 0, id_, index_object_;
    double kalman_pred_x, kalman_pred_y, last_x_pos_, last_y_pos_, starting_x_pos_, starting_y_pos_;
    cv::MatND hist_;
    std::queue<predict_position_t> history_ ;
    double time_since_start_, distance_from_conture_;


};
#endif //PORTALS_KALMANCONT_H
