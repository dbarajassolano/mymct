#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "../include/mymct.h"
#include "../include/file_parse.h"

static int parse_by_type(struct inputs *finputs, char *parval) {
	
	int               *val_int    = finputs->val;
	unsigned int      *val_uint   = finputs->val;
	long int          *val_long   = finputs->val;
	unsigned long int *val_ulong  = finputs->val;
	double            *val_double = finputs->val;
	char              *tail = NULL;

	if (finputs->cl_o > 0) {
		fprintf(stderr, "%s defined in file but overriden at call\n", finputs->name);
		return 0;
	}
	
	if ((finputs->touched > 0) && (finputs->cl_o == 0)) {
		fprintf(stderr, "Multiple definitions of %s\n", finputs->name);
		return -1;
	}
	
	switch (finputs->type) {
	case ISINT:
		*val_int    = strtol (parval, &tail, 0);
		break;
	case ISUINT:
		*val_uint   = strtoul(parval, &tail, 0);
		break;		
	case ISLONG:
		*val_long   = strtol (parval, &tail, 0);
		break;
	case ISULONG:
		*val_ulong  = strtoul(parval, &tail, 0);
		break;
	case ISDOUBLE:
		*val_double = strtod (parval, &tail);
		break;
	}
	if (*tail != '\0') {
		fprintf(stderr, "Couldn't understand the string %s\n", parval);
		return -1;
	}
	finputs->touched = 1;
	
	return 0;
}

static int parval_parse(struct inputs *finputs, char *parname, char *parval)
{
	int i = 0;
	while (1) {
		if (finputs[i].name == 0)
			break;
		if (strcmp(finputs[i].name, parname) == 0) {
			if (parse_by_type(&finputs[i], parval) != 0)
				return -1;
		}
		i++;
	}
	return 0;
}

int file_parse(char *fin, struct inputs *finputs)
{
	FILE *fp = NULL;
	char *line = NULL, *parname = NULL, *parval = NULL;
	size_t length = 0;
	int i, N_read, incomplete_flag = 0;
	
	fp = fopen(fin, "r");
	if (PTRCHECK("No input file", (void *) &fp, NULL))
		goto ERROR_NO_FILE;
	
	i = 0;
	while (getline(&line, &length, fp) != -1) {
		i++;
		N_read = sscanf(line, "%ms : %ms", &parname, &parval);
		if (N_read < 2) {
			fprintf(stderr, "Couldn't read parameter or value at input file, line %d\n", i);
			goto ERROR_READ;
		}
		if (parval_parse(finputs, parname, parval) != 0)
			goto ERROR_READ;
		free(parname); free(parval); parname = NULL; parval = NULL;
	}
	
	i = 0;
	while (finputs[i].name != 0) {
		if (finputs[i].touched != 1) {
			fprintf(stderr, "Missing parameter %s\n", finputs[i].name);
			incomplete_flag = 1;
		}
		i++;
	}
	if (incomplete_flag)
		goto ERROR_READ;
	
	fclose(fp); free(line); free(parname); free(parval);
	return 0;
	
ERROR_READ:
	fclose(fp); free(line); free(parname); free(parval);
	return -1;
	
ERROR_NO_FILE:
	return -1;
}

void inputs_print(struct inputs *finputs)
{
	int i = 0;
	while (finputs[i].name != 0) {
		printf("%s:\t", finputs[i].name);
		switch (finputs[i].type) {
		case ISINT:
			printf("%d\n", *((int *) finputs[i].val));
			break;
		case ISUINT:
			printf("%u\n", *((unsigned int *) finputs[i].val));
			break;    
		case ISLONG:
			printf("%ld\n", *((long int *) finputs[i].val));
			break;
		case ISULONG:
			printf("%lu\n", *((unsigned long int *) finputs[i].val));
			break;
		case ISDOUBLE:
			printf("%g\n", *((double *) finputs[i].val));
			break;
		}
		i++;
	}
}

