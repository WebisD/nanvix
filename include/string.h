/*
 * Copyright(C) 2011-2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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

/**
 * @file
 * 
 * @brief String library.
 */

#ifndef STRING_H_
#define STRING_H_

	#include <sys/types.h>
	
#ifdef _CX
	#include <locale.h>
	#include <stddef.h>
#endif /* _CX */

	/**
	 * @defgroup stringlib String Library
	 * 
	 * @brief String operations.
	 * 
	 * @todo stpcpy()
	 * @todo stpncpy()
	 * @todo strcoll_l()
	 * @todo strerror_l()
	 * @todo strerror_r()
	 * @todo strndup()
	 * @todo strsignal()
	 * @todo strtok_r()
	 * @todo strxfrm_l()
	 */
	/**@{*/

#ifdef _XSI
	extern void *memccpy(void *, const void *, int, size_t);
#endif /* _XSI */

#ifdef _CX
	extern char *stpcpy(char *restrict, const char *restrict);
	extern char *stpncpy(char *restrict, const char *restrict, size_t);
	extern int strcoll_l(const char *, const char *, locale_t);
	extern char *strdup(const char *);
	extern char *strerror_l(int, locale_t);
	extern int strerror_r(int, char *, size_t);
	extern char *strndup(const char *, size_t);
	extern size_t strnlen(const char *, size_t);
	extern char *strsignal(int);
	extern char *strtok_r(char *restrict, const char *restrict,char **restrict);
	extern size_t strxfrm_l(char *restrict,const char *restrict,size_t,locale_t);
#endif /* _CX */

	extern void *memchr(const void *, int, size_t);
	extern int memcmp(const void *, const void *, size_t);
	extern void *memcpy (void *, const void *, size_t);
	extern void *memmove(void *, const void *, size_t);
	extern void *memset(void *, int, size_t);
	extern char *strcat(char *, const char *);
	extern char *strchr(const char *, int);
	extern int strcmp(const char *, const char *);
	extern int strcoll(const char *, const char *);
	extern char *strcpy(char *, const char *);
	extern size_t strcspn(const char *, const char *);
	extern char *strerror(int);
	extern size_t strlen(const char *);
	extern char *strncat(char *, const char *, size_t);
	extern int strncmp(const char *, const char *, size_t);
	extern char *strncpy(char *, const char *, size_t);
	extern char *strpbrk(const char *, const char *);
	extern char *strrchr(const char *, int);
	extern size_t strspn(const char *, const char *);
	extern char *strstr(const char *, const char *);
	extern char *strtok(char *, const char *);
	extern size_t strxfrm(char *, const char *, size_t);
	
	/**@}*/

#endif /* STRING_H_ */
