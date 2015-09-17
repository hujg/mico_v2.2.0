
/***

History:
2015-09-10: Ted: Create

*/

#include <Object.h>
#include <API_MSG.h>
#include <SysCom.h>
// for MICO
#include "MICO.h"
// for user
#include <ObjDevice.h>
#include <ObjLights.h>
#include <ObjMusic.h>

// for MICO
#define Object_DBG(M, ...) custom_log("Object > DBG", M, ##__VA_ARGS__)
#define Object_INF(M, ...) custom_log("Object > INF", M, ##__VA_ARGS__)
#define Object_ERR(M, ...) custom_log("Object > ERR", M, ##__VA_ARGS__)
#define Object_Log_Trace() custom_log_trace("OBJECTMODULE")


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
static SObjectMng objectBackup[MAX_OBJECT_NUM];
// mutex of Object operation
static mico_mutex_t ObjectMutex = NULL;


static OSStatus ObjectSubscribe(const char* obj_name, SthreadId suber_id);
static OSStatus ObjectUnsubscribe(const char* obj_name, SthreadId unsuber_id);
static OSStatus ObjectDataIndMsgSend(char* obj_name, char* dist_name, u16 data, SthreadId t_id);
static OSStatus ObjectStringIndMsgSend(char* obj_name, char* dist_name, char* str, SthreadId t_id);
static void ObjectHandleMessage();
static void ObjectHandleSubscribeReqMessage(SysComMsg* msg);
static OSStatus ObjectSubscribeRespMsgSend(char* obj_name, OSStatus err_state, SthreadId receiver, u16 trans_id);
static void ObjectHandleDistQueryReqMessage(SysComMsg* msg);
static OSStatus ObjectDistQueryRespMsgSend(char* obj_name, char* dist_name, u8 ds, SthreadId receiver, u16 trans_id);



void ObjectModule_Thread(void* arg)
{
    u8 obj_index, dist_index, sub_index;
    SObjectDist* dist_temp = NULL;
    SObjectDist* dist_bak_temp = NULL;

    arg = arg;
    
    Object_Log_Trace();

    Object_INF("ObjectModule_Thread Created");
    
    while(1) {
        //mico_thread_sleep(2);
        Object_DBG("ObjectModule_Thread: Control loop");
        // Send Indication Message if MO changed
        for(obj_index=0; obj_index<MAX_OBJECT_NUM; obj_index++) {
            if(object[obj_index].used == false) {
                Object_DBG("ObjectModule_Thread: object[%d] is not used", obj_index);
                continue;
            }
            
            for(dist_index=0; dist_index<MAX_DIST_NUM_IN_OBJECT; dist_index++) {
                if(object[obj_index].dist[dist_index].used == false) {
                    Object_DBG("ObjectModule_Thread: object[%d].dist[%d] is not used", obj_index, dist_index);
                    continue;
                }
                
                dist_temp = &object[obj_index].dist[dist_index];
                dist_bak_temp = &objectBackup[obj_index].dist[dist_index];
                
                if(dist_temp->dsType == EDistType_Data) {
                    Object_DBG("ObjectModule_Thread: dsType:%d dist_data:%d dist_data_bak:%d", dist_temp->dsType, dist_temp->data, dist_bak_temp->data);
                    if(dist_temp->data == dist_bak_temp->data) {
                        Object_DBG("ObjectModule_Thread: dist_data is equal with dist_data_bak");
                        continue;
                    }
                    Object_DBG("ObjectModule_Thread: %s/%s was changed", object[obj_index].objName, object[obj_index].dist[dist_index].distName);
                    // MO data changed, send indication data message to subscriber
                    for(sub_index=0; sub_index<MAX_SUBSCRIBE_NUM_IN_OBJECT; sub_index++) {
                        if(object[obj_index].subscribe[sub_index].used == false) {
                            Object_DBG("ObjectModule_Thread: object[%d].subscribe[%d] is not used", obj_index, sub_index);
                            continue;
                        }
                        Object_DBG("ObjectModule_Thread: %s/%s changed to %d, Send Indication message to Thread(%d)",
                                    object[obj_index].objName,
                                    object[obj_index].dist[dist_index].distName,
                                    object[obj_index].dist[dist_index].data,
                                    object[obj_index].subscribe[sub_index].subscriber_thread_id);
                        
                        ObjectDataIndMsgSend(object[obj_index].objName, 
                                            object[obj_index].dist[dist_index].distName, 
                                            object[obj_index].dist[dist_index].data,
                                            object[obj_index].subscribe[sub_index].subscriber_thread_id);
                    }
                    
                    dist_bak_temp->data = dist_temp->data;
                }
                else if(dist_temp->dsType == EDistType_String) {
                    if(strcmp(dist_temp->str, dist_bak_temp->str) == 0) {
                        continue;
                    }
                    Object_DBG("ObjectModule_Thread: %s/%s was changed", object[obj_index].objName, object[obj_index].dist[dist_index].distName);
                    // MO string changed, send indication string message to subscriber
                    for(sub_index=0; sub_index<MAX_SUBSCRIBE_NUM_IN_OBJECT; sub_index++) {
                        if(object[obj_index].subscribe[sub_index].used == false) {
                            continue;
                        }
                        Object_DBG("ObjectModule_Thread: %s/%s changed to %s, Send indication message to Thread(%d)",
                                    object[obj_index].objName,
                                    object[obj_index].dist[dist_index].distName,
                                    object[obj_index].dist[dist_index].str,
                                    object[obj_index].subscribe[sub_index].subscriber_thread_id);
                        
                        ObjectStringIndMsgSend(object[obj_index].objName, 
                                            object[obj_index].dist[dist_index].distName, 
                                            object[obj_index].dist[dist_index].str,
                                            object[obj_index].subscribe[sub_index].subscriber_thread_id);
                    }
                    
                    strncpy(dist_bak_temp->str, dist_temp->str, strlen(dist_temp->str));
                }
            }
        }
        
        // Handle Subscribe/Query Message
        ObjectHandleMessage();
    }
}


