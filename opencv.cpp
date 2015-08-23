//
// Created by andrej on 9.7.2015.
//

#include "opencv.h"
#define upDown true


int learning_history = 1000;
int thresholding = 1300;
int min_area = 1500;
int min_dist_to_create = 20;
double max_dist_to_pars = 80;
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
    if (!cap.open(0)) {
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
    double dT =  ((tick - prev_tick ) / cv::getTickFrequency()); //seconds
    prev_tick = tick;
    if(with_fps) {
        printf("FPS ticks : %f\n", (float) 1 / dT);
    }

    cv::erode(*fg_mask, *fg_mask, cv::Mat(), cv::Point(-1, -1), 5);
    cv::dilate(*fg_mask, *fg_mask, cv::Mat(), cv::Point(-1, -1), 8);

    if(with_gui) {
        cv::imshow("Threshold", *fg_mask);
    }
    vector<vector<cv::Point>> contours;
    cv::findContours(*fg_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    vector<vector<cv::Point> > objects;
    vector<cv::Rect> objectsBox;


    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect bBox;
        bBox = cv::boundingRect(contours[i]);

        if (bBox.area() >= min_area) {
            objects.push_back(contours[i]);
            objectsBox.push_back(bBox);
        }
    }

    for (size_t i = 0; i < tracked_objects.size(); i++) {
        int contourID = parsingContours(objects,objectsBox,tracked_objects[i].getKalmanXpos(),tracked_objects[i].getKalmanYpos(),max_dist_to_pars);
        if (contourID == -1 || (tracked_objects[i].get_counter() < 14)){
            contourID = parsingContours(objects,objectsBox,tracked_objects[i].get_centerX(),tracked_objects[i].get_centerY(),max_dist_to_pars);
            if(contourID == -1) {
                if (tracked_objects[i].get_counter() < 2) {
                    tracked_objects.erase(tracked_objects.begin() + i);
                }
                else {
                    if (person_flow == upDown) {
                        if (!(tracked_objects[i].get_centerY() > frame_height - frame_height / 6 ||
                              tracked_objects[i].get_centerY() < frame_height / 6)) {

                            cv::Rect objectsBoxKalman;
                            tracked_objects[i].kalmanMakeCalculate(*frame, objectsBoxKalman, false, dT);
                        }
                        else {
                             counter_person_flow((int) i, person_flow);
                             tracked_objects.erase(tracked_objects.begin() + i);
                        }
                    }
                    else {
                        if (!(tracked_objects[i].get_centerX() > frame_width - frame_width / 6 ||
                              tracked_objects[i].get_centerX() < frame_width / 6)) {

                            cv::Rect objectsBoxKalman;
                            tracked_objects[i].kalmanMakeCalculate(*frame, objectsBoxKalman, false, dT);
                        }
                        else {
                            counter_person_flow((int) i, person_flow);
                            tracked_objects.erase(tracked_objects.begin() + i);
                        }
                    }
                }
                if (tracked_objects[i].get_usingRate() > 30) {
                    counter_person_flow((int)i,person_flow);
                    tracked_objects.erase(tracked_objects.begin() + i);
                }
            }
            else {

                tracked_objects[i].kalmanMakeCalculate(*frame, objectsBox[contourID], true, dT);
       
                objectsBox.erase (objectsBox.begin()+ contourID);
                objects.erase (objects.begin()+ contourID);
            }
        }
        else{

            tracked_objects[i].kalmanMakeCalculate(*frame, objectsBox[contourID], true, dT);
           
            objectsBox.erase (objectsBox.begin()+ contourID);
            objects.erase (objects.begin()+ contourID);
        }

    }

    for (size_t i = 0; i < objects.size(); i++) {
        bool create = true;
        int x = objectsBox[i].x + objectsBox[i].width / 2;
        int y = objectsBox[i].y + objectsBox[i].height / 2;

        for (size_t k = 0; k < tracked_objects.size(); k++) {
            double distance = CalcDistance(x , tracked_objects[k].get_centerX(), y, tracked_objects[k].get_centerY());
            if (min_dist_to_create > distance) {
                create = false;
            }
        }

        if (create && ((x > frame_width - frame_width / 6 || x < frame_width / 6) || (y > frame_height - frame_height / 6 || y < frame_height / 6))) {
            kalmanCont newObject;
            newObject.set_id(id);
            newObject.set_startingYpos(objectsBox[i].y + objectsBox[i].height / 2);
            newObject.set_startingXpos(objectsBox[i].x + objectsBox[i].width / 2);


            newObject.kalmanMakeCalculate(*frame, objectsBox[i], true, dT);
            
            tracked_objects.push_back(newObject);
            id++;
            id = (id > 10) ? 0 : id;
        }
    }
    for (size_t i = 0; i < tracked_objects.size(); i++) {

        tracked_objects[i].add_usingRate();
        tracked_objects[i].add_counter();
        if (with_gui) {
            cv::rectangle(*frame, tracked_objects[i].objectsBoxCopy,CV_RGB(tracked_objects[i].R, tracked_objects[i].G, tracked_objects[i].B), 2);
            cv::Point center;
            center.x = (int) tracked_objects[i].get_centerX();
            center.y = (int) tracked_objects[i].get_centerY();
            cv::circle(*frame, center, 2, CV_RGB(tracked_objects[i].R, tracked_objects[i].G, tracked_objects[i].B), -1);
            stringstream sstr;
            sstr << "Objekt" << tracked_objects[i].get_id();
            cv::putText(*frame, sstr.str(), cv::Point(center.x + 3, center.y - 3), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                            CV_RGB(tracked_objects[i].R, tracked_objects[i].G, tracked_objects[i].B), 2);
        }
    }
    if (with_gui) {
        stringstream ss;
        ss << out;
        string counter = ss.str();
        putText(*frame, counter.c_str(), cv::Point(5, 30), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 255, 0),1);


        stringstream ss2;
        ss2 << in;
        string counter2 = ss2.str();
        if (person_flow == upDown) {
            putText(*frame, counter2.c_str(), cv::Point(5, frame_height - 30), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 0, 255),1);
        }
        else {
            putText(*frame, counter2.c_str(), cv::Point(frame_width - 70, 30 ), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 0, 255),1);
        }
        cv::imshow("Tracking", *frame);
    }
    if (!with_gui){
       // printf("in: %d, out: %d\n",in,out);
    }
    return 0;

}

