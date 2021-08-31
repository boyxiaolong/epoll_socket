#include "../include/log.h"

#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <string>

void snprintf_s(char*buf, int _max, int& len, const char* fmt, ...) {
    if (len >= 0 && len < _max) {
        va_list arg;
        va_start(arg, fmt);
        len += vsnprintf(buf + len, _max - len, fmt, arg);
        va_end(arg);
    }
    buf[_max - 1] = 0;
}

void log(const char* file, const char* fun, int line, const char* content) {
    char buf[2048];
    time_t t = time(NULL);
    struct tm* ptm = localtime(&t);
    int len = 0;
    snprintf_s(buf, sizeof(buf), len, 
        "%04d%02d%02d-%02d:%02d:%02d [%lu] %s:%s:%02d  %s\n", 
        ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, pthread_self(),
        file, fun, line, content);
    printf("%s", buf);
}

void Log(const char* file, const char* fun, int line, const char* msg, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    char buf[1024];
    int len = 0;
    memset(&buf[0], 0, sizeof(buf));
    vsnprintf(buf, sizeof(buf), msg, arg_ptr);
    log(file, fun, line, buf);
    va_end(arg_ptr);
}