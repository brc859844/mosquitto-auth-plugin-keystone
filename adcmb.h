#ifndef __ADCMB_H__
#define __ADCMB_H__


#ifdef __cplusplus
	extern "C" {
#endif



typedef struct
{
    	size_t 			size;
    	size_t 			allocated;
    	void *  		block;
	char * 			buffer;
}
adc_MB_t;



extern adc_MB_t *		adc_MB_New(size_t);
extern void 			adc_MB_Free(adc_MB_t *);
extern void 			adc_MB_Init(adc_MB_t *, size_t);
extern void 			adc_MB_Clean(adc_MB_t *);
extern void 			adc_MB_Reuse(adc_MB_t *);
extern size_t 			adc_MB_Size(adc_MB_t *);
extern void *			adc_MB_Contents(adc_MB_t *);
extern void 			adc_MB_Resize(adc_MB_t *, size_t);
extern void 			adc_MB_Append(adc_MB_t *, void *, size_t);
extern char * 			adc_MB_ToString(adc_MB_t *);
extern void 			adc_MB_vsprintf(adc_MB_t *, const char *, ...);
extern void 			adc_MB_SetBuf(adc_MB_t *, size_t);
extern void 			adc_MB_memcpy(adc_MB_t *, size_t, void *, size_t);


#ifdef __cplusplus
 	}
#endif
#endif		/* __ADCMB_H__ */

