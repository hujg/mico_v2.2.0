
/***

History:
2015-09-10: Ted: Create

*/

#ifndef _OBJECT_H
#define _OBJECT_H

#ifdef __cplusplus
 extern "C" {
#endif 



#include <Object_int.h>
#include <ThreadMng.h>

//
typedef enum _ObjectErr_t {
    // without error
    NoErr = 0,
    // exceed the max number
    ErrExceed,
    // not exist
    ErrNotExist,
    // dist full
    ErrDistFull
} ObjectErr_t;

// Max number of Object
#define MAX_OBJECT_NUM                  8
// Max number of subscriber in one Object
#define MAX_SUBSCRIBE_NUM_IN_OBJECT     8
// Max number of dist in one Object
#define MAX_DIST_NUM_IN_OBJECT          8
// objName[] string length
#define ESM_OBJECT_OBJECT_NAME_MAX_LENGTH   16
// distName[] string length
#define ESM_OBJECT_DIST_NAME_MAX_LENGTH     16
// reason string length
#define ESM_OBJECT_REASON_MAX_LENGTH        16



void ObjectInit(void);
ObjectErr_t ObjectCreate(char* obj_name, u8 size_of_name);
ObjectErr_t ObjectAddDist(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name);
ObjectErr_t ObjectSubscribe(char* obj_name, u8 size_of_obj_name, threadId_t suber_id);
ObjectErr_t ObjectUnsubscribe(char* obj_name, u8 size_of_obj_name, threadId_t unsuber_id);
ObjectErr_t ObjectGetValue(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name, u16 &value);
ObjectErr_t ObjectSetValue(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name, u16 value);



#ifdef __cplusplus
}
#endif

#endif // _OBJECT_H

// end of file



