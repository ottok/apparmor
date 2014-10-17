/*
 *   Copyright (c) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007
 *   NOVELL (All rights reserved)
 *
 *   Copyright (c) 2013
 *   Canonical Ltd. (All rights reserved)
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of version 2 of the GNU General Public
 *   License published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, contact Novell, Inc.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include <string>
#include <sstream>

#include "parser.h"
#include "profile.h"
#include "libapparmor_re/apparmor_re.h"

#include <unistd.h>
#include <linux/unistd.h>


#define SD_CODE_SIZE (sizeof(u8))
#define SD_STR_LEN (sizeof(u16))


int __sd_serialize_profile(int option, Profile *prof);

static void print_error(int error)
{
	switch (error) {
	case -ESPIPE:
		PERROR(_("Bad write position\n"));
		break;
	case -EPERM:
		PERROR(_("Permission denied\n"));
		break;
	case -ENOMEM:
		PERROR(_("Out of memory\n"));
		break;
	case -EFAULT:
		PERROR(_("Couldn't copy profile: Bad memory address\n"));
		break;
	case -EPROTO:
		PERROR(_("Profile doesn't conform to protocol\n"));
		break;
	case -EBADMSG:
		PERROR(_("Profile does not match signature\n"));
		break;
	case -EPROTONOSUPPORT:
		PERROR(_("Profile version not supported by Apparmor module\n"));
		break;
	case -EEXIST:
		PERROR(_("Profile already exists\n"));
		break;
	case -ENOENT:
		PERROR(_("Profile doesn't exist\n"));
		break;
	case -EACCES:
		PERROR(_("Permission denied; attempted to load a profile while confined?\n"));
		break;
	default:
		PERROR(_("Unknown error (%d): %s\n"), -error, strerror(-error));
		break;
	}
}

int load_profile(int option, Profile *prof)
{
	int retval = 0;
	int error = 0;

	PDEBUG("Serializing policy for %s.\n", prof->name);
	retval = __sd_serialize_profile(option, prof);

	if (retval < 0) {
		error = retval;	/* yeah, we'll just report the last error */
		switch (option) {
		case OPTION_ADD:
			PERROR(_("%s: Unable to add \"%s\".  "),
			       progname, prof->name);
			print_error(error);
			break;
		case OPTION_REPLACE:
			PERROR(_("%s: Unable to replace \"%s\".  "),
			       progname, prof->name);
			print_error(error);
			break;
		case OPTION_REMOVE:
			PERROR(_("%s: Unable to remove \"%s\".  "),
			       progname, prof->name);
			print_error(error);
			break;
		case OPTION_STDOUT:
			PERROR(_("%s: Unable to write to stdout\n"),
			       progname);
			break;
		case OPTION_OFILE:
			PERROR(_("%s: Unable to write to output file\n"),
			       progname);
		default:
			PERROR(_("%s: ASSERT: Invalid option: %d\n"),
			       progname, option);
			exit(1);
			break;
		}

	} else if (conf_verbose) {
		switch (option) {
		case OPTION_ADD:
			printf(_("Addition succeeded for \"%s\".\n"),
			       prof->name);
			break;
		case OPTION_REPLACE:
			printf(_("Replacement succeeded for \"%s\".\n"),
			       prof->name);
			break;
		case OPTION_REMOVE:
			printf(_("Removal succeeded for \"%s\".\n"),
			       prof->name);
			break;
		case OPTION_STDOUT:
		case OPTION_OFILE:
			break;
		default:
			PERROR(_("%s: ASSERT: Invalid option: %d\n"),
			       progname, option);
			exit(1);
			break;
		}
	}

	return error;
}



enum sd_code {
	SD_U8,
	SD_U16,
	SD_U32,
	SD_U64,
	SD_NAME,		/* same as string except it is items name */
	SD_STRING,
	SD_BLOB,
	SD_STRUCT,
	SD_STRUCTEND,
	SD_LIST,
	SD_LISTEND,
	SD_ARRAY,
	SD_ARRAYEND,
	SD_OFFSET
};

