/*************************************************************************
 *   Cboy, a Game Boy emulator
 *   Copyright (C) 2012 jkbenaim
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#ifndef _LIBC_H_
#define _LIBC_H_

#include <stddef.h>
#include <stdint.h>

static inline uint32_t be16toh(uint32_t big_endian_16bits) {
  return big_endian_16bits;
}

static inline uint32_t be32toh(uint32_t big_endian_32bits) {
  return big_endian_32bits;
}

static inline void *memcpy(void *dest, const void *src, size_t n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;
 
  for (int i=0; i<n; i++)
    cdest[i] = csrc[i];

  return dest;
}

static inline char *strcat(char *dest, const char *src)
{
  char *ret = dest;
  while (*dest)
    dest++;
  while ((*dest++ = *src++))
    ;
  return ret;
}

static inline int strcmp(const char* s1, const char* s2) {
  while(*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }

  return *(const unsigned char*) s1 - *(const unsigned char*)s2;
}

static inline size_t strlen(const char *s) {
  size_t i;
  for (i = 0; s[i] != '\0'; i++) ;
  return i;
}

static inline char *strncpy(char *dest, const char *src, size_t n)
{
  char *ret = dest;
  do {
    if (!n--)
      return ret;
  } while ((*dest++ = *src++));
  while (n--)
    *dest++ = 0;
  return ret;
}

static inline char *strrchr(const char *s, int c) {
  char* ret=0;
  do {
    if( *s == (char)c )
      ret=(char*)s;
  } while(*s++);
  return ret;
}

static inline void exit(int statuscode) {
  while (1);
  __builtin_unreachable();
}

static inline int rand(void) {
  return 0;
}

#endif