// Object initialization
void ObjectInit(void)
{
    OSStatus err;
    
    memset(object, 0, MAX_OBJECT_NUM * sizeof(SObjectMng));
    memset(objectBackup, 0, MAX_OBJECT_NUM * sizeof(SObjectMng));
    err = mico_rtos_init_mutex(&ObjectMutex);
    if(err) {
        Object_ERR("ObjectInit: ObjectMutex initialized failed");
    }
    
    Object_INF("ObjectInit Finished");
}

// Create Object
OSStatus ObjectCreate(const char* obj_name)
{
    u8 index = 0;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    do {
        if(object[index].used == false) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild is fulled, %s create failed", obj_name);
        ret = kObjFullErr;
        goto ObjectCreate_RET;
    }
    
    object[index].used = true;
    strncpy(object[index].objName, obj_name, strlen(obj_name));
    
    objectBackup[index].used = true;
    strncpy(objectBackup[index].objName, obj_name, strlen(obj_name));
    
    Object_INF("Create ObjectChild %s Successfully", object[index].objName);
    ret = kNoErr;

ObjectCreate_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// Add Dist into Object
OSStatus ObjectAddDist(const char* obj_name, const char* dist_name, u8 ds)
{
    u8 index;
    u8 dist_index;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, strlen(obj_name)) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = kObjNotExistErr;
        goto ObjectAddDist_RET;
    }
    
    dist_index = 0;
    
    do {
        if(object[index].dist[dist_index].used == false) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        Object_ERR("Dist is fulled, add %s is failed", dist_name);
        ret = kDistFullErr;
        goto ObjectAddDist_RET;
    }
    
    object[index].dist[dist_index].used = true;
    object[index].dist[dist_index].dsType = ds;
    strncpy(object[index].dist[dist_index].distName, dist_name, strlen(dist_name));
    
    objectBackup[index].dist[dist_index].used = true;
    objectBackup[index].dist[dist_index].dsType = ds;
    strncpy(objectBackup[index].dist[dist_index].distName, dist_name, strlen(dist_name));
    
    Object_INF("Add %s/%s Successfully", object[index].objName, object[index].dist[dist_index].distName);
    ret = kNoErr;
    
