/**
******************************************************************************
* @file    MicoFogCloud.c 
* @author  Eshen Wang
* @version V1.0.0
* @date    17-Mar-2015
* @brief   This file contains the implementations of cloud service interfaces 
*          for MICO.
operation
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
#include "MICONotificationCenter.h"

#include "MicoFogCloud.h"
#include "fogcloud.h"
#include "FogCloudUtils.h"

#define fogcloud_log(M, ...) custom_log("MicoFogCloud", M, ##__VA_ARGS__)
#define fogcloud_log_trace() custom_log_trace("MicoFogCloud")

// cloud status
#define DEFAULT_MicoFogCloud_CLOUD_CONNECTED_MSG_2CLOUD     "{\"MicoFogCloud\":\"connected\"}"
#define DEFAULT_MicoFogCloud_CLOUD_CONNECTED_MSG_2MCU       "[MicoFogCloud]Cloud: connected\r\n"
#define DEFAULT_MicoFogCloud_CLOUD_DISCONNECTED_MSG_2MCU    "[MicoFogCloud]Cloud: disconnected\r\n"

// wifi status
#define DEFAULT_MicoFogCloud_STATION_UP_MSG_2MCU            "[MicoFogCloud]Wi-Fi: Station up\r\n"
#define DEFAULT_MicoFogCloud_STATION_DOWN_MSG_2MCU          "[MicoFogCloud]Wi-Fi: Station down\r\n"

// OTA status
#define DEFAULT_MicoFogCloud_OTA_CHECK_MSG_2MCU             "[MicoFogCloud]OTA: Checking ...\r\n"
#define DEFAULT_MicoFogCloud_OTA_UPDATE_MSG_2MCU            "[MicoFogCloud]OTA: Update && reboot ...\r\n"
#define DEFAULT_MicoFogCloud_OTA_UP_TO_DATE_MSG_2MCU        "[MicoFogCloud]OTA: Up-to-date\r\n"
#define DEFAULT_MicoFogCloud_OTA_DOWNLOAD_FAILED_MSG_2MCU   "[MicoFogCloud]OTA: Download failed\r\n"

// dev activate status
#define DEFAULT_MicoFogCloud_DEV_ACTIVATE_START_MSG_2MCU    "[MicoFogCloud]Activate: Start ...\r\n"
#define DEFAULT_MicoFogCloud_DEV_ACTIVATE_OK_MSG_2MCU       "[MicoFogCloud]Activate: Success\r\n"
#define DEFAULT_MicoFogCloud_DEV_ACTIVATE_FAILED_MSG_2MCU   "[MicoFogCloud]Activate: Failed\r\n"

// restore config status
#define DEFAULT_MicoFogCloud_RESET_CLOUD_INFO_START_MSG_2MCU   "[MicoFogCloud]CloudReset: Start ...\r\n"
#define DEFAULT_MicoFogCloud_RESET_CLOUD_INFO_OK_MSG_2MCU      "[MicoFogCloud]CloudReset: Success\r\n"
#define DEFAULT_MicoFogCloud_RESET_CLOUD_INFO_FAILED_MSG_2MCU  "[MicoFogCloud]CloudReset: Failed\r\n"


static mico_semaphore_t _wifi_station_on_sem = NULL;
static mico_semaphore_t _reset_cloud_info_sem = NULL;
mico_semaphore_t _fogcloud_connect_sem = NULL;

extern OSStatus MicoStartFogCloudConfigServer ( mico_Context_t * const inContext );
extern void  set_RF_LED_cloud_connected     ( mico_Context_t * const inContext );
extern void  set_RF_LED_cloud_disconnected  ( mico_Context_t * const inContext );

// override by user in user_main.c
WEAK OSStatus user_fogcloud_msg_handler(mico_Context_t* context, 
                            const char* topic, const unsigned int topicLen,
                            unsigned char *inBuf, unsigned int inBufLen)
{
  fogcloud_log("WARNING: user_fogcloud_msg_handler not override, use default, do nothing.");
  return kNoErr;
}

void mvdNotify_WifiStatusHandler(WiFiEvent event, mico_Context_t * const inContext)
{
  fogcloud_log_trace();
  (void)inContext;
  switch (event) {
  case NOTIFY_STATION_UP:
    inContext->appStatus.isWifiConnected = true;
    if(NULL == _wifi_station_on_sem){
      mico_rtos_init_semaphore(&_wifi_station_on_sem, 1);
    }
    mico_rtos_set_semaphore(&_wifi_station_on_sem);
    break;
  case NOTIFY_STATION_DOWN:
    inContext->appStatus.isWifiConnected = false;
    break;
  case NOTIFY_AP_UP:
    break;
  case NOTIFY_AP_DOWN:
    break;
  default:
    break;
  }
  return;
}

#define DEVICE_RESET_RETRY_CNT    3
OSStatus easycloud_reset_cloud_info(mico_Context_t * const context)
{
  OSStatus err = kUnknownErr;
  MVDResetRequestData_t devDefaultResetData;
  mico_Context_t *inContext = (mico_Context_t *)context;
  int retry_cnt = 1;
  
  do{
    /* cloud context init */
    err = fogCloudInit(inContext);
    if(kNoErr == err){
      fogcloud_log("[MicoFogCloud]Device FogCloud context init [OK]");
    }
    else{
      fogcloud_log("[MicoFogCloud]Device FogCloud context init [FAILED]");
      retry_cnt++;
      continue;
    }
    
    /* cloud info reset */
    fogcloud_log("[MicoFogCloud]Device reset FogCloud info try[%d] ...", retry_cnt);
    memset((void*)&devDefaultResetData, 0, sizeof(devDefaultResetData));
    strncpy(devDefaultResetData.loginId,
            inContext->flashContentInRam.appConfig.fogcloudConfig.loginId,
            MAX_SIZE_LOGIN_ID);
    strncpy(devDefaultResetData.devPasswd,
            inContext->flashContentInRam.appConfig.fogcloudConfig.devPasswd,
            MAX_SIZE_DEV_PASSWD);
    strncpy(devDefaultResetData.user_token,
            inContext->micoStatus.mac,
            MAX_SIZE_USER_TOKEN);
    err = fogCloudResetCloudDevInfo(inContext, devDefaultResetData);
    if(kNoErr == err){
      fogcloud_log("[MicoFogCloud]Device reset FogCloud info [OK]");
    }
    else{
      fogcloud_log("[MicoFogCloud]Device reset FogCloud info [FAILED]");
      retry_cnt++;
    }
    
  }while((kNoErr != err) && (retry_cnt <= DEVICE_RESET_RETRY_CNT));
  
  return err;
}

