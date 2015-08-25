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
  json_object *send_json_object = NULL;
  const char *upload_data = NULL;

  int energy_info = 100;
  
  require(mico_context, exit);
  
  user_log("user_main: start");
  while(1){

    mico_thread_sleep(1);
    user_log("ongoing...");

    // test
    if(!MicoFogCloudIsConnect(mico_context)) {
        user_log("appStatus.fogcloudStatus.isCloudConnected = false");
        continue;
    }
    
    send_json_object = json_object_new_object();
    if(NULL == send_json_object){
        user_log("create json object error!");
        err = kNoMemoryErr;
    }
    else {
        json_object_object_add(send_json_object, "energy_info", json_object_new_int(energy_info)); 
        upload_data = json_object_to_json_string(send_json_object);
        if(NULL == upload_data){
            user_log("create upload data string error!");
            err = kNoMemoryErr;
        }
        else {
            MicoFogCloudMsgSend(mico_context, NULL, 0, (unsigned char*)upload_data, strlen(upload_data));
            user_log("ready sended, %s", (unsigned char*)upload_data);
            err = kNoErr;
        }
        // free json object memory
        json_object_put(send_json_object);
        send_json_object = NULL;
        user_log("free json_object_put");
    }
    
    
    // recv_msg->data = <topic><data>
    err = MicoFogCloudMsgRecv(mico_context, &recv_msg, 200);
    user_log("err = %d", err);
    if(kNoErr == err){
      user_log("Msg recv: topic[%d]=[%.*s]\tdata[%d]=[%.*s]", 
               recv_msg->topic_len, recv_msg->topic_len, recv_msg->data, 
               recv_msg->data_len, recv_msg->data_len, recv_msg->data + recv_msg->topic_len);
      
      // send msg to topic: "device_id/out"
      err = MicoFogCloudMsgSend(mico_context, NULL, 0, recv_msg->data + recv_msg->topic_len , recv_msg->data_len);
      if(kNoErr == err){
        user_log("Msg echo success!");
      }
      else{
        user_log("Msg echo failed! err=%d.", err);
      }
      
      // NOTE: free msg memory after used.
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
