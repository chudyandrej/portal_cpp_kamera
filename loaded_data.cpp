//
// Created by andrej on 10.7.2015.
//

#include <fstream>
#include "loaded_data.h"
#include "communication.h"
#include "opencv.h"



int save_settings_to_var() {
    std::hash<std::string> str_hash;
     const char *data_from_http;
     const char *data_from_file;
    data_from_file = load_settings();

    data_from_http = get_HTTP_request("http://apis-portals.herokuapp.com/api/portal_endpoint/settings/1");
    if (data_from_file != NULL && data_from_http != NULL) {
        std::string str1(data_from_file);
        std::string str2(data_from_http);
        std::cout << str1 << std::endl;
        std::cout << str2 << std::endl;


         if (str_hash(str1) != str_hash(str2)) {
            write_settings_to_file("data", data_from_http);
            free((char *) data_from_http);
            free((char *) data_from_file);
            data_from_file = load_settings();
            free((char *) data_from_file);
        }
        return 0;
    }
    else{
        free((char *) data_from_http);
        free((char *) data_from_file);
        return -1;
    }



}

char *load_settings(){
    char *data_from_file =load_settings_from_file("data");
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
    return NULL;
}


char *load_settings_from_file(const char *url){
    FILE * pFile;
    char file_content [201];
    char *pointer_to_space;
    pFile = fopen (url , "r");
    if (pFile == NULL) {
        fprintf( stderr, "Error opening file!\n" );
        return NULL;
    }
    else {
        if ( fgets (file_content , 200 , pFile) == NULL ){
            fprintf( stderr, "Error reading file!\n" );
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