void MicoFogCloudDevCloudInfoResetThread(void *arg)
{
  OSStatus err = kUnknownErr;
  mico_Context_t *inContext = (mico_Context_t *)arg;
  
  // stop FogCloud service first
  err = fogCloudStop(inContext);
  require_noerr_action( err, exit, fogcloud_log("ERROR: stop FogCloud service failed!") );
      
  err = easycloud_reset_cloud_info(inContext);
  if(kNoErr == err){
    inContext->appStatus.fogcloudStatus.isCloudConnected = false;
    
    mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
    inContext->flashContentInRam.appConfig.fogcloudConfig.isActivated = false;
    MICOUpdateConfiguration(inContext);
    mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);
    
    fogcloud_log("[MicoFogCloud]MicoFogCloudDevCloudInfoResetThread: cloud reset success!");
    
    // send ok semaphore
    mico_rtos_set_semaphore(&_reset_cloud_info_sem);
  }
  
exit:
  if(kNoErr != err){
    fogcloud_log("MicoFogCloudDevCloudInfoResetThread EXIT: err=%d",err);
  }
  mico_rtos_delete_thread(NULL);
  return;
}

void MicoFogCloudMainThread(void *arg)
{
  OSStatus err = kUnknownErr;
  mico_Context_t *inContext = (mico_Context_t *)arg;
  
#ifndef DISABLE_FOGCLOUD_OTA_CHECK
  MVDOTARequestData_t devOTARequestData;
#endif
 // MVDActivateRequestData_t devDefaultActivateData;
  
  // wait for station on
  fogcloud_log("MicoFogCloud start, wait for Wi-Fi...");
  while(kNoErr != mico_rtos_get_semaphore(&_wifi_station_on_sem, MICO_WAIT_FOREVER));
  
  /* check reset cloud info */
  if((inContext->flashContentInRam.appConfig.fogcloudConfig.needCloudReset) && 
     (inContext->flashContentInRam.appConfig.fogcloudConfig.isActivated)){
       // start a thread to reset device info on FogCloud
       mico_rtos_init_semaphore(&_reset_cloud_info_sem, 1);
       mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "MicoFogCloudDevReset", 
                               MicoFogCloudDevCloudInfoResetThread, 0x800, 
                               inContext );
       err = mico_rtos_get_semaphore(&_reset_cloud_info_sem, MICO_WAIT_FOREVER);
       if(kNoErr == err){
         mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
         inContext->flashContentInRam.appConfig.fogcloudConfig.needCloudReset = false;
         inContext->flashContentInRam.appConfig.fogcloudConfig.isActivated = false;
         err = MICOUpdateConfiguration(inContext);
         mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);
         fogcloud_log("MicoFogCloud Cloud reset success, system reboot...");
         // reset success, system restart
         inContext->micoStatus.sys_state = eState_Software_Reset;
         if(inContext->micoStatus.sys_state_change_sem != NULL ){
           mico_rtos_set_semaphore(&inContext->micoStatus.sys_state_change_sem);
         }
         mico_thread_sleep(MICO_WAIT_FOREVER);
       }
       else{
         fogcloud_log("MicoFogCloud Cloud reset failed!");
       }
     }
  
  /* check OTA when wifi on */
