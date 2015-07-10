//
// Created by andrej on 9.7.2015.
//

#include <string.h>
#include "communication.h"

int get_int_json( const char *text, const char *key){
    cJSON *json;
   // printf("%s",text);
    json=cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
    else{
        int format = cJSON_GetObjectItem(json,key)->valueint;
        cJSON_Delete(json);
        return format;
    }
}
double get_double_json( const char *text, const char *key){
    cJSON *json;
   // printf("%s",text);
    json=cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
    else{
        double format = cJSON_GetObjectItem(json,key)->valuedouble;
        cJSON_Delete(json);
        return format;
    }
}

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

const char *create_json(const char *direction, int tag, int raspiId){
    char *results;
    char *test;
    cJSON *root;

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "direction", cJSON_CreateString(direction));
    cJSON_AddNumberToObject(root, "tagId", tag);
    cJSON_AddNumberToObject(root, "raspiId", raspiId);
    test = cJSON_Print(root);
    results = (char*) malloc(sizeof(char) * strlen(test));
    strcpy (results,test);
    return results;
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
        request.perform();
    }
    catch ( curlpp::LogicError & e ) {

    }
    catch ( curlpp::RuntimeError & e ) {

    }
}