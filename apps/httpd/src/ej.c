/*
 * Tiny Embedded JavaScript parser
 *
 * Copyright (C) 2001 Broadcom Corporation
 *
 * $Id: ej.c,v 1.1.1.1 2001/09/21 16:33:33 mhuang Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cgimain.h"
#include "httpd.h"

static void
ejGet(int argc, char **argv, FILE *stream)
{
	char value[WEB_BIG_BUF_SIZE_MAX]; // big ipsec table

	if (argc < 2)
		return;

   value[0] = '\0';
   cgiGetVar(argv[1], value);

	fputs(value, stream);
}

static void
ejGetTest(int argc, char **argv, FILE *stream)
{
	char value[WEB_BUF_SIZE_MAX];

	if (argc < 2)
		return;

   value[0] = '\0';
   cgiGetTestVar(argv[1], value);

	fputs(value, stream);
}

static void
ejGetOther(int argc, char **argv, FILE *stream)
{
	char value[WEB_BUF_SIZE_MAX];

	if (argc < 2) return;

   value[0] = '\0';
   cgiGetVarOther(argc, argv, value);

	fputs(value, stream);
}

static void
ejFncCmd(int argc, char **argv, FILE *stream)
{
	if (argc < 2) return;

   cgiFncCmd(argc, argv);
}

struct ej_handler ej_handlers[] = {
	{ "ejGet", ejGet },
	{ "ejGetTest", ejGetTest },
	{ "ejGetOther", ejGetOther },
	{ "ejFncCmd", ejFncCmd },
	{ NULL, NULL }
};

static char * get_arg(char *args, char **next);
static void call(char *func, FILE *stream);

static char *
get_arg(char *args, char **next)
{
	char *arg, *end;

	/* Parse out arg, ... */
	if (!(end = strchr(args, ','))) {
		end = args + strlen(args);
		*next = NULL;
	} else
		*next = end + 1;

	/* Skip whitespace and quotation marks on either end of arg */
	for (arg = args; isspace(*arg) || *arg == '"'; arg++);
	for (*end-- = '\0'; isspace(*end) || *end == '"'; end--)
		*end = '\0';

	return arg;
}

static void
call(char *func, FILE *stream)
{
	char *args, *end, *next;
	int argc;
	char * argv[16];
	struct ej_handler *handler;

	/* Parse out ( args ) */
	if (!(args = strchr(func, '(')))
		return;
	if (!(end = strchr(func, ')')))
		return;
	*args++ = *end = '\0';

	/* Set up argv list */
	argv[0] = func;
	for (argc = 1; argc < 16 && args; argc++, args = next) {
		if (!(argv[argc] = get_arg(args, &next)))
			break;
	}

	/* Call handler */
	for (handler = &ej_handlers[0]; handler->pattern; handler++) {
		if (strncmp(handler->pattern, func, strlen(handler->pattern)) == 0)
			handler->output(argc, argv, stream);
	}
}

void
do_ej(char *path, FILE *stream)
{
	FILE *fp;
	int c;
	char pattern[256], *func = NULL, *end = NULL;
	int len = 0;

	if (!(fp = fopen(path, "r")))
		return;

	while ((c = getc(fp)) != EOF) {

		/* Add to pattern space */
		pattern[len++] = c;
		pattern[len] = '\0';
		if (len == (sizeof(pattern) - 1))
			goto release;

		/* Look for <% ... */
		if (!func && strncmp(pattern, "<%", strlen(pattern)) == 0) {
			if (strlen(pattern) == 2)
				func = pattern + 2;
			continue;
		}

		/* Look for the function call */
		if (func) {
			if ((end = strstr(func, "%>"))) {
				/* Skip whitespace on either end of function call */
				for (; isspace(*func); func++);
				for (*end-- = '\0'; isspace(*end); end--)
					*end = '\0';

				/* Call function */
				call(func, stream);
				func = NULL;
				len = 0;
			}
			continue;
		}

	release:
		/* Release pattern space */
		fputs(pattern, stream);
		func = NULL;
		len = 0;
	}

	fclose(fp);
}
