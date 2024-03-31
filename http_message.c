#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/constant.h"
#include "http/message.h"

/*
 * Write start_line (with CRLF)
 */
void write_start_line(char* buffer, const char* start_line) {

    strcat(buffer, start_line);
    strcat(buffer, CRLF);
}

/*
 * Write Header with (key, value) set
 */
void write_header(char* buffer, const char* key, const char* value) {
    strcat(buffer, key);
    strcat(buffer, ": ");
    strcat(buffer, value);
    strcat(buffer, CRLF);
}

/*
 * Write Body
 */
int write_body(char* buffer, const char* bodyBuffer, int bodySize) {
    int initSize = strlen(buffer);
    memcpy(buffer + initSize, bodyBuffer, bodySize); // file can have '\0', so you need to use memcpy
    return initSize + bodySize;
}

/*
 * Generate response with body(with size), contentType, startLine and copy to buffer
 */
int generate_response(char* buffer, const char *body, int bodySize, const char *content_type, const char *start_line) {

    write_start_line(buffer, start_line);

    // TODO: 헤더 별도 분리
    if(content_type != NULL) {
        write_header(buffer, "Content-Type", content_type);

        char temp[10];
        snprintf(temp, 9, "%d", bodySize);
        write_header(buffer, "Content-Length", temp);
    }

    strcat(buffer, CRLF);

    if(body == NULL) return strlen(buffer);

    return write_body(buffer, body, bodySize);
}

struct http_header* parse_single_header(char* line) {
    struct http_header* header = (struct http_header*)malloc(sizeof(struct http_header));

    int len = strlen(line);
    bzero(header->key, 64);
    bzero(header->value, 512);
    int d = -1;
    for(int i = 0; i<len; ++i) {
        if(line[i] == ':') {
            d = i;
            break;
        }
    }

    if(d == -1 || d == 0 || d == len-1) return;

    strncpy(header->key, line, d);
    if(line[d+1] == ' ') {
        strncpy(header->value, line+d+2, strlen(line+d+1));
    } else {
        strncpy(header->value, line+d+1, strlen(line+d+1));
    }

    return header;
}

/*
 * parse request, and load result to buffer
 */
struct request_message* parse_request(char* requestBuffer) {
    struct request_message *message = malloc(sizeof(struct  request_message));
    char* temp = malloc(strlen(requestBuffer));
    char* ptr = strtok(requestBuffer, "\n");       // strtok : change delim to '\0', and return the pointer

    char* startLine = (char*)malloc(strlen(ptr)+1);
    strncpy(startLine, ptr, strlen(ptr));    // first line is start_line

    strcpy(temp, requestBuffer+strlen(startLine)+1);
    message->startLine = parse_start_line_of_request(startLine);

    ptr = strtok(temp, CRLF);

    // TODO: 파싱한 헤더를 저장할 자료구조 고안
    while(ptr != NULL) { // Continue parsing
        parse_single_header(ptr);
        ptr = strtok(NULL, CRLF);
    }

    free(startLine);
    return message;
}

/* parse start line of request message, and return the start_line struct */
struct start_line* parse_start_line_of_request(char* buffer) {
    struct start_line *line = malloc(sizeof(struct start_line));
    char* ptr = strtok(buffer, " "); // Execute strtok() with ' '
    strncpy(line->method, ptr, strlen(ptr) + 1);

    ptr = strtok(NULL, " ");
    strncpy(line->uri, ptr, strlen(ptr) + 1);

    ptr = strtok(NULL, " ");
    strncpy(line->version, ptr, strlen(ptr) + 1);

    return line;
}


