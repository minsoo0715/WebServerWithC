#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http/common.h"

#include "common/constant.h"
#include "common/util.h"

const char* get_contentType(char* fileName) {
    const char *dot = strrchr(fileName, '.');
    if(!dot || dot == fileName) return NULL;
    if(strcmp(dot, ".html") == 0) {
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

int load_file(const char * fileName, char** fileBuffer)
{
    int fileSize;
    char* path = (char*)malloc(200);

    strcat(path, RESOURCE_ROOT);
    strcat(path, fileName);

    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        return -1;
    }

    fseek(fp, 0L, SEEK_END); // move cursor to end of file
    fileSize = ftell(fp);
    *fileBuffer = (char*)malloc(fileSize);
    fseek(fp, 0L, SEEK_SET);
    fread(*fileBuffer, fileSize, 1, fp);

    fclose(fp);
    free(path);
    return fileSize;
}
