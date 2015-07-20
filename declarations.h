//
// Created by andrej on 18.7.2015.
//

#ifndef PORTALS_DECLARATIONS_H
#define PORTALS_DECLARATIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

#include <string.h>


extern bool with_gui,with_fps;

extern sem_t *cap_m_1,*cap_m_2,*cap_m_3,*push_m_1,*push_m_2,*push_m_3,*write_to_list,*data_flow;

void arguments_read(int argc, char **argv);
void contro_c(int param);
void init ();
void dealock_void();

#endif //PORTALS_DECLARATIONS_H
