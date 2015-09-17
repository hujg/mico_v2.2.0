
/***

History:
2015-09-06: Ted: Create

*/

#ifndef _API_MSG_H
#define _API_MSG_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
#include <Object.h>


#define API_BASE        0x1000
#define TRANSID_BASE    0x2000

// ==== Element define ==== //

// EoperationStatus
typedef enum _ESubOperationStatus_t {
    ESubOK = 0,
    ESubREJECTED,
    ESubFAILED
} ESubOperationStatus;

// EsubscribeType
typedef enum _EsubscribeType_t {
    Subscribe = 0,
    Unsubscribe
} EsubscribeType;

// ==== end of Element define ==== //

// ==== Message Type ==== //

typedef u16     MsgType_t;

// SapiObjectSubscribeReq
#define API_OBJECT_SUBSCRIBE_REQ_MSG        (API_BASE + 1)
// SapiObjectSubscribeResp
#define API_OBJECT_SUBSCRIBE_RESP_MSG       (API_BASE + 2)
// SapiObjectInd
#define API_OBJECT_IND_MSG                  (API_BASE + 3)
// SapiObjectDistNameQueryReq
#define API_OBJECT_DISTNAME_QUERY_REQ_MSG   (API_BASE + 4)
// SapiObjectDistNameQueryResp
#define API_OBJECT_DISTNAME_QUERY_RESP_MSG  (API_BASE + 5)

// ==== end of Message Type ==== //

// ==== Message Transfer ID ==== //

// API_OBJECT_SUBSCRIBE_REQ_MSG/API_OBJECT_SUBSCRIBE_RESP_MSG
#define ESM_OBJECT_SUBSCRIBE_TRANSID        (TRANSID_BASE + 1)
// API_OBJECT_IND_MSG
#define ESM_OBJECT_IND_TRANSID              (TRANSID_BASE + 2)
// API_OBJECT_DISTNAME_QUERY_REQ_MSG/API_OBJECT_DISTNAME_QUERY_RESP_MSG
#define ESM_OBJECT_DISTNAME_QUERY_TRANSID   (TRANSID_BASE + 3)

// ==== end of Message Transfer ID ==== //

// message format define
typedef struct _SysComMsg_t {
    SthreadId   sender;
    SthreadId   receiver;
    MsgType_t   msgType;
    u16         transId;
    void*       payload;
} SysComMsg;

// ==== Message Payload ==== //

// API_OBJECT_SUBSCRIBE_REQ_MSG
typedef struct _SapiObjectSubscribeReq_t {
    EsubscribeType  subscribeType;
    char            objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
} SapiObjectSubscribeReq;

// API_OBJECT_SUBSCRIBE_RESP_MSG
typedef struct _SapiObjectSubscribeResp_t {
    char                objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    ESubOperationStatus status;
    char                reason[ESM_OBJECT_REASON_MAX_LENGTH];
} SapiObjectSubscribeResp;

// API_OBJECT_IND_MSG
typedef struct _SapiObjectInd_t {
    char    objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    char    distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
    u8      dsType;
    u16     data;
    char    str[ESM_OBJECT_DIST_STRING_MAX_LENGTH];
} SapiObjectInd;

// API_OBJECT_DISTNAME_QUERY_REQ_MSG
typedef struct _SapiObjectDistNameQueryReq_t {
    char    objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    char    distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
    u8      dsType;
} SapiObjectDistNameQueryReq;

// API_OBJECT_DISTNAME_QUERY_RESP_MSG
typedef struct _SapiObjectDistNameQueryResp_t {
    char    objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    char    distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
    u8      dsType;
    u16     data;
    char    str[ESM_OBJECT_DIST_STRING_MAX_LENGTH];
    ESubOperationStatus status;
    char    reason[ESM_OBJECT_REASON_MAX_LENGTH];
} SapiObjectDistNameQueryResp;

// ==== end of Message Payload ==== //


#ifdef __cplusplus
}
#endif

#endif // _API_MSG_H

// end of file


