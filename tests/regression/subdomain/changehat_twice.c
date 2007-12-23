/* $Id$ */

/*
 *	Copyright (C) 2002-2005 Novell/SUSE
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License as
 *	published by the Free Software Foundation, version 2 of the
 *	License.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <linux/unistd.h>

#include "changehat.h"

int main(int argc, char *argv[])
{
	int rc, magic;

	if (argc != 5){
		fprintf(stderr, "usage: %s profile1 profile2 goodmagic|badmagic file\n",
			argv[0]);
		return 1;
	}

	/* change hat if hatname != nochange */
	if (strcmp(argv[1], "nochange") != 0){
		rc = change_hat(argv[1], SD_ID_MAGIC+1);
		if (rc == -1){
			fprintf(stderr, "FAIL: changehat %s failed - %s\n",
				argv[1], strerror(errno));
			exit(1);
		}
	}

	magic=SD_ID_MAGIC+1;
	if (strcmp(argv[3], "badmagic") == 0){
		magic^=0xdeadbeef;
	}

	if (strcmp(argv[1], "nochange") != 0){
		rc = change_hat(argv[2], magic);
		if (rc == -1){
			fprintf(stderr, "FAIL: changehat %s failed - %s\n",
				argv[1], strerror(errno));
			exit(1);
		}
	}

	rc = do_open(argv[4]);
	if (rc != 0)
		return rc;

	printf("PASS\n");
	return 0;
}
