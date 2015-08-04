//
// Created by andrej on 9.7.2015.
//

#include <string.h>



#include "communication.h"

using namespace std;

vector<string> buffer_transactions;
bool empty_bufer = true;

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


void send_transaction(const char *direction) {

    const char *json = create_json(direction, 100001);
    int length = (int) strlen(json);
    int exit_code = post_HTTP_request("http://apis-portals.herokuapp.com/api/portal_endpoint/transaction/1" ,json, length );
    if(exit_code == -1){
        std::string str(json);
        buffer_transactions.push_back(str);
        empty_bufer = false;
        for(int i = 0; i < buffer_transactions.size(); i++ ) {
            printf("%s\n", buffer_transactions[i].c_str());
        }
    }
    else {
        if (!empty_bufer) {
            empty_bufer = true;
            while(buffer_transactions.size() != 0){
                post_HTTP_request("http://apis-portals.herokuapp.com/api/portal_endpoint/transaction/1",buffer_transactions[0].c_str(), (int) buffer_transactions[0].length());
                buffer_transactions.erase(buffer_transactions.begin());
            }
        }
    }
    free((char*)json);
}


int post_HTTP_request(const char *url ,const char *json, int length ){
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
        return 0;
    }

    catch (const curlpp::LibcurlRuntimeError &e) {
        return -1;
    }

}

char *create_json(const char *direction, int tag){

    char *results;
    char message[50];

    sprintf (message,"{\"tagId\":%d,\"direction\":\"%s\"}",tag,direction);

    results = (char*) malloc(sizeof(char) * strlen(message));

    strcpy (results,message);

    return results;

}
