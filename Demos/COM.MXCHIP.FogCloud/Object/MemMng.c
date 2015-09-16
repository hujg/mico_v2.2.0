
/***

History:
2013-07-31: Ted: Create
2015-09-11: Change to used for MusicCup

*/


#include <MemMng.h>
#include <string.h>
#include <stdio.h>
// for MICO
#include "MICO.h"

// for MICO
#define MemMng_DBG(M, ...) custom_log("MemMng > DBG", M, ##__VA_ARGS__)
#define MemMng_ERR(M, ...) custom_log("MemMng > ERR", M, ##__VA_ARGS__)

// if Memery used
typedef enum _EMemValid_t {
	EMemUnused = 0,
	EMemUsed
} EMemValid;

// Pool struct of Memery Manage
typedef struct _SMem_t {
	addP_t      next;
	addP_t      prev;
	EMemValid   used;
} SMem;
// size of struct of SMem
#define	SIZEOF_STRUCT_MEM	sizeof(SMem)

// the minimum unit of Memery Pool
#define	MEM_SIZE_ALIGN		4

// Memery StartAdd and EndAdd, measured by u8
//#define	MEM_MNG_START	(Bank1_SRAM1_ADDR + 0x000000)
//#define	MEM_MNG_END		(Bank1_SRAM1_ADDR + 0x080000)

// align the Memery Segment
#define	GetAlignSegSize(size)	(((size) + MEM_SIZE_ALIGN - 1) & ~(MEM_SIZE_ALIGN - 1))

// size of Memery alignment segment
#define	MEM_ALIGN_SEG_SIZE	    GetAlignSegSize(SIZEOF_STRUCT_MEM)

//
//#define	MEM_ALIGN_SIZE_MAX	    GetAlignSegSize(SRAM_SIZE)

// Memery pool
static u8 MemeryBuffer[SRAM_SIZE];
// Ram start/end address point
static addP_t g_ram_s;
static addP_t g_ram_e;
// the first free memery pool address point
static addP_t g_low_free;
// mutex of memery operation
static mico_mutex_t MemeryMutex = NULL;


static void PlugHoles(SMem *mem);


void MemInit(void)
{
	SMem *mem;
    OSStatus err;
	
	g_ram_s = (addP_t)MemeryBuffer;
	g_ram_e = (addP_t)&MemeryBuffer[SRAM_SIZE - 1] - MEM_ALIGN_SEG_SIZE + 1;    // need check ?
	MemMng_DBG("g_ram_s:0x%8X, g_ram_e:0x%8X", g_ram_s, g_ram_e);
    MemMng_DBG("MemeryBuffer:0x%8X, &MemeryBuffer[SRAM_SIZE - 1]:0x%8X", (addP_t)MemeryBuffer, (addP_t)&MemeryBuffer[SRAM_SIZE - 1] - MEM_ALIGN_SEG_SIZE + 1);
    MemMng_DBG("size of SIZEOF_STRUCT_MEM:%d", SIZEOF_STRUCT_MEM);
	
	mem = (SMem *)g_ram_s;
	mem->next = g_ram_e;
	mem->prev = NULL;
	mem->used = EMemUnused;
	
	g_low_free = (addP_t)mem;
	
	mem = (SMem *)g_ram_e;
	mem->next = NULL;
	mem->prev = g_ram_s;
	mem->used = EMemUsed;
    
    err = mico_rtos_init_mutex(&MemeryMutex);
    if(err) {}
    
    MemMng_DBG("MemInit Finished");
}


void *MemMalloc(u32 size)
{
	SMem *mem;
	SMem *mem_next;
	SMem *new_mem;
	addP_t ret_addr;
    
    mico_rtos_lock_mutex(&MemeryMutex);

	if(size == 0) {
		MemMng_ERR("Size incorrect");
		ret_addr = NULL;
	}
    else {
        size = GetAlignSegSize(size);
        
        mem = (SMem *)g_low_free;
        if(mem == NULL) {
            MemMng_ERR("g_low_free = NULL");
            ret_addr = NULL;
        }
        else {
            while(mem->next != NULL) {
                if((mem->used == EMemUsed) || ((mem->next - (addP_t)mem - MEM_ALIGN_SEG_SIZE*2) < size)) {
                    mem = (SMem *)mem->next;
                }
                else {
                    break;
                }
            }
            
            if(mem->next == NULL) {
                MemMng_ERR("No Free Memery or suitable Size, mem->next = NULL");
                ret_addr = NULL;
            }
            else {
                mem_next = (SMem *)mem->next;
                new_mem = (SMem *)((addP_t)mem + MEM_ALIGN_SEG_SIZE + size);
                memset(new_mem, 0, sizeof(SMem));
                
                mem->next = (addP_t)new_mem;
                mem->used = EMemUsed;
                ret_addr = (addP_t)mem + MEM_ALIGN_SEG_SIZE;
                
                new_mem->next = (addP_t)mem_next;
                new_mem->prev = (addP_t)mem;
                new_mem->used = EMemUnused;
                
                mem_next->prev = (addP_t)new_mem;
                
                mem = (SMem *)g_low_free;
                while((mem->used == EMemUsed) || (mem->next == NULL)) {
                    mem = (SMem *)mem->next;
                }
                if(mem->next == NULL) {
                    g_low_free = NULL;
                }
                else {
                    g_low_free = (addP_t)mem;
                }
            }
        }
    }
    
    mico_rtos_unlock_mutex(&MemeryMutex);
	
	return (void*)(addP_t)(ret_addr);
}


void MemFree(void *mem)
{
	SMem *mem_low_free;
	SMem *free_mem;
    
    mico_rtos_lock_mutex(&MemeryMutex);

	if(mem == NULL) {
		MemMng_ERR("mem is NULL");
	}
    else {
        if(((addP_t)mem < g_ram_s) || ((addP_t)mem > g_ram_e)) {
            MemMng_ERR("mem is out of range");
        }
        else {
            free_mem = (SMem *)((addP_t)mem - MEM_ALIGN_SEG_SIZE);
            free_mem->used = EMemUnused;
            mem_low_free = (SMem *)g_low_free;
            if((addP_t)mem < (addP_t)mem_low_free) {
                g_low_free = (addP_t)free_mem;
            }
            
            PlugHoles(free_mem);
        }
    }
	    
    mico_rtos_unlock_mutex(&MemeryMutex);
}


static void PlugHoles(SMem *mem)
{
    SMem* free_mem;
    SMem* next_mem;
    
    free_mem = (SMem *)g_ram_s;
    
    do {
        if(free_mem->used == EMemUnused) {
            g_low_free = (addP_t)free_mem;
            break;
        }
        else {
            free_mem = (SMem*)free_mem->next;
        }
    } while(1);
    
    do {
        if(free_mem->used == EMemUnused) {
            do {
                next_mem = (SMem*)free_mem->next;
                if(next_mem->used == EMemUnused) {
                    free_mem->next = next_mem->next;
                }
                else {
                    free_mem = (SMem*)free_mem->next;
                    break;
                }
            } while(1);
        }
        else {
            free_mem = (SMem*)free_mem->next;
        }
    } while(free_mem->next != NULL);
}


void SramPrintValueCheck(addP_t addr, u32 len)
{
	u32 i;
    
    mico_rtos_lock_mutex(&MemeryMutex);

	MemMng_DBG("SramPrint >>");
	addr &= ~((u32)MEM_SIZE_ALIGN);
	len = GetAlignSegSize(len);
	for(i=0; i<(len/MEM_SIZE_ALIGN); i++) {
		MemMng_DBG("0x%8X: %04X %04X", addr, *(__IO u16*)(addr), *(__IO u16*)(addr + 2));
		addr += MEM_SIZE_ALIGN;
	}
    
    mico_rtos_unlock_mutex(&MemeryMutex);
}


void SramPrintMngCheck()
{
    SMem *mem;
    
    mico_rtos_lock_mutex(&MemeryMutex);
    
    MemMng_DBG("SramPrintMngCheck: >>");
    
    mem = (SMem *)g_ram_s;
    do {
        MemMng_DBG("mem:0x%8X, mem->next:0x%8X, mem->prev:0x%8X, mem->used:%d", (addP_t)mem, mem->next, mem->prev, mem->used);
        
        if(mem->next != NULL) {
            mem = (SMem *)mem->next;
        }
        else {
            break;
        }
    } while(1);
    
    mico_rtos_unlock_mutex(&MemeryMutex);
}

// end of file


