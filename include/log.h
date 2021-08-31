#ifndef log_h_
#define log_h_
#include <stdarg.h>

void Log(const char* file, const char* fun, int line, const char* msg, ...);

#define LOG(msg, ...) Log(__FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)
#endif