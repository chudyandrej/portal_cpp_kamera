//
// Created by andrej on 10.7.2015.
//

#include <fstream>
#include "loaded_data.h"
#include "communication.h"
#include "opencv.h"


void save_settings_to_var() {
    std::hash<std::string> str_hash;
     const char *data_from_http;
     const char *data_from_file;

    data_from_file = load_settings();

    data_from_http = get_HTTP_request("http://apis-portals.herokuapp.com/api/portal_endpoint/settings/1");
    if (data_from_file != NULL && data_from_http != NULL) {
        std::string str1(data_from_file);
        std::string str2(data_from_http);

        if (str_hash(str1) != str_hash(str2)) {
            printf("Inconsistent data, enrollment\n");
            write_settings_to_file("/home/andrej/ClionProjects/portals/data.log", data_from_http);
            free((char *) data_from_http);
            free((char *) data_from_file);
            data_from_file = load_settings();
            free((char *) data_from_file);
        }
        printf("Data has been successfully loaded!\n");
    }
    else{
        free((char *) data_from_http);
        free((char *) data_from_file);
        if (data_from_file != NULL)
            printf("Server not a reachable. Program using variable from file.\n");
        else
        fprintf(stderr,"Loading data was unsuccessful! Program using default values.\n");
    }
}

char *load_settings(){
    char *data_from_file =load_settings_from_file("/home/andrej/ClionProjects/portals/data.log");

    if(data_from_file != NULL) {
        learning_history = get_int_json(data_from_file, "learning_history");
        thresholding = get_int_json(data_from_file, "thresholding");
        min_area = get_int_json(data_from_file, "min_area");
        min_dist_to_create = get_int_json(data_from_file, "min_dist_to_create");
        frame_width = get_int_json(data_from_file, "frame_width");
        frame_height = get_int_json(data_from_file, "frame_height");
        max_dist_to_pars = get_double_json(data_from_file,"max_dist_to_parse");
        shadow_thresh = get_double_json(data_from_file,"shadow_thresh");
        return data_from_file;
    }
    else
        return NULL;
}


char *load_settings_from_file(const char *url){
    FILE * pFile;
    char file_content [200];
    char *pointer_to_space;

    if ((pFile = fopen (url , "r")) == NULL) {
        fprintf( stderr, "Error opening file whit URL: %s!\n",url);
        return NULL;
    }
    else {
        if (fgets (file_content , 200 , pFile) == NULL ){
            fprintf( stderr, "Error reading file! whit UTL: %s\n",url);
            return NULL;
        }
       // puts (file_content);
        fclose (pFile);
    }
    pointer_to_space = (char*) malloc(sizeof(char) * (strlen(file_content) + 1));
    strcpy (pointer_to_space,file_content);
    return pointer_to_space;
}

void write_settings_to_file(const char *url, const char *newConfiguration){
    ofstream data;
    data.open( url, std::ofstream::out );
    data.write( (char*)newConfiguration, (int) strlen(newConfiguration) );
    data.close();
}

int get_int_json( const char *text, const char *key){
    cJSON *json;
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

char *create_json(const char *direction, int tag){
    char *results,*test;
    sprintf(test,"{\"tagId\":%d,\"direction\":\"%s\"}",tag,direction) ;

    results = (char*) malloc(sizeof(char) * strlen(test));
    strcpy (results,test);
    printf("%s\n",results);
    return results;

}