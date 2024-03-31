#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/constant.h"
#include "http/header.h"

void initHeaderArray(struct http_header_array* array) {
    array->cap = 10;
    array->n = 0;
    array->store = (struct http_header*)malloc(sizeof(struct http_header) * array->cap);
}

void freeHeaders(struct http_header_array* array) {
    free(array->store);
}

int pushHeader(struct http_header_array* array, const char* key, const char* value) {
    if(array->cap == array->n) {
        array->cap *= 2;
        array->store = (struct http_header*)realloc(array->store, sizeof(struct http_header) * array->cap);
    }

    struct http_header* cur = &array->store[array->n];
    strcpy(cur->key, key);
    strcpy(cur->value, value);

    ++(array->n);
}

const char* getHeaderValue(struct http_header_array* array, const char* findKey) {
    for(int i = 0; i<array->n; ++i) {
        if(!strcmp(array->store[i].key, findKey)) {
            return array->store[i].value;
        }
    }
    return NULL;
}

void printAllHeader(struct http_header_array* array) {
    for(int i = 0; i<array->n; ++i) {
        printf("%s: %s", array->store[i].key, array->store[i].value);
        printf(CRLF);
    }
}