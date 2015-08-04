#include "opencv.h"
#include "communication.h"
#include <signal.h>
#include "loaded_data.h"
#include "declarations.h"

#include <thread>
#include <unistd.h>

typedef struct {
    cv::Mat frame;
    cv::Mat fgKNN;
    double tick;
} frame_wrap_t;

int delay = 100000;
bool with_gui =false;
bool with_fps = false;
bool end_while = true;
bool person_flow = true;
frame_wrap_t frame1,frame2,frame3;
vector<frame_wrap_t> frames;
cv::VideoCapture cap;
sem_t *cap_m_1,*cap_m_2,*cap_m_3,*push_m_1,*push_m_2,*push_m_3,*write_to_list,*data_flow;

void openCV();
void BG_thred1();
void BG_thred2();
void BG_thred3();
void socket();


int main(int argc, char *argv[]){
    setbuf(stdout, NULL);
    arguments_read(argc, &argv[0]);
    save_tags();
    dealock_void();
    signal(SIGTERM, contro_c);
    signal(SIGINT, contro_c);
    init ();
    cap = init_cap_bg("/home/andrej/Music/pi_video6.mkv");

    std::thread cv (openCV);
    std::thread thred1 (BG_thred1);
    std::thread thred2 (BG_thred2);
    std::thread thred3 (BG_thred3);
    std::thread socket (socket);

    cv.join();
    thred1.join();
    thred2.join();
    thred3.join();
    socket.join();

    return EXIT_SUCCESS;
}

void openCV() {
    int counter =0;
    cv::Mat original_frame, subtract_frame;
    double frame_tick;
    namedWindow("Trashold",0);
    namedWindow("Tracking",0);
    while (end_while){
        sem_wait(data_flow);

        sem_wait(write_to_list);

        original_frame =frames[0].frame;
        subtract_frame =frames[0].fgKNN;
        frame_tick = frames[0].tick;
        frames.erase (frames.begin());
        counter++;
        if(counter == 3 && frames.size() == 0 ){
            sem_post(push_m_1);
            counter = 0;
        }
        sem_post(write_to_list);

        make_calculation(original_frame, subtract_frame, frame_tick);
        if(with_gui) {
            waitKey(1);
        }
    }
}
void BG_thred1(){
    while(end_while) {

        sem_wait(cap_m_1);

        if(!cap.read(frame1.frame)) {
            fprintf(stderr, "The camera has been disconnected!\n");
            dealock_void();
            exit(EXIT_FAILURE);
        }
        frame1.tick = (double) cv::getTickCount();
        sem_post(cap_m_2);

        BgSubtractor(frame1.frame , frame1.fgKNN);

        sem_wait(push_m_1);
        sem_wait(write_to_list);
        frames.push_back(frame1);
        sem_post(write_to_list);
        sem_post(data_flow);

        sem_post(push_m_2);
        usleep((__useconds_t) delay);
    }
}
void BG_thred2(){

    while(end_while) {
        sem_wait(cap_m_2);
        if(!cap.read(frame2.frame)) {
            fprintf(stderr, "The camera has been disconnected!\n");
            dealock_void();
            exit(EXIT_FAILURE);
        }
        frame2.tick = (double) cv::getTickCount();
        sem_post(cap_m_3);

        BgSubtractor(frame2.frame , frame2.fgKNN);

        sem_wait(push_m_2);
        sem_wait(write_to_list);

        frames.push_back(frame2);

        sem_post(write_to_list);
        sem_post(data_flow);
        sem_post(push_m_3);
        usleep((__useconds_t) delay);
    }
}
void BG_thred3(){

    while(end_while) {
        sem_wait(cap_m_3);
        if(!cap.read(frame3.frame)) {
            fprintf(stderr, "The camera has been disconnected!\n");
            dealock_void();
            exit(EXIT_FAILURE);
        }
        frame3.tick = (double) cv::getTickCount();
        sem_post(cap_m_1);

        BgSubtractor(frame3.frame , frame3.fgKNN);

        sem_wait(push_m_3);
        sem_wait(write_to_list);

        frames.push_back(frame3);

        sem_post(write_to_list);
        sem_post(data_flow);
        usleep((__useconds_t) delay);

    }
}
void socket(){


}




