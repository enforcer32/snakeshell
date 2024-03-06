#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef ARG_MAX
#define ARG_MAX 131072
#endif

char **parse_str(char *str, const char *delim)
{
	char *token = NULL;
	char **tokens = NULL;
	char *saveptr = NULL;
	size_t i = 0;

	tokens = malloc(ARG_MAX * sizeof(char *));
	if(!tokens) return NULL;
	memset(tokens, 0, ARG_MAX);

	token = strtok_r(str, delim, &saveptr);
	while (token) {
		tokens[i++] = token;
		token = strtok_r(NULL, delim, &saveptr);
	}

	tokens[i] = NULL;
	return tokens;
}
