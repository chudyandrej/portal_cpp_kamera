//
// Created by andrej on 9.7.2015.
//

#include <string.h>
#include "communication.h"

using namespace std;

const char *get_HTTP_request(const char *url){
    const char *point_to_content;
    char *malloc_space;
    std::string get_content;
    try {
        std::ostringstream os;
        os << curlpp::options::Url(std::string(url));
        get_content = os.str();
    }
    catch(curlpp::RuntimeError & e){
        return NULL;
    }
    catch(curlpp::LogicError & e){
        return NULL;
    }

    point_to_content = get_content.c_str();
    malloc_space = (char*) malloc(sizeof(char) * strlen(point_to_content));
    strcpy (malloc_space,point_to_content);
    return  malloc_space;
}



void post_HTTP_request(const char *url ,const char *json, int length ){
    try {
        curlpp::Easy request;
        request.setOpt(new curlpp::options::Url(url));
        request.setOpt(new curlpp::options::Verbose(true));
        std::list<std::string> header;
        header.push_back("Content-Type: application/json");
        request.setOpt(new curlpp::options::HttpHeader(header));
        request.setOpt(new curlpp::options::PostFields(json));
        request.setOpt(new curlpp::options::PostFieldSize(length));

        std::cout.setstate(std::ios::failbit) ;
        request.perform();                     // <-- call
        std::cout.clear() ;

    }
    catch ( curlpp::LogicError & e ) {
            printf("\nerror1\n");
    }
    catch ( curlpp::RuntimeError & e ) {
        printf("\nerror2\n");
    }

}