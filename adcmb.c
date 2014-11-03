/*
Copyright (c) 2014, Brett Cameron 

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "adcmb.h"

#define MIN_BLOCK_SIZE 	0x0000010
#define MAX_BLOCK_SIZE 	0x8000000

#define MAX_TMP_BUF 512


#if defined __GNUC__
#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif


#define adc_MB_Assert(x) \
    do { \
        if (unlikely (!(x))) { \
            fprintf (stderr, "Assertion failed: %s (%s: %d)\n", #x, __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)


#define adc_MB_AllocAssert(x) \
    do { \
        if (unlikely (!(x))) { \
            fprintf (stderr, "FATAL ERROR: OUT OF MEMORY (%s: %d)\n", __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)



void adc_MB_SetBuf(adc_MB_t * mb, size_t len)
{

    adc_MB_Assert(mb);

    if (mb->buffer != NULL) {
	free(mb->buffer);
    }

    adc_MB_AllocAssert((mb->buffer = (char *) malloc(len * sizeof(char))));
}


adc_MB_t *adc_MB_New(size_t size)
{
    adc_MB_t *mb;

    adc_MB_AllocAssert((mb = (adc_MB_t *) calloc(1, sizeof(adc_MB_t))));
    adc_MB_Init(mb, size);

    return (mb);
}


void adc_MB_Free(adc_MB_t * mb)
{
    if (mb == NULL) {
	return;
    }

    adc_MB_Clean(mb);

    if (mb->buffer != NULL) {
	free(mb->buffer);
    }

    free(mb);
}


void adc_MB_Init(adc_MB_t * mb, size_t size)
{

    adc_MB_Assert(mb);

    if (size < MIN_BLOCK_SIZE) {
	mb->allocated = MIN_BLOCK_SIZE + 1;
    } else {
	mb->allocated = size + 1;
    }

    mb->size = size;

    adc_MB_AllocAssert((mb->block = (void *) calloc(1, mb->allocated)));
    adc_MB_SetBuf(mb, MAX_TMP_BUF);
}


void adc_MB_Clean(adc_MB_t * mb)
{
    if (mb == NULL || mb->block == NULL) {
	return;
    }

    free(mb->block);
    mb->block = NULL;

    mb->size = 0;
    mb->allocated = 0;
}


void adc_MB_Reuse(adc_MB_t * mb)
{

    if (mb == NULL || mb->block == NULL) {
	return;
    }

    mb->size = 0;
}


size_t adc_MB_Size(adc_MB_t * mb)
{
    adc_MB_Assert(mb);
    return (mb->size);
}


char *adc_MB_ToString(adc_MB_t * mb)
{

    char *tmp;

    if (mb == NULL || mb->block == NULL) {
	return (NULL);
    }

    /* Note that this is safe because we always add an extra byte */
    tmp = (char *) mb->block;
    tmp[mb->size] = '\0';

    return (tmp);

}


void *adc_MB_Contents(adc_MB_t * mb)
{

    return (mb == NULL ? NULL : mb->block);

}


void adc_MB_Resize(adc_MB_t * mb, size_t size)
{

    void *tmp = NULL;
    int bytes;

    adc_MB_Assert(mb);

    if (size <= mb->allocated) {
	mb->size = size;
	return;
    }

    if (mb->allocated == 0) {
	bytes = 1;
    } else {
	bytes = mb->allocated;
    }

    while (bytes < size && bytes <= MAX_BLOCK_SIZE) {
	bytes *= 2;
    }

    /* Add an extra byte just for good measure */
    bytes++;
    adc_MB_Assert(bytes <= MAX_BLOCK_SIZE);

    adc_MB_AllocAssert((tmp = (void *) malloc(bytes)));
    memset(tmp, '\0', bytes);
    memcpy(tmp, mb->block, mb->size);

    free(mb->block);

    mb->block = tmp;
    mb->size = size;
    mb->allocated = bytes;
}


void adc_MB_Append(adc_MB_t * mb, void *data, size_t len)
{

    int size;

    adc_MB_Assert(mb);

    size = mb->size;
    adc_MB_Resize(mb, (size + len));
    memcpy(((unsigned char *) mb->block) + size, data, len);
}


void adc_MB_memcpy(adc_MB_t * mb, size_t offset, void *data, size_t len)
{
    /* Must fit within existing space */
    adc_MB_Assert((offset + len) < mb->allocated);

    memcpy(((unsigned char *) mb->block + offset), data, len);
}


void adc_MB_vsprintf(adc_MB_t * mb, const char *fmt, ...)
{

    va_list args;

    int n;

    va_start(args, fmt);
    n = vsprintf(mb->buffer, fmt, args);
    va_end(args);

    adc_MB_Append(mb, mb->buffer, n);
}