const char *sd_code_names[] = {
	"SD_U8",
	"SD_U16",
	"SD_U32",
	"SD_U64",
	"SD_NAME",
	"SD_STRING",
	"SD_BLOB",
	"SD_STRUCT",
	"SD_STRUCTEND",
	"SD_LIST",
	"SD_LISTEND",
	"SD_ARRAY",
	"SD_ARRAYEND",
	"SD_OFFSET"
};


static inline void sd_write8(std::ostringstream &buf, u8 b)
{
	buf.write((const char *) &b, 1);
}

static inline void sd_write16(std::ostringstream &buf, u16 b)
{
	u16 tmp;
	tmp = cpu_to_le16(b);
	buf.write((const char *) &tmp, 2);
}

static inline void sd_write32(std::ostringstream &buf, u32 b)
{
	u32 tmp;
	tmp = cpu_to_le32(b);
	buf.write((const char *) &tmp, 4);
}

static inline void sd_write64(std::ostringstream &buf, u64 b)
{
	u64 tmp;
	tmp = cpu_to_le64(b);
	buf.write((const char *) &tmp, 8);
}

static inline void sd_write_uint8(std::ostringstream &buf, u8 b)
{
	sd_write8(buf, SD_U8);
	sd_write8(buf, b);
}

static inline void sd_write_uint16(std::ostringstream &buf, u16 b)
{
	sd_write8(buf, SD_U16);
	sd_write16(buf, b);
}

static inline void sd_write_uint32(std::ostringstream &buf, u32 b)
{
	sd_write8(buf, SD_U32);
	sd_write32(buf, b);
}

static inline void sd_write_uint64(std::ostringstream &buf, u64 b)
{
	sd_write8(buf, SD_U64);
	sd_write64(buf, b);
}

static inline void sd_write_name(std::ostringstream &buf, const char *name)
{
	PDEBUG("Writing name '%s'\n", name);
	if (name) {
		sd_write8(buf, SD_NAME);
		sd_write16(buf, strlen(name) + 1);
		buf.write(name, strlen(name) + 1);
	}
}

static inline void sd_write_blob(std::ostringstream &buf, void *b, int buf_size, char *name)
{
	sd_write_name(buf, name);
	sd_write8(buf, SD_BLOB);
	sd_write32(buf, buf_size);
	buf.write((const char *) b, buf_size);
}


static char zeros[64];
#define align64(X) (((X) + (typeof(X)) 7) & ~((typeof(X)) 7))
static inline void sd_write_aligned_blob(std::ostringstream &buf, void *b, int b_size,
				 const char *name)
{
	sd_write_name(buf, name);
	/* pad calculation MUST come after name is written */
	size_t pad = align64(buf.tellp() + ((std::streamoff) 5l)) - (buf.tellp() + ((std::streamoff) 5l));
	sd_write8(buf, SD_BLOB);
	sd_write32(buf, b_size + pad);
	buf.write(zeros, pad);
	buf.write((const char *) b, b_size);
}

static void sd_write_strn(std::ostringstream &buf, char *b, int size, const char *name)
{
	sd_write_name(buf, name);
	sd_write8(buf, SD_STRING);
	sd_write16(buf, size);
	buf.write(b, size);
}

static inline void sd_write_string(std::ostringstream &buf, char *b, const char *name)
{
	sd_write_strn(buf, b, strlen(b) + 1, name);
}

static inline void sd_write_struct(std::ostringstream &buf, const char *name)
{
	sd_write_name(buf, name);
	sd_write8(buf, SD_STRUCT);
}

static inline void sd_write_structend(std::ostringstream &buf)
{
	sd_write8(buf, SD_STRUCTEND);
}

static inline void sd_write_array(std::ostringstream &buf, const char *name, int size)
{
	sd_write_name(buf, name);
	sd_write8(buf, SD_ARRAY);
	sd_write16(buf, size);
}

static inline void sd_write_arrayend(std::ostringstream &buf)
{
	sd_write8(buf, SD_ARRAYEND);
}

