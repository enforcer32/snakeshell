#include "handler.h"
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int command_handler(char **args)
{
	size_t command = 0;
	const char *cmds[] = { "info", "exit", "cd" };

	for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
		if (!strcmp(cmds[i], args[0])) {
			command = i + 1;
			break;
		}
	}

	switch (command) {
	case 1:
		info();
		break;
	case 2:
		return raise(SIGTERM);
	case 3:
		return cd(args);
	default:
		return -1;
	}

	return 0;
}

void info(void)
{
	puts("SnakeShell is a Shell for Unix-Like Operating Systems");
}

int cd(char **args)
{
	if (!args[1]) {
		fprintf(stderr, "Expected argument to cd\n");
		return 1;
	}

	if (chdir(args[1]) != 0) {
		perror("cd");
		return 1;
	}

	return 0;
}
