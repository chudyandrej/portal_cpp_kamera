//
// Created by andrej on 9.7.2015.
//

#include "opencv.h"
int learning_history = 1000;
int thresholding = 80;
int min_area = 40000;
int min_dist_to_create = 400;
double max_dist_to_pars = 250;
double shadow_thresh = 0.5;
int frame_width = 1280;
int frame_height = 720;


int make_detection_transactions(){

    Ptr<BackgroundSubtractorKNN> pKNN; //MOG2 Background subtractor.


    int id = 0;
    int direction;
    int in = 0;
    int out = 0;


    cv::Mat frame;

    vector<kalmanCont> KalObjects;

    pKNN = createBackgroundSubtractorKNN();
    pKNN->setShadowThreshold(shadow_thresh);
    pKNN->setDetectShadows(true);
    pKNN->setDist2Threshold(3000);
    pKNN->setShadowValue(0);
    cv::VideoCapture cap;

    if (!cap.open(0)) {
        cout << "Webcam not connected.\n" << "Please verify\n";
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
    cap.set(CV_CAP_PROP_FPS, 2);
    namedWindow("Tracking", 0);
    namedWindow("Threshold", 0);

    while (1) {

        cap >> frame;
        cv::Mat res;
        frame.copyTo(res);
        cv::Mat rangeRes;
        pKNN->apply(frame, rangeRes);

        cv::erode(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 1);
        cv::dilate(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);

        cv::imshow("Threshold", rangeRes);
        //  line( res, Point( 0, 200 ), Point( 1280, 200), Scalar( 110, 220, 0 ),  2, 8 );
        //line( res, Point( 0, 520 ), Point( 1280, 520), Scalar( 110, 220, 0 ),  2, 8 );
        vector<vector<cv::Point> > contours;
        cv::findContours(rangeRes, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        // <<<<< Contours detection
        // >>>>> Filtering
        vector<vector<cv::Point> > objects;
        vector<cv::Rect> objectsBox;


        for (size_t i = 0; i < contours.size(); i++) {
            cv::Rect bBox;
            bBox = cv::boundingRect(contours[i]);


            // Searching for a bBox almost square
            if (bBox.area() >= min_area) {
                objects.push_back(contours[i]);
                objectsBox.push_back(bBox);

            }
        }

        for (size_t i = 0; i < objects.size(); i++) {

            int x = objectsBox[i].x + objectsBox[i].width / 2;
            int y = objectsBox[i].y + objectsBox[i].height / 2;


            int index_object = parsingContours(KalObjects, x, y,max_dist_to_pars);

            if (index_object == -1) {
                bool create = true;
                int min = min_dist_to_create;
                for (size_t k = 0; k < KalObjects.size(); k++) {
                    double distance = CalcDistance(x, KalObjects[k].getKalmanXpos(), y, KalObjects[k].getKalmanYpos());
                    //printf("Idem %f\n", distance);
                    if (min > distance) {
                        create = false;
                    }
                }
                if (create) {
                    kalmanCont newObject(0);
                    newObject.id = id;
                    newObject.set_startingYpos(y);
                    id++;
                    id = (id > 10) ? 0 : id;
                    newObject.kalmanMakeCalculate(res, objectsBox[i], false);
                    KalObjects.push_back(newObject);

                }

            }
            else {
                KalObjects[index_object].kalmanMakeCalculate(res, objectsBox[i], false);

            }
        }
        for (size_t i = 0; i < KalObjects.size(); i++) {

            KalObjects[i].add_usingRate();
            KalObjects[i].add_counter();
            KalObjects[i].set_addCounture(false);

            if (KalObjects[i].get_usingRate() > 1) {
                if (KalObjects[i].get_counter() < 5)
                    KalObjects.erase(KalObjects.begin() + i);


                if (KalObjects[i].get_counter() > 25) {
                    if (!(KalObjects[i].get_centerY() > frame_height - frame_height / 8 || KalObjects[i].get_centerY() < frame_height / 8)) {
                        cv::Rect objectsBoxKalman;
                        KalObjects[i].kalmanMakeCalculate(res, objectsBoxKalman, true);
                    }
                    else {
                        if ((direction = (int) (KalObjects[i].get_startingYpos() - KalObjects[i].get_centerY())) < 0) {
                            if (abs(direction) > frame_height / 2)
                                in++;
                        }
                        else {
                            if (abs(direction) > frame_height / 2)
                                out++;
                        }
                        KalObjects.erase(KalObjects.begin() + i);

                    }
                }
                if (KalObjects[i].get_usingRate() > 20) {
                    if ((direction = (int) (KalObjects[i].get_startingYpos() - KalObjects[i].get_centerY())) < 0) {
                        if (abs(direction) > frame_height / 2)
                            in++;
                    }
                    else {
                        if (abs(direction) > frame_height / 2)
                            out++;
                    }
                    KalObjects.erase(KalObjects.begin() + i);
                }
            }
            //*** print main contours ***//
            cv::rectangle(res, KalObjects[i].objectsBoxCopy,
                          CV_RGB(KalObjects[i].R, KalObjects[i].G, KalObjects[i].B), 2);
            cv::Point center;
            center.x = (int) KalObjects[i].get_centerX();
            center.y = (int) KalObjects[i].get_centerY();
            cv::circle(res, center, 2, CV_RGB(KalObjects[i].R, KalObjects[i].G, KalObjects[i].B), -1);
            stringstream sstr;
            sstr << "Objekt" << KalObjects[i].id;
            cv::putText(res, sstr.str(), cv::Point(center.x + 3, center.y - 3), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                        CV_RGB(KalObjects[i].R, KalObjects[i].G, KalObjects[i].B), 2);
            //*** print main contours ***//
        }
        stringstream ss;
        ss << out;
        string counter = ss.str();
        putText(res, counter.c_str(), cv::Point(20, 10), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 255, 0),
                1);

        stringstream ss2;
        ss2 << in;
        string counter2 = ss2.str();
        putText(res, counter2.c_str(), cv::Point(5, 150), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, cv::Scalar(0, 0, 255),
                1);

        cv::imshow("Tracking", res);

        if (waitKey(1) == 27){ //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
    return 0;
}

int parsingContours(vector<kalmanCont>& KalObjects, int x,int y,  double max) {
    double distance = -1;
    int r = -1;
    for (size_t i = 0; i <KalObjects.size() ; i++) {
        distance = CalcDistance(x,KalObjects[i].getKalmanXpos(),y, KalObjects[i].getKalmanYpos());

        if (max > distance && !(KalObjects[i].get_addCounture())) {
            max = distance;
            r = (int)i;
            KalObjects[i].set_addCounture(true);
        }
    }

    if (r == -1 ) {
        for (size_t k = 0; k < KalObjects.size(); k++) {
            distance = CalcDistance(x, KalObjects[k].get_centerX(), y, KalObjects[k].get_centerY());

            if (max > distance && !(KalObjects[k].get_addCounture())) {
                max = distance;
                r = (int) k;
                KalObjects[k].set_addCounture(true);
            }
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