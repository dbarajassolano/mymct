#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/time.h>

#include "mymct.h"

const double kMicro = 1.0e-6;

static char* get_error_message(const char *message, const char *fname, int lineno, const char *fxname) {
	const char *format  = "ERROR: %s at %s:%d, %s";
	size_t needed = snprintf(NULL, 0, format, message, fname, lineno, fxname);
	char *buffer  = malloc(needed + 1);
	if (!buffer)
		return NULL;
	sprintf(buffer, format, message, fname, lineno, fxname);
	return buffer;
}

int ptrcheck(const char *message, const char *fname, int lineno, const char *fxname, void *ptr, ...){
	
	va_list ap;
	void **i = NULL;
	char *err_message = get_error_message(message, fname, lineno, fxname);
	if (!err_message) {
		fprintf(stderr, "Couldn't verify pointers at %s:%d, %s\n", fname, lineno, fxname);
		return 1;
	}
	va_start(ap, ptr);
	for (i = ptr; i != NULL; i = va_arg(ap, void*)){
		if (!(*i)){
			if (errno)
				perror(err_message);
			else
				fprintf(stderr, "%s\n", err_message);
			free(err_message);
			return 1;
		}
	}
	va_end(ap);
	free(err_message);
	return 0;
}

/* Get time of the day with millisecond precision.  
   
   Can be used for wall-clock timing.  Multiple versions of this function
   exist on the Internet, albeit minor differences.  I got this one from
   Scott Baden <baden@ucsd.edu>, and changed the error printing to
   stderr */
double getTime() {
	
	struct timeval TV;
	const int RC = gettimeofday(&TV, NULL);
	
	if(RC == -1) {
		fprintf(stderr, "ERROR: Bad call to gettimeofday\n");
		return(-1);
	}
	return(((double)TV.tv_sec) + kMicro*((double)TV.tv_usec));
}
