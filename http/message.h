/*
 * Header defining structures, functions about http message
 */

#ifndef __HTTP__MESSAGE_H__
#define __HTTP__MESSAGE_H__

#include "header_array.h"

/* Structure for saving request start_line attributes */
struct request_start_line {
    char method[16];
    char uri[128];
    char version[9];
};

/* Structure for saving response start_line attributes */
struct response_start_line {
    char version[9];
    unsigned short statusCode;
    char status_text[16];
};

/* Define http header */
struct http_header {
    char key[64];
    char value[512];
};

/* Define request message */
struct request_message {
    struct request_start_line* startLine;
    struct http_header_array* headers;
};

void get_start_line_by_status(struct response_start_line* start_line, unsigned short status_code);

void write_start_line(char* buffer, const struct response_start_line* start_line);
void write_header(char* buffer, const struct http_header* header);
int write_body(char* buffer, const char* bodyBuffer, int bodySize);

int generate_response(char* buffer, const char *body, int bodySize, const struct http_header_array* headerArray, const struct response_start_line* start_line);
struct request_message* parse_request(char* requestBuffer);
void parse_single_header(char* line, struct http_header* header);
struct request_start_line* parse_start_line_of_request(char* buffer);

#endif