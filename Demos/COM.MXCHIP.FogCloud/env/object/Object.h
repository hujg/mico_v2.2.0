
/***

History:
2015-09-10: Ted: Create

*/

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

// ==== Object Manage ==== //

// Object struct of Dist
typedef struct _SObjectDist_t {
    bool    valid;
    char    distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
    u16     data;
} SObjectDist;

// Object struct of Subscribe
typedef struct _SObjectSubscribe_t {
    bool        valid;
    SthreadId   subscriber_thread_id;
} SObjectSubscribe;

// Object struct of Object Child
typedef struct _SObject_t {
    bool             valid;
    char             objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    SObjectDist      dist[MAX_DIST_NUM_IN_OBJECT];
    SObjectSubscribe subscribe[MAX_SUBSCRIBE_NUM_IN_OBJECT];
} SObjectMng;

// ==== end of Object Manage ==== //


void ObjectInit(void);
ObjectErr_t ObjectCreate(char* obj_name, u8 size_of_name);
ObjectErr_t ObjectAddDist(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name);
ObjectErr_t ObjectSubscribe(char* obj_name, u8 size_of_obj_name, threadId_t suber_id);
ObjectErr_t ObjectUnsubscribe(char* obj_name, u8 size_of_obj_name, threadId_t unsuber_id);
u16 ObjectGetValue(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name);
ObjectErr_t ObjectSetValue(char* obj_name, u8 size_of_obj_name, char* dist_name, u8 size_of_dist_name, u16 value);


// end of file



