


#include <Object.h>
#include <SysCom.h>
#include <API_MSG.h>
// for MICO
#include "MICO.h"

// for MICO
#define Music_DBG(M, ...) custom_log("Object > DBG", M, ##__VA_ARGS__)
#define Music_ERR(M, ...) custom_log("Object > ERR", M, ##__VA_ARGS__)
#define Music_Log_Trace() custom_log_trace("OBJECTMODULE")

static OSStatus MusicSubscribeObjMusicReqMsg(char* obj_name);
static void MusicHandleMessage();


void Music_Thread(void* arg)
{
    Music_Log_Trace();
    
    MusicSubscribeObjMusicReqMsg("Music-1");
    
    while(1) {
        MusicHandleMessage();
    }
}

static OSStatus MusicSubscribeObjMusicReqMsg(char* obj_name)
{
    OSStatus err = kUnknownErr;
    SysComMsg* msg = NULL;
    SapiObjectSubscribeReq* req_msg = NULL;
    
    msg = SysComCreateMsg(API_OBJECT_SUBSCRIBE_REQ_MSG,
                        sizeof(SapiObjectSubscribeReq),
                        ESM_OBJECT_SUBSCRIBE_TRANSID,
                        ESM_MUSIC_THREAD_ID,
                        ESM_OBJECT_THREAD_ID);
                        
    if(msg == NULL) {
        Music_ERR("MusicSubscribeObjMusicReqMsg: Message is NULL");
        err = kNoMsgErr;
    }
    
    req_msg = (SapiObjectSubscribeReq*)SysComGetPayload(msg);
    strncpy(req_msg->objName, obj_name, strlen(obj_name));
    req_msg->subscribeType = Subscribe;
    
    Music_DBG("MusicSubscribeObjMusicReqMsg: Music Thread Subscribe %s", req_msg->objName);
    
    err = SysComSendMsg(msg);
    
    return err;
}

static void MusicHandleMessage()
{
    OSStatus err = kUnknownErr;
    SysComMsg* msg = NULL;
    SapiObjectInd* ind_msg = NULL;
    
    err = SysComHandleMsg(msg, ESM_MUSIC_THREAD_ID, 100);
    if(err != kNoErr) {
        return ;
    }
    
    if(msg == NULL) {
        Music_ERR("MusicHandleMessage: Message is NULL");
        return ;
    }
    
    if(msg->receiver != ESM_MUSIC_THREAD_ID) {
        Music_ERR("MusicHandleMessage: Message Receiver(%d) is not Music Thread", msg->receiver);
        return ;
    }
    
    Music_DBG("MusicHandleMessage: TransID(0x%04X)", msg->transId);
    
    ind_msg = (SapiObjectInd*)SysComGetPayload(msg);
    
    if(ind_msg->dsType == EDistType_Data) {
        Music_DBG("MusicHandleMessage: Receive Indication Message %s/%s:%d dsType:%d", ind_msg->objName, ind_msg->distName, ind_msg->data, ind_msg->dsType);
    }
    else {
        Music_DBG("MusicHandleMessage: Receive Indication Message %s/%s:%s dsType:%d", ind_msg->objName, ind_msg->distName, ind_msg->str, ind_msg->dsType);
    }
}


