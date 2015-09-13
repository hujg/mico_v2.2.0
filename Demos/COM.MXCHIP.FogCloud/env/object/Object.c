
/***

History:
2015-09-10: Ted: Create

*/

#include <Object.h>
// for MICO
#include "MICO.h"

// for MICO
#define Object_DBG(M, ...) custom_log("Object > DBG", M, ##__VA_ARGS__)
#define Object_ERR(M, ...) custom_log("Object > ERR", M, ##__VA_ARGS__)


// ==== Object Manage ==== //

// Object struct of Dist
typedef struct _SObjectDist_t {
    bool    used;
    char    distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
    u8      dsType;
    u16     data;
    char    str[ESM_OBJECT_DIST_STRING_MAX_LENGTH];
} SObjectDist;

// Object struct of Subscribe
typedef struct _SObjectSubscribe_t {
    bool        used;
    SthreadId   subscriber_thread_id;
} SObjectSubscribe;

// Object struct of Object Child
typedef struct _SObject_t {
    bool             used;
    char             objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    SObjectDist      dist[MAX_DIST_NUM_IN_OBJECT];
    SObjectSubscribe subscribe[MAX_SUBSCRIBE_NUM_IN_OBJECT];
} SObjectMng;

// ==== end of Object Manage ==== //


// object pages
static SObjectMng object[MAX_OBJECT_NUM];
// mutex of Object operation
static mico_mutex_t ObjectMutex = NULL;


// Object initialization
void ObjectInit(void)
{
    OSStatus err;
    
    memset(object, 0, MAX_OBJECT_NUM * sizeof(SObjectMng));
    err = mico_rtos_init_mutex(&ObjectMutex);
    if(err) {}
    
    Object_DBG("ObjectInit Finished");
}
// end of Object initialization