static void getopt_long_option_destroy(struct option *long_opt, int Noptargs) {
	int j;
	for (j = 0; j < Noptargs; j++)
		free((char*) long_opt[j].name);
	free(long_opt);
}

static struct option *getopt_long_option_create(char **_optargs, int *_Noptargs, struct inputs *finputs)
{
	int i, j;

	/* Build the short option string, and determine number of
	 * optional args */
	int Noptargs = 0;
	i = 0;
	while (finputs[i].name != 0) {
		if (finputs[i].cl_name != 0)
			Noptargs++;
		i++;
	}
	char cl_name[] = {'a', 0};
	char *optargs = NULL;
	optargs = malloc(Noptargs * 2 + 5);
	if (PTRCHECK("Malloc error", &optargs, NULL)) {
		*_optargs = NULL;
		return NULL;
	}
	optargs[0] = 0;
	i = 0;
	while (finputs[i].name != 0) {
		if (finputs[i].cl_name != 0) {
			cl_name[0] = (char) finputs[i].cl_name;
			strcat(optargs, cl_name);
			strcat(optargs, ":");
		}
		i++;
	}
	strcat(optargs, "i:o:");

	/* Build the option struct */
	struct option *long_opt = NULL;
	long_opt = malloc((Noptargs + 3) * sizeof(*long_opt));
	if (PTRCHECK("Malloc error", &long_opt, NULL))
		return NULL;
	i = 0; j = 0;
	while (finputs[i].name != 0) {
		if (finputs[i].cl_name != 0) {
			long_opt[j].name = NULL;
			j++;
		}
		i++;
	}
	i = 0; j = 0;			
	while (finputs[i].name != 0) {
		if (finputs[i].cl_name != 0) {
			long_opt[j].name = strdup(finputs[i].name);
			if (PTRCHECK("Strdup error", &(long_opt[j].name), NULL))
				goto FREE_AT_ERROR;
			long_opt[j].val = finputs[i].cl_name;
			long_opt[j].has_arg = required_argument;
			long_opt[j].flag = NULL;
			j++;
		}
		i++;
	}
	long_opt[j].name        = "input";
	long_opt[j].val         = 'i';
	long_opt[j].has_arg     = required_argument;
	long_opt[j].flag        = NULL;
	long_opt[j + 1].name    = "output";
	long_opt[j + 1].val     = 'o';
	long_opt[j + 1].has_arg = required_argument;
	long_opt[j + 1].flag    = NULL;
	long_opt[j + 2].name    = 0;
	long_opt[j + 2].val     = 0;
	long_opt[j + 2].has_arg = 0;
	long_opt[j + 2].flag    = 0;

	*_optargs = optargs;
	*_Noptargs = Noptargs;
	return long_opt;

FREE_AT_ERROR:
	getopt_long_option_destroy(long_opt, Noptargs);
	free(optargs);
	*_optargs = NULL;
	return NULL;
}

int cl_parse(int argc, char **argv, struct inputs *finputs, char **_fin, char **_fout) {

	char *optargs;
	int Noptargs = 0;
	struct option *long_opt = getopt_long_option_create(&optargs, &Noptargs, finputs);
	if (PTRCHECK("Couldn't create getopt_long options", &optargs, &long_opt, NULL))
		return -1;
	int i, c, i_opt = 0;
	char *fin = NULL, *fout = NULL;
	while (1) {		
		c = getopt_long(argc, argv, optargs, long_opt, &i_opt);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			break;			
		case 'i':
			fin = optarg;
			break;
		case 'o':
			fout = optarg;
			break;
		case '?':
			break;			
		}		
		i = 0;
		while (finputs[i].name != 0) {
			if ((finputs[i].cl_name != 0) && (c == finputs[i].cl_name)) {
				if (parse_by_type(&finputs[i], optarg) == 0) {
					finputs[i].cl_o = 1;
					break;
				}
			}
			i++;
		}
	}

	*_fin = fin; *_fout = fout;
	free(optargs);
	getopt_long_option_destroy(long_opt, Noptargs);
	return 0;
}
