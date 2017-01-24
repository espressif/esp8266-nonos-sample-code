#include <stdio.h>
//#include "os.h"
#include <fcntl.h>

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

#define ICACHE_FLASH_ATTR __attribute__((section(".irom0.text")))

int ICACHE_FLASH_ATTR __attribute__((weak))
_open_r(struct _reent *r, const char *filename, int flags, int mode)
{
    return 0;
}

_ssize_t ICACHE_FLASH_ATTR __attribute__((weak))
_read_r(struct _reent *r, int fd, void *buf, size_t cnt)
{
	return -1;
}

_ssize_t ICACHE_FLASH_ATTR __attribute__((weak))
_write_r(struct _reent *r, int fd, void *buf, size_t cnt)
{
	return -1;
}

_off_t ICACHE_FLASH_ATTR __attribute__((weak))
_lseek_r(struct _reent *r, int fd, _off_t pos, int whence)
{
	return -1;
}

int ICACHE_FLASH_ATTR __attribute__((weak))
_close_r(struct _reent *r, int fd)
{
	return -1;
}

int ICACHE_FLASH_ATTR __attribute__((weak))
_rename_r(struct _reent *r, const char *from, const char *to)
{
    return 0;
}

int ICACHE_FLASH_ATTR __attribute__((weak))
_unlink_r(struct _reent *r, const char *filename)
{
    return 0;
}

int ICACHE_FLASH_ATTR __attribute__((weak))
_fstat_r(struct _reent *r, int fd, struct stat *s)
{
    return 0;
}

void *ICACHE_FLASH_ATTR __attribute__((weak))
_sbrk_r(void *ptr, int incr)
{
	return 0;
}

char *sys_reverse(char *s)
{
	char temp = 0;
	char *p = s;
	char *q = s;
	while(*q)
		++q;
	q--;
	while(q > p){
		temp = *p;
		*p++ = *q;
		*q-- = temp;
	}
	return s;
}

char *sys_itoa(int n)
{
	int i = 0, Negative = 0;
	static char s[32];
	Negative = n;
	if (Negative < 0){
		n = -n;
	}
	do {
		s[i++] = n%10 + '0';
		n = n / 10;
	}while(n > 0);

	if (Negative < 0){
		s[i++] = '-';
	}
	s[i] = '\0';
	return sys_reverse(s);
}
