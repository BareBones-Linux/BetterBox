/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 * Copyright (C) 2006 Gabriel Somlo <somlo at cmu.edu>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */
//config:config WHICH
//config:	bool "which (4 kb)"
//config:	default y
//config:	help
//config:	which is used to find programs in your PATH and
//config:	print out their pathnames.

//applet:IF_WHICH(APPLET_NOFORK(which, which, BB_DIR_USR_BIN, BB_SUID_DROP, which))

//kbuild:lib-$(CONFIG_WHICH) += which.o

//usage:#define which_trivial_usage
//usage:       "[-a] COMMAND..."
//usage:#define which_full_usage "\n\n"
//usage:       "Locate COMMAND\n"
//usage:     "\n	-a	Show all matches"
//usage:
//usage:#define which_example_usage
//usage:       "$ which login\n"
//usage:       "/bin/login\n"

#include "libbb.h"

int which_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int which_main(int argc UNUSED_PARAM, char **argv)
{
	const char *env_path;
	int status = 0;

	env_path = getenv("PATH");
	if (!env_path)
		env_path = bb_default_root_path;

	getopt32(argv, "^" "a" "\0" "-1"/*at least one arg*/);
	argv += optind;

	do {
		int missing = 1;

		/* If file contains a slash don't use PATH */
		if (strchr(*argv, '/')) {
			if (file_is_executable(*argv)) {
				missing = 0;
				puts(*argv);
			}
		} else {
			const char *path;
			char *p;

			path = env_path;
			/* NOFORK NB: xmalloc inside find_executable(), must have no allocs above! */
			while ((p = find_executable(*argv, &path)) != NULL) {
				missing = 0;
				puts(p);
				free(p);
				if (!option_mask32) /* -a not set */
					break;
			}
		}
		status |= missing;
	} while (*++argv);

	return status;
}
