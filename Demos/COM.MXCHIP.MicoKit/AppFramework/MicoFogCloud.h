/**
******************************************************************************
* @file    MicoFogCloud.h 
* @author  Eshen Wang
* @version V1.0.0
* @date    17-Mar-2015
* @brief   This header contains the cloud service interfaces 
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

#ifndef __MICO_FOGCLOUD_H_
#define __MICO_FOGCLOUD_H_

#include "MICODefine.h"
#include "MicoFogCloudDef.h"
#include "FogCloudUtils.h"

/*******************************************************************************
 * DEFINES
 ******************************************************************************/

/*******************************************************************************
 * USER INTERFACES
 ******************************************************************************/

/***** init *****/

// init FogCloud
OSStatus MicoStartFogCloudService(mico_Context_t* const inContext);
// restore default config for FogCloud
void MicoFogCloudRestoreDefault(mico_Context_t* const context);


/***** get MicoFogCloud state *****/

// device activate state
bool MicoFogCloudIsActivated(mico_Context_t* const context);
// cloud connect state
bool MicoFogCloudIsConnect(mico_Context_t* const context);

void mico_fogcloud_waitfor_connect(mico_Context_t* const context, uint32_t timeout_ms);

/****** send && recv message ******/
// Module <=> Cloud
OSStatus MicoFogCloudMsgSend(mico_Context_t* const context, const char* topic,
                             unsigned char *inBuf, unsigned int inBufLen);

//OSStatus MicoFogCloudMsgRecv(mico_Context_t* const context, void *fogcloudMsg.
//                             int *timeout_ms);

/* device control */

//activate
OSStatus MicoFogCloudActivate(mico_Context_t* const context, 
                              MVDActivateRequestData_t activateData);
//authorize
OSStatus MicoFogCloudAuthorize(mico_Context_t* const context,
                               MVDAuthorizeRequestData_t authorizeData);
//reset device info on cloud
OSStatus MicoFogCloudResetCloudDevInfo(mico_Context_t* const context,
                                       MVDResetRequestData_t devResetData);
//OTA
OSStatus MicoFogCloudFirmwareUpdate(mico_Context_t* const context,
                                    MVDOTARequestData_t OTAData);
//get device state info(activate/connect)
OSStatus MicoFogCloudGetState(mico_Context_t* const context,
                              MVDGetStateRequestData_t getStateRequestData,
                              void* outDevState);

/*******************************************************************************
* INTERNAL FUNCTIONS
*******************************************************************************/

// override by user in user_main.c
WEAK OSStatus user_fogcloud_msg_handler(mico_Context_t* context, 
                                        const char* topic, const unsigned int topicLen,
                                        unsigned char *inBuf, unsigned int inBufLen);

#endif
