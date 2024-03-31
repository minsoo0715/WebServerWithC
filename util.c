#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/constant.h"
#include "common/util.h"
#include "http/common.h"

/*
 * Parse extension from fileName, and return appropriate mimetype
*/
const char* get_contentType(char* fileName) {
    const char *dot = strrchr(fileName, '.'); // find pointer that points the location of '.'
    if(!dot || dot == fileName) return NULL;    // if there isn't '.', return NULL
    if(strcmp(dot, ".html") == 0) {             // match file extension with mimetype
        return TEXT_HTML;
    } else if(strcmp(dot, ".jpg") == 0) {
        return IMAGE_JPEG;
    } else if(strcmp(dot, ".ico") == 0) {
        return FAVICON;
    } else if(strcmp(dot, ".pdf") == 0) {
        return PDF;
    } else if(strcmp(dot, ".mp3") == 0) {
        return MP3;
    } else if(strcmp(dot, ".gif") == 0) {
        return GIF;
    } else {
        return NULL;
    }
}

/*
 * Load file by fileName
 * first, allocate fileBuffer, and load file to it
 * finally, return fileSize
*/
int load_file(const char * fileName, char** fileBuffer)
{
    int fileSize;
    char* path = (char*)malloc(200); // Temporal variable for file's path
    bzero(path, 200);

    strcat(path, RESOURCE_ROOT);
    strcat(path, fileName);

    FILE *fp = fopen(path, "rb"); // Read file with readonly, binary mode
    if(fp == NULL) {
        return -1;
    }

    fseek(fp, 0L, SEEK_END);   // Move cursor to end of file
    fileSize = ftell(fp);                 // Get fileSize
    *fileBuffer = (char*)malloc(fileSize);  // Allocate fileBuffer with fileSize
    fseek(fp, 0L, SEEK_SET);   // Move cursor to start of file
    fread(*fileBuffer, fileSize, 1, fp); // read file

    fclose(fp); // Close fp
    free(path);    // Deallocate path
    return fileSize;  // Return fileSize
}

int load_str(const char * contents, char** fileBuffer) {
    int len = strlen(contents);
    *fileBuffer = (char*) malloc(len);
    strcpy(*fileBuffer, contents);
    return len;
}

