/**
  ******************************************************************************
  * @file    MICOAppEntrance.c 
  * @author  William Xu
  * @version V1.0.0
  * @date    05-May-2014
  * @brief   Mico application entrance, addd user application functons and threads.
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
#include "MICOAppDefine.h"
#include "MicoFogCloud.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)
#define app_log_trace() custom_log_trace("APP")

/* default user_main callback function, this must be override by user. */
WEAK OSStatus user_main( mico_Context_t * const mico_context )
{
  app_log("ERROR: user_main undefined!");
  return kNotHandledErr;
}

/* default user_main callback function, this may be override by user. */
WEAK void userRestoreDefault_callback(mico_Context_t *mico_context)
{
  //app_log("INFO: call default userRestoreDefault_callback, do nothing!");  // log in ISR may cause error
}

/* user main thread created by MICO APP thread */
void user_main_thread(void* arg)
{
  OSStatus err = kUnknownErr;
  mico_Context_t *mico_context = (mico_Context_t *)arg;
  
#if (MICO_CLOUD_TYPE != CLOUD_DISABLED)
  // wait semaphore for cloud connection
  //mico_fogcloud_waitfor_connect(mico_context, MICO_WAIT_FOREVER);  // block to wait fogcloud connect
  //app_log("Cloud connected, do user_main function.");
#endif
  
  // loop in user mian function && must not return
  err = user_main(mico_context);
  UNUSED_PARAMETER(err);
  
  // never get here only if user work error.
  app_log("ERROR: user_main thread exit err=%d, system reboot...", err);
  MicoSystemReboot();
}

OSStatus startUserMainThread(mico_Context_t *mico_context)
{
  app_log_trace();
  OSStatus err = kNoErr;
  require_action(mico_context, exit, err = kParamErr);
  
  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "user_main", 
                                user_main_thread, STACK_SIZE_USER_MAIN_THREAD, 
                                mico_context );
exit:
  return err;
}

/* MICO system callback: Restore default configuration provided by application */
void appRestoreDefault_callback(mico_Context_t *mico_context)
{
  mico_context->flashContentInRam.appConfig.configDataVer = CONFIGURATION_VERSION;
  mico_context->flashContentInRam.appConfig.bonjourServicePort = BONJOUR_SERVICE_PORT;
  
  // restore fogcloud config
  MicoFogCloudRestoreDefault(mico_context);
  // restore user config
  userRestoreDefault_callback(mico_context);
}

/* MICO APP entrance */
OSStatus MICOStartApplication( mico_Context_t * const mico_context )
{
  app_log_trace();
  OSStatus err = kNoErr;
    
  require_action(mico_context, exit, err = kParamErr);
    
  // LED on when Wi-Fi connected.
  MicoSysLed(false);
    
  /* Bonjour for service searching */
  if(mico_context->flashContentInRam.micoSystemConfig.bonjourEnable == true) {
    MICOStartBonjourService( Station, mico_context );
  }
  
  /* start cloud service */
#if (MICO_CLOUD_TYPE == CLOUD_FOGCLOUD)
  err = MicoStartFogCloudService( mico_context );
  app_log("MICO CloudService: FogCloud.");
  require_noerr_action( err, exit, app_log("ERROR: Unable to start FogCloud service.") );
#elif (MICO_CLOUD_TYPE == CLOUD_ALINK)
  app_log("MICO CloudService: Alink.");
#elif (MICO_CLOUD_TYPE == CLOUD_DISABLED)
  app_log("MICO CloudService: disabled.");
#else
  #error "MICO cloud service type is not defined"?
#endif
  
  /* start user thread */
  err = startUserMainThread( mico_context );
  require_noerr_action( err, exit, app_log("ERROR: start user_main thread failed!") );

exit:
  return err;
}
