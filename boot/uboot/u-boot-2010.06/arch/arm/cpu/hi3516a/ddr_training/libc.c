/******************************************************************************
 *    COPYRIGHT (C) 2013 hisi. Hisilicon
 *    All rights reserved.
 *
******************************************************************************/

#include "libc.h"

void *memcpy(void *dst, const void *src, unsigned int len)
{
	const char *s = src;
	char *d = dst;

	while (len--)
		*d++ = *s++;
	return dst;
}

void *memset(void *b, int c, unsigned int len)
{
	char *bp = b;

	while (len--)
		*bp++ = (unsigned char)c;
	return b;
}

int memcmp(const void *b1, const void *b2, unsigned int len)
{
	const unsigned char *p1, *p2;
	unsigned char *tmp;

	for (p1 = b1, p2 = b2; len > 0; len--, p1++, p2++) {
		if (*p1 != *p2) {
			*tmp = (*p1) - (*p2);
			return *tmp;
		}
	}
	return 0;
}

int strcmp(const char *s1, const char *s2)
{
	int tmp = 0;
	for (; *s1 == *s2 && *s1 != '\0'; s1++, s2++)
		;
	tmp = (unsigned char)*s1 - (unsigned char)*s2;
	return tmp;
}

int strncmp(const char *s1, const char *s2, unsigned int len)
{
	int tmp = 0;

	for (; *s1 == *s2 && *s1 != '\0' && len > 0; len--, s1++, s2++)
		;
	tmp = (unsigned char)*s1 - (unsigned char)*s2;
	return tmp;
}

void strcpy(char *dst, const char *src)
{

	while (*src != '\0')
		*dst++ = *src++;
	*dst = '\0';
}

void strcat(char *dst, const char *src)
{

	while (*dst != '\0')
		dst++;
	while (*src != '\0')
		*dst++ = *src++;
	*dst = '\0';
}

char *strchr(const char *s, char ch)
{
	for (; *s != '\0'; s++) {
		if (*s == ch)
			return (char *)s;
	}
	return NULL;
}

unsigned int strlen(const char *s)
{
	unsigned int len = 0;

	while (*s++ != '\0')
		len++;
	return len;
}

