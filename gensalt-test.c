/*
 * salt-test.c -- test suite for gensalt()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gensalt.h"

struct test {
	char *template;
	char *expected;
	char *msg;
};

static struct test t[] = {
	/* sanity check: empty string */
	{ "",				"",				"" },

	/* literal characters */
	{ "x",				"x",				"" },

	/* {} counts only apply after a character set,
	 * so the { and } characters are not treated specially
	 */
	{ "x{1}",			"x{1}",				"" },

	/* test that {} counts work properly; default is 1, empty is 0 */
	{ "[x]{}",			"",				"" },
	{ "[x]{0}",			"",				"" },
	{ "[x]{1}",			"x",				"" },
	{ "[x]",			"x",				"" },
	{ "[x]{2}",			"xx",				"" },

	/* test that {} counts and ranges work properly */
	{ "[xy]{0}",			"",				"" },
	{ "[xy]{1}",			"x",				"" },
	{ "[xy]",			"x",				"" },
	{ "[xy]{2}",			"xy",				"" },
	{ "[x-z]{0}",			"",				"" },
	{ "[x-z]{1}",			"x",				"" },
	{ "[x-z]",			"x",				"" },
	{ "[x-z]{2}",			"xy",				"" },
	{ "[a-z]{26}",			"abcdefghijklmnopqrstuvwxyz",	"" },
	{ "[0-9a-z]{20}",		"0123456789abcdefghij",		"" },
	{ "[A-Za-z0-9./]{2}",		"AB",				"" },
	{ "_[A-Za-z0-9./]{8}",		"_ABCDEFGH",			"" },
	{ "$1$[A-Za-z0-9./]{8}",	"$1$ABCDEFGH",			"" },
	{ "$1$[A-Za-z0-9./]{16}",	"$1$ABCDEFGHIJKLMNOP",		"" },
	{ "$2a$[0-9]{2}$",		"$2a$01$",			"" },

	/* verify that special characters are only special in context */
	{ "{",				"{",				"" },
	{ "}",				"}",				"" },
	{ "-",				"-",				"" },
	{ "[",				NULL,				"expected literal or ']'" },
	{ "]",				"]",				"" },
	{ "[[]",			"[",				"" },
	{ "[-]",			"-",				"" },
	{ "[{]",			"{",				"" },
	{ "[}]",			"}",				"" },
	{ "[!-[]",			"!",				"" },
	{ "[!--]",			"!",				"" },
	{ "[!-{]",			"!",				"" },
	{ "[!-}]",			"!",				"" },
	{ "[!-]",			NULL,				"expected end of range" },

	/* verify escape turns special characters into literals */
	{ "\\[a-z]",			"[a-z]",			"" },
	{ "[!-\\]]",			"!",				"" },
	{ "[a-z]\\{2}",			"a{2}",				"" },

	/* verify range completion */
	{ "[a",				NULL,				"expected literal or ']'" },
	{ "[a-",			NULL,				"expected end of range" },

	/* test backward range detection */
	{ "[z-a]",			NULL,				"backwards range" },

	/* test empty ranges result in empty strings */
	{ "[]",				"",				"" },
	{ "[]{2}",			"",				"" },

	/* test {} count syntax */
	{ "[x]{",			NULL,				"expected digit or '}'" },
	{ "[x]{a}",			NULL,				"expected digit or '}'" },

	/* (these two are a bit wierd, may not work in future */
	{ "[x]{\061}",			"x",				"" },
	{ "[x]{\0610}",			"xxxxxxxxxx",			"" },

	/* test escape at end of string and escape'd escape char */
	{ "\\",				"\\",				"" },
	{ "\\\\",			"\\",				"" },

	/* test proper octal escape calculation */
	{ "[\\141-\\172]{26}",		"abcdefghijklmnopqrstuvwxyz",	"" },
	{ "\\1",			"\001",				"" },
	{ "\\11",			"\011",				"" },
	{ "\\111",			"\111",				"" },
	{ "\\1111",			"\1111",			"" },
	{ "\\1a",			"\001a",			"" },
	{ "\\11a",			"\011a",			"" },
	{ "\\111a",			"\111a",			"" },
};

#define TESTS	(sizeof(t)/sizeof(struct test))

static int seq;

static void
reset(void)
{
	seq = 0;
}

int
genchar(int n)
{
	if (seq >= n)
		seq = 0;
	return seq++;
}

int
main(int argc, char **argv)
{
	int i;
	char *result;
	int fails = 0;

	for (i = 0; i < TESTS; ++i) {
		reset();
		result = gensalt(t[i].template, genchar);
		if (result == NULL) {
			if (t[i].expected != NULL) {
				printf("%d: expected \"%s\", got NULL(%s)\n", i, t[i].expected, gensalt_msg());
				++fails;
			} else if (strcmp(t[i].msg, gensalt_msg()) != 0) {
				printf("%d: expected msg \"%s\", got \"%s\"\n", i, t[i].msg, gensalt_msg());
				++fails;
			}
		} else {
			if (t[i].expected == NULL) {
				printf("%d: expected NULL(%s), got \"%s\"\n", i, t[i].msg, result);
				++fails;
			} else if (strcmp(result, t[i].expected) != 0) {
				printf("%d: expected \"%s\", got \"%s\"\n", i, t[i].expected, result);
				++fails;
			}
		}
	}
	exit(fails != 0);
}
