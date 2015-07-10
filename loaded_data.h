//
// Created by andrej on 10.7.2015.
//

#ifndef PORTALS_LOADED_DATA_H
#define PORTALS_LOADED_DATA_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>

int save_settings_to_var();
char *load_settings();
char *load_settings_from_file(const char *url);
void write_settings_to_file(const char *url, const char *newConfiguration);

extern int learning_history;
extern int thresholding ;
extern int min_area;
extern int min_dist_to_create;
extern double max_dist_to_pars;
extern double shadow_thresh;
extern int frame_width;
extern int frame_height;

#endif //PORTALS_LOADED_DATA_H
