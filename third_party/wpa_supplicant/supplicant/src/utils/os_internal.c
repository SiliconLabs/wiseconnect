/*
 * wpa_supplicant/hostapd / Internal implementation of OS specific functions
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This file is an example of operating system specific  wrapper functions.
 * This version implements many of the functions internally, so it can be used
 * to fill in missing functions from the target system C libraries.
 *
 * Some of the functions are using standard C library calls in order to keep
 * this file in working condition to allow the functions to be tested on a
 * Linux target. Please note that OS_NO_C_LIB_DEFINES needs to be defined for
 * this file to work correctly. Note that these implementations are only
 * examples and are not optimized for speed.
 */

#include "includes.h"
#include "common.h"
#include "supplicant_mgmt_if.h"

void * _os_zalloc(size_t size);
void * os_memdup(const void *src, size_t len);

//extern uint32 pac_file_read_write_loc;
uint32 pac_file_read_write_loc; //TODO: Temporarily defined here. pac file read write loc is not yet known.
extern void erase_flash_memory(uint32 erase_start_addr, uint32 erase_len);
extern void write_on_flash_memory(uint8 *dst, uint8 *src, uint32 len);

int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const uint8 *aa = a;
	const uint8 *bb = b;
	size_t i;
	uint8 res;

	for (res = 0, i = 0; i < len; i++)
		res |= aa[i] ^ bb[i];

	return res;
}

void os_sleep(os_time_t sec, os_time_t usec)
{
#ifdef __LINUX__
	if (sec)
	{
	sleep(sec);
	}	
	if (usec)
	{
	usleep(usec);
	}
#else
	uint64 timeout_ms = 0;
	timeout_ms = sec * 1000 + usec / 1000;

	// Wait until the specified timeout
	OS_DELAY(timeout_ms);
#endif
}

#ifdef __LINUX__
int os_get_time(struct os_time *t)
{
	int res;
	struct timeval tv;
	res = gettimeofday(&tv, NULL);
	t->sec = tv.tv_sec;
	t->usec = tv.tv_usec;
	return res;
}
#else
int os_get_time(struct os_time *t) 
{
	static uint32_t last_tick_count = 0;
	static uint64_t total_ticks = 0;

    uint32_t current_tick_count = osKernelGetTickCount();

    // Detect tick count wrap-around
	// Assuming that the tick count wraps around every 49.7 days.
    if (current_tick_count < last_tick_count) {
        total_ticks += (0xFFFFFFFF - last_tick_count) + current_tick_count + 1;
    } else {
        total_ticks += current_tick_count - last_tick_count;
    }

    last_tick_count = current_tick_count;

    // Convert milliseconds to seconds and microseconds
    t->sec = total_ticks / 1000;
    t->usec = (total_ticks % 1000) * 1000;

    return 0;
}
#endif 

#ifdef __LINUX__
int os_get_reltime(struct os_reltime *t)
{
	int res;
	struct timeval tv;
	res = gettimeofday(&tv, NULL);
	t->sec = tv.tv_sec;
	t->usec = tv.tv_usec;
	return res;
}
#endif

void os_daemonize_terminate(const char *pid_file)
{
#if DISABLED_IN_SUPP_20
	if (pid_file)
		unlink(pid_file);
#endif
}

int os_get_random(unsigned char *buf, size_t len)
{
#if __LINUX__
	FILE *f;
	size_t rc;

	f = fopen("/dev/urandom", "rb");
	if (f == NULL) {
		printf("Could not open /dev/urandom.\n");
		return -1;
	}

	rc = fread(buf, 1, len, f);
	fclose(f);

	return rc != len ? -1 : 0;
#else
	if (buf == NULL) {
		return -1;
	}

	return ((crypto_get_random(buf, len) != 0)? -1 : 0);
#endif
}

unsigned long os_random(void)
{
#ifdef __LINUX__
	return random();
#else
	uint32_t random_value = 0;
	if (crypto_get_random((unsigned char *)&random_value, sizeof(random_value)) != 0) {
		SL_MGMT_ASSERT(0); // Assert in case of failure
	}

	return random_value;
#endif
}

