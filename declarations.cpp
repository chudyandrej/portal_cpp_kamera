//
// Created by andrej on 18.7.2015.
//
#include "declarations.h"

void arguments_read(int argc, char **argv){
    for (int i =0; i < argc ; i++) {
        if (strcmp(argv[i], "-gui") == 0) {
            with_gui = true;
        }
        else if (strcmp(argv[i], "-fps") == 0) {
            with_fps = true;
        }
        else if (strcmp(argv[i], "-leftRight") == 0){

            person_flow = false;
        }

    }

}

void contro_c(int param){
    dealock_void();
    fprintf(stderr,"Error: Bpl priaty signal SIGINT.\nProgram sa ukončuje.\n");
    exit(2);
}

void init (){
    cap_m_1 = sem_open("/cap_mutex_1", O_CREAT | O_EXCL, 0666, 1);
    cap_m_2 = sem_open("/cap_mutex_2", O_CREAT | O_EXCL, 0666, 0);
    cap_m_3 = sem_open("/cap_mutex_3", O_CREAT | O_EXCL, 0666, 0);
    push_m_1 = sem_open("/push_mutex_1", O_CREAT | O_EXCL, 0666, 1);
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
