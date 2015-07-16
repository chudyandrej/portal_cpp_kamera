#include "opencv.h"
#include "communication.h"
#include <thread>
#include <fstream>
#include <array>
#include <iostream>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include "loaded_data.h"


cv::Mat old_rangeRes,old_frame;


cv::VideoCapture cap;
sem_t *stoping_BG,*data_flow;

void openCV();
void BG_thred1();
void BG_thred2();
void BG_thred3();
void communication();
void contro_c(int param);
void init ();
void dealock_void();

int main(){
    setbuf(stdout, NULL);

    dealock_void();
    signal(SIGTERM, contro_c);
    init ();
    cap = init_cap_bg("/home/andrej/Music/video_easy/pi_video4.mkv");


    std::thread cv (openCV);
    std::thread thred1 (BG_thred1);



   cv.join();
    thred1.join();

    return EXIT_SUCCESS;
}


void openCV(){
    while (1){
        sem_wait(data_flow);
        make_calculation(old_frame, old_rangeRes);
        waitKey(1);
        sem_post(stoping_BG);
    }
}

void BG_thred1(){
    bool first =false;
    cv::Mat rangeRes,frame;
    while(1) {
        sem_wait(stoping_BG);
        if (first){
            frame.copyTo(old_frame);
            rangeRes.copyTo(old_rangeRes);
            sem_post(data_flow);
        }
        cap >> frame;
        BgSubtractor(frame , rangeRes);
        first = true;
    }
}


void contro_c(int param){
    kill(getpid()*-1, SIGTERM);
    fprintf(stderr,"Error: Bpl priaty signal SIGINT.\nProgram sa ukončuje.\n");
    dealock_void();
    exit(2);
}


void init (){
    stoping_BG = sem_open("/cap_mutex_1", O_CREAT | O_EXCL, 0666, 2);
    data_flow = sem_open("/opencv_flow", O_CREAT | O_EXCL, 0666, 0);

}
void dealock_void(){
    sem_close(stoping_BG);
    sem_close(data_flow);

    sem_unlink("/cap_mutex_1");
    sem_unlink("/opencv_flow");
}