char * os_rel2abs_path(const char *rel_path)
{
#if DISABLED_IN_SUPP_20
	char *buf = NULL, *cwd, *ret;
	size_t len = 128, cwd_len, rel_len, ret_len;

	if (rel_path[0] == '/')
		return os_strdup(rel_path);

	for (;;) {
		buf = os_malloc(len);
		if (buf == NULL)
			return NULL;
		cwd = getcwd(buf, len);
		if (cwd == NULL) {
			os_free(buf);
			if (errno != ERANGE) {
				return NULL;
			}
			len *= 2;
		} else {
			break;
		}
	}

	cwd_len = os_strlen(cwd);
	rel_len = os_strlen(rel_path);
	ret_len = cwd_len + 1 + rel_len + 1;
	ret = os_malloc(ret_len);
	if (ret) {
		os_memcpy(ret, cwd, cwd_len);
		ret[cwd_len] = '/';
		os_memcpy(ret + cwd_len + 1, rel_path, rel_len);
		ret[ret_len - 1] = '\0';
	}
	os_free(buf);
	return ret;
#else
	return NULL;
#endif
}

int os_program_init(void)
{
	return 0;
}

void os_program_deinit(void)
{
}

int os_setenv(const char *name, const char *value, int overwrite)
{
#if DISABLED_IN_SUPP_20
	return setenv(name, value, overwrite);
#else
    return 0;
#endif
}

int os_unsetenv(const char *name)
{
#if DISABLED_IN_SUPP_20
#if defined(__FreeBSD__) || defined(__NetBSD__)
	unsetenv(name);
	return 0;
#else
	return unsetenv(name);
#endif
#else
    return 0;
#endif
}

FILE * os_fopen(const char *pac_file, const char *rw)
{
	if((os_strncmp(rw,"rb",2) == 0) && (*(uint32 *)pac_file_read_write_loc == 0xFFFFFFFF))
	{
		return NULL; /*File read fail: Invalid data*/	
	}
	pac_file_read_write_loc = cpf_get_eap_pac_file_addr(); /*Initialize it with the initial location in flash mem*/
	return (FILE*)1; /*Return success*/
}

#ifdef DEBUG_CODE_ENABLE_SUPPLICANT
uint32 dbg_check_point = 0;
#endif
char * os_readfile(const char *name, size_t *ret_len)
{
#if DISABLED_IN_SUPP_20
	FILE *f;
	char *buf;

	f = fopen(name, "rb");
	if (f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	*len = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = os_malloc(*len);
	if (buf == NULL) {
		fclose(f);
		return NULL;
	}

	if (fread(buf, 1, *len, f) != *len) {
		fclose(f);
		os_free(buf);
		return NULL;
	}

	fclose(f);

	return buf;
#else
	char *buf = NULL;
	uint32 len = 0;
	uint32 write_addr;

	if (strcmp("wifiuser.pem", name) == 0)
	{
		write_addr = (uint32 )cpf_get_cert_flash_addr();
		len = (uint32)(*(uint32 *)write_addr);
		if ( (*(uint32 *)write_addr != 0xFFFFFFFF) && (len <= cpf_get_max_eap_cert_len()))
    {
#ifdef ENABLE_UMAC_ROM_PTR
      buf = OS_MALLOC(len + 1);
#else
      buf = os_malloc(len + 1);
#endif
      if (buf)
      {
        write_addr = write_addr + 8;
        if (buf)
          memcpy((char*)buf, (uint8 *)write_addr, len);
        buf[len] = '\0';
        len++;
      } else {
        SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_BUF_NOT_ALLOCATED_IN_OS_READ_FILE);
      }
#ifdef DEBUG_CODE_ENABLE_SUPPLICANT
      dbg_check_point++;
#endif
    }
    else
    {
    }
  }

  *ret_len = len;
  if (!buf){
        SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_BUF_NOT_ALLOCATED_IN_OS_READ_FILE_1);
  }
	return buf;
#endif
}

char * os_fgets(char *buf , int len, FILE * f)
{

	uint16 read_len = 0;
	uint8 *readline = (uint8 *)(pac_file_read_write_loc);	
	uint16 ii = 0;
	if ((pac_file_read_write_loc >= ((uint32)cpf_get_eap_pac_file_addr() + *(uint32 *)cpf_get_eap_pac_file_len_addr())) || (readline[ii] == 0) )
	{
        wpa_printf(MSG_DEBUG, "EAP-FAST: readfile fail '%d' fail",pac_file_read_write_loc);    
		return NULL;
	}
	while ((readline[ii] != '\n') && (read_len < (len - 1)) && (readline[ii] != 0) &&
		(pac_file_read_write_loc < ((uint32)cpf_get_eap_pac_file_addr() + *(uint32 *)cpf_get_eap_pac_file_len_addr()))) 
	{
		buf[read_len++] = readline[ii];
		ii++;
		pac_file_read_write_loc++;
	}
	buf[read_len] = '\0';
	if (readline[ii] == '\n')	{
		pac_file_read_write_loc++;
	}
	return buf;
}