static inline void sd_write_list(std::ostringstream &buf, const char *name)
{
	sd_write_name(buf, name);
	sd_write8(buf, SD_LIST);
}

static inline void sd_write_listend(std::ostringstream &buf)
{
	sd_write8(buf, SD_LISTEND);
}

void sd_serialize_dfa(std::ostringstream &buf, void *dfa, size_t size)
{
	if (dfa)
		sd_write_aligned_blob(buf, dfa, size, "aadfa");
}

void sd_serialize_rlimits(std::ostringstream &buf, struct aa_rlimits *limits)
{
	if (!limits->specified)
		return;

	sd_write_struct(buf, "rlimits");
	sd_write_uint32(buf, limits->specified);
	sd_write_array(buf, NULL, RLIM_NLIMITS);
	for (int i = 0; i < RLIM_NLIMITS; i++) {
		sd_write_uint64(buf, limits->limits[i]);
	}
	sd_write_arrayend(buf);
	sd_write_structend(buf);
}

void sd_serialize_xtable(std::ostringstream &buf, char **table)
{
	int count;
	if (!table[4])
		return;
	sd_write_struct(buf, "xtable");
	count = 0;
	for (int i = 4; i < AA_EXEC_COUNT; i++) {
		if (table[i])
			count++;
	}

	sd_write_array(buf, NULL, count);
	for (int i = 4; i < count + 4; i++) {
		int len = strlen(table[i]) + 1;

		/* if its a namespace make sure the second : is overwritten
		 * with 0, so that the namespace and name are \0 seperated
		 */
		if (*table[i] == ':') {
			char *tmp = table[i] + 1;
			strsep(&tmp, ":");
		}
		sd_write_strn(buf, table[i], len, NULL);
	}
	sd_write_arrayend(buf);
	sd_write_structend(buf);
}

void sd_serialize_profile(std::ostringstream &buf, Profile *profile,
			 int flattened)
{
	uint64_t allowed_caps;

	sd_write_struct(buf, "profile");
	if (flattened) {
		assert(profile->parent);
		char *name = (char *) malloc(3 + strlen(profile->name) +
				    strlen(profile->parent->name));
		if (!name)
			return;
		sprintf(name, "%s//%s", profile->parent->name, profile->name);
		sd_write_string(buf, name, NULL);
		free(name);
	} else {
		sd_write_string(buf, profile->name, NULL);
	}

	/* only emit this if current kernel at least supports "create" */
	if (perms_create) {
		if (profile->xmatch) {
			sd_serialize_dfa(buf, profile->xmatch, profile->xmatch_size);
			sd_write_uint32(buf, profile->xmatch_len);
		}
	}

	sd_write_struct(buf, "flags");
	/* used to be flags.debug, but that's no longer supported */
	sd_write_uint32(buf, profile->flags.hat);
	sd_write_uint32(buf, profile->flags.complain);
	sd_write_uint32(buf, profile->flags.audit);
	sd_write_structend(buf);
	if (profile->flags.path) {
		int flags = 0;
		if (profile->flags.path & PATH_CHROOT_REL)
			flags |= 0x8;
		if (profile->flags.path & PATH_MEDIATE_DELETED)
			flags |= 0x10000;
		if (profile->flags.path & PATH_ATTACH)
			flags |= 0x4;
		if (profile->flags.path & PATH_CHROOT_NSATTACH)
			flags |= 0x10;

		sd_write_name(buf, "path_flags");
		sd_write_uint32(buf, flags);
	}

#define low_caps(X) ((u32) ((X) & 0xffffffff))
#define high_caps(X) ((u32) (((X) >> 32) & 0xffffffff))
	allowed_caps = (profile->caps.allow) & ~profile->caps.deny;
	sd_write_uint32(buf, low_caps(allowed_caps));
	sd_write_uint32(buf, low_caps(allowed_caps & profile->caps.audit));
	sd_write_uint32(buf, low_caps(profile->caps.deny & profile->caps.quiet));
	sd_write_uint32(buf, 0);

	sd_write_struct(buf, "caps64");
	sd_write_uint32(buf, high_caps(allowed_caps));
	sd_write_uint32(buf, high_caps(allowed_caps & profile->caps.audit));
	sd_write_uint32(buf, high_caps(profile->caps.deny & profile->caps.quiet));
	sd_write_uint32(buf, 0);
	sd_write_structend(buf);

	sd_serialize_rlimits(buf, &profile->rlimits);

	if (profile->net.allow && kernel_supports_network) {
		size_t i;
		sd_write_array(buf, "net_allowed_af", get_af_max());
		for (i = 0; i < get_af_max(); i++) {
			u16 allowed = profile->net.allow[i] &
				~profile->net.deny[i];
			sd_write_uint16(buf, allowed);
			sd_write_uint16(buf, allowed & profile->net.audit[i]);
			sd_write_uint16(buf, profile->net.deny[i] & profile->net.quiet[i]);
		}
		sd_write_arrayend(buf);
	} else if (profile->net.allow && (warnflags & WARN_RULE_NOT_ENFORCED))
		pwarn(_("profile %s network rules not enforced\n"), profile->name);

	if (profile->policy.dfa) {
		sd_write_struct(buf, "policydb");
		sd_serialize_dfa(buf, profile->policy.dfa, profile->policy.size);
		sd_write_structend(buf);
	}

	/* either have a single dfa or lists of different entry types */
	sd_serialize_dfa(buf, profile->dfa.dfa, profile->dfa.size);
	sd_serialize_xtable(buf, profile->exec_table);

	sd_write_structend(buf);
}

