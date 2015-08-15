/***************************************************************************************************************************/
/*                                                 Configuration settings                                                  */
/**/    int ID = 1;
/**/    const char *serverURL = "//192.168.1.14:3000";
/****************************************************************************************************************************/


#include "opencv.h"
#include "communication.h"
#include <signal.h>
#include "loaded_data.h"
#include "declarations.h"
#include "easywsclient.h"

#include <thread>
#include <unistd.h>

using easywsclient::WebSocket;
static WebSocket::pointer ws1 = NULL;

typedef struct {
    cv::Mat frame;
    cv::Mat fgKNN;
    double tick;
} frame_wrap_t;


int delay = 1;
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
void web_socket();
void handle_message(const std::string & message);



int main(int argc, char *argv[]){

    setbuf(stdout, NULL);

    arguments_read(argc, &argv[0]);

    dealock_void();
    signal(SIGTERM, contro_c);
    signal(SIGINT, contro_c);
    init ();
    cap = init_cap_bg("/home/andrej/Music/colisions/video.mkv");

    std::thread cv (openCV);
    std::thread thred1 (BG_thred1);
    std::thread thred2 (BG_thred2);
    std::thread thred3 (BG_thred3);
   // std::thread socket (web_socket);

    cv.join();
    thred1.join();
    thred2.join();
    thred3.join();
    // socket.join();

    return EXIT_SUCCESS;
}

void openCV() {
    int counter =0;
    cv::Mat original_frame, subtract_frame;
    double frame_tick;
    if(with_gui) {
        namedWindow("Trashold", 0);
        namedWindow("Tracking", 0);
    }
    while (end_while){
        sem_wait(data_flow);

        sem_wait(write_to_list);

        original_frame =frames[0].frame;
        subtract_frame =frames[0].fgKNN;
        frame_tick = frames[0].tick;
        frames.erase (frames.begin());
        sem_post(write_to_list);

        counter++;
       // printf("%d\n",(int)frames.size());
        if(counter == 3 && frames.size() == 0 ){
            sem_post(push_m_1);
            counter = 0;
        }


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
        sem_post(cap_m_2);
        frame1.tick = (double) cv::getTickCount();

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
        sem_post(cap_m_3);
        frame2.tick = (double) cv::getTickCount();

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
        sem_post(cap_m_1);
        frame3.tick = (double) cv::getTickCount();
        BgSubtractor(frame3.frame , frame3.fgKNN);

        sem_wait(push_m_3);
        sem_wait(write_to_list);

        frames.push_back(frame3);

        sem_post(write_to_list);
        sem_post(data_flow);
        usleep((__useconds_t) delay);

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

