/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <nanvix/fs.h>
#include <nanvix/syscall.h>
#include <errno.h>

/* Software versioning. */
#define VERSION_MAJOR 1 /* Major version. */
#define VERSION_MINOR 0 /* Minor version. */

/*
 * Program arguments.
 */
static struct
{
	char *dirname; /* Directory name. */
} args = { NULL };

/*
 * Prints program version and exits.
 */
static void version(void)
{
	printf("mkdir (Nanvix Coreutils) %d.%d\n\n", VERSION_MAJOR, VERSION_MINOR);
	printf("Copyright(C) 2011-2014 Pedro H. Penna\n");
	printf("This is free software under the "); 
	printf("GNU General Public License Version 3.\n");
	printf("There is NO WARRANTY, to the extent permitted by law.\n\n");
	
	exit(EXIT_SUCCESS);
}

/*
 * Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: mkdir [options] <dirname>\n\n");
	printf("Brief: Creates directories.\n\n");
	printf("Options:\n");
	printf("  --help    Display this information and exit\n");
	printf("  --version Display program version and exit\n");
	
	exit(EXIT_SUCCESS);
}

/*
 * Gets program arguments.
 */
static void getargs(int argc, char *const argv[])
{
	int i;     /* Loop index.       */
	char *arg; /* Current argument. */
		
	/* Read command line arguments. */
	for (i = 1; i < argc; i++)
	{
		arg = argv[i];
		
		/* Parse command line argument. */
		if (!strcmp(arg, "--help")) {
			usage();
		}
		else if (!strcmp(arg, "--version")) {
			version();
		}
		else {
			args.dirname = arg;
		}
	}
	
	/* Missing argument. */
	if ((args.dirname == NULL))
	{
		fprintf(stderr, "mkdir: missing operand\n");
		usage();
	}
}

ssize_t call_sys_open(const char *path, int oflag, mode_t mode)
{
	ssize_t ret;
	
	__asm__ volatile (
		"int $0x80"
		: "=a" (ret)
		: "0" (NR_open),
		  "b" (path),
		  "c" (oflag),
		  "d" (mode)
	);
	
	/* Error. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}
	
	return ((ssize_t)ret);
}

int mkdir(const char* name, int flag) 
{
	//open file
	call_sys_open(name, flag, S_IFDIR);

	return 0;
}

/*
 * Creates directories
 */
int main(int argc, char *const argv[])
{
	getargs(argc, argv);
	
	/* Failed to mkdir(). */
	if (mkdir(args.dirname, S_IRWXU|S_IRWXG|S_IRWXO) < 0)
	{
		fprintf(stderr, "mkdir: cannot mkdir()\n");
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