void sd_serialize_top_profile(std::ostringstream &buf, Profile *profile)
{
	uint32_t version;

	version = ENCODE_VERSION(force_complain, policy_version,
				 parser_abi_version, kernel_abi_version);

	sd_write_name(buf, "version");
	sd_write_uint32(buf, version);

	if (profile_ns) {
		sd_write_string(buf, profile_ns, "namespace");
	} else if (profile->ns) {
		sd_write_string(buf, profile->ns, "namespace");
	}

	sd_serialize_profile(buf, profile, profile->parent ? 1 : 0);
}

int cache_fd = -1;
int __sd_serialize_profile(int option, Profile *prof)
{
	int fd = -1;
	int error = -ENOMEM, size, wsize;
	std::ostringstream work_area;
	char *filename = NULL;

	switch (option) {
	case OPTION_ADD:
		if (asprintf(&filename, "%s/.load", subdomainbase) == -1)
			goto exit;
		if (kernel_load) fd = open(filename, O_WRONLY);
		break;
	case OPTION_REPLACE:
		if (asprintf(&filename, "%s/.replace", subdomainbase) == -1)
			goto exit;
		if (kernel_load) fd = open(filename, O_WRONLY);
		break;
	case OPTION_REMOVE:
		if (asprintf(&filename, "%s/.remove", subdomainbase) == -1)
			goto exit;
		if (kernel_load) fd = open(filename, O_WRONLY);
		break;
	case OPTION_STDOUT:
		filename = strdup("stdout");
		fd = dup(1);
		break;
	case OPTION_OFILE:
		fd = dup(fileno(ofile));
		break;
	default:
		error = -EINVAL;
		goto exit;
		break;
	}

	if (fd < 0 && (kernel_load || option == OPTION_OFILE || option == OPTION_STDOUT)) {
		PERROR(_("Unable to open %s - %s\n"), filename,
		       strerror(errno));
		error = -errno;
		goto exit;
	}

	error = 0;

	free(filename);

	if (option == OPTION_REMOVE) {
		char *name, *ns = NULL;
		int len = 0;

		if (profile_ns) {
			len += strlen(profile_ns) + 2;
			ns = profile_ns;
		} else if (prof->ns) {
			len += strlen(prof->ns) + 2;
			ns = prof->ns;
		}
		if (prof->parent) {
			name = (char *) malloc(strlen(prof->name) + 3 +
				      strlen(prof->parent->name) + len);
			if (!name) {
				PERROR(_("Memory Allocation Error: Unable to remove ^%s\n"), prof->name);
				error = -errno;
				goto exit;
			}
			if (ns)
				sprintf(name, ":%s:%s//%s", ns,
					prof->parent->name, prof->name);
			else
				sprintf(name, "%s//%s", prof->parent->name,
					prof->name);
		} else if (ns) {
			name = (char *) malloc(len + strlen(prof->name) + 1);
			if (!name) {
				PERROR(_("Memory Allocation Error: Unable to remove %s:%s."), ns, prof->name);
				error = -errno;
				goto exit;
			}
			sprintf(name, ":%s:%s", ns, prof->name);
		} else {
			name = prof->name;
		}
		size = strlen(name) + 1;
		if (kernel_load) {
			wsize = write(fd, name, size);
			if (wsize < 0)
				error = -errno;
		}
		if (prof->parent || ns)
			free(name);
	} else {
		sd_serialize_top_profile(work_area, prof);

		size = (long) work_area.tellp();
		if (kernel_load || option == OPTION_STDOUT || option == OPTION_OFILE) {
			std::string tmp = work_area.str();
			wsize = write(fd, tmp.c_str(), size);
			if (wsize < 0) {
				error = -errno;
			} else if (wsize < size) {
				PERROR(_("%s: Unable to write entire profile entry\n"),
				       progname);
				error = -EIO;
			}
		}
		if (cache_fd != -1) {
			std::string tmp = work_area.str();
			wsize = write(cache_fd, tmp.c_str(), size);
			if (wsize < 0) {
				error = -errno;
			} else if (wsize < size) {
				PERROR(_("%s: Unable to write entire profile entry to cache\n"),
				       progname);
				error = -EIO;
			}
		}
	}

	close(fd);

	if (!prof->hat_table.empty() && option != OPTION_REMOVE) {
		if (load_flattened_hats(prof, option) == 0)
			return 0;
	}


exit:
	return error;
}

