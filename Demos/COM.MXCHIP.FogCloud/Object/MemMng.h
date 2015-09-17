
/***

History:
2013-07-31: Ted: Create
2015-09-11: Change to used for MusicCup

*/

#ifndef __MEMMNG_H
#define __MEMMNG_H

#ifdef __cplusplus
 extern "C" {
#endif 



#include <Object_int.h>


typedef u32     addP_t;


// Memery Pool size
#define	SRAM_SIZE	(2*1024)


void MemInit(void);
void *MemMalloc(u32 size);
void MemFree(void *mem);
void SramPrintValueCheck(addP_t addr, u32 len);
void SramPrintMngCheck();



#ifdef __cplusplus
}
#endif

#endif // __MEMMNG_H

// end of file



