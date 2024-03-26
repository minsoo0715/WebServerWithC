#include "http/message.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/constant.h"

int generate_response(char* buffer, const char *body, int bodySize, const char *content_type, const char *start_line) {
    char temp[BUF_SIZE];

    strcat(buffer, start_line);
    strcat(buffer, CRLF);

    // TODO: 헤더 별도 분리
    if(content_type != NULL) {
        strcat(buffer, "Content-Type: ");
        strcat(buffer, content_type);
        strcat(buffer, CRLF);

        strcat(buffer, "Content-Length: ");
        snprintf(temp, sizeof(temp), "%d", bodySize);
        strcat(buffer, temp);
        strcat(buffer, CRLF);
    }

    strcat(buffer, CRLF);

    int initSize = strlen(buffer);

    if(body == NULL) return initSize;

    memcpy(buffer + initSize, body, bodySize);
    return initSize + bodySize;
}

struct request_message* parse_request(char* requestBuffer) {
    struct request_message *message = malloc(sizeof(struct  request_message));
    char startLine[BUF_SIZE];
    char* ptr = strtok(requestBuffer, "\n");
    strncpy(startLine, ptr, strlen(ptr));

    message->startLine = parse_startLine(startLine);
    while(ptr != NULL) {
        ptr = strtok(NULL, "\n");
    }
    return message;
}

struct start_line* parse_startLine(char* buffer) {
    struct start_line *line = malloc(sizeof(struct start_line));
    char* ptr = strtok(buffer, " ");
    strncpy(line->method, ptr, strlen(ptr));
    ptr = strtok(NULL, " ");
    strncpy(line->uri, ptr, strlen(ptr));
    ptr = strtok(NULL, " ");
    strncpy(line->version, ptr, strlen(ptr));

    return line;
}


