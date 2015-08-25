/**
******************************************************************************
* @file    user_main.c 
* @author  Eshen Wang
* @version V1.0.0
* @date    14-May-2015
* @brief   user main functons in user_main thread.
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
******************************************************************************
*/ 

#include "MICODefine.h"
#include "MicoFogCloud.h"
#include "user_uart.h"

#define user_log(M, ...) custom_log("USER", M, ##__VA_ARGS__)
#define user_log_trace() custom_log_trace("USER")


/* MICO user callback: Restore default configuration provided by user
* called when Easylink buttion long pressed
*/
void userRestoreDefault_callback(mico_Context_t *mico_context)
{
  //user_log("INFO: restore user configuration.");
}


/* user main function, called by AppFramework after FogCloud connected.
*/
OSStatus user_main( mico_Context_t * const mico_context )
{
  user_log_trace();
  OSStatus err = kUnknownErr;
  fogcloud_msg_t *recv_msg = NULL;
  
  unsigned char* responseMsg = NULL;
  int responseMsgLen = 0;
  unsigned char* ptr = NULL;
    
  require(mico_context, exit);
  
  //init user uart
  err = user_uart_init(mico_context);
  require_noerr_action(err, exit, 
                       user_log("ERROR: user uart init failed!") );
  
  while(1){
    
    // get msg from cloud
    // msg format: recv_msg->data = <topic><data>
    err = MicoFogCloudMsgRecv(mico_context, &recv_msg, 200);
    if(kNoErr == err){
      user_log("Cloud => Module: topic[%d]=[%.*s]\tdata[%d]=[%.*s]", 
               recv_msg->topic_len, recv_msg->topic_len, recv_msg->data, 
               recv_msg->data_len, recv_msg->data_len, recv_msg->data + recv_msg->topic_len);
      
      // transfer msg to uart
      err = user_uart_send(recv_msg->data + recv_msg->topic_len , recv_msg->data_len);
      if(kNoErr == err){
        user_log("Msg send to uart success!");
      }
      else{
        user_log("Msgsend to uart failed! err=%d.", err);
      }
      
      // add prefix MAC && echo to cloud
      // responseTopic = device_id/out, message = [MAC]msg
      responseMsgLen = strlen(mico_context->micoStatus.mac) + 2 + recv_msg->data_len;
      responseMsg = (unsigned char*)malloc(responseMsgLen + 1);
      if(NULL == responseMsg){
        err = kNoMemoryErr;
        user_log("Handle cloud msg: no memory error!");
      }
      else{
        memset(responseMsg, 0x00, responseMsgLen);
        ptr = responseMsg;
        memcpy(ptr, "[", 1);
        ptr += 1;
        memcpy(ptr, (const void*)&(mico_context->micoStatus.mac), strlen(mico_context->micoStatus.mac));
        ptr += strlen(mico_context->micoStatus.mac);
        memcpy(ptr, "]", 1);
        ptr += 1;
        memcpy(ptr, recv_msg->data + recv_msg->topic_len, recv_msg->data_len);
        ptr += recv_msg->data_len;
        memcpy(ptr, '\0', 1);
        
        err = MicoFogCloudMsgSend(mico_context, NULL, 0, responseMsg, responseMsgLen);
        if(NULL != responseMsg){
          ptr = NULL;
          free(responseMsg);
        }
        
        if(kNoErr == err){
          user_log("echo msg to cloud success!");
        }
        else{
          user_log("echo msg to cloud error, err=%d.", err);
        }
      }
      
      // NOTE: must free msg memory after been used.
      if(NULL != recv_msg){
        free(recv_msg);
        recv_msg = NULL;
      }
    }
  }
  
exit:
  user_log("ERROR: user_main exit with err=%d", err);
  return err;
}