ObjectAddDist_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// Subscribe/Unsubscribe Interface
static OSStatus ObjectSubscribe(const char* obj_name, SthreadId suber_id)
{
    u8 index;
    u8 sub_index;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true)
            && (strncmp(object[index].objName, obj_name, strlen(obj_name)) == 0)) {
            break;
        }
        
        index++;
    }while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = kObjNotExistErr;
        goto ObjectSubscribe_RET;
    }
    
    for(sub_index=0; sub_index<MAX_SUBSCRIBE_NUM_IN_OBJECT; sub_index++) {
        if(object[index].subscribe[sub_index].used == true) {
            if(object[index].subscribe[sub_index].subscriber_thread_id == suber_id) {
                Object_ERR("ObjectSubscribe: Thread(%d) has already Subscribed", suber_id);
                ret = kSubExistErr;
                goto ObjectSubscribe_RET;
            }
        }
    }
    
    sub_index = 0;
    
    do {
        if(object[index].subscribe[sub_index].used == false) {
            break;
        }
        
        sub_index++;
    }while(sub_index < MAX_SUBSCRIBE_NUM_IN_OBJECT);
    
    if(sub_index >= MAX_SUBSCRIBE_NUM_IN_OBJECT) {
        Object_ERR("No room of new Subscribe for Thread %d", suber_id);
        ret = kSubFullErr;
        goto ObjectSubscribe_RET;
    }
    
    object[index].subscribe[sub_index].used = true;
    object[index].subscribe[sub_index].subscriber_thread_id = suber_id;
    Object_INF("Subscribe successfully for Thread %d", suber_id);
    ret = kNoErr;
    
ObjectSubscribe_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// Unsubscribe Interface
static OSStatus ObjectUnsubscribe(const char* obj_name, SthreadId unsuber_id)
{
    u8 index;
    u8 sub_index;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true)
            && (strncmp(object[index].objName, obj_name, strlen(obj_name)) == 0)) {
            break;
        }
        
        index++;
    }while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = kObjNotExistErr;
        goto ObjectUnsubscribe_RET;
    }
    
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
        ret = kSubNotExistErr;
        goto ObjectUnsubscribe_RET;
    }
    
    object[index].subscribe[sub_index].used = false;
    object[index].subscribe[sub_index].subscriber_thread_id = 0;
    Object_INF("Unsubscribe successfully for Thread %d", unsuber_id);
    ret = kNoErr;
    
ObjectUnsubscribe_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}
// end of Subscribe/Unsubscribe Interface

// get Object dist value
OSStatus ObjectGetValue(const char* obj_name, const char* dist_name, u16* value)
{
    u8 index;
    u8 dist_index;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, strlen(obj_name)) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = kObjNotExistErr;
        goto ObjectGetValue_RET;
    }
    
    dist_index = 0;
    
    do{
        if((object[index].dist[dist_index].used == true)
            && (strncmp(object[index].dist[dist_index].distName, dist_name, strlen(dist_name)) == 0)) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        Object_ERR("Dist %s isn't exist", dist_name);
        ret = kDistNotEixstErr;
        goto ObjectGetValue_RET;
    }
    
    *value = object[index].dist[dist_index].data;
    Object_DBG("Get %s/%s as %d Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].data);
    ret = kNoErr;
    
ObjectGetValue_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// set Object dist value
OSStatus ObjectSetValue(const char* obj_name, const char* dist_name, u16 value)
{
    u8 index;
    u8 dist_index;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, strlen(obj_name)) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = kObjNotExistErr;
        goto ObjectSetValue_RET;
    }
    
    dist_index = 0;
    
    do{
        if((object[index].dist[dist_index].used == true)
            && (strncmp(object[index].dist[dist_index].distName, dist_name, strlen(dist_name)) == 0)) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        Object_ERR("Dist %s isn't exist", dist_name);
        ret = kDistNotEixstErr;
        goto ObjectSetValue_RET;
    }
    
    objectBackup[index].dist[dist_index].data = object[index].dist[dist_index].data;
    Object_DBG("Set %s/%s previous data as %d", objectBackup[index].objName, objectBackup[index].dist[dist_index].distName, objectBackup[index].dist[dist_index].data);
    object[index].dist[dist_index].data = value;
    Object_DBG("Set %s/%s current data as %d Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].data);
    ret = kNoErr;
    
ObjectSetValue_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// get Object dist string
OSStatus ObjectGetString(const char* obj_name, const char* dist_name, char* str)
{
    u8 index;
    u8 dist_index;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, strlen(obj_name)) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = kObjNotExistErr;
        goto ObjectGetString_RET;
    }
    
    dist_index = 0;
    
    do{
        if((object[index].dist[dist_index].used == true)
            && (strncmp(object[index].dist[dist_index].distName, dist_name, strlen(dist_name)) == 0)) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        Object_ERR("Dist %s isn't exist", dist_name);
        ret = kDistNotEixstErr;
        goto ObjectGetString_RET;
    }
    
    strncpy(str, object[index].dist[dist_index].str, strlen(object[index].dist[dist_index].str));
    Object_DBG("Get %s/%s as %s Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].str);
    ret = kNoErr;
    
ObjectGetString_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return ret;
}