#ifndef DISABLE_FOGCLOUD_OTA_CHECK
  fogcloud_log(DEFAULT_MicoFogCloud_OTA_CHECK_MSG_2MCU);
  memset((void*)&devOTARequestData, 0, sizeof(devOTARequestData));
  strncpy(devOTARequestData.loginId,
          inContext->flashContentInRam.appConfig.fogcloudConfig.loginId,
          MAX_SIZE_LOGIN_ID);
  strncpy(devOTARequestData.devPasswd,
          inContext->flashContentInRam.appConfig.fogcloudConfig.devPasswd,
          MAX_SIZE_DEV_PASSWD);
  strncpy(devOTARequestData.user_token,
          inContext->micoStatus.mac,
          MAX_SIZE_USER_TOKEN);
  err = fogCloudDevFirmwareUpdate(inContext, devOTARequestData);
  if(kNoErr == err){
    if(inContext->appStatus.fogcloudStatus.RecvRomFileSize > 0){
      fogcloud_log(DEFAULT_MicoFogCloud_OTA_UPDATE_MSG_2MCU);
      // set bootloader to reboot && update app fw
      mico_rtos_lock_mutex(&inContext->flashContentInRam_mutex);
      memset(&inContext->flashContentInRam.bootTable, 0, sizeof(boot_table_t));
      inContext->flashContentInRam.bootTable.length = inContext->appStatus.fogcloudStatus.RecvRomFileSize;
      inContext->flashContentInRam.bootTable.start_address = UPDATE_START_ADDRESS;
      inContext->flashContentInRam.bootTable.type = 'A';
      inContext->flashContentInRam.bootTable.upgrade_type = 'U';
      if(inContext->flashContentInRam.micoSystemConfig.configured != allConfigured)
        inContext->flashContentInRam.micoSystemConfig.easyLinkByPass = EASYLINK_SOFT_AP_BYPASS;
      MICOUpdateConfiguration(inContext);
      mico_rtos_unlock_mutex(&inContext->flashContentInRam_mutex);
      inContext->micoStatus.sys_state = eState_Software_Reset;
      if(inContext->micoStatus.sys_state_change_sem != NULL ){
        mico_rtos_set_semaphore(&inContext->micoStatus.sys_state_change_sem);
      }
      mico_thread_sleep(MICO_WAIT_FOREVER);
    }
    else{
      fogcloud_log(DEFAULT_MicoFogCloud_OTA_UP_TO_DATE_MSG_2MCU);
    }
  }
  else{
    fogcloud_log(DEFAULT_MicoFogCloud_OTA_DOWNLOAD_FAILED_MSG_2MCU);
  }
#endif   // DISABLE_FOGCLOUD_OTA_CHECK
  
  /* activate when wifi on */
