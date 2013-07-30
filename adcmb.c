/* ###################################################################################################################### */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "adcmb.h"



#define MIN_BLOCK_SIZE 	0x0000010
#define MAX_BLOCK_SIZE 	0x8000000


static char *buffer = NULL;


#define MAX_LEN 128
#define MAX_BUF 512


static char errstr[MAX_LEN];






const char *adc_MB_ErrorString()

{

	return (errstr);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_SetBuf(size_t len)

{

	if (buffer != NULL)
	{
	   free(buffer); buffer = NULL;
	}

	buffer = (char *) malloc(len);

	if (! buffer)
	{
	   sprintf(errstr, "malloc(): %s [%s, %d]", strerror(errno), __FILE__, __LINE__);

	   return (-1);
	}


	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

adc_MB_t *adc_MB_New(size_t size)

{

	adc_MB_t *		block = NULL;


	errstr[0] = '\0';


	if (buffer == NULL)
	{
	   if (adc_MB_SetBuf(MAX_BUF) == -1)
	   {
	      return (NULL);
      	   }

	}


    	block = (adc_MB_t *) malloc(sizeof(adc_MB_t));

    	if (! block)
	{
	   sprintf(errstr, "malloc(): %s [%s, %d]", strerror(errno), __FILE__, __LINE__);

	   return (NULL);
	}


    	if (adc_MB_Init(block, size) != 0)
	{
	   if (block)
	   {
	      free(block); block = NULL;
	   }

    	}


	return (block);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_Free(adc_MB_t *block)

{

	errstr[0] = '\0';


   	if (block == NULL)
	{
	   return (0);
	}


    	adc_MB_Clean(block);

    	free(block);


	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_Init(adc_MB_t *block, size_t size)

{


	errstr[0] = '\0';


	if (block == NULL)
	{
	   sprintf(errstr, "Invalid input parameter [%s, %d]", __FILE__, __LINE__);

	   return (-1);
	}


    	block->size = size;


	if (size < MIN_BLOCK_SIZE)
	{
	   block->allocated = MIN_BLOCK_SIZE + 1;
	}
    	else
	{
	   block->allocated = size + 1;
	}


    	if ((block->block = (void *) malloc(block->allocated)) == NULL)
	{
	   sprintf(errstr, "malloc(): %s [%s, %d]", strerror(errno), __FILE__, __LINE__);

	   return (-1);
	}


	/* Initialize the memory */

	memset(block->block, '\0', block->allocated);


	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_Clean(adc_MB_t *block)

{

	errstr[0] = '\0';


	if (block == NULL || block->block == NULL)
	{
	   return (0);
	}


	free(block->block);


    	block->size = 0; block->allocated = 0;

    	block->block = NULL;


	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_Reuse(adc_MB_t *block)

{

	errstr[0] = '\0';


	if (block == NULL || block->block == NULL)
	{
	   return (0);
	}


    	block->size = 0;

	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

size_t adc_MB_Size(adc_MB_t *block)

{

	errstr[0] = '\0';


	if (block == NULL)
	{
	   sprintf(errstr, "Invalid input parameter [%s, %d]", __FILE__, __LINE__);

	   return ((size_t) -1);
	}


    	return (block->size);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

char *adc_MB_ToString(adc_MB_t *block)

{

	char * 		tmp;


	errstr[0] = '\0';


	if (block == NULL || block->block == NULL)
	{
	   return (NULL);
	}


	/* Note that this is safe because we always add an extra byte */

	tmp = (char *) block->block;

	tmp[block->size] = '\0';


    	return (tmp);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

void *adc_MB_Contents(adc_MB_t *block)

{

	errstr[0] = '\0';

	return (block == NULL ? NULL : block->block);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_Resize(adc_MB_t *block, size_t size)

{

    	void * 		tmp = NULL;

    	int 		bytes;



	errstr[0] = '\0';


	if (block == NULL)
	{
	   sprintf(errstr, "Invalid input parameter [%s, %d]", __FILE__, __LINE__);

	   return (-1);
	}


	if (size <= block->allocated)
	{
	   block->size = size;

	   return (0);
    	}


	if (block->allocated == 0)
	{
	   bytes = 1;
	}
	else
	{
	   bytes = block->allocated;
	}


    	while (bytes < size && bytes <= MAX_BLOCK_SIZE)
	{
	   bytes *= 2;
	}


	/* Add an extra byte just for good measure */

	bytes++;


    	if (bytes > MAX_BLOCK_SIZE)
	{
	   sprintf(errstr, "Maximum permitted block size exceeded (%d, %d)", bytes, MAX_BLOCK_SIZE);

	   return (-1);
	}


    	tmp = (void *) malloc(bytes);

    	if (! tmp)
	{
	   sprintf(errstr, "malloc(): %s [%s, %d]", strerror(errno), __FILE__, __LINE__);

	   return (-1);
	}


	memset(tmp, '\0', bytes);


    	memcpy(tmp, block->block, block->size);

	free(block->block);


    	block->block = tmp; block->size = size; block->allocated = bytes;


	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_Append(adc_MB_t *block, void *data, size_t len)

{

	int 		size;


	errstr[0] = '\0';


	if (block == NULL)
	{
	   sprintf(errstr, "Invalid input parameter [%s, %d]", __FILE__, __LINE__);

	   return (-1);
	}


    	size = block->size;


    	if (adc_MB_Resize(block, size + len) != 0)
	{
	   return (-1);
	}


    	memcpy(((unsigned char *) block->block) + size, data, len);


	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_memcpy(adc_MB_t *mb, size_t offset, void *data, size_t len)

{

	errstr[0] = '\0';


	if ((offset + len) >= mb->allocated)
	{
	   sprintf(errstr, "Insufficient space in buffer for requested operation");

	   return (-1);
	}


	memcpy(((unsigned char *) mb->block + offset), data, len);

	return (0);

}

/* ---------------------------------------------------------------------------------------------------------------------- */

int adc_MB_vsprintf(adc_MB_t *mb, const char *fmt, ...)

{

	va_list 		args;

	int 			n;


	va_start(args, fmt);

	n = vsprintf(buffer, fmt, args);

	va_end(args);


	return (adc_MB_Append(mb, buffer, n));

}

/* ###################################################################################################################### */
