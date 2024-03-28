#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/constant.h"
#include "http/message.h"

/*
 * Generate response with body(with size), contentType, startLine and copy to buffer
 */
int generate_response(char* buffer, const char *body, int bodySize, const char *content_type, const char *start_line) {
    char temp[10];

    // Write start_line (with CRLF)
    strcat(buffer, start_line);
    strcat(buffer, CRLF);

    // TODO: 헤더 별도 분리
    if(content_type != NULL) {
        strcat(buffer, "Content-Type: ");
        strcat(buffer, content_type);
        strcat(buffer, CRLF);

        strcat(buffer, "Content-Length: ");
        snprintf(temp, 9, "%d", bodySize);
        strcat(buffer, temp);
        strcat(buffer, CRLF);
    }

    strcat(buffer, CRLF);

    int initSize = strlen(buffer); // Buffer size before appending fileBuffer
    if(body == NULL) return strlen(buffer);

    memcpy(buffer + initSize, body, bodySize); // file can have '\0', so you need to use memcpy
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