#if UNUSED_FEAT_IN_SUPP_29
int os_fdatasync(FILE *stream)
{
	return 0;
}
#endif

int os_fclose(FILE *v) 
{
	pac_file_read_write_loc = cpf_get_eap_pac_file_len_addr(); /*Reset it with the initial location in flash mem*/
	return 0;
}

size_t  os_fwrite(const char *buf, size_t num_rec, size_t len, FILE * f) 
{
	char *tmp_buf;
	if (len > cpf_get_max_pac_file_len()) {
		return 0;
	}
#ifdef ENABLE_UMAC_ROM_PTR
      tmp_buf = OS_MALLOC(len + 8 + 1);
#else
      tmp_buf = os_malloc(len + 8 + 1);;
#endif
	if(tmp_buf == NULL)
		return 0;
	*(uint32*)&tmp_buf[0] = len;	
	*(uint32*)&tmp_buf[4] = (uint32)cpf_get_eap_pac_file_addr();	
	os_memcpy((tmp_buf + 8), buf, len);
	tmp_buf[len + 8] = 0;


	/*erase the location of PAC*/
	erase_flash_memory(cpf_get_eap_pac_file_len_addr(), 4096);
	
	/* Perform the write operation */
	write_on_flash_memory((uint8*)cpf_get_eap_pac_file_len_addr(), (uint8 *)tmp_buf, len + 8 + 1);
	os_free(tmp_buf);
	return len;
}

#if !K60_PORTING
void * os_malloc(size_t size)
{
	return malloc(size);
}

void os_free(void *ptr)
{
	free(ptr);
}

void * os_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}
#endif

void * _os_zalloc(size_t size)
{
	void *n = os_malloc(size);
	if (n)
		os_memset(n, 0, size);
	return n;
}

#if !K60_PORTING
void * os_malloc(size_t size)
{
	return malloc(size);
}

void * os_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

void os_free(void *ptr)
{
	free(ptr);
}
#endif

void * os_memcpy(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;
	while (n--)
		*d++ = *s++;
	return dest;
}


void * os_memmove(void *dest, const void *src, size_t n)
{
	if (dest < src)
		os_memcpy(dest, src, n);
	else {
		/* overlapping areas */
		char *d = (char *) dest + n;
		const char *s = (const char *) src + n;
		while (n--)
			*--d = *--s;
	}
	return dest;
}


void * os_memset(void *s, int c, size_t n)
{
  if((c == 0) || (c == '\0'))
  {
    sl_memset(s, 0, n);
  }
  else
  {
	char *p = s;
	while (n--)
		*p++ = c;
  }
	return s;
}


int os_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *p1 = s1, *p2 = s2;

	if (n == 0)
		return 0;

	while (*p1 == *p2) {
		p1++;
		p2++;
		n--;
		if (n == 0)
			return 0;
	}

	return *p1 - *p2;
}


char * os_strdup(const char *s)
{
	char *res;
	size_t len;
	if (s == NULL)
		return NULL;
	len = os_strlen(s);
	res = os_malloc(len + 1);
	if (res)
		os_memcpy(res, s, len + 1);
	return res;
}


size_t os_strlen(const char *s)
{
	const char *p = s;
	while (*p)
		p++;
	return p - s;
}


int os_strcasecmp(const char *s1, const char *s2)
{
	/*
	 * Ignoring case is not required for main functionality, so just use
	 * the case sensitive version of the function.
	 */
	return os_strcmp(s1, s2);
}


int os_strncasecmp(const char *s1, const char *s2, size_t n)
{
	/*
	 * Ignoring case is not required for main functionality, so just use
	 * the case sensitive version of the function.
	 */
	return os_strncmp(s1, s2, n);
}


char * os_strchr(const char *s, int c)
{
	while (*s) {
		if (*s == c)
			return (char *) s;
		s++;
	}
	return NULL;
}


char * os_strrchr(const char *s, int c)
{
	const char *p = s;
	while (*p)
		p++;
	p--;
	while (p >= s) {
		if (*p == c)
			return (char *) p;
		p--;
	}
	return NULL;
}


int os_strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2) {
		if (*s1 == '\0')
			break;
		s1++;
		s2++;
	}

	return *s1 - *s2;
}


