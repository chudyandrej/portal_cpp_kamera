//
// Created by andrej on 10.7.2015.
//


#include "loaded_data.h"



vector<string> tag_list;

void save_settings_to_var() {       //hlavný void načíta hodnoty premenných zo súboru overí aktuálnosť zo serverom a zapíše
    std::hash<std::string> str_hash;
    const char *data_from_http;
    const char *data_from_file;
    char url[100];
    sprintf (url,"%s/api/portal_endpoint/settings/%d",serverURL,ID);

    data_from_file = load_settings();
    data_from_http = get_HTTP_request(url);
    if (data_from_file != NULL && data_from_http != NULL) {
        std::string str1(data_from_file);
        std::string str2(data_from_http);

        if (str_hash(str1) != str_hash(str2)) {
            printf("Inconsistent data, enrollment\n");
            write_settings_to_file("/home/pi/data.log", data_from_http);
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

void save_tags() {       //hlavný void načíta hodnoty premenných zo súboru overí aktuálnosť zo serverom a zapíše
    std::hash<std::string> str_hash;
    const char *data_from_http;
    const char *data_from_file;
    char url[100];
    sprintf (url,"%s/api/portal_endpoint/permissions/%d",serverURL,ID);

    data_from_file = load_tags();
    data_from_http = get_HTTP_request(url);

    if (data_from_file != NULL && data_from_http != NULL) {
        std::string str1(data_from_file);
        std::string str2(data_from_http);

        if (str_hash(str1) != str_hash(str2)) {
            printf("Inconsistent tag, enrollment\n");
            write_settings_to_file("/home/pi/tags.txt", data_from_http);
            free((char *) data_from_http);
            free((char *) data_from_file);
            tag_list.clear();
            data_from_file = load_tags();
            free((char *) data_from_file);
        }
        printf("Tag has been successfully loaded!\n");
    }
    else{
        free((char *) data_from_http);
        free((char *) data_from_file);
        if (data_from_file != NULL)
            printf("Server not a reachable. Program using tags from file.\n");
        else
            fprintf(stderr,"Loading tags was unsuccessful! Program dont have tagList.\n");
    }
}


char *load_tags(){
    char *tas_from_file = load_data_from_file("/home/pi/tags.txt");

    int max = get_int_json(tas_from_file, "numberOfTags");

    for (int i = 0; i < max ; i++){
        char tagName[10];
        sprintf (tagName,"tag%d",i);
        tag_list.push_back(get_string_json(tas_from_file,tagName));
    }
    return tas_from_file;
}


char *load_settings(){
    char *data_from_file = load_data_from_file("/home/pi/data.log");

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

char *load_data_from_file(const char *url){
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

string get_string_json( const char *text, const char *key){
    cJSON *json;
    json=cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return NULL;
    }
    else{
        string format = cJSON_GetObjectItem(json,key)->valuestring;
        cJSON_Delete(json);
        return format;
    }
}

