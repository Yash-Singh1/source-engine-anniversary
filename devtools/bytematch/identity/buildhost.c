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
#include <time.h>
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

/*
 * ld64-236.3 compact_unwind.cpp aligns its second-level page pointer with
 * ((uintptr_t)pageEnd & -4096) on a calloc(pageCount, 4096) buffer. On OS X 10.9,
 * allocations under 127 KB came from libmalloc's 512-byte small zone rather than
 * a 4096-byte page boundary, shifting the gap before the last page by base % 4096.
 * When BM_UNWIND_CALLOC_OFFSET is set, offset calloc(n, 4096) by that amount.
 */
static void *unwind_orig = NULL;
static void *unwind_shifted = NULL;
static size_t unwind_off = (size_t)-1;

static void *my_calloc(size_t count, size_t size)
{
	if (unwind_off == (size_t)-1) {
		const char *env = getenv("BM_UNWIND_CALLOC_OFFSET");
		unwind_off = env ? (size_t)strtoul(env, NULL, 0) : 0;
	}
	if (unwind_off && size == 4096 && count > 1 && unwind_orig == NULL) {
		void *p = calloc(count + 1, 4096);
		if (!p)
			return NULL;
		if (((uintptr_t)p & 4095) == 0) {
			unwind_orig = p;
			unwind_shifted = (char *)p + unwind_off;
			return unwind_shifted;
		}
		return p;
	}
	return calloc(count, size);
}

static void my_free(void *ptr)
{
	if (ptr && ptr == unwind_shifted) {
		ptr = unwind_orig;
		unwind_orig = NULL;
		unwind_shifted = NULL;
	}
	free(ptr);
}

static time_t my_time(time_t *tloc)
{
	const char *env = getenv("BM_BUILD_TIME");
	if (env && *env) {
		time_t t = (time_t)strtoll(env, NULL, 0);
		if (tloc)
			*tloc = t;
		return t;
	}
	return time(tloc);
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
INTERPOSE(calloc);
INTERPOSE(free);
INTERPOSE(time);
