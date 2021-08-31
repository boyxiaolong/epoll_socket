#ifndef log_h_
#define log_h_

void log(const char* file, const char* fun, int line, const char* content);
void Log(const char* file, const char* fun, int line, const char* msg, ...);

void snprintf_s(char*buf, int _max, int& len, const char* fmt, ...);

#define LOG(msg, ...) Log(__FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)
#endif