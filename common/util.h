#ifndef __COMMON__UTIL_H__
#define __COMMON__UTIL_H__

//    return ContentType by detecting file extensions
const char* get_contentType(char* fileName);
int load_file(const char * fileName, char** fileBuffer);

#endif
