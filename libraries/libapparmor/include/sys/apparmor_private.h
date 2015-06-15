/*
 * Copyright 2014 Canonical Ltd.
 *
 * The libapparmor library is licensed under the terms of the GNU
 * Lesser General Public License, version 2.1. Please see the file
 * COPYING.LGPL.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SYS_APPARMOR_PRIVATE_H
#define _SYS_APPARMOR_PRIVATE_H	1

#include <stdio.h>
#include <sys/stat.h>

__BEGIN_DECLS

int _aa_is_blacklisted(const char *name, const char *path);

void _aa_autofree(void *p);
void _aa_autoclose(int *fd);
void _aa_autofclose(FILE **f);

int _aa_asprintf(char **strp, const char *fmt, ...);

int _aa_dirat_for_each(int dirfd, const char *name, void *data,
		       int (* cb)(int, const char *, struct stat *, void *));

__END_DECLS

#endif	/* sys/apparmor_private.h */