int parsingContours(vector<vector<cv::Point>> &objects,vector<cv::Rect> &objectsBox, float x,float y,  double max) {
    double distance;
    int r = -1;
    for (int i = 0; i < objects.size(); i++) {
        distance = CalcDistance(x, objectsBox[i].x + objectsBox[i].width / 2, y, objectsBox[i].y + objectsBox[i].height / 2);

        if (max > distance) {
            max = distance;
            r = i;
        }
    }
    return  r;
}

double CalcDistance(float x1, float x2, float y1, float y2){
    float a, b;
    double distance;

    a = abs(x1 - x2);
    b = abs(y1 - y2);
    distance = sqrt(pow((double) a, 2) + pow((double) b, 2));

    return distance;
}



void counter_person_flow(int object_index , bool direction){
    double distance;

    if (direction == upDown) {
        distance = (tracked_objects[object_index].get_startingYpos() - tracked_objects[object_index].get_centerY());
    }
    else{
        distance = (tracked_objects[object_index].get_startingXpos() - tracked_objects[object_index].get_centerX());
    }

    int frame_size = (direction == upDown)? frame_height : frame_width;

    if (distance < 0) {
        if (abs(distance) > frame_size / 2) {
            in++;
          //  std::thread(send_transaction,"in").detach();
        }

    }
    else {
        if (abs(distance) > frame_size / 2) {
            out++;
            //std::thread(send_transaction,"out").detach();
        }
    }
   
}





