#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/constant.h"
#include "http/header_array.h"

/* Initialize http_header_array */
void initHeaderArray(struct http_header_array* array) {
    array->cap = 10; // set initial capacity as 10
    array->n = 0; // empty
    array->store = (struct http_header*)malloc(sizeof(struct http_header) * array->cap); // allocate store with capacity of 10
}

/* For deallocating */
void freeHeaders(struct http_header_array* array) {
    free(array->store);
}

/* add new header into http_header_array */
void pushHeader(struct http_header_array* array, const char* key, const char* value) {
    if(array->cap == array->n) { // if headers is full
        array->cap *= 2;         // increase capacity by twice
        // reallocate array
        array->store = (struct http_header*)realloc(array->store, sizeof(struct http_header) * array->cap);
    }

    struct http_header* cur = &array->store[array->n]; // number of elements is top index
    strcpy(cur->key, key);
    strcpy(cur->value, value);

    ++(array->n); // move to next
}

/* get Header value from http_header_array by receiving key string */
const char* getHeaderValue(struct http_header_array* array, const char* findKey) {
    for(int i = 0; i<array->n; ++i) {
        if(!strcmp(array->store[i].key, findKey)) {
            return array->store[i].value;
        }
    }
    return NULL;
}

/* print all header from http_header_array in http message format */
void printAllHeader(struct http_header_array* array) {
    for(int i = 0; i<array->n; ++i) {
        printf("%s: %s", array->store[i].key, array->store[i].value);
        printf(CRLF);
    }
}