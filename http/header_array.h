/* Header for defining http_header_array's functions and structure to save http_header */
#ifndef __HTTP__HEADER_H__
#define __HTTP__HEADER_H__

#include "message.h"

/* The structure for saving http headers */
struct http_header_array {
    struct http_header* store; /* array of http_headers */
    int cap; /* capacity of array */
    int n; /* number of elements */
};

void initHeaderArray(struct http_header_array* array);
void freeHeaders(struct http_header_array* array);
void pushHeader(struct http_header_array* array, const char* key, const char* value);
const char* getHeaderValue(struct http_header_array* array, const char* findKey);
void printAllHeader(struct http_header_array* array);

#endif