/*  while(false == inContext->flashContentInRam.appConfig.fogcloudConfig.isActivated){
    // auto activate, using default login_id/dev_pass/user_token
    fogcloud_log(DEFAULT_MicoFogCloud_DEV_ACTIVATE_START_MSG_2MCU);
    //    MicoFogCloudDevInterfaceSend(DEFAULT_MicoFogCloud_DEV_ACTIVATE_START_MSG_2MCU, 
    //                        strlen(DEFAULT_MicoFogCloud_DEV_ACTIVATE_START_MSG_2MCU));
    memset((void*)&devDefaultActivateData, 0, sizeof(devDefaultActivateData));
    strncpy(devDefaultActivateData.loginId,
            inContext->flashContentInRam.appConfig.fogcloudConfig.loginId,
            MAX_SIZE_LOGIN_ID);
    strncpy(devDefaultActivateData.devPasswd,
            inContext->flashContentInRam.appConfig.fogcloudConfig.devPasswd,
            MAX_SIZE_DEV_PASSWD);
    strncpy(devDefaultActivateData.user_token,
            inContext->micoStatus.mac,
            MAX_SIZE_USER_TOKEN);
    err = fogCloudDevActivate(inContext, devDefaultActivateData);
    if(kNoErr == err){
      fogcloud_log("device activate success!");
      //      MicoFogCloudDevInterfaceSend(DEFAULT_MicoFogCloud_DEV_ACTIVATE_OK_MSG_2MCU, 
      //                          strlen(DEFAULT_MicoFogCloud_DEV_ACTIVATE_OK_MSG_2MCU));
    }
    else{
      fogcloud_log("device activate failed, err = %d, retry in %d s ...", err, 1);
      //      MicoFogCloudDevInterfaceSend(DEFAULT_MicoFogCloud_DEV_ACTIVATE_FAILED_MSG_2MCU, 
      //                          strlen(DEFAULT_MicoFogCloud_DEV_ACTIVATE_FAILED_MSG_2MCU));
    }
    mico_thread_sleep(1);
  }
  fogcloud_log("device already activated.");
*/
  
  /* start FogCloud service */
  err = fogCloudStart(inContext);
  require_noerr_action(err, exit, 
                       fogcloud_log("ERROR: MicoFogCloudCloudInterfaceStart failed!") );
  
  /* start configServer for fogcloud (server for activate/authorize/reset/ota cmd from user APP) */
  err = MicoStartFogCloudConfigServer( inContext);
  require_noerr_action(err, exit, 
                       fogcloud_log("ERROR: start FogCloud configServer failed!") );
  
  while(1){
    mico_thread_sleep(1);
    if(inContext->appStatus.fogcloudStatus.isCloudConnected){
      set_RF_LED_cloud_connected(inContext);
    }
    else{
      set_RF_LED_cloud_disconnected(inContext);
    }
  }
  
exit:
  fogcloud_log("[MicoFogCloud]MicoFogCloudMainThread exit err=%d.", err);
  mico_rtos_delete_thread(NULL);
  return;
}

/*******************************************************************************
* FogCloud  interfaces init
******************************************************************************/

// reset default value
void MicoFogCloudRestoreDefault(mico_Context_t* const context)
{
  bool need_reset = false;
  
  // save reset flag
  if(context->appStatus.fogcloudStatus.isActivated){
    need_reset = true;
  }
  
  // reset all MicoFogCloud config params
  memset((void*)&(context->flashContentInRam.appConfig.fogcloudConfig), 
         0, sizeof(fogcloud_config_t));
  
  context->flashContentInRam.appConfig.fogcloudConfig.isActivated = false;
  sprintf(context->flashContentInRam.appConfig.fogcloudConfig.deviceId, DEFAULT_DEVICE_ID);
  sprintf(context->flashContentInRam.appConfig.fogcloudConfig.masterDeviceKey, DEFAULT_DEVICE_KEY);
  sprintf(context->flashContentInRam.appConfig.fogcloudConfig.romVersion, DEFAULT_ROM_VERSION);
  
  sprintf(context->flashContentInRam.appConfig.fogcloudConfig.loginId, DEFAULT_LOGIN_ID);
  sprintf(context->flashContentInRam.appConfig.fogcloudConfig.devPasswd, DEFAULT_DEV_PASSWD);
  sprintf(context->flashContentInRam.appConfig.fogcloudConfig.userToken, context->micoStatus.mac);
  
  // set reset flag for next startup
  if(need_reset){
    context->flashContentInRam.appConfig.fogcloudConfig.needCloudReset = true;
    context->flashContentInRam.appConfig.fogcloudConfig.isActivated = true;
  }
  else{
    context->flashContentInRam.appConfig.fogcloudConfig.needCloudReset = false;
  }
}

