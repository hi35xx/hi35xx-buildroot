/******************************************************************************
 *    COPYRIGHT (C) 2013 hisi. Hisilicon
 *    All rights reserved.
 *
******************************************************************************/

#ifndef LIBCH
#define LIBCH
/******************************************************************************/

#define NULL ((void *)0)

void *memcpy(void *dst, const void *src, unsigned int len);

void *memset(void *b, int c, unsigned int len);

int memcmp(const void *b1, const void *b2, unsigned int len);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, unsigned int len);

void strcpy(char *dst, const char *src);

void strcat(char *dst, const char *src);

char *strchr(const char *s, char ch);

unsigned int strlen(const char *s);

/******************************************************************************/
#endif /* LIBCH */





