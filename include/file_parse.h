#ifndef FILE_PARSE_H
#define FILE_PARSE_H

/* Macros */
#define ISINT    0
#define ISDOUBLE 1
#define ISLONG   2
#define ISUINT   3
#define ISULONG  4
#define IN_PROPS 0
#define IN_CONTEXT 1

/* Structures */
struct inputs {
	const char *name;
	int cl_name;
	void *val;
	int type;
	int touched;
	int cl_o;
};

/* Function prototypes */
int file_parse(char *fin, struct inputs *finputs);
void inputs_print(struct inputs *finputs);
int cl_parse(int argc, char **argv, struct inputs *finputs, char **_fin, char **_fout, char **_fopt);

#endif
