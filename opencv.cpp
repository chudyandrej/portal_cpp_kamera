//
// Created by andrej on 9.7.2015.
//

#include "opencv.h"
#define upDown true


int learning_history = 1000;
int thresholding = 1300;
int min_area = 1800;
int min_dist_to_create = 100;
double max_dist_to_pars = 100;
double shadow_thresh = 0.7;
int frame_width = 320;
int frame_height = 240;
int id = 0;
int in = 0;
int out = 0;

double prev_tick = 0;

vector<kalmanCont> tracked_objects;
Ptr<BackgroundSubtractorKNN> pKNN;



cv::VideoCapture init_cap_bg(const char *url) {
    cv::VideoCapture cap;
    if (!cap.open(url)) {
        cout << "Webcam not connected.\n" << "Please verify\n";
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
    pKNN = createBackgroundSubtractorKNN();
    pKNN->setShadowThreshold(shadow_thresh);
    pKNN->setDetectShadows(true);
    pKNN->setDist2Threshold(thresholding);
    pKNN->setHistory(learning_history);
    pKNN->setShadowValue(0);
    return cap;
}

void BgSubtractor(cv::Mat *frame, cv::Mat *fg_mask) {
    pKNN->apply(*frame, *fg_mask);
}

int ProcessFrame(cv::Mat *frame, cv::Mat *fg_mask, double tick) {

    double dT = ((tick - prev_tick ) / cv::getTickFrequency()); //seconds
    prev_tick = tick;
    if(with_fps) {
        printf("FPS ticks : %f\n", (float) 1 / dT);
    }
    cv::Mat hsv;
    cvtColor(*frame, hsv, CV_BGR2HSV);

    cv::erode(*fg_mask, *fg_mask, cv::Mat(), cv::Point(-1, -1), 5);
    cv::dilate(*fg_mask, *fg_mask, cv::Mat(), cv::Point(-1, -1), 8);

    if(with_gui) {
        cv::imshow("Threshold", *fg_mask);
    }
    vector<vector<cv::Point>> contours;
    cv::findContours(*fg_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    vector<contour_t> found_contures;
    contour_t new_contour;
    int counter = 0;

    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect bBox;
        cv::Moments mu;
        bBox = cv::boundingRect(contours[i]);
        mu = moments( contours[i], false);

        if (bBox.area() >= min_area) {
            new_contour.id = counter;
            new_contour.contours = contours[i];
            new_contour.mu = mu;
            new_contour.contour_use = false;
            counter++;
            found_contures.push_back(new_contour);
        }
    }

    loadValidCounureToObject(found_contures, tracked_objects);                             //načítanie všetkých vzdialeností od kontur a usporiadanie

    std::sort(tracked_objects.begin(),tracked_objects.end(),comp);
    for (size_t i = 0; i < tracked_objects.size(); i++) {
        tracked_objects[i].set_index_object((int) i);
        if (tracked_objects[i].selected_counture.size() >= 1){                           //ak má objekt v okolí nejaké kontúry
            found_contures[tracked_objects[i].selected_counture[0].ID].candidate_object.push_back(tracked_objects[i]);    //pushne do contúry svoje ID
        }gir
    }
    for (size_t i = 0; i < tracked_objects.size(); i++) {


        int contourID = parsingContours(found_contures, tracked_objects[i]);

        if (contourID == -1){
            if (tracked_objects[i].counter() < 2) {
                tracked_objects.erase(tracked_objects.begin() + i);
                i--;
                continue;
            }
            else {
                if (!(tracked_objects[i].last_y_pos() > frame_height - frame_height / 6 ||
                        tracked_objects[i].last_y_pos() < frame_height / 6)) {
                    tracked_objects[i].kalmanMakeCalculate(*frame, dT);
                }
                else {
                    if (((tracked_objects[i].starting_y_pos() < frame_height / 2 &&
                            tracked_objects[i].last_y_pos() <  frame_height / 6 ) ||
                            (tracked_objects[i].starting_y_pos() > frame_height / 2 &&
                                    tracked_objects[i].last_y_pos() > frame_height - frame_height / 6)) &&
                            !(tracked_objects[i].change_startin_pos())) {

                        tracked_objects[i].kalmanMakeCalculate(*frame, dT);
                    }
                    else {
                        counterAbsPersonFlow((int) i);
                        tracked_objects.erase(tracked_objects.begin() + i);
                        i--;
                        continue;
                    }

                }
            }
            if (tracked_objects[i].get_usingRate() > 30) {
                counterAbsPersonFlow((int) i);
                tracked_objects.erase(tracked_objects.begin() + i);
                i--;
                continue;
            }
        }
        else{
            found_contures[contourID].contour_use = true;
            cv::MatND hist;// = CalcHistogramBase(hsv, found_contures[contourID].contours, contourID, tracked_objects[i].hist());
            tracked_objects[i].kalmanMakeCalculate(*frame, found_contures[contourID].mu, dT, hist);
            if (tracked_objects[i].starting_y_pos() < frame_height / 2 && tracked_objects[i].last_y_pos() > frame_height - frame_height / 4 ){
                if (counterAbsPersonFlow((int) i) == 0) {
                    tracked_objects[i].set_startingYpos(frame_height);
                    tracked_objects[i].set_change_startin_pos(true);
                }
            }
            if (tracked_objects[i].starting_y_pos() > frame_height / 2 && tracked_objects[i].last_y_pos() < frame_height / 4 ){
                if(counterAbsPersonFlow((int) i) == 0) {
                    tracked_objects[i].set_startingYpos(0);
                    tracked_objects[i].set_change_startin_pos(true);
                }
            }
        }
    }

    for (size_t i = 0; i < found_contures.size(); i++) {
        if (!found_contures[i].contour_use) {

            bool create = true;
            double x = found_contures[i].mu.m10 / found_contures[i].mu.m00;
            double y = found_contures[i].mu.m01 / found_contures[i].mu.m00;


            for (size_t k = 0; k < tracked_objects.size(); k++) {
                double distance = CalcDistance(x, tracked_objects[k].last_x_pos(), y, tracked_objects[k].last_y_pos());
                if (min_dist_to_create > distance) {
                    create = false;
                }
            }
            if (create) {
                kalmanCont newObject;
                newObject.set_id(id);
                newObject.set_startingYpos(y);
                newObject.set_startingXpos(x);

                cv::MatND hist;// = CalcHistogramContour(hsv, found_contures[i].contours, (int) i);
                newObject.kalmanMakeCalculate(*frame, found_contures[i].mu, dT, hist);

                tracked_objects.push_back(newObject);
                id++;
                id = (id > 10) ? 0 : id;
            }
        }
        found_contures[i].contour_use = false;
    }
    for (size_t i = 0; i < tracked_objects.size(); i++) {

        tracked_objects[i].add_usingRate();
        tracked_objects[i].set_counter();
        tracked_objects[i].clear_history_frams();
        if (with_gui) {
            cv::Point center;
            center.x = (int) tracked_objects[i].last_x_pos();
            center.y = (int) tracked_objects[i].last_y_pos();
            cv::circle(*frame, center, 2, CV_RGB(tracked_objects[i].R, tracked_objects[i].G, tracked_objects[i].B), -1);
            stringstream sstr;
            sstr << "Objekt" << tracked_objects[i].id();
            cv::putText(*frame, sstr.str(), cv::Point(center.x + 3, center.y - 3), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                            CV_RGB(tracked_objects[i].R, tracked_objects[i].G, tracked_objects[i].B), 2);
        }
    }
    if (with_gui) {
        stringstream ss;
        ss << out;
        string counter1 = ss.str();
        putText(*frame, counter1.c_str(), cv::Point(5, 30), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 255, 0),1);

        stringstream ss2;
        ss2 << in;
        string counter2 = ss2.str();
        putText(*frame, counter2.c_str(), cv::Point(5, frame_height - 30), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 0, 255),1);
        cv::imshow("Tracking", *frame);
    }
    if (!with_gui){
       // printf("in: %d, out: %d\n",in,out);
    }
    return 0;

}

