#include <opencv2/core/core.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include "opencv2/imgcodecs.hpp"
#include "kalmanCont.h"
#include "cJSON.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sstream>


using namespace curlpp::options;
using namespace std;
using namespace cv;

Ptr<BackgroundSubtractorMOG2> pMOG2; //MOG2 Background subtractor.

double vypDistance(float x_1, float x_2, float y_1, float y_2);
int parsingContours(vector<kalmanCont>& KalObjects, int x,int y);
int openCV();
void doit(const char *text);



int main(){
    std::string asAskedInQuestion;
    int exitCODE;
    pid_t OpenCV = fork();				// Vytvaranie forka OpenCV
    if (OpenCV == 0) {                  // Zaciatok forka OpenCV
        exitCODE = openCV();
        if (exitCODE == -1)
            exit(2);
    }
    else if (OpenCV > 0){
        try {
            std::ostringstream os;
            os << curlpp::options::Url(std::string("http://apis-portals.herokuapp.com/api/portal_endpoint/settings/1"));

            asAskedInQuestion = os.str();

            std::cout << asAskedInQuestion << std::endl;
        }
        catch(curlpp::RuntimeError & e){
            std::cout << e.what() << std::endl;
        }
        catch(curlpp::LogicError & e){
            std::cout << e.what() << std::endl;
        }
        const  char * c = asAskedInQuestion.c_str();

        doit(c);

        cJSON *root;
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "direction", cJSON_CreateString("In"));
        cJSON_AddNumberToObject(root, "tagId", 123456789);
        char *test = cJSON_Print(root);
        printf("%s\n",test);
        int i = (int) strlen(test);
        char *url = "http://192.168.1.101:3000/api/portal_endpoint/transaction/1";

        try {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            request.setOpt(new curlpp::options::Url(url));
            request.setOpt(new curlpp::options::Verbose(true));

            std::list<std::string> header;
            header.push_back("Content-Type: application/json");

            request.setOpt(new curlpp::options::HttpHeader(header));

            request.setOpt(new curlpp::options::PostFields(test));
            request.setOpt(new curlpp::options::PostFieldSize(i));

            request.perform();
        }
        catch ( curlpp::LogicError & e ) {
            std::cout << e.what() << std::endl;
        }
        catch ( curlpp::RuntimeError & e ) {
            std::cout << e.what() << std::endl;
        }


        waitpid(OpenCV, NULL, 0);

    }


    return EXIT_SUCCESS;
}

int parsingContours(vector<kalmanCont>& KalObjects, int x,int y) {
    double distance = -1;
    double min = 250;
    int r = -1;
    for (size_t i = 0; i <KalObjects.size() ; i++) {
         distance = vypDistance(x,KalObjects[i].getKalmanXpos(),y, KalObjects[i].getKalmanYpos());

        if (min > distance && !(KalObjects[i].get_addCounture())) {
            min = distance;
            r = (int)i;
            KalObjects[i].set_addCounture(true);
        }
    }

    if (r == -1 ) {
        for (size_t k = 0; k < KalObjects.size(); k++) {
             distance = vypDistance(x, KalObjects[k].get_centerX(), y, KalObjects[k].get_centerY());

            if (min > distance && !(KalObjects[k].get_addCounture())) {
                min = distance;
                r = (int) k;
                KalObjects[k].set_addCounture(true);
            }
        }
    }

    printf("vzdialenost: %f %d\n",  distance,r);

    return  r;
}

double vypDistance(float x_1, float x_2, float y_1, float y_2){

    float a, b;
    double distance;
    a = abs(x_1 - x_2);
    b = abs(y_1 - y_2);
    distance = sqrt(pow((double) a, 2) + pow((double) b, 2));
    return distance;
}

int openCV(){
    int id = 0;
    double Tau = 0.5;
    int shadowColor = 0;
    int direction;
    int in = 0;
    int out = 0;
    const int FRAME_WIDTH = 1280;
    const int FRAME_HEIGHT = 720;

    cv::Mat frame;

    vector<kalmanCont> KalObjects;

    pMOG2 = createBackgroundSubtractorMOG2(1000, 80, true);
    pMOG2->setShadowThreshold(Tau);
    pMOG2->setShadowValue(shadowColor);
    cv::VideoCapture cap;

    if (!cap.open("/home/andrej/Music/vdeo4.avi")) {
        cout << "Webcam not connected.\n" << "Please verify\n";
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
    cap.set(CV_CAP_PROP_FPS, 2);
    namedWindow("Tracking", 0);
    namedWindow("Threshold", 0);

    while (1) {

        cap >> frame;
        cv::Mat res;
        frame.copyTo(res);
        cv::Mat rangeRes;
        pMOG2->apply(frame, rangeRes);

        cv::erode(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 8);
        cv::dilate(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 18);

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
            if (bBox.area() >= 40000) {
                objects.push_back(contours[i]);
                objectsBox.push_back(bBox);

            }
        }

        for (size_t i = 0; i < objects.size(); i++) {

            int x = objectsBox[i].x + objectsBox[i].width / 2;
            int y = objectsBox[i].y + objectsBox[i].height / 2;


            int index_object = parsingContours(KalObjects, x, y);

            if (index_object == -1) {
                bool create = true;
                int min = 400;
                for (size_t k = 0; k < KalObjects.size(); k++) {
                    double distance = vypDistance(x, KalObjects[k].getKalmanXpos(), y,
                                                  KalObjects[k].getKalmanYpos());
                    printf("Idem %f\n", distance);
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


                if (KalObjects[i].get_counter() > 10) {
                    if (!(KalObjects[i].get_centerY() > FRAME_HEIGHT - 150 || KalObjects[i].get_centerY() < 150)) {
                        cv::Rect objectsBoxKalman;
                        KalObjects[i].kalmanMakeCalculate(res, objectsBoxKalman, true);
                    }
                    else {
                        if ((direction = (int) (KalObjects[i].get_startingYpos() - KalObjects[i].get_centerY())) < 0) {
                            if (abs(direction) > FRAME_HEIGHT / 2)
                                in++;
                        }
                        else {
                            if (abs(direction) > FRAME_HEIGHT / 2)
                                out++;
                        }
                        KalObjects.erase(KalObjects.begin() + i);

                    }
                }
                if (KalObjects[i].get_usingRate() > 20) {
                    if ((direction = (int) (KalObjects[i].get_startingYpos() - KalObjects[i].get_centerY())) < 0) {
                        if (abs(direction) > FRAME_HEIGHT / 2)
                            in++;
                    }
                    else {
                        if (abs(direction) > FRAME_HEIGHT / 2)
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
        putText(res, counter.c_str(), cv::Point(100, 150), FONT_HERSHEY_SCRIPT_SIMPLEX, 4, cv::Scalar(0, 255, 0),
                5);

        stringstream ss2;
        ss2 << in;
        string counter2 = ss2.str();
        putText(res, counter2.c_str(), cv::Point(100, 600), FONT_HERSHEY_SCRIPT_SIMPLEX, 4, cv::Scalar(0, 0, 255),
                5);

        cv::imshow("Tracking", res);

        if (waitKey(1) == 27){ //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
    return 0;
}
void doit( const char *text)
{
    char *out;
    cJSON *json;

    json=cJSON_Parse(text);
    if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
    else
    {
        out = cJSON_Print(json);
        int format = cJSON_GetObjectItem(json,"data1")->valueint;
        cJSON_Delete(json);
        printf(" Hodnota : %d\n",format);

    }
}