/* bleah the kernel should just loop and do multiple load, but to support
 * older systems we need to do this
 */
#define PROFILE_HEADER_SIZE
static char header_version[] = "\x04\x08\x00version";

static char *next_profile_buffer(char *buffer, int size)
{
	char *b = buffer;

	for (; size - sizeof(header_version); b++, size--) {
		if (memcmp(b, header_version, sizeof(header_version)) == 0) {
			return b;
		}
	}
	return NULL;
}

static int write_buffer(int fd, char *buffer, int size, bool set)
{
	const char *err_str = set ? "profile set" : "profile";
	int wsize = write(fd, buffer, size);
	if (wsize < 0) {
		PERROR(_("%s: Unable to write %s\n"), progname, err_str);
		return -errno;
	} else if (wsize < size) {
		PERROR(_("%s: Unable to write %s\n"), progname, err_str);
		return -EPROTO;
	}
	return 0;
}

int sd_load_buffer(int option, char *buffer, int size)
{
	int fd = -1;
	int error, bsize;
	char *filename = NULL;

	/* TODO: push backup into caller */
	if (!kernel_load)
		return 0;

	switch (option) {
	case OPTION_ADD:
		if (asprintf(&filename, "%s/.load", subdomainbase) == -1)
			return -ENOMEM;
		break;
	case OPTION_REPLACE:
		if (asprintf(&filename, "%s/.replace", subdomainbase) == -1)
			return -ENOMEM;
		break;
	default:
		return -EINVAL;
	}

	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		PERROR(_("Unable to open %s - %s\n"), filename,
		       strerror(errno));
		error = -errno;
		goto out;
	}

	if (kernel_supports_setload) {
		error = write_buffer(fd, buffer, size, true);
	} else {
		char *b, *next;

		error = 0;	/* in case there are no profiles */
		for (b = buffer; b; b = next, size -= bsize) {
			next = next_profile_buffer(b + sizeof(header_version),
						   size);
			if (next)
				bsize = next - b;
			else
				bsize = size;
			error = write_buffer(fd, b, bsize, false);
			if (error)
				break;
		}
	}
	close(fd);

out:
	free(filename);

	return error;
}
