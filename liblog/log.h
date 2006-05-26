
#ifndef _LOG_H
#define _LOG_H

typedef enum {
  LOG_DEBUG = -20,
  LOG_VERBOSE = -10,
  LOG_NORMAL = 0,
  LOG_WARNING = 10,
  LOG_ERROR = 20
} LOG_LEVEL;

extern void (*log_sink)(int, const char*, void*);
extern LOG_LEVEL log_level;
extern void* log_data;

void lprintf(LOG_LEVEL level, const char* fmt, ...);
void log_sink_default(int level, const char* msg, void* data);

#endif
