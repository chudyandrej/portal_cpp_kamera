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

using namespace curlpp::options;

const char *get_HTTP_request(const char *url);

void post_HTTP_request(const char *url ,const char *json, int length );


#endif //PORTALS_COMMUNICATION_H
