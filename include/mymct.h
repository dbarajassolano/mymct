#ifndef MYMCT_H
#define MYMCT_H

/* Function prototypes */
int ptrcheck(const char *message, const char *fname, int lineno, const char *fxname, void *ptr, ...);
double getTime();

/* Macros */
#define PTRCHECK(message, ...) ptrcheck(message, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
