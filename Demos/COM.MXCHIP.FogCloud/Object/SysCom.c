
/***

History:
2015-09-06: Ted: Create

*/

#include <stdio.h>
#include <SysCom.h>
#include <MemMng.h>


// for MICO
#define SysCom_DBG(M, ...) custom_log("SysCom > DBG", M, ##__VA_ARGS__)
#define SysCom_ERR(M, ...) custom_log("SysCom > ERR", M, ##__VA_ARGS__)


// max number of SysCom
#define MAX_SYSCOM_NUM      10
// max string length of SysCom queue name
#define ESM_SYSCOM_QUEUE_NAME_MAX_LENGTH    16
// size of struct SysComMsg
#define SIZEOF_STRUCT_SYSCOM    sizeof(SysComMsg)


static mico_queue_t SysComQueue[MAX_THREAD_NUM];
static mico_mutex_t SysComMutex = NULL;



void SysComInit()
{
    u8 i;
    char queue_name[ESM_SYSCOM_QUEUE_NAME_MAX_LENGTH];
    OSStatus err = kUnknownErr;
    
    err = mico_rtos_init_mutex(&SysComMutex);
    if(err != kNoErr) {
        SysCom_ERR("SysComInit: mutex initialized failed");
    }
    
    for(i=0; i<MAX_THREAD_NUM; i++) {
        sprintf(queue_name, "SysCom Queue %d", i);
        err = mico_rtos_init_queue(&SysComQueue[i], queue_name, sizeof(addP_t), MAX_SYSCOM_NUM);
        if(err != kNoErr) {
            SysCom_ERR("SysComInit: SysComQueue[%d] initialized failed", i);
        }
    }
    
    SysCom_DBG("SysComInit() finished");
}

SysComMsg *SysComCreateMsg(MsgType_t msgtype, u16 sizeofmsg, u16 transid, SthreadId sender, SthreadId receiver)
{
    SysComMsg* msg;
    void* payload;
    
    mico_rtos_lock_mutex(&SysComMutex);
    
    msg = (SysComMsg*)MemMalloc(SIZEOF_STRUCT_SYSCOM);
    if(msg == NULL) {
        SysCom_ERR("SysComCreateMsg: msg malloc failed");
    }
    else {
        SysCom_DBG("SysComCreateMsg: malloc msg(addr: 0x%08X) successfully", (addP_t)msg);
        payload = MemMalloc(sizeofmsg);
        if(payload == NULL) {
            SysCom_ERR("SysComCreateMsg: malloc payload failed");
            SysComDestroyMsg(msg);
            SysCom_DBG("SysComCreateMsg: msg destroyed");
            msg = NULL;
        }
        else {
            SysCom_DBG("SysComCreateMsg: malloc payload(addr: 0x%08X) successfully", (addP_t)payload);
            msg->sender = sender;
            msg->receiver = receiver;
            msg->msgType = msgtype;
            msg->transId = transid;
            msg->payload = payload;
        }
    }
    
    mico_rtos_unlock_mutex(&SysComMutex);
    
    return (SysComMsg*)msg;
}

OSStatus SysComDestroyMsg(SysComMsg *msg)
{
    OSStatus err = kUnknownErr;
    
    mico_rtos_lock_mutex(&SysComMutex);
    
    if(msg == NULL) {
        SysCom_ERR("SysComDestroyMsg: msg is NULL");
        err = kParamErr;
    }
    else {
        if(msg->payload == NULL) {
            SysCom_ERR("SysComDestroyMsg: payload is NULL");
            err = kParamErr;
        }
        else {
            MemFree(msg->payload);
        }
        
        MemFree(msg);
        SysCom_DBG("SysComDestroyMsg: msg destroyed");
        err = kNoErr;
    }
        
    mico_rtos_unlock_mutex(&SysComMutex);
    
    return err;
}

