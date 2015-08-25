/**
******************************************************************************
* @file    MicoFogCloudDef.h 
* @author  Eshen Wang
* @version V1.0.0
* @date    17-Mar-2015
* @brief   This header contains the defines of MICO FogCloud. 
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

#ifndef __MICO_FOGCLOUD_DEF_H_
#define __MICO_FOGCLOUD_DEF_H_

#include "FogCloudServiceDef.h"

/*******************************************************************************
 * DEFINES
 ******************************************************************************/

// default device settings
#define DEFAULT_LOGIN_ID                 "admin"
#define DEFAULT_DEV_PASSWD               "12345678"
   
// default device info
#define DEFAULT_DEVICE_ID                "none"
#define DEFAULT_DEVICE_KEY               "none"

#define STACK_SIZE_FOGCLOUD_MAIN_THREAD   0x500

/* MQTT topic sub-level, device_id/out/status */
#define PUBLISH_TOPIC_CHANNEL_STATUS     "status"

#define FOGCLOUD_CONFIG_SERVER_PORT       8001    // fogcloud config server port

// disalbe FogCloud OTA check when system start
//#define DISABLE_FOGCLOUD_OTA_CHECK


/*******************************************************************************
 * STRUCTURES
 ******************************************************************************/

/* device configurations stored in flash */
typedef struct _fogcloud_config_t
{
  /* cloud connect params */
  bool              isActivated;                         // device activate flag
  char              deviceId[MAX_SIZE_DEVICE_ID];        // get from cloud server
  char              masterDeviceKey[MAX_SIZE_DEVICE_KEY];// get from cloud server
  char              romVersion[MAX_SIZE_FW_VERSION];     // get from cloud server
  
  char              loginId[MAX_SIZE_LOGIN_ID];          // not used for wechat dev
  char              devPasswd[MAX_SIZE_DEV_PASSWD];      // not used for wechat dev
  char              userToken[MAX_SIZE_USER_TOKEN];      // use MAC addr instead for wechat
  
  /* reset flag */
  bool              needCloudReset;                     // need reset cloud when set
} fogcloud_config_t;

/* device status */
typedef struct _fogcloud_status_t
{
  bool isActivated;                     // device activate status in ram
  bool              isCloudConnected;   // cloud service connect status
  uint64_t          RecvRomFileSize;    // return OTA data size for bootTable.length, 0 means not need to update
} fogcloud_status_t;

typedef struct _fogcloud_context_t {
  fogcloud_config_t config_info;        // fogcloud config info
  fogcloud_status_t status;             // fogcloud running status
} fogcloud_context_t;

typedef struct _get_state_req_data_t {
char loginId[MAX_SIZE_LOGIN_ID];
char devPasswd[MAX_SIZE_DEV_PASSWD];
char user_token[MAX_SIZE_USER_TOKEN];
} MVDGetStateRequestData_t;

#endif