// set Object dist string
OSStatus ObjectSetString(const char* obj_name, const char* dist_name, char* str)
{
    u8 index;
    u8 dist_index;
    OSStatus ret;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    index = 0;
    
    do {
        if((object[index].used == true) 
            && (strncmp(object[index].objName, obj_name, strlen(obj_name)) == 0)) {
            break;
        }
        
        index++;
    } while(index < MAX_OBJECT_NUM);
    
    if(index >= MAX_OBJECT_NUM) {
        Object_ERR("ObjectChild %s isn't exist", obj_name);
        ret = kObjNotExistErr;
        goto ObjectSetString_RET;
    }
    
    dist_index = 0;
    
    do{
        if((object[index].dist[dist_index].used == true)
            && (strncmp(object[index].dist[dist_index].distName, dist_name, strlen(dist_name)) == 0)) {
            break;
        }
        
        dist_index++;
    } while(dist_index < MAX_DIST_NUM_IN_OBJECT);
    
    if(dist_index >= MAX_DIST_NUM_IN_OBJECT) {
        Object_ERR("Dist %s isn't exist", dist_name);
        ret = kDistNotEixstErr;
        goto ObjectSetString_RET;
    }
    
    strncpy(objectBackup[index].dist[dist_index].str, object[index].dist[dist_index].str, strlen(object[index].dist[dist_index].str));
    Object_DBG("Set %s/%s previous string as %s", objectBackup[index].objName, objectBackup[index].dist[dist_index].distName, objectBackup[index].dist[dist_index].str);
    strncpy(object[index].dist[dist_index].str, str, strlen(str));
    Object_DBG("Set %s/%s current string as %s Successfully", object[index].objName, object[index].dist[dist_index].distName, object[index].dist[dist_index].str);
    ret = kNoErr;
    
ObjectSetString_RET:
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
            else if(object[i].dist[j].dsType == EDistType_String){
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

// Object Thread send Changed Data Indication Message to subscribed Thread
static OSStatus ObjectDataIndMsgSend(char* obj_name, char* dist_name, u16 data, SthreadId t_id)
{
    OSStatus err = kUnknownErr;
    SysComMsg* msg = NULL;
    SapiObjectInd *ind_msg = NULL;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    msg = SysComCreateMsg(API_OBJECT_IND_MSG, 
                        sizeof(SapiObjectInd), 
                        ESM_OBJECT_IND_TRANSID, 
                        ESM_OBJECT_THREAD_ID, 
                        t_id);
    if(msg == NULL) {
        Object_ERR("ObjectDataIndMsgSend: Message is NULL");
        err = kNoMsgErr;
        goto ObjectDataIndMsgSend_RET;
    }
    
    ind_msg = (SapiObjectInd*)SysComGetPayload(msg);
    
    strncpy(ind_msg->objName, obj_name, strlen(obj_name));
    strncpy(ind_msg->distName, dist_name, strlen(dist_name));
    ind_msg->dsType = EDistType_Data;
    ind_msg->data = data;
    Object_DBG("ObjectDataIndMsgSend: Indication Message send with payload %s/%s:%d dsType:%d",
                ind_msg->objName,
                ind_msg->distName,
                ind_msg->data,
                ind_msg->dsType);
    
    err = SysComSendMsg(msg);
    
ObjectDataIndMsgSend_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return err;
}

// Object Thread send Changed String Indication Message to subscribed Thread
static OSStatus ObjectStringIndMsgSend(char* obj_name, char* dist_name, char* str, SthreadId t_id)
{
    OSStatus err = kUnknownErr;
    SysComMsg* msg = NULL;
    SapiObjectInd *ind_msg = NULL;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    msg = SysComCreateMsg(API_OBJECT_IND_MSG, 
                        sizeof(SapiObjectInd), 
                        ESM_OBJECT_IND_TRANSID, 
                        ESM_OBJECT_THREAD_ID, 
                        t_id);
    if(msg == NULL) {
        Object_ERR("ObjectStringIndMsgSend: Message is NULL");
        err = kNoMsgErr;
        goto ObjectStringIndMsgSend_RET;
    }
    
    ind_msg = (SapiObjectInd*)SysComGetPayload(msg);
    
    strncpy(ind_msg->objName, obj_name, strlen(obj_name));
    strncpy(ind_msg->distName, dist_name, strlen(dist_name));
    ind_msg->dsType = EDistType_String;
    strncpy(ind_msg->str, str, strlen(str));
    Object_DBG("ObjectStringIndMsgSend: Indication Message send with payload %s/%s:%s dsType:%d",
                ind_msg->objName,
                ind_msg->distName,
                ind_msg->str,
                ind_msg->dsType);
    
    err = SysComSendMsg(msg);
    
ObjectStringIndMsgSend_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return err;
}

// Handle Subscribe/Query Message
static void ObjectHandleMessage()
{
    OSStatus err = kUnknownErr;
    SysComMsg* msg = NULL;
    
    Object_INF("ObjectHandleMessage: Started");
    err = SysComHandleMsg(msg, ESM_OBJECT_THREAD_ID, 100);
    if((err == kNoErr) && (msg != NULL)) {
        switch(msg->msgType) {
            case API_OBJECT_SUBSCRIBE_REQ_MSG:
                ObjectHandleSubscribeReqMessage(msg);
                break;
            case API_OBJECT_DISTNAME_QUERY_REQ_MSG:
                ObjectHandleDistQueryReqMessage(msg);
                break;
            default:
                break;
        }
        
        err = SysComDestroyMsg(msg);
        if(err != kNoErr) {
            Object_ERR("ObjectHandleMessage: SysComDestroyMsg failed with err_code(%d)", err);
        }
    }
}


// Handle Subscribe Message
static void ObjectHandleSubscribeReqMessage(SysComMsg* msg)
{
    OSStatus err = kUnknownErr;
    SapiObjectSubscribeReq* req_msg = NULL;
    
    if(msg == NULL) {
        Object_ERR("ObjectHandleSubscribeReqMessage: Message is NULL");
        return ;
    }
    
    if(msg->receiver != ESM_OBJECT_THREAD_ID) {
        Object_ERR("ObjectHandleSubscribeReqMessage: msg->receiver(%d) is not correct", msg->receiver);
        return ;
    }
    
    req_msg = (SapiObjectSubscribeReq*)SysComGetPayload(msg);
    if(req_msg == NULL) {
        Object_ERR("ObjectHandleSubscribeReqMessage: Payload is NULL");
        return ;
    }
    
    switch(req_msg->subscribeType) {
        case Subscribe:
            err = ObjectSubscribe(req_msg->objName, msg->sender);
            break;
        case Unsubscribe:
            err = ObjectUnsubscribe(req_msg->objName, msg->sender);
            break;
        default:
            Object_ERR("ObjectHandleSubscribeReqMessage: Unknow subscribeType(%d)", req_msg->subscribeType);
            break;
    }
    Object_DBG("ObjectHandleSubscribeReqMessage: Receive %s subscribeType:%d Subscribe Request Message",
                req_msg->objName,
                req_msg->subscribeType);
    
    err = ObjectSubscribeRespMsgSend(req_msg->objName, err, msg->sender, msg->transId);
    Object_DBG("ObjectHandleSubscribeReqMessage: ObjectSubscribeRespMsgSend sended with err_code(%d)", err);
}

// Object send Subscribe Response Message
static OSStatus ObjectSubscribeRespMsgSend(char* obj_name, OSStatus err_state, SthreadId receiver, u16 trans_id)
{
    OSStatus err = kUnknownErr;
    SysComMsg* msg = NULL;
    SapiObjectSubscribeResp* resp_msg = NULL;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    msg = SysComCreateMsg(API_OBJECT_SUBSCRIBE_RESP_MSG, 
                        sizeof(SapiObjectSubscribeResp), 
                        trans_id, 
                        ESM_OBJECT_THREAD_ID, 
                        receiver);
    if(msg == NULL) {
        Object_ERR("ObjectSubscribeRespMsgSend: Message is NULL");
        err = kNoMsgErr;
        goto ObjectSubscribeRespMsgSend_RET;
    }
    
    resp_msg = (SapiObjectSubscribeResp*)SysComGetPayload(msg);
    
    strncpy(resp_msg->objName, obj_name, strlen(obj_name));
    switch(err_state) {
        case kNoErr:
            resp_msg->status = ESubOK;
            strncpy(resp_msg->reason, "Success", strlen("Success"));
            break;
        case kSubExistErr:
            resp_msg->status = ESubREJECTED;
            strncpy(resp_msg->reason, "Already Exist", strlen("Already Exist"));
            break;
        default:
            resp_msg->status = ESubFAILED;
            strncpy(resp_msg->reason, "Failed", strlen("Failed"));
            break;
    }
    Object_DBG("ObjectSubscribeRespMsgSend: Subscribe response Message send with payload %s Status:%d Reason:%s",
                resp_msg->objName,
                resp_msg->status,
                resp_msg->reason);
    
    err = SysComSendMsg(msg);
    
ObjectSubscribeRespMsgSend_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return err;
}

// Handle Query Request Message
static void ObjectHandleDistQueryReqMessage(SysComMsg* msg)
{
    OSStatus err = kUnknownErr;
    SapiObjectDistNameQueryReq* req_msg = NULL;
    
    if(msg == NULL) {
        Object_ERR("ObjectHandleDistQueryReqMessage: Message is NULL");
        return ;
    }
    
    if(msg->receiver != ESM_OBJECT_THREAD_ID) {
        Object_ERR("ObjectHandleDistQueryReqMessage: msg->receiver(%d) is not correct", msg->receiver);
        return ;
    }
    
    req_msg = (SapiObjectDistNameQueryReq*)SysComGetPayload(msg);
    if(req_msg == NULL) {
        Object_ERR("ObjectHandleDistQueryReqMessage: Payload is NULL");
        return ;
    }
    
    Object_DBG("ObjectHandleDistQueryReqMessage: Receive %s/%s dsType:%d Query Request Message", req_msg->objName, req_msg->distName, req_msg->dsType);
    
    err = ObjectDistQueryRespMsgSend(req_msg->objName, req_msg->distName, req_msg->dsType, msg->sender, msg->transId);
    Object_DBG("ObjectHandleDistQueryReqMessage: ObjectDistQueryRespMsgSend sended with err_code(%d)", err);
}

// Object send Dist Query Response Message
static OSStatus ObjectDistQueryRespMsgSend(char* obj_name, char* dist_name, u8 ds, SthreadId receiver, u16 trans_id)
{
    OSStatus err = kUnknownErr;
    SysComMsg* msg = NULL;
    SapiObjectDistNameQueryResp* resp_msg = NULL;
    
    mico_rtos_lock_mutex(&ObjectMutex);
    
    msg = SysComCreateMsg(API_OBJECT_DISTNAME_QUERY_RESP_MSG, 
                        sizeof(SapiObjectDistNameQueryResp),
                        trans_id,
                        ESM_OBJECT_THREAD_ID,
                        receiver);
    if(msg == NULL) {
        Object_ERR("ObjectDistQueryRespMsgSend: Message is NULL");
        err = kNoMsgErr;
        goto ObjectDistQueryRespMsgSend_RET;
    }
    
    resp_msg = (SapiObjectDistNameQueryResp*)SysComGetPayload(msg);
    
    strncpy(resp_msg->objName, obj_name, strlen(obj_name));
    strncpy(resp_msg->distName, dist_name, strlen(dist_name));
    resp_msg->dsType = ds;
    
    switch(ds) {
        case EDistType_Data:
            err = ObjectGetValue(obj_name, dist_name, &(resp_msg->data));
            break;
        case EDistType_String:
            err = ObjectGetString(obj_name, dist_name, resp_msg->str);
            break;
        default:
            Object_ERR("ObjectDistQueryRespMsgSend: Unknow dsType(%d)", resp_msg->dsType);
            break;
    }
    
    switch(err) {
        case kNoErr:
            resp_msg->status = ESubOK;
            strncpy(resp_msg->reason, "Success", strlen("Success"));
            break;
        default:
            resp_msg->status = ESubFAILED;
            strncpy(resp_msg->reason, "Failed", strlen("Failed"));
            break;
    }
    
    if(ds == EDistType_Data) {
        Object_DBG("ObjectDistQueryRespMsgSend: Dist Query Request Message send with payload %s/%s:%d dsType:%d Status:%d Reason:%s",
                    resp_msg->objName,
                    resp_msg->distName,
                    resp_msg->data,
                    resp_msg->dsType,
                    resp_msg->status,
                    resp_msg->reason);
    }
    else if(ds == EDistType_String){
        Object_DBG("ObjectDistQueryRespMsgSend: Dist Query Request Message send with payload %s/%s:%s dsType:%d Status:%d Reason:%s",
                    resp_msg->objName,
                    resp_msg->distName,
                    resp_msg->str,
                    resp_msg->dsType,
                    resp_msg->status,
                    resp_msg->reason);
    }
    
    err = SysComSendMsg(msg);
    
ObjectDistQueryRespMsgSend_RET:
    mico_rtos_unlock_mutex(&ObjectMutex);
    
    return err;
}

// end of file


