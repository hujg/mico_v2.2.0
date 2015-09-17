
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


// Max number of Object
#define MAX_OBJECT_NUM                  4
// Max number of subscriber in one Object
#define MAX_SUBSCRIBE_NUM_IN_OBJECT     4
// Max number of dist in one Object
#define MAX_DIST_NUM_IN_OBJECT          6
// objName[] string length
#define ESM_OBJECT_OBJECT_NAME_MAX_LENGTH   16
// distName[] string length
#define ESM_OBJECT_DIST_NAME_MAX_LENGTH     16
// dist string value length
#define ESM_OBJECT_DIST_STRING_MAX_LENGTH   64
// reason string length
#define ESM_OBJECT_REASON_MAX_LENGTH        32

// Object dist value element of data or string
#define EDistType_Data      0
#define EDistType_String    1



void ObjectModule_Thread(void* arg);
void ObjectInit(void);
OSStatus ObjectCreate(const char* obj_name);
OSStatus ObjectAddDist(const char* obj_name, const char* dist_name, u8 ds);
OSStatus ObjectGetValue(const char* obj_name, const char* dist_name, u16* value);
OSStatus ObjectSetValue(const char* obj_name, const char* dist_name, u16 value);
OSStatus ObjectGetString(const char* obj_name, const char* dist_name, char* str);
OSStatus ObjectSetString(const char* obj_name, const char* dist_name, char* str);
void ObjectPrint();



#ifdef __cplusplus
}
#endif

#endif // _OBJECT_H

// end of file



