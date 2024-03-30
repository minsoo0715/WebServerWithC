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
void write_body(char* buffer, const char* bodyBuffer, int initSize, int bodySize) {
    strcat(buffer, CRLF);
    memcpy(buffer+initSize, body, bodySize); // file can have '\0', so you need to use memcpy
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


    int initSize = strlen(buffer); // Buffer size before appending fileBuffer
    if(body == NULL) return initSize;

    write_body(buffer, body, initSize, bodySize);
    return initSize + bodySize;
}

/*
 * parse request, and load result to buffer
 */
struct request_message* parse_request(char* requestBuffer) {
    struct request_message *message = malloc(sizeof(struct  request_message));

    char* ptr = strtok(requestBuffer, "\n");       // strtok : change delim to '\0', and return the pointer

    char* startLine = (char*)malloc(strlen(ptr)+1);
    strncpy(startLine, ptr, strlen(ptr));    // first line is start_line

    message->startLine = parse_start_line_of_request(startLine);
    while(ptr != NULL) { // Continue parsing
        ptr = strtok(NULL, "\n");
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


