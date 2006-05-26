#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include "log.h"

void (*log_sink)(int, const char*, void*) = &log_sink_default;
LOG_LEVEL log_level = LOG_NORMAL;
void* log_data = NULL;


void lprintf(LOG_LEVEL level, const char* fmt, ...) {
  if(level < log_level)
    return;
  
  va_list ap;
  char* msg;
  
  va_start(ap, fmt);
  
  if(vasprintf(&msg, fmt, ap) >= 0) {
    log_sink(level, msg, log_data);
    free(msg);
  }
  
  va_end(ap);
}

void log_sink_default(int level, const char* msg, void* data) {
  if(level >= LOG_WARNING)
    fprintf(stderr, msg);
  else
    fprintf(stdout, msg);
}