OSStatus MicoStartFogCloudService(mico_Context_t* const inContext)
{
  OSStatus err = kUnknownErr;
  
  //init MicoFogCloud status
  inContext->appStatus.fogcloudStatus.isCloudConnected = false;
  inContext->appStatus.fogcloudStatus.RecvRomFileSize = 0;
  inContext->appStatus.fogcloudStatus.isActivated = inContext->flashContentInRam.appConfig.fogcloudConfig.isActivated;
  
  //init cloud service interface
  err = fogCloudInit(inContext);
  require_noerr_action(err, exit, 
                       fogcloud_log("ERROR: FogCloud interface init failed!") );
  
  // add wifi notify && semaphore
  if(NULL == _wifi_station_on_sem){
    err = mico_rtos_init_semaphore(&_wifi_station_on_sem, 1);
    require_noerr_action(err, exit, 
                         fogcloud_log("ERROR: mico_rtos_init_semaphore (_wifi_station_on_sem) failed!") );
  }
  err = MICOAddNotification( mico_notify_WIFI_STATUS_CHANGED, (void *)mvdNotify_WifiStatusHandler );
  require_noerr_action(err, exit, 
                       fogcloud_log("ERROR: MICOAddNotification (mico_notify_WIFI_STATUS_CHANGED) failed!") );
  
  // init cloud connect semaphore
  if(NULL == _fogcloud_connect_sem){
    err = mico_rtos_init_semaphore(&_fogcloud_connect_sem, 1);
    require_noerr_action(err, exit, 
                         fogcloud_log("ERROR: mico_rtos_init_semaphore (_fogcloud_connect_sem) failed!") );
  }
  
  // start MicoFogCloud main thread (dev reset && ota check, then start fogcloud service)
  err = mico_rtos_create_thread(NULL, MICO_APPLICATION_PRIORITY, "fogcloud_main", 
                                MicoFogCloudMainThread, STACK_SIZE_FOGCLOUD_MAIN_THREAD, 
                                inContext );
  
//  // start configServer for fogcloud (server for activate/authorize/reset/ota cmd from user APP)
//  err = MicoStartFogCloudConfigServer( inContext);
//  require_noerr_action(err, exit, 
//                       fogcloud_log("ERROR: start FogCloud configServer failed!") );
  
exit:
  return err;
}

/*******************************************************************************
* MicoFogCloud get state
*******************************************************************************/

// cloud connect state
bool MicoFogCloudIsConnect(mico_Context_t* const context)
{
  if(NULL == context){
    return false;
  }
  return context->appStatus.fogcloudStatus.isCloudConnected;
}

// device activate state
bool MicoFogCloudIsActivated(mico_Context_t* const context)
{
  if(NULL == context){
    return false;
  }
  return context->flashContentInRam.appConfig.fogcloudConfig.isActivated;
}

void mico_fogcloud_waitfor_connect(mico_Context_t* const context, uint32_t timeout_ms){
  if(NULL == _fogcloud_connect_sem){
    mico_rtos_init_semaphore(&_fogcloud_connect_sem, 1);
  }
  while(kNoErr != mico_rtos_get_semaphore(&_fogcloud_connect_sem, timeout_ms));
}

/*******************************************************************************
* FogCloud control interfaces
******************************************************************************/

//activate
OSStatus MicoFogCloudActivate(mico_Context_t* const context, 
                              MVDActivateRequestData_t activateData)
{
  OSStatus err = kUnknownErr;
  
  if(context->flashContentInRam.appConfig.fogcloudConfig.isActivated){
    // already activated, just do authorize
    err = fogCloudDevAuthorize(context, activateData);
    require_noerr_action(err, exit, 
                         fogcloud_log("ERROR: device authorize failed! err=%d", err) );
  }
  else {
    // activate
    err = fogCloudDevActivate(context, activateData);
    require_noerr_action(err, exit, 
                         fogcloud_log("ERROR: device activate failed! err=%d", err) );
  }
  return kNoErr;
  
exit:
  return err;
}

