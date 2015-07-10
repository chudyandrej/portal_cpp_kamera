#include <unistd.h>
#include "opencv.h"
#include "communication.h"
#include <thread>
#include <fstream>
#include "loaded_data.h"



void openCV() {
    int exitCODE;
    exitCODE = make_detection_transactions();
    if (exitCODE == -1)
      exit(2);
}

void communication(){
   // const char *c = get_HTTP_request("./data");
    int code =  save_settings_to_var();

    //printf("Cislo :%d\n", cislo);

  //  const char *json = create_json("in", 100001, 1);
  //  int length = (int) strlen(json);
    //post_HTTP_request("http://192.168.1.101:3000/api/portal_endpoint/transaction/1" ,json, length );
}


int main(){

    std::thread first (openCV);     // spawn new thread that calls foo()
    std::thread second (communication);  // spawn new thread that calls bar(0)

    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes
    return EXIT_SUCCESS;
}
