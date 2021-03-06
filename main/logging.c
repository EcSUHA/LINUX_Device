


#include "logging.h"


#ifndef ESP_PLATFORM
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdio.h>
#include "c_timeutils.h"

static struct timeval startTime;

// Have the initi function called at startup.
static void init() __attribute__ ((constructor));
static void
init() {

  gettimeofday(&startTime, NULL);

}

//FNs from:_
//https://github.com/nkolban/esp32-snippets/blob/master/c-utils/c_timeutils.c

/**
 * Subtract one timeval from another.
 */
struct timeval timeval_sub(struct timeval *a, struct timeval *b) {
	struct timeval result;
	result.tv_sec = a->tv_sec - b->tv_sec;
	result.tv_usec = a->tv_usec - b->tv_usec;
	if (a->tv_usec < b->tv_usec) {
		result.tv_sec -= 1;
		result.tv_usec += 1000000;
	}
	return result;
} // timeval_sub



/**
 * Convert a timeval into the number of msecs.
 */
uint32_t timeval_toMsecs(struct timeval *a) {
	return a->tv_sec * 1000 + a->tv_usec/1000;
} // timeval_toMsecs




/**
 * Write a log message to stdout.  The function takes the "type" of the
 * message as a single character which is likely to be 'E' for Error, 'W' for
 * Warning, 'D' for Debug and 'V' for Verbose.  Then there is a printf format
 * string followed by parameters.  The output is of the same form as ESP32 logging.
 */
void
scde_log(char *tag, char type, char *fmt, ...) {
  struct timeval now;
  gettimeofday(&now, NULL);
  struct timeval diff_tv = timeval_sub(&now, &startTime);
  uint32_t diff = timeval_toMsecs(&diff_tv);
  printf("%c (%d) %s: ", type, diff, tag);
  va_list vl;
  va_start(vl, fmt);
  vprintf(fmt, vl);
  va_end(vl);
  printf("\n");
}

#endif
