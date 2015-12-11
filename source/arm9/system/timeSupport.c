#include <sys/time.h>

time_t *punixTime;
struct _reent;

//---------------------------------------------------------------------------------
int __libnds_gtod(struct _reent *ptr, struct timeval *tp, struct timezone *tz) {
//---------------------------------------------------------------------------------

	if (tp != NULL) {
		tp->tv_sec = *punixTime;
		tp->tv_usec = 0;
	}
	if (tz != NULL) {
		tz->tz_minuteswest = 0;
		tz->tz_dsttime = 0;
	}

	return 0;

}