void *SysComGetPayload(SysComMsg* msg)
{
    mico_rtos_lock_mutex(&SysComMutex);
    
    SysCom_DBG("SysComGetPayload: get the msg(addr: 0x%08X) payload(addr: 0x%08X)", (addP_t)msg, (addP_t)msg->payload);
    
    mico_rtos_unlock_mutex(&SysComMutex);
    
    return msg->payload;
}

SthreadId SysComGetSender(SysComMsg* msg)
{
    mico_rtos_lock_mutex(&SysComMutex);
    
    SysCom_DBG("SysComGetSender: get msg(addr: 0x%08X) sender(%d)", (addP_t)msg, msg->sender);
    
    mico_rtos_unlock_mutex(&SysComMutex);
    
    return msg->sender;
}

void SysComSetSender(SysComMsg* msg, SthreadId sender)
{
    mico_rtos_lock_mutex(&SysComMutex);
    
    msg->sender = sender;
    SysCom_DBG("SysComSetSender: set msg(addr: 0x%08X) sender(%d)", (addP_t)msg, msg->sender);
    
    mico_rtos_unlock_mutex(&SysComMutex);
}

SthreadId SysComGetReceiver(SysComMsg* msg)
{
    mico_rtos_lock_mutex(&SysComMutex);
    
    SysCom_DBG("SysComGetReceiver: get msg(addr: 0x%08X) receiver(%d)", (addP_t)msg, msg->receiver);
    
    mico_rtos_unlock_mutex(&SysComMutex);
    
    return msg->receiver;
}

void SysComSetReceiver(SysComMsg* msg, SthreadId receiver)
{
    mico_rtos_lock_mutex(&SysComMutex);
    
    msg->receiver = receiver;
    SysCom_DBG("SysComSetReceiver: set msg(addr: 0x%08X) receiver(%d)", (addP_t)msg, msg->receiver);
    
    mico_rtos_unlock_mutex(&SysComMutex);
}

OSStatus SysComSendMsg(SysComMsg *msg)
{
    OSStatus err = kUnknownErr;
    
    mico_rtos_lock_mutex(&SysComMutex);
    
    err = mico_rtos_push_to_queue(&SysComQueue[msg->receiver], &msg, 0);
    if(err != kNoErr) {
        SysCom_ERR("SysComSend: msg(addr: 0x%08X) push to SysComQueue[%d] failed with err(%d)", (addP_t)msg, msg->receiver, err);
    }
    else {
        SysCom_DBG("SysComSend: msg(addr: 0x%08X) push to SysComQueue[%d] successfully", (addP_t)msg, msg->receiver);
    }
    
    mico_rtos_unlock_mutex(&SysComMutex);
    
    return err;
}

OSStatus SysComHandleMsg(void* msg, SthreadId tid, u32 timeout_ms)
{
    OSStatus err = kUnknownErr;
    void* queue_msg = NULL;
    
    mico_rtos_lock_mutex(&SysComMutex);
    
    err = mico_rtos_pop_from_queue(&SysComQueue[tid], &queue_msg, timeout_ms);
    if(err != kNoErr) {
        SysCom_ERR("SysComHandleMsg: Receive queue_msg failed with err(%d)", err);
    }
    else {
      SysCom_DBG("SysComHandleMsg: msg(value: 0x%08X, addr: 0x%08X)", (addP_t)msg, (addP_t)&msg);
      SysCom_DBG("SysComHandleMsg: queue_msg(value: 0x%08X, addr: 0x%08X) pop from SysComQueue[%d] successfully", (addP_t)queue_msg, (addP_t)&queue_msg, tid);
      *(addP_t*)msg = (addP_t)queue_msg;
      SysCom_DBG("SysComHandleMsg: msg(value: 0x%08X, addr: 0x%08X)", (addP_t)msg, (addP_t)&msg);
      SysCom_DBG("SysComHandleMsg: queue_msg(value: 0x%08X, addr: 0x%08X)", (addP_t)queue_msg, (addP_t)&queue_msg);
    }
    
    mico_rtos_unlock_mutex(&SysComMutex);
    
    return err;
}



// end of file


