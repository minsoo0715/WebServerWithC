#ifndef __COMMON__UTIL_H__
#define __COMMON__UTIL_H__

const char* get_contentType(char* fileName);
int load_file(const char * fileName, char** fileBuffer);
int load_str(const char * contents, char** fileBuffer);


#endif
