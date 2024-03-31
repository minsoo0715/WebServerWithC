#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/constant.h"
#include "http/common.h"
#include "http/header.h"
#include "http/message.h"

void get_start_line_by_status(struct response_start_line* start_line, unsigned short status_code) {
    const char* status_text;

    if(status_code == 200) {
        status_text = HTTP_STATUS_TEXT_200;
    } else if(status_code == 404) {
        status_text = HTTP_STATUS_TEXT_404;
    }

    strcpy(start_line->version, HTTP_VERSION);
    strcpy(start_line->status_text, status_text);
    start_line->statusCode = status_code;
}


/*
 * Write start_line (with CRLF)
 */
void write_start_line(char* buffer, const struct response_start_line* start_line) {

    strcat(buffer, start_line->version);
    strcat(buffer, " ");

    char temp[8];
    snprintf(temp, 6, "%u", start_line->statusCode);
    strcat(buffer, temp);
    strcat(buffer, " ");

    strcat(buffer, start_line->status_text);
    strcat(buffer, CRLF);
}

/*
 * Write Header with (key, value) set
 */
void write_header(char* buffer, const struct http_header* header) {
    strcat(buffer, header->key);
    strcat(buffer, ": ");
    strcat(buffer, header->value);
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
int generate_response(char* buffer, const char *body, int bodySize, const struct http_header_array* headerArray, const struct response_start_line* start_line) {

    write_start_line(buffer, start_line);

    for(int i = 0; i<headerArray->n; ++i) {
        struct http_header h = headerArray->store[i];
        write_header(buffer, &h);
    }

    strcat(buffer, CRLF);
    if(body == NULL) return strlen(buffer);

    return write_body(buffer, body, bodySize);
}

void parse_single_header(char* line, struct http_header* header) {

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

    message->headers = malloc(sizeof(struct http_header));
    initHeaderArray(message->headers);

    ptr = strtok(temp, CRLF);

    while(ptr != NULL) { // Continue parsing
        struct http_header header;
        parse_single_header(ptr, &header);
        pushHeader(message->headers, header.key, header.value);
        ptr = strtok(NULL, CRLF);
    }

    free(startLine);
    return message;
}

/* parse start line of request message, and return the start_line struct */
struct request_start_line* parse_start_line_of_request(char* buffer) {
    struct request_start_line *line = malloc(sizeof(struct request_start_line));
    char* ptr = strtok(buffer, " "); // Execute strtok() with ' '
    strncpy(line->method, ptr, strlen(ptr) + 1);

    ptr = strtok(NULL, " ");
    strncpy(line->uri, ptr, strlen(ptr) + 1);

    ptr = strtok(NULL, " ");
    strncpy(line->version, ptr, strlen(ptr) + 1);

    return line;
}