// Create Object
ObjectErr_t ObjectCreate(const char* obj_name, u8 size_of_name)
{
    u8 index = 0;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    do {
        if(object[index].used == false) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild is fulled, %s create failed", obj_name);
        ret = ErrExceed;
    }
    else {
        object[index].used = true;
        strncpy(object[index].objName, obj_name, size_of_name);
        Object_DBG("Create ObjectChild %s Successfully", object[index].objName);
        ret = NoErr;
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}
// end of Create Object

// Add Dist into Object
ObjectErr_t ObjectAddDist(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, u8 ds)
{
    u8 index;
    u8 dist_index;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = ErrNotExist;
    }
    else {
        dist_index = 0;
        
        do {
            if(object[index].dist[dist_index].used == false) {
                break;
            }
            
            dist_index++;
        } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
        
        if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
            Object_ERR("Dist is fulled, add %s is failed", dist_name);
            ret = ErrDistFull;
        }
        else {
            object[index].dist[dist_index].used = true;
            object[index].dist[dist_index].dsType = ds;
            strncpy(object[index].dist[dist_index].distName, dist_name, size_of_dist_name);
            Object_DBG("Add %s/%s Successfully", object[index].objName, object[index].dist[dist_index].distName);
            ret = NoErr;
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// Subscribe/Unsubscribe Interface
ObjectErr_t ObjectSubscribe(const char* obj_name, u8 size_of_obj_name, SthreadId suber_id)
{
    u8 index;
    u8 sub_index;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true)
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    }while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = ErrNotExist;
    }
    else {
        sub_index = 0;
        
        do {
            if(object[index].subscribe[sub_index].used == false) {
                break;
            }
            
            sub_index++;
        }while(sub_index < MAX_SUBSCRIBE_NUM_IN_OBJECT);
        
        if(sub_index >= MAX_SUBSCRIBE_NUM_IN_OBJECT) {
            Object_ERR("No room of new Subscribe for Thread %d", suber_id);
            ret = ErrExceed;
        }
        else {
            object[index].subscribe[sub_index].used = true;
            object[index].subscribe[sub_index].subscriber_thread_id = suber_id;
            Object_DBG("Subscribe successfully for Thread %d", suber_id);
            ret = NoErr;
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

ObjectErr_t ObjectUnsubscribe(const char* obj_name, u8 size_of_obj_name, SthreadId unsuber_id)
{
    u8 index;
    u8 sub_index;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true)
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    }while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = ErrNotExist;
    }
    else {
        sub_index = 0;
        
        do {
            if((object[index].subscribe[sub_index].used == true)
                && (object[index].subscribe[sub_index].subscriber_thread_id == unsuber_id)) {
                break;
            }
            
            sub_index++;
        }while(sub_index < MAX_SUBSCRIBE_NUM_IN_OBJECT);
        
        if(sub_index >= MAX_SUBSCRIBE_NUM_IN_OBJECT) {
            Object_ERR("Have no Subscriber or Thread(%d) had not subscribed", unsuber_id);
            ret = ErrNotExist;
        }
        else {
            object[index].subscribe[sub_index].used = false;
            object[index].subscribe[sub_index].subscriber_thread_id = 0;
            Object_DBG("Unsubscribe successfully for Thread %d", unsuber_id);
            ret = NoErr;
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}
// end of Subscribe/Unsubscribe Interface

// get Object dist value
ObjectErr_t ObjectGetValue(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, u16* value)
{
    u8 index;
    u8 dist_index;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = ErrNotExist;
    }
    else {
        dist_index = 0;
        
        do{
            if((object[index].dist[dist_index].used == true)
                && (strncmp(object[index].dist[dist_index].distName, dist_name, size_of_dist_name) == 0)) {
                break;
            }
            
            dist_index++;
        } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
        
        if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
            Object_ERR("Dist %s isn't exist", dist_name);
            ret = ErrNotExist;
        }
        else {
            *value = object[index].dist[dist_index].data;
            Object_DBG("Get %s/%s as %d Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].data);
            ret = NoErr;
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// set Object dist value
ObjectErr_t ObjectSetValue(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, u16 value)
{
    u8 index;
    u8 dist_index;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = ErrNotExist;
    }
    else {
        dist_index = 0;
        
        do{
            if((object[index].dist[dist_index].used == true)
                && (strncmp(object[index].dist[dist_index].distName, dist_name, size_of_dist_name) == 0)) {
                break;
            }
            
            dist_index++;
        } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
        
        if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
            Object_ERR("Dist %s isn't exist", dist_name);
            ret = ErrNotExist;
        }
        else {
            object[index].dist[dist_index].data = value;
            Object_DBG("Set %s/%s as %d Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].data);
            ret = NoErr;
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// get Object dist string
ObjectErr_t ObjectGetString(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, char* str)
{
    u8 index;
    u8 dist_index;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = ErrNotExist;
    }
    else {
        dist_index = 0;
        
        do{
            if((object[index].dist[dist_index].used == true)
                && (strncmp(object[index].dist[dist_index].distName, dist_name, size_of_dist_name) == 0)) {
                break;
            }
            
            dist_index++;
        } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
        
        if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
            Object_ERR("Dist %s isn't exist", dist_name);
            ret = ErrNotExist;
        }
        else {
            strncpy(str, object[index].dist[dist_index].str, strlen(object[index].dist[dist_index].str));
            Object_DBG("Get %s/%s as %s Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].str);
            ret = NoErr;
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// set Object dist string
ObjectErr_t ObjectSetString(const char* obj_name, u8 size_of_obj_name, const char* dist_name, u8 size_of_dist_name, char* str)
{
    u8 index;
    u8 dist_index;
    ObjectErr_t ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, size_of_obj_name) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = ErrNotExist;
    }
    else {
        dist_index = 0;
        
        do{
            if((object[index].dist[dist_index].used == true)
                && (strncmp(object[index].dist[dist_index].distName, dist_name, size_of_dist_name) == 0)) {
                break;
            }
            
            dist_index++;
        } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
        
        if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
            Object_ERR("Dist %s isn't exist", dist_name);
            ret = ErrNotExist;
        }
        else {
            strncpy(object[index].dist[dist_index].str, str, strlen(str));
            Object_DBG("Set %s/%s as %s Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].str);
            ret = NoErr;
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}


void ObjectPrint()
{
    u16 i, j;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    Object_DBG("ObjectPrint:");
    
    for(i=0; i<MAX_OBJECT_NUM; i++) {
        if(object[i].used == false) {
            continue;
        }
        
        Object_DBG("Index %d: %s", i, object[i].objName);
        
        for(j=0; j<MAX_DIST_NUM_IN_OBJECT; j++) {
            if(object[i].dist[j].used == false) {
                continue;
            }
            
            if(object[i].dist[j].dsType == EDistType_Data) {
                Object_DBG("%s/%s: %d", object[i].objName, object[i].dist[j].distName, object[i].dist[j].data);
            }
            else {
                Object_DBG("%s/%s: %s", object[i].objName, object[i].dist[j].distName, object[i].dist[j].str);
            }
        }
        
        for(j=0; j<MAX_SUBSCRIBE_NUM_IN_OBJECT; j++) {
            if(object[i].subscribe[j].used == false) {
                continue;
            }
            
            Object_DBG("Subscriber: Thread(%d)", object[i].subscribe[j].subscriber_thread_id);
        }
    }
    
    mico_rtos_unlock_mutex(&ObjectMutex);
}

// end of file


