//
// Created by andrej on 9.7.2015.
//

#include "opencv.h"

#define upDown true


int learning_history = 1000;
int thresholding = 1300;
int min_area = 1500;
int min_dist_to_create = 100;
double max_dist_to_pars = 80;
double shadow_thresh = 0.7;
int frame_width = 320;
int frame_height = 240;
int id = 0;
int in = 0;
int out = 0;
double precTick = 0;
vector<kalmanCont> KalObjects;
Ptr<BackgroundSubtractorKNN> pKNN;


cv::VideoCapture init_cap_bg(const char *url){

    cv::VideoCapture cap;
    if (!cap.open(0)) {
        cout << "Webcam not connected.\n" << "Please verify\n";
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
    cap.set(CV_CAP_PROP_FPS , 30);
    pKNN = createBackgroundSubtractorKNN();
    pKNN->setShadowThreshold(shadow_thresh);
    pKNN->setDetectShadows(true);
    pKNN->setDist2Threshold(thresholding);
    pKNN->setHistory(learning_history);
    pKNN->setShadowValue(0);
    return cap;
}

void BgSubtractor(cv::Mat &frames , cv::Mat &rangeRess){
    pKNN->apply(frames, rangeRess);
}

void make_calculation(cv::Mat &res, cv::Mat &rangeRes, double tick){


    cv::Mat thresh_frame;
    rangeRes.copyTo(thresh_frame);

    double dT =  ((tick - precTick ) / cv::getTickFrequency()); //seconds
    precTick = tick;
    if(with_fps) {
        printf("FPS ticks : %f\n", (float) 1 / dT);
    }

    cv::erode(thresh_frame, thresh_frame, cv::Mat(), cv::Point(-1, -1), 5);
    cv::dilate(thresh_frame, thresh_frame, cv::Mat(), cv::Point(-1, -1), 8);

    if(with_gui) {
        cv::imshow("Trashold", thresh_frame);
    }
    vector<vector<cv::Point>> contours;
    cv::findContours(thresh_frame, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

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

    for (size_t i = 0; i < KalObjects.size(); i++) {
        int contourID = parsingContours(objects,objectsBox,KalObjects[i].getKalmanXpos(),KalObjects[i].getKalmanYpos(),max_dist_to_pars);
        if (contourID == -1 || (KalObjects[i].get_counter() < 14)){
            contourID = parsingContours(objects,objectsBox,KalObjects[i].get_centerX(),KalObjects[i].get_centerY(),max_dist_to_pars);
            if(contourID == -1) {
                if (KalObjects[i].get_counter() < 2) {
                    KalObjects.erase(KalObjects.begin() + i);
                }
                else {
                    if (person_flow == upDown) {
                        if (!(KalObjects[i].get_centerY() > frame_height - frame_height / 6 ||
                              KalObjects[i].get_centerY() < frame_height / 6)) {

                            cv::Rect objectsBoxKalman;
                            KalObjects[i].kalmanMakeCalculate(res, objectsBoxKalman, false, dT);
                        }
                        else {
                            counter_person_flow((int) i, person_flow);
                        }
                    }
                    else {
                        if (!(KalObjects[i].get_centerX() > frame_width - frame_width / 6 ||
                              KalObjects[i].get_centerX() < frame_width / 6)) {

                            cv::Rect objectsBoxKalman;
                            KalObjects[i].kalmanMakeCalculate(res, objectsBoxKalman, false, dT);
                        }
                        else {
                            counter_person_flow((int) i, person_flow);
                        }
                    }
                }
                if (KalObjects[i].get_usingRate() > 30) {
                    counter_person_flow((int)i,person_flow);
                }
            }
            else {

                KalObjects[i].kalmanMakeCalculate(res, objectsBox[contourID], true, dT);
                KalObjects[i].set_addCounture(true);
                objectsBox.erase (objectsBox.begin()+ contourID);
                objects.erase (objects.begin()+ contourID);
            }
        }
        else{

            KalObjects[i].kalmanMakeCalculate(res, objectsBox[contourID], true, dT);
            KalObjects[i].set_addCounture(true);
            objectsBox.erase (objectsBox.begin()+ contourID);
            objects.erase (objects.begin()+ contourID);
        }

    }

    for (size_t i = 0; i < objects.size(); i++) {
        bool create = true;
        int x = objectsBox[i].x + objectsBox[i].width / 2;
        int y = objectsBox[i].y + objectsBox[i].height / 2;

        for (size_t k = 0; k < KalObjects.size(); k++) {
            double distance = CalcDistance(x , KalObjects[k].get_centerX(), y, KalObjects[k].get_centerY());
            if (min_dist_to_create > distance) {
                create = false;
            }
        }

        if (create && ((x > frame_width - frame_width / 6 || x < frame_width / 6) || (y > frame_height - frame_height / 6 || y < frame_height / 6))) {
            kalmanCont newObject;
            newObject.set_id(id);
            newObject.set_startingYpos(objectsBox[i].y + objectsBox[i].height / 2);
            newObject.set_startingXpos(objectsBox[i].x + objectsBox[i].width / 2);


            newObject.kalmanMakeCalculate(res, objectsBox[i], true, dT);
            newObject.set_addCounture(true);
            KalObjects.push_back(newObject);
            id++;
            id = (id > 10) ? 0 : id;
        }
    }
        for (size_t i = 0; i < KalObjects.size(); i++) {

            KalObjects[i].add_usingRate();
            KalObjects[i].add_counter();
            KalObjects[i].set_addCounture(false);
            if (with_gui) {
                cv::rectangle(res, KalObjects[i].objectsBoxCopy,CV_RGB(KalObjects[i].R, KalObjects[i].G, KalObjects[i].B), 2);
                cv::Point center;
                center.x = (int) KalObjects[i].get_centerX();
                center.y = (int) KalObjects[i].get_centerY();
                cv::circle(res, center, 2, CV_RGB(KalObjects[i].R, KalObjects[i].G, KalObjects[i].B), -1);
                stringstream sstr;
                sstr << "Objekt" << KalObjects[i].get_id();
                cv::putText(res, sstr.str(), cv::Point(center.x + 3, center.y - 3), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                                CV_RGB(KalObjects[i].R, KalObjects[i].G, KalObjects[i].B), 2);
            }
        }
        if (with_gui) {
            stringstream ss;
            ss << out;
            string counter = ss.str();
            putText(res, counter.c_str(), cv::Point(5, 30), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 255, 0),1);


            stringstream ss2;
            ss2 << in;
            string counter2 = ss2.str();
            if (person_flow == upDown) {
                putText(res, counter2.c_str(), cv::Point(5, frame_height - 30), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 0, 255),1);
            }
            else {
                putText(res, counter2.c_str(), cv::Point(frame_width - 70, 30 ), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 0, 255),1);
            }
            cv::imshow("Tracking", res);
        }
        if (!with_gui){
           // printf("in: %d, out: %d\n",in,out);
        }

}

int parsingContours(vector<vector<cv::Point>> &objects,vector<cv::Rect> &objectsBox, float x,float y,  double max) {
    double distance;
    int r = -1;
    for (size_t i = 0; i < objects.size(); i++) {
        distance = CalcDistance(x,objectsBox[i].x + objectsBox[i].width / 2,y, objectsBox[i].y + objectsBox[i].height / 2);

        if (max > distance) {
            max = distance;
            r = (int)i;
        }
    }
    return  r;
}

double CalcDistance(float x_1, float x_2, float y_1, float y_2){
    float a, b;
    double distance;

    a = abs(x_1 - x_2);
    b = abs(y_1 - y_2);
    distance = sqrt(pow((double) a, 2) + pow((double) b, 2));

    return distance;
}



void counter_person_flow(int object_index , bool direction){
    double distance;

    if (direction == upDown) {
        distance = (KalObjects[object_index].get_startingYpos() - KalObjects[object_index].get_centerY());
    }
    else{
        distance = (KalObjects[object_index].get_startingXpos() - KalObjects[object_index].get_centerX());
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
    KalObjects.erase(KalObjects.begin() + object_index);
}





