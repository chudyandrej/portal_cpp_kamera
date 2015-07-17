#include "opencv.h"
#include "communication.h"
#include <thread>
#include <fstream>

#include <signal.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include "loaded_data.h"

typedef struct {
    cv::Mat frame;
    cv::Mat fgKNN;
    double tick;
} frame_wrap_t;

bool with_gui =false;
bool with_fps = false;
frame_wrap_t frame1,frame2,frame3;
vector<frame_wrap_t> frames;
cv::VideoCapture cap;
sem_t *cap_m_1,*cap_m_2,*cap_m_3,*push_m_1,*push_m_2,*push_m_3,*write_to_list,*data_flow;



void openCV();
void BG_thred1();
void BG_thred2();
void BG_thred3();

void contro_c(int param);
void init ();
void dealock_void();

int main(int argc, char *argv[]){
    setbuf(stdout, NULL);
    for (int i =0; i < argc ; i++) {
        if (strcmp(argv[i], "-gui") == 0) {
            with_gui = true;
        }
        else if (strcmp(argv[i], "-fps") == 0) {
            with_fps = true;
        }
    }
    dealock_void();
    signal(SIGTERM, contro_c);
    signal(SIGINT, contro_c);
    init ();
    cap = init_cap_bg("/home/andrej/Music/video_easy/pi_video4.mkv");
    sem_post(cap_m_1);
    sem_post(push_m_1);

    std::thread cv (openCV);
    std::thread thred1 (BG_thred1);
    std::thread thred2 (BG_thred2);
    std::thread thred3 (BG_thred3);



    cv.join();
    thred1.join();
    thred2.join();
    thred3.join();
    return EXIT_SUCCESS;
}


void openCV() {

    while (1){

        sem_wait(data_flow);
        cv::Mat m1,m2;
        double m3;
        sem_wait(write_to_list);

        m1=frames[0].frame;
        m2=frames[0].fgKNN;
        m3 = frames[0].tick;
        frames.erase (frames.begin());
        sem_post(write_to_list);

        printf("%d\n",frames.size());
        make_calculation(m1, m2, m3);
        if(with_gui) {
            waitKey(1);
        }
    }


}

void BG_thred1(){
    while(1) {

        sem_wait(cap_m_1);

        cap >> frame1.frame;
        frame1.tick = (double) cv::getTickCount();
        sem_post(cap_m_2);

        BgSubtractor(frame1.frame , frame1.fgKNN);

        sem_wait(push_m_1);

        sem_wait(write_to_list);
        frames.push_back(frame1);
        sem_post(write_to_list);
        sem_post(data_flow);

        sem_post(push_m_2);
        usleep(1);




    }
}
void BG_thred2(){
    while(1) {
        sem_wait(cap_m_2);
        cap >> frame2.frame;
        frame2.tick = (double) cv::getTickCount();
        sem_post(cap_m_3);

        BgSubtractor(frame2.frame , frame2.fgKNN);

        sem_wait(push_m_2);
        sem_wait(write_to_list);

        frames.push_back(frame2);

        sem_post(write_to_list);
        sem_post(data_flow);
        sem_post(push_m_3);
        usleep(50000);
    }
}
void BG_thred3(){
    while(1) {
        sem_wait(cap_m_3);
        cap >> frame3.frame;
        frame3.tick = (double) cv::getTickCount();
        sem_post(cap_m_1);

        BgSubtractor(frame3.frame , frame3.fgKNN);

        sem_wait(push_m_3);
        sem_wait(write_to_list);

        frames.push_back(frame3);

        sem_post(write_to_list);
        sem_post(data_flow);
        sem_post(push_m_1);
        usleep(100000);

    }
}

void contro_c(int param){
    dealock_void();
    fprintf(stderr,"Error: Bpl priaty signal SIGINT.\nProgram sa ukončuje.\n");

    exit(2);
}


void init (){
    cap_m_1 = sem_open("/cap_mutex_1", O_CREAT | O_EXCL, 0666, 0);
    cap_m_2 = sem_open("/cap_mutex_2", O_CREAT | O_EXCL, 0666, 0);
    cap_m_3 = sem_open("/cap_mutex_3", O_CREAT | O_EXCL, 0666, 0);
    push_m_1 = sem_open("/push_mutex_1", O_CREAT | O_EXCL, 0666, 0);
    push_m_2 = sem_open("/push_mutex_2", O_CREAT | O_EXCL, 0666, 0);
    push_m_3 = sem_open("/push_mutex_3", O_CREAT | O_EXCL, 0666, 0);
    write_to_list = sem_open("/atomic_operation", O_CREAT | O_EXCL, 0666, 1);
    data_flow = sem_open("/opencv_flow", O_CREAT | O_EXCL, 0666, 0);

}
void dealock_void(){
    sem_close(cap_m_1);
    sem_close(cap_m_2);
    sem_close(cap_m_3);
    sem_close(push_m_1);
    sem_close(push_m_2);
    sem_close(push_m_3);
    sem_close(write_to_list);
    sem_close(data_flow);

    sem_unlink("/cap_mutex_1");
    sem_unlink("/cap_mutex_2");
    sem_unlink("/cap_mutex_3");
    sem_unlink("/push_mutex_1");
    sem_unlink("/push_mutex_2");
    sem_unlink("/push_mutex_3");
    sem_unlink("/atomic_operation");
    sem_unlink("/opencv_flow");
}


