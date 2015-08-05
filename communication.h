//
// Created by andrej on 9.7.2015.
//

#ifndef PORTALS_COMMUNICATION_H
#define PORTALS_COMMUNICATION_H


#include <iosfwd>
#include <sstream>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include "cJSON.h"
#include <stdio.h>
#include <bits/stringfwd.h>
#include <curlpp/cURLpp.hpp>
#include <vector>


using namespace curlpp::options;

void send_transaction(const char *direction);

const char *get_HTTP_request(const char *url);

int post_HTTP_request(const char *url ,const char *json, int length );

char *create_json(const char *direction, int tag);

extern const char *serverURL;
extern int ID;


#endif //PORTALS_COMMUNICATION_H
