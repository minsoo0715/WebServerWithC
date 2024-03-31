/*
 * Header defining structures, functions about http message
 * (especially parsing)
 */

#ifndef __HTTP__MESSAGE_H__
#define __HTTP__MESSAGE_H__

/* Structure for saving start_line attributes */
struct start_line {
    char method[16];
    char uri[128];
    char version[9];
};

struct http_header {
    char key[64];
    char value[512];
};

/* Define request message */
struct request_message {
    struct start_line* startLine;
};
void write_start_line(char* buffer, const char* start_line);
void write_header(char* buffer, const char* key, const char* value);
int write_body(char* buffer, const char* bodyBuffer, int bodySize);

int generate_response(char* buffer, const char *body, int bodySize, const char *content_type, const char *start_line);
struct request_message* parse_request(char* requestBuffer);
int get_newline_pos(char* buffer, int start, int len);
struct start_line* parse_start_line_of_request(char* buffer);

#endif