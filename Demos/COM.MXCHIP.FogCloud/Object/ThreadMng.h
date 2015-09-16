
/***

History:
2015-09-06: Ted: Create

*/


#ifndef _THREADMNG_H
#define _THREADMNG_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include "MICODefine.h"


typedef u16  SthreadId;

// Thread Manage
typedef struct _SEUm_t {
    SthreadId        t_id;
    mico_Context_t  *mico_context;
} SEUm;

// Max number of Threads
#define MAX_THREAD_NUM          8

// Thread Manage
#define ESM_OBJECT_THREAD_ID    0
#define ESM_LIGHTS_THREAD_ID    1
#define ESM_MUSIC_THREAD_ID     2
#define ESM_SYNCMNG_THREAD_ID   3


#ifdef __cplusplus
}
#endif

#endif // _THREADMNG_H

// end of file


