/**
  ******************************************************************************
  * @file    user_main.c 
  * @author  Eshen Wang
  * @version V1.0.0
  * @date    17-Mar-2015
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
#include "fogcloud_msg_dispatch.h"

#include "user_properties.h"
#include "micokit_ext.h"

#define user_log(M, ...) custom_log("USER", M, ##__VA_ARGS__)
#define user_log_trace() custom_log_trace("USER")


/* properties defined in user_properties.c by user
 */
// device services &&¡¡properties table
extern struct mico_service_t  service_table[];
// user context
extern user_context_t g_user_context;


/* MICO user callback: Restore default configuration provided by user
 * called when Easylink buttion long pressed
 */
void userRestoreDefault_callback(mico_Context_t *mico_context)
{
  //user_log("INFO: restore user configuration.");
}

/* FogCloud message receive callback: handle cloud messages here
 */
OSStatus user_fogcloud_msg_handler(mico_Context_t* mico_context, 
                            const char* topic, const unsigned int topicLen,
                            unsigned char *inBuf, unsigned int inBufLen)
{
  user_log_trace();
  OSStatus err = kUnknownErr;
  mico_fogcloud_msg_t fogcloud_msg;
  int retCode = MSG_PROP_UNPROCESSED;
  
  if((NULL == mico_context) || (NULL == topic) || (0 == topicLen) ) {
    user_log("ERROR: mico_cloudmsg_dispatch params error, err=%d", err);
    return kParamErr;
  }
  
  fogcloud_msg.topic = topic;
  fogcloud_msg.topic_len = topicLen;
  fogcloud_msg.data = inBuf;
  fogcloud_msg.data_len = inBufLen;
  
  err = mico_fogcloud_msg_dispatch(mico_context, service_table, &fogcloud_msg, &retCode);    
  if(kNoErr != err){
    user_log("ERROR: mico_cloudmsg_dispatch error, err=%d", err);
  }
  else
  {
    if(MSG_PROP_WROTE == retCode){
      g_user_context.status.user_config_need_update = true;  // user params need update in flash
    }
  }
  
  return err;
}

//---------------------------- User work function ------------------------------

//--- show actions for user, such as OLED, RGB_LED, DC_Motor
void system_state_display( mico_Context_t * const mico_context, user_context_t *user_context)
{
  // display H/T on OLED
  char temp_hum_str[16] = {'\0'};

  if(!mico_context->appStatus.isWifiConnected){
    snprintf(temp_hum_str, 16, "Conn %s", mico_context->flashContentInRam.micoSystemConfig.ssid);
  }
  else if(!mico_context->appStatus.fogcloudStatus.isCloudConnected){
    if(mico_context->flashContentInRam.appConfig.fogcloudConfig.isActivated){
      snprintf(temp_hum_str, 16, "%s", "Conn FogCloud... ");
    }
    else{
      snprintf(temp_hum_str, 16, "%s", "Wait activate...");
    }
  }
  else{
    // temperature/humidity display on OLED
    snprintf(temp_hum_str, 16, "T: %2dC  H: %2d%%  ", user_context->status.temperature, user_context->status.humidity);
  }
  OLED_ShowString(0, 6, (uint8_t*)temp_hum_str);
}

/* user main function, called by AppFramework after FogCloud connected.
 */
OSStatus user_main( mico_Context_t * const mico_context )
{
  user_log_trace();
  OSStatus err = kUnknownErr;
  
#if (MICO_CLOUD_TYPE != CLOUD_DISABLED)
  /* start properties notify task */
  err = mico_start_properties_notify(mico_context, service_table, 
                                     MICO_PROPERTIES_NOTIFY_INTERVAL_MS, 
                                     STACK_SIZE_NOTIFY_THREAD);
  require_noerr_action( err, exit, user_log("ERROR: mico_start_properties_notify err = %d.", err) );
#endif
  
  while(1){
    
    // check every 1 seconds
    mico_thread_sleep(1);
    
    // system work state show on OLED
    system_state_display(mico_context, &g_user_context);
  }
  
exit:
  user_log("ERROR: user_main exit with err=%d", err);
  return err;
}
