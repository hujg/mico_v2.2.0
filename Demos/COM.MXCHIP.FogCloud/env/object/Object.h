
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
// dist string value length
#define ESM_OBJECT_DIST_STRING_MAX_LENGTH   64
// reason string length
#define ESM_OBJECT_REASON_MAX_LENGTH        16

// Object dist value element of data or string
#define EDistType_Data      0
#define EDistType_String    1



void ObjectInit(void);
ObjectErr_t ObjectCreate(const char* obj_name, u8 size_of_name);
ObjectErr_t ObjectAddDist(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, u8 ds);
ObjectErr_t ObjectSubscribe(const char* obj_name, u8 size_of_obj_name, SthreadId suber_id);
ObjectErr_t ObjectUnsubscribe(const char* obj_name, u8 size_of_obj_name, SthreadId unsuber_id);
ObjectErr_t ObjectGetValue(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, u16* value);
ObjectErr_t ObjectSetValue(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, u16 value);
ObjectErr_t ObjectGetString(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, char* str);
ObjectErr_t ObjectSetString(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, char* str);
void ObjectPrint();



#ifdef __cplusplus
}
#endif

#endif // _OBJECT_H

// end of file



