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

#ifndef __ADCMB_H__
#define __ADCMB_H__


#ifdef __cplusplus
extern "C" {
#endif



    typedef struct {
	size_t size;
	size_t allocated;
	void *block;
	char *buffer;
    } adc_MB_t;



    extern adc_MB_t *adc_MB_New(size_t);
    extern void adc_MB_Free(adc_MB_t *);
    extern void adc_MB_Init(adc_MB_t *, size_t);
    extern void adc_MB_Clean(adc_MB_t *);
    extern void adc_MB_Reuse(adc_MB_t *);
    extern size_t adc_MB_Size(adc_MB_t *);
    extern void *adc_MB_Contents(adc_MB_t *);
    extern void adc_MB_Resize(adc_MB_t *, size_t);
    extern void adc_MB_Append(adc_MB_t *, void *, size_t);
    extern char *adc_MB_ToString(adc_MB_t *);
    extern void adc_MB_vsprintf(adc_MB_t *, const char *, ...);
    extern void adc_MB_SetBuf(adc_MB_t *, size_t);
    extern void adc_MB_memcpy(adc_MB_t *, size_t, void *, size_t);


#ifdef __cplusplus
}
#endif
#endif				/* __ADCMB_H__ */
