/**************************************************************************************************/
/*                                                 Configuration settings                         */
/**/    int ID = 1;
/**/    const char *serverURL = "//192.168.1.14:3000";
/*************************************************************************************************/

#include <signal.h>
#include <queue>
#include <thread>
#include <unistd.h>
#include <new>

#include "opencv.h"
#include "communication.h"
#include "loaded_data.h"
#include "declarations.h"
#include "easywsclient.h"

using easywsclient::WebSocket;
static WebSocket::pointer ws1 = NULL;

typedef struct {
    cv::Mat frame;
    cv::Mat fg_mask;
    double tick;
} frame_wrap_t;

int delay = 0;
bool with_gui = false;
bool with_fps = false;
bool end_while = true;
bool person_flow = true;

queue<frame_wrap_t*> frames;
cv::VideoCapture cap;
sem_t *cap_m_1,*cap_m_2,*cap_m_3,*push_m_1,*push_m_2, *push_m_3,*write_to_list,*data_flow;

void openCV();
void BG_thread1();
void BG_thread2();
void BG_thread3();
void web_socket();
void handle_message(const std::string & message);

int main(int argc, char *argv[]) {

    setbuf(stdout, NULL);
    arguments_read(argc, &argv[0]);
    dealock_void();
    signal(SIGTERM, contro_c);
    signal(SIGINT, contro_c);
    init();
    cap = init_cap_bg("/home/andrej/Music/video3/pi_video3.mkv");

    std::thread cv(openCV);
    std::thread thread1(BG_thread1);
    std::thread thread2(BG_thread2);
    std::thread thread3(BG_thread3);
   // std::thread socket (web_socket);

    cv.join();
    thread1.join();
    thread2.join();
    thread3.join();
    // socket.join();

    return EXIT_SUCCESS;
}

void openCV() {
    cv::Mat frame, fg_mask;
    int counter = 0;
    double frame_tick;
    if (with_gui) {
        namedWindow("Threshold", 0);
        namedWindow("Tracking", 0);
    }
    while (end_while) {
        sem_wait(data_flow);
        sem_wait(write_to_list);
        frame = frames.front()->frame;
        fg_mask = frames.front()->fg_mask;
        frame_tick = frames.front()->tick;
        delete frames.front();
        frames.pop();
        sem_post(write_to_list);
        counter++;
        if (counter == 3){
            counter = 0;
            sem_post(push_m_1);
        }
		int exit_code = ProcessFrame(&frame, &fg_mask, frame_tick);
        if (exit_code != 0){
            printf("WTF ???");
        }
        if(with_gui) {
            waitKey(1);
        }
    }
}

void BG_thread1(){

    while(end_while) {
        frame_wrap_t *frame1 = new frame_wrap_t;
        if (frame1 == NULL) {
            printf("malloc failed\n");
        }
        sem_wait(cap_m_1);

        if(!cap.read(frame1->frame)) {
            fprintf(stderr, "The camera has been disconnected!\n");
            dealock_void();
            exit(EXIT_FAILURE);
        }
        usleep((__useconds_t) delay);
        sem_post(cap_m_2);
        frame1->tick = (double) cv::getTickCount();
     
        BgSubtractor(&(frame1->frame), &(frame1->fg_mask));

        sem_wait(push_m_1);
        sem_wait(write_to_list);
        frames.push(frame1);
        sem_post(write_to_list);
        sem_post(data_flow);
        sem_post(push_m_2);
    }
}

void BG_thread2() {

    while(end_while) {
        frame_wrap_t *frame2 = new frame_wrap_t;
        if (frame2 == NULL) {
            printf("malloc failed\n");
        }        
        sem_wait(cap_m_2);
        if(!cap.read(frame2->frame)) {
            fprintf(stderr, "The camera has been disconnected!\n");
            dealock_void();
            exit(EXIT_FAILURE);
        }
        usleep((__useconds_t) delay);
        sem_post(cap_m_3);
        frame2->tick = (double) cv::getTickCount();

        BgSubtractor(&(frame2->frame), &(frame2->fg_mask));
        sem_wait(push_m_2);
        sem_wait(write_to_list);

        frames.push(frame2);
        sem_post(write_to_list);
        sem_post(data_flow);
        sem_post(push_m_3);
    }
}

void BG_thread3() {

    while(end_while) {

        frame_wrap_t *frame3 = new frame_wrap_t;
        if (frame3 == NULL) {
            printf("malloc failed\n");
        }        
        sem_wait(cap_m_3);
        if(!cap.read(frame3->frame)) {
            fprintf(stderr, "The camera has been disconnected!\n");
            dealock_void();
            exit(EXIT_FAILURE);
        }
        usleep((__useconds_t) delay);
        sem_post(cap_m_1);
        frame3->tick = (double) cv::getTickCount();
        BgSubtractor(&(frame3->frame), &(frame3->fg_mask));
       
        sem_wait(push_m_3);
        sem_wait(write_to_list);

        frames.push(frame3);
        sem_post(write_to_list);
        sem_post(data_flow);
    }
}

void web_socket(){
    char json_message[40] ;
    char url[50];
    sprintf (url,"ws:%s",serverURL);
    printf("%s\n",url);
    save_settings_to_var();
    save_tags();

    ws1 = WebSocket::from_url(url);
    sprintf (json_message,"{\"topic\":\"authPortal\",\"body\":\"%d\"}",ID);
    ws1->send(json_message);
    while (ws1->getReadyState() != WebSocket::CLOSED) {
        ws1->poll();
        ws1->dispatch(handle_message);
        usleep (1 * pow(10,6));                     //sleep 1s
    }
    delete ws1;
}

void handle_message(const std::string & message){
    printf(">>> %s\n", message.c_str());
    if (message == "ping"){
        ws1->send("pong");
    }
}

