
/***

History:
2015-09-06: Ted: Create

*/



#define API_BASE        0x1000
#define TRANSID_BASE    0x2000
#define THREAD_ID_BASE  0x3000

// ==== Element define ==== //

// EoperationStatus
typedef enum _EoperationStatus_t {
    OK = 0,
    REJECTED,
    FAILED
} EoperationStatus;

// EsubscribeType
typedef enum _EsubscribeType_t {
    Subscribe = 0,
    Unsubscribe
} EsubscribeType;

// ==== end of Element define ==== //

// ==== Message Type ==== //

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

// API_OBJECT_SUBSCRIBE_REQ_MSG
#define ESM_OBJECT_SUBSCRIBE_REQ_TRANSID        (TRANSID_BASE + 1)
// API_OBJECT_SUBSCRIBE_RESP_MSG
#define ESM_OBJECT_SUBSCRIBE_RESP_TRANSID       (TRANSID_BASE + 2)
// API_OBJECT_IND_MSG
#define ESM_OBJECT_IND_TRANSID                  (TRANSID_BASE + 3)
// API_OBJECT_DISTNAME_QUERY_REQ_MSG
#define ESM_OBJECT_DISTNAME_QUERY_REQ_TRANSID   (TRANSID_BASE + 4)
// API_OBJECT_DISTNAME_QUERY_RESP_MSG
#define ESM_OBJECT_DISTNAME_QUERY_RESP_TRANSID  (TRANSID_BASE + 5)

// ==== end of Message Transfer ID ==== //

// ==== Message Payload ==== //

// API_OBJECT_SUBSCRIBE_REQ_MSG
typedef struct _SapiObjectSubscribeReq_t {
    EsubscribeType  subscribeType;
    char            objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
} SapiObjectSubscribeReq;

// API_OBJECT_SUBSCRIBE_RESP_MSG
typedef struct _SapiObjectSubscribeResp_t {
    char             objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    EoperationStatus status;
    char             reason[ESM_OBJECT_REASON_MAX_LENGTH];
} SapiObjectSubscribeResp;

// API_OBJECT_IND_MSG
typedef struct _SapiObjectInd_t {
    char        objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    char        distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
    u16         data;
} SapiObjectInd;

// API_OBJECT_DISTNAME_QUERY_REQ_MSG
typedef struct _SapiObjectDistNameQueryReq_t {
    char        objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    char        distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
} SapiObjectDistNameQueryReq;

// API_OBJECT_DISTNAME_QUERY_RESP_MSG
typedef struct _SapiObjectDistNameQueryResp_t {
    EoperationStatus status;
    char             reason[ESM_OBJECT_REASON_MAX_LENGTH];
    char             objName[ESM_OBJECT_OBJECT_NAME_MAX_LENGTH];
    char             distName[ESM_OBJECT_DIST_NAME_MAX_LENGTH];
    u16              data;
} SapiObjectDistNameQueryResp;

// ==== end of Message Payload ==== //



// end of file


