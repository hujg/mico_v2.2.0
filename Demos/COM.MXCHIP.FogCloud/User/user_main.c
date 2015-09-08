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


int energy = 0;
int interval = 0;
int lights = 0;
int remind = 0;
int volume = 0;
bool subscribe = true;
char* track = NULL;
char* url_path = NULL;


static mico_thread_t downstream_thread_handle = NULL;
static mico_thread_t upstream_thread_handle = NULL;

extern void upstream_thread(void* arg);
extern void downstream_thread(void* arg);

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
  
  require(mico_context, exit);
  
  energy = 90;
  interval = 30;
  lights = 10;
  remind = 30;
  volume = 10;
  track = "Yesterday";
  url_path = "www.mp3.com";
  
  // start the downstream thread to handle user command
  err = mico_rtos_create_thread(&downstream_thread_handle, MICO_APPLICATION_PRIORITY, "downstream", 
                                downstream_thread, STACK_SIZE_DOWNSTREAM_THREAD, 
                                mico_context );
  require_noerr_action( err, exit, user_log("ERROR: create downstream thread failed!") );
    
  // start the upstream thread to upload temperature && humidity to user
  err = mico_rtos_create_thread(&upstream_thread_handle, MICO_APPLICATION_PRIORITY, "upstream", 
                                upstream_thread, STACK_SIZE_UPSTREAM_THREAD, 
                                mico_context );
  require_noerr_action( err, exit, user_log("ERROR: create uptream thread failed!") );
  
  user_log("user_main: start");
  while(1){

    mico_thread_sleep(10);
    user_log("user_main: Running");

    // test
    if(!MicoFogCloudIsConnect(mico_context)) {
        user_log("appStatus.fogcloudStatus.isCloudConnected = false");
        continue;
    }
    
  }
  
exit:
  user_log("ERROR: user_main exit with err=%d", err);
  return err;
}
