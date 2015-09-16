
/***

History:
2015-09-06: Ted: Create

*/

#ifndef _SYSCOM_H
#define _SYSCOM_H

#ifdef __cplusplus
 extern "C" {
#endif 


#include <Object_int.h>
#include <ThreadMng.h>
// for MICO
#include "MICO.h"


void SysComInit();
SysComMsg *SysComCreateMsg(MsgType_t msgtype, u16 sizeofmsg, u16 transid, SthreadId sender, SthreadId receiver);
OSStatus SysComDestroyMsg(SysComMsg *msg);
void *SysComGetPayload(SysComMsg* msg);
SthreadId SysComGetSender(SysComMsg* msg);
void SysComSetSender(SysComMsg* msg, SthreadId sender);
SthreadId SysComGetReceiver(SysComMsg* msg);
void SysComSetReceiver(SysComMsg* msg, SthreadId receiver);
OSStatus SysComSendMsg(SysComMsg *msg);
OSStatus SysComHandleMsg(void* msg, SthreadId tid, u32 timeout_ms);


#ifdef __cplusplus
}
#endif

#endif // _SYSCOM_H

// end of file
