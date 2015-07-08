#include <ccan/json/json.h>
#include <ccan/tap/tap.h>

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool expect_literal(const char **sp, const char *str);

static char *chomp(char *s)
{
	char *e;
	
	if (s == NULL || *s == 0)
		return s;
	
	e = strchr(s, 0);
	if (e[-1] == '\n')
		*--e = 0;
	return s;
}
