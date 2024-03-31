#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/constant.h"
#include "http/common.h"
#include "http/header_array.h"
#include "http/message.h"

/*
 * Map statusCode with response_start_line fields
 */
void get_start_line_by_status(struct response_start_line* start_line, unsigned short status_code) {
    const char* status_text;

    if(status_code == 200) {
        status_text = HTTP_STATUS_TEXT_200;
    } else if(status_code == 404) {
        status_text = HTTP_STATUS_TEXT_404;
    } else return;

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
    snprintf(temp, 6, "%u", start_line->statusCode);  // convert start_line->statusCode to string
    strcat(buffer, temp);
    strcat(buffer, " ");

    strcat(buffer, start_line->status_text);
    strcat(buffer, CRLF);
}

/*
 * Write Header with (key, value) set
 * format= field-name ":" [ field-value ] CRLF
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
 * format=  Status-Line
 *          *( General-Header
 *          | Response-Header
 *          | Entity-Header )
 *          CRLF
 *          [ Entity-Body ]
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

/*
 * Parse Single Header, and save results to header
 * format= field-name ":" [ field-value ] CRLF
 */
void parse_single_header(char* line, struct http_header* header) {

    int len = strlen(line);
    bzero(header->key, 64);
    bzero(header->value, 512);

    int d = -1;
    for(int i = 0; i<len; ++i) { // find ':' token in line
        if(line[i] == ':') {
            d = i;
            break;
        }
    }

    // if d == -1, line don't contain ':'
    // if d is initial and last character of line, it is invalid format
    if(d == -1 || d == 0 || d == len-1) return;

    strncpy(header->key, line, d); // key range is 0~(d-1) (length: d)
    if(line[d+1] == ' ') { // if it has ' '
        strncpy(header->value, line+d+2, strlen(line+d+1)); // then, copy rest of the string, except ': '
    } else {
        strncpy(header->value, line+d+1, strlen(line+d+1)); // copy rest of the string except ':'
    }
}

/*
 * Parse request, and load result to buffer
 */
struct request_message* parse_request(char* requestBuffer) {
    struct request_message *message = malloc(sizeof(struct request_message));
    char* temp = malloc(strlen(requestBuffer));     // temp buffer to parse start_line
    char* ptr = strtok(requestBuffer, "\n");       // strtok : change delim to '\0', and return the pointer

    char* startLine = (char*)malloc(strlen(ptr)+1);
    strncpy(startLine, ptr, strlen(ptr));    // first line is start_line

    strcpy(temp, requestBuffer+strlen(startLine)+1); // copy to temp buffer except start line
    message->startLine = parse_start_line_of_request(startLine); // parse start line, and save it to message->startLine

    message->headers = malloc(sizeof(struct http_header_array)); // allocation message->headers
    initHeaderArray(message->headers);                          // init

    ptr = strtok(temp, CRLF); // line identification is CRLF(\r\n)

    while(ptr != NULL) { // Continue parsing
        struct http_header header;                                    // to save header info temporally
        parse_single_header(ptr, &header);                       // pass line to parse_single_header()
        pushHeader(message->headers, header.key, header.value); // add header into headers
        ptr = strtok(NULL, CRLF);
    }

    free(startLine);
    return message;
}

/* Parse start line of request message, and return the start_line struct (pointer)
 * format= Method SP Request-URI SP HTTP-Version CRLF
 */
struct request_start_line* parse_start_line_of_request(char* buffer) {
    struct request_start_line *line = malloc(sizeof(struct request_start_line)); // allocate request_start_line
    char* ptr = strtok(buffer, " "); // Execute strtok() with ' '
    strncpy(line->method, ptr, strlen(ptr) + 1); // Method

    ptr = strtok(NULL, " ");
    strncpy(line->uri, ptr, strlen(ptr) + 1);    // Request-URI

    ptr = strtok(NULL, " ");
    strncpy(line->version, ptr, strlen(ptr) + 1); // HTTP-Version

    return line;
}


