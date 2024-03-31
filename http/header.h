#ifndef __HTTP__HEADER_H__
#define __HTTP__HEADER_H__

#include "message.h"

struct http_header_array {
    struct http_header* store;
    int cap;
    int n;
};

void initHeaderArray(struct http_header_array* array);
void freeHeaders(struct http_header_array* array);
int pushHeader(struct http_header_array* array, const char* key, const char* value);
const char* getHeaderValue(struct http_header_array* array, const char* findKey);
void printAllHeader(struct http_header_array* array);

#endif