void loadValidCounureToObject(vector<contour_t> &found_contures, vector<kalmanCont> &tracked_object){
    double max = max_dist_to_pars;
    double distance;

    for (size_t k = 0; k < tracked_objects.size(); k++) {
        for (size_t i = 0; i < found_contures.size(); i++) {
            distance = CalcDistance(tracked_object[k].get_kalman_x_pos(),
                                    found_contures[i].mu.m10 / found_contures[i].mu.m00,
                                    tracked_object[k].get_kalman_y_pos(),
                                    found_contures[i].mu.m01 / found_contures[i].mu.m00);

            if (distance < max) {
                tracked_object[k].push_selected_conture(found_contures[i].id, distance);

            }
        }
        tracked_object[k].sort_conture_low_high();

    }
}

double CalcDistance(double x1, double x2, double y1, double y2){
    double a, b;
    double distance;

    a = abs(x1 - x2);
    b = abs(y1 - y2);
    distance = sqrt(pow(a, 2) + pow(b, 2));

    return distance;
}

int parsingContours(vector<contour_t> &found_contures, kalmanCont &tracked_object) {
    double max = max_dist_to_pars;
    double distance;
    int conture_id = -1;


    for (int i = 0; i < found_contures.size(); i++) {
        distance = CalcDistance(tracked_object.get_kalman_x_pos(), found_contures[i].mu.m10 / found_contures[i].mu.m00,
                                tracked_object.get_kalman_y_pos(), found_contures[i].mu.m01 / found_contures[i].mu.m00);
        distance = distance + 20 * found_contures[i].candidate_object.size();
        if (distance < max && !found_contures[i].contour_use) {
            max = distance;
            conture_id = i ;
        }
    }

    return  conture_id;
}

int counterAbsPersonFlow(int object_index){
    double distance;
    int result = -1;

    distance = (tracked_objects[object_index].starting_y_pos() - tracked_objects[object_index].get_kalman_y_pos());

    if (distance < 0) {
        if (abs(distance) > frame_height / 2) {
            in++;
            result = 0;
            //  std::thread(send_transaction,"in").detach();
        }
    }
    else {
        if (abs(distance) > frame_height / 2) {
            out++;
            result = 0;
            //std::thread(send_transaction,"out").detach();
        }
    }
    return result;
}

bool comp(kalmanCont a,kalmanCont b){
    return a.distance_nearest_counture() < b.distance_nearest_counture();
}