int os_strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;

	while (*s1 == *s2) {
		if (*s1 == '\0')
			break;
		s1++;
		s2++;
		n--;
		if (n == 0)
			return 0;
	}

	return *s1 - *s2;
}


#if UNUSED_FEAT_IN_SUPP_29
char * os_strncpy(char *dest, const char *src, size_t n)
{
	char *d = dest;
	while (n--) {
		*d = *src;
		if (*src == '\0')
			break;
		d++;
		src++;
	}
	return dest;
}
#endif
size_t os_strlcpy(char *dest, const char *src, size_t siz)
{
	const char *s = src;
	size_t left = siz;

	if (left) {
		/* Copy string up to the maximum size of the dest buffer */
		while (--left != 0) {
			if ((*dest++ = *s++) == '\0')
				break;
		}
	}

	if (left == 0) {
		/* Not enough room for the string; force NUL-termination */
		if (siz != 0)
			*dest = '\0';
		while (*s++)
			; /* determine total src string length */
	}

	return s - src - 1;
}





char * os_strstr(const char *haystack, const char *needle)
{
	size_t len = os_strlen(needle);
	while (*haystack) {
		if (os_strncmp(haystack, needle, len) == 0)
			return (char *) haystack;
		haystack++;
	}

	return NULL;
}


//#endif
#ifdef SUPPLICANT_NON_ROM
int os_snprintf(char *str, size_t size, const char *format, ...)
{
#if 1 //TODO_PORTING
	va_list ap;
	int ret;

	/* See http://www.ijs.si/software/snprintf/ for portable
	 * implementation of snprintf.
	 */

	va_start(ap, format);
	ret = vsnprintf(str, size, format, ap);
	va_end(ap);
	if (size > 0)
		str[size - 1] = '\0';
	return ret;
#endif
}

#define ITOA_8(v)  ( (v) + '0')
u16 os_itoa (u32 temp_dig, u8 *dst, u16 dst_len)
{  
   s16 ii, jj;
   u8  t_buf[10];

   if (temp_dig == 0) 
   {
       dst[0] = ITOA_8(0);
	   dst[1] = '\0';
	   return 1;
   }
   for (ii = 0; (temp_dig != 0) && (ii < 10); ii++)
   { 
      t_buf[ii] = ITOA_8((u8)(temp_dig % 10));
      temp_dig = temp_dig/10;
   }
   ii--;
   for (jj = 0; (ii >=0) && (dst_len--); ii--, jj++) 
   {
	  dst[jj] = t_buf[ii];
   }
   dst[jj] = '\0';

   return jj;
}

static uint8 ip_str[20];
char * inet_ntoa(struct in_addr addr)
{
	   uint16_t jj = 0;  
       //! check if this movement is required or  not
	   jj += os_itoa((uint8)(addr.s_addr >> 24), &ip_str[jj], 3);
       ip_str[jj] = '.';
	   jj += os_itoa((uint8)(addr.s_addr >> 16), &ip_str[jj], 3);
       ip_str[jj] = '.';
	   jj += os_itoa((uint8)(addr.s_addr >> 8), &ip_str[jj], 3);
       ip_str[jj] = '.';
	   jj += os_itoa((uint8)(addr.s_addr), &ip_str[jj], 3);
     return (char *)ip_str;
}
#endif

#if TODO_PORTING
int os_exec(const char *program, const char *arg, int wait_completion)
{
	pid_t pid;
	int pid_status;

	pid = fork();
	if (pid < 0) {
		wpa_printf(MSG_ERROR, "fork: %s", strerror(errno));
		return -1;
	}

	if (pid == 0) {
		/* run the external command in the child process */
		const int MAX_ARG = 30;
		char *_program, *_arg, *pos;
		char *argv[MAX_ARG + 1];
		int i;

		_program = os_strdup(program);
		_arg = os_strdup(arg);

		argv[0] = _program;

		i = 1;
		pos = _arg;
		while (i < MAX_ARG && pos && *pos) {
			while (*pos == ' ')
				pos++;
			if (*pos == '\0')
				break;
			argv[i++] = pos;
			pos = os_strchr(pos, ' ');
			if (pos)
				*pos++ = '\0';
		}
		argv[i] = NULL;

		execv(program, argv);
		wpa_printf(MSG_ERROR, "execv: %s", strerror(errno));
		os_free(_program);
		os_free(_arg);
		exit(0);
		return -1;
	}

	if (wait_completion) {
		/* wait for the child process to complete in the parent */
		waitpid(pid, &pid_status, 0);
	}

	return 0;
}
#endif