//authorize
OSStatus MicoFogCloudAuthorize(mico_Context_t* const context,
                               MVDAuthorizeRequestData_t authorizeData)
{
  OSStatus err = kUnknownErr;
  mico_Context_t *inContext = context;
  
  if(context->flashContentInRam.appConfig.fogcloudConfig.isActivated){
    err = fogCloudDevAuthorize(inContext, authorizeData);
    require_noerr_action(err, exit, 
                         fogcloud_log("ERROR: device authorize failed! err=%d", err) );
  }
  else{
    fogcloud_log("ERROR: device not activate!");
    err = kStateErr;
  }
  
exit:
  return err;
}

//OTA
OSStatus MicoFogCloudFirmwareUpdate(mico_Context_t* const context,
                                    MVDOTARequestData_t OTAData)
{
  OSStatus err = kUnknownErr;
  mico_Context_t *inContext = context;
  
  err = fogCloudDevFirmwareUpdate(inContext, OTAData);
  require_noerr_action(err, exit, 
                       fogcloud_log("ERROR: Firmware Update error! err=%d", err) );
  return kNoErr;
  
exit:
  return err;
}

//reset device info on cloud
OSStatus MicoFogCloudResetCloudDevInfo(mico_Context_t* const context,
                                       MVDResetRequestData_t devResetData)
{
  OSStatus err = kUnknownErr;
  mico_Context_t *inContext = context;
  
  err = fogCloudResetCloudDevInfo(inContext, devResetData);
  require_noerr_action(err, exit, 
                       fogcloud_log("ERROR: reset device info on cloud error! err=%d", err) );
  return kNoErr;
  
exit:
  return err;
}

//get state of the MVD( e.g. isActivate/isConnected)
OSStatus MicoFogCloudGetState(mico_Context_t* const context,
                              MVDGetStateRequestData_t getStateRequestData,
                              void* outDevState)
{
  //OSStatus err = kUnknownErr;
  mico_Context_t *inContext = context;
  json_object* report = (json_object*)outDevState;
  
  if((NULL == context) || (NULL == outDevState)){
    return kParamErr;
  }
  
  // login_id/dev_passwd ok ?
  if((0 != strncmp(context->flashContentInRam.appConfig.fogcloudConfig.loginId, 
                   getStateRequestData.loginId, 
                   strlen(context->flashContentInRam.appConfig.fogcloudConfig.loginId))) ||
     (0 != strncmp(context->flashContentInRam.appConfig.fogcloudConfig.devPasswd, 
                   getStateRequestData.devPasswd, 
                   strlen(context->flashContentInRam.appConfig.fogcloudConfig.devPasswd))))
  {
    fogcloud_log("ERROR: MVDGetState: loginId/devPasswd mismatch!");
    return kMismatchErr;
  }
  
  json_object_object_add(report, "isActivated",
                         json_object_new_boolean(inContext->flashContentInRam.appConfig.fogcloudConfig.isActivated)); 
  json_object_object_add(report, "isConnected",
                         json_object_new_boolean(inContext->appStatus.fogcloudStatus.isCloudConnected));
  json_object_object_add(report, "version",
                         json_object_new_string(inContext->flashContentInRam.appConfig.fogcloudConfig.romVersion));
  
  return kNoErr;
}

/*******************************************************************************
* MicoFogCloud message send interface
******************************************************************************/

// MCU => Cloud
// if topic is NULL, send to default topic: device_id/out,
// else send to sub-channel: device_id/out/<topic>
OSStatus MicoFogCloudMsgSend(mico_Context_t* const context, const char* topic, 
                                   unsigned char *inBuf, unsigned int inBufLen)
{
  fogcloud_log_trace();
  OSStatus err = kUnknownErr;
  
  err = fogCloudSendtoChannel(topic, inBuf, inBufLen);  // transfer raw data
  require_noerr_action( err, exit, fogcloud_log("ERROR: send to cloud error! err=%d", err) );
  return kNoErr;
  
exit:
  return err;
}


/*******************************************************************************
* MicoFogCloud message exchange protocol
******************************************************************************/

// handle cloud msg here, for example: send to USART or echo to cloud
OSStatus MicoFogCloudCloudMsgProcess(mico_Context_t* context, 
                                     const char* topic, const unsigned int topicLen,
                                     unsigned char *inBuf, unsigned int inBufLen)
{
  fogcloud_log_trace();
  OSStatus err = kUnknownErr;
  
  err = user_fogcloud_msg_handler(context, topic, topicLen, inBuf, inBufLen);
  
  return err;
}
