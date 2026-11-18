/*
 * DYLD_INSERT_LIBRARIES shim that makes the clang driver and ld64 behave as
 * on Valve's 2014 build host. Built for the host (arm64); both tools are
 * ad-hoc signed and not hardened, so dyld honours the variable.
 *
 * Paths: a scratch directory appears at another absolute path, a userland
 * stand-in for a chroot. ld64 records input paths verbatim in N_OSO debug
 * notes, so linking objects that "live" under /Users/buildbot/... reproduces
 * Valve's debug map without creating that directory (which needs root).
 *
 *   BM_PATHMAP="/Users/buildbot/=/scratch/root/Users/buildbot/[;from=to...]"
 *
 * Every path argument of open, stat, lstat, access, fopen and opendir that
 * starts with a "from" prefix is rewritten to "to" + rest. Nothing else
 * changes: getcwd and realpath still report real paths.
 *
 * qsort: replaced by the OS X 10.9/10.10 implementation (qsort109.c), which
 * decides how ld64 orders sections of equal rank.
 */
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_MAPS 16

static struct { char from[PATH_MAX]; size_t len; char to[PATH_MAX]; } maps[MAX_MAPS];
static int nmaps = -1;

static void load_maps(void)
{
	nmaps = 0;
	const char *env = getenv("BM_PATHMAP");
	if (!env)
		return;
	char buf[MAX_MAPS * 2 * PATH_MAX];
	strlcpy(buf, env, sizeof(buf));
	char *save = NULL;
	for (char *item = strtok_r(buf, ";", &save); item && nmaps < MAX_MAPS; item = strtok_r(NULL, ";", &save)) {
		char *eq = strchr(item, '=');
		if (!eq)
			continue;
		*eq = '\0';
		strlcpy(maps[nmaps].from, item, PATH_MAX);
		strlcpy(maps[nmaps].to, eq + 1, PATH_MAX);
		maps[nmaps].len = strlen(item);
		nmaps++;
	}
}

static const char *map(const char *path, char *out)
{
	if (nmaps < 0)
		load_maps();
	if (!path)
		return path;
	for (int i = 0; i < nmaps; i++) {
		if (strncmp(path, maps[i].from, maps[i].len) == 0) {
			snprintf(out, PATH_MAX, "%s%s", maps[i].to, path + maps[i].len);
			return out;
		}
	}
	return path;
}

static int my_open(const char *path, int flags, ...)
{
	mode_t mode = 0;
	if (flags & O_CREAT) {
		va_list ap;
		va_start(ap, flags);
		mode = (mode_t)va_arg(ap, int);
		va_end(ap);
	}
	char b[PATH_MAX];
	return open(map(path, b), flags, mode);
}

static int my_stat(const char *path, struct stat *st)
{
	char b[PATH_MAX];
	return stat(map(path, b), st);
}

static int my_lstat(const char *path, struct stat *st)
{
	char b[PATH_MAX];
	return lstat(map(path, b), st);
}

static int my_access(const char *path, int mode)
{
	char b[PATH_MAX];
	return access(map(path, b), mode);
}

static FILE *my_fopen(const char *path, const char *mode)
{
	char b[PATH_MAX];
	return fopen(map(path, b), mode);
}

static DIR *my_opendir(const char *path)
{
	char b[PATH_MAX];
	return opendir(map(path, b));
}

#define INTERPOSE(fn) \
	__attribute__((used, section("__DATA,__interpose"))) \
	static const struct { const void *replacement, *original; } interpose_##fn = { (const void *)my_##fn, (const void *)fn }

void bm_qsort_109(void *a, size_t n, size_t es, int (*cmp)(const void *, const void *));
#define my_qsort bm_qsort_109

INTERPOSE(open);
INTERPOSE(stat);
INTERPOSE(lstat);
INTERPOSE(access);
INTERPOSE(fopen);
INTERPOSE(opendir);
INTERPOSE(qsort);
