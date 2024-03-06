#include "ansi_colors.h"
#include "parser.h"
#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

static char *g_prompt = NULL;
static char *g_input = NULL;
static char **g_args = NULL;

static char *get_prompt(const char *color)
{
	char *prompt;
	char *username;
	char hostname[HOST_NAME_MAX + 1];
	char cwd[PATH_MAX + 1];
	size_t len;
	int written;

	username = getenv("USER");
	if (!username)
		username = "SSUSER";

	if (gethostname(hostname, sizeof(hostname)) == -1)
		strncpy(hostname, "SSHOST", HOST_NAME_MAX);

	if (!getcwd(cwd, sizeof(cwd)))
		strncpy(cwd, "/ss", PATH_MAX);
	else
		strncpy(cwd, strrchr(cwd, '/'), PATH_MAX);

	len = strlen(username) + strlen(hostname) + strlen(cwd) +
	      strlen(color) + strlen(ANSI_COLOR_RESET) + 7;

	prompt = malloc(len * sizeof(char));
	if(!prompt)return NULL;
	memset(prompt, 0, len);

	written = snprintf(prompt, len, "%s[%s@%s %s]$ %s", color, username,
			   hostname, cwd, ANSI_COLOR_RESET);

	if (written <= 0 || written != len - 1) {
		// #ifdef DEBUG
		fprintf(stderr, "Written = %d, len = %ld\n", written, len - 1);
		// #endif
		return NULL;
	}

	return prompt;
}

static char *get_input(const char *prompt)
{
	char *input = readline(prompt);
	if (!input || strlen(input) == 0)
		return NULL;

	add_history(input);
	return input;
}

static int ss_execute(char **args)
{
	int status;
	pid_t pid;

	/* Builtin Command Handler */
	status = command_handler(args);
	if (status >= 0)
		return status;

	/* OS Command Handler */
	pid = fork();
	if (pid == -1) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		if (execvp(args[0], args) < 0) {
			fprintf(stderr, ANSI_COLOR_WHITE "SS: command not found '%s'" ANSI_COLOR_RESET "\n", args[0]);
			exit(1);
		}
		exit(0);
	} else {
		waitpid(pid, &status, WUNTRACED);
	}

	return status;
}

static void ss_cleanup(void)
{
	free(g_prompt);
	free(g_input);
	free(g_args);
	fflush(stdout);
	fflush(stdin);
}

static void signal_handler(int signum)
{
	exit(0);
}

static void init_signal_handler(void)
{
	struct sigaction sigact, sigactold;

	sigact.sa_handler = signal_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;

	sigaction(SIGINT, NULL, &sigactold);
	if (sigactold.sa_handler != SIG_IGN)
		sigaction(SIGINT, &sigact, NULL);

	sigaction(SIGTERM, NULL, &sigactold);
	if (sigactold.sa_handler != SIG_IGN)
		sigaction(SIGTERM, &sigact, NULL);
}

int main(int argc, char **argv)
{
	fflush(stdout);
	fflush(stdin);

	atexit(ss_cleanup);
	init_signal_handler();

	int status = 0;

	while (1) {
		if (status == 0)
			g_prompt = get_prompt(ANSI_COLOR_GREEN);
		else
			g_prompt = get_prompt(ANSI_COLOR_RED);

		if (!g_prompt)
			g_prompt = "$ ";

		g_input = get_input(g_prompt);
		if (!g_input)
			continue;

		g_args = parse_str(g_input, " ");
		status = ss_execute(g_args);

		/* Cleanup */
		free(g_args);
		g_args = NULL;
		free(g_input);
		g_input = NULL;
		free(g_prompt);
		g_prompt = NULL;
	}

	return status;
}
