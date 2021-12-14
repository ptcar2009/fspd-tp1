#include "logs.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 3

char *time_stamp() {
  char *timestamp = (char *)malloc(sizeof(char) * 100);
  time_t ltime;
  ltime = time(NULL);
  struct tm *tm;
  tm = localtime(&ltime);
  sprintf(timestamp, "%04d/%02d/%02d %02d:%02d:%02d", tm->tm_year + 1900,
          tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  return timestamp;
}

#if (LOG_LEVEL >= LOG_LEVEL_INFO)
void INFO(char *format, ...) {
  va_list args;
  va_start(args, format);
  char *ts = time_stamp();
  printf("%s [INFO] ", ts);
  free(ts);
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
#else
void INFO(char *format, ...){};
#endif

#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
void DEBUG(char *format, ...) {
  va_list args;
  va_start(args, format);
  char *ts = time_stamp();
  printf("%s [DEBUG] ", ts);
  free(ts);
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
#else
void DEBUG(char *format, ...){};
#endif

#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
void ERROR(char *format, ...) {
  va_list args;
  va_start(args, format);
  char *ts = time_stamp();
  printf("%s [ERROR] ", ts);
  free(ts);
  vprintf(format, args);
  printf(" ERROR: %s\n", strerror(errno));
  printf("\n");
  va_end(args);
}
#else
void ERROR(char *format, ...){};
#endif

#if (LOG_LEVEL >= LOG_LEVEL_FATAL)
void FATAL(char *format, ...) {
  va_list args;
  va_start(args, format);
  char *ts = time_stamp();
  printf("%s [FATAL] ", ts);
  free(ts);
  vprintf(format, args);
  printf(" FATAL: %s\n", strerror(errno));
  printf("\n");
  va_end(args);
  exit(1);
}
#else
void FATAL(char *format, ...){};
#endif
