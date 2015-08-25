/**
  ******************************************************************************
  * @file    user_config.h 
  * @author  Eshen Wang
  * @version V1.0.0
  * @date    14-May2015
  * @brief   This file contains user config for app.
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

#ifndef __USER_CONFIG_H_
#define __USER_CONFIG_H_


/*******************************************************************************
 *                             APP INFO
 ******************************************************************************/
#define APP_INFO                           "Wechat Demo based on MICO OS"

#define FIRMWARE_REVISION                  "MICO_WECHAT_1_0"
#define SERIAL_NUMBER                      "20150522"
#define PROTOCOL                           "com.mxchip.wechat"

/* product type */
// NOTE: create your own product on fogcloud developper center and replace it.
#define PRODUCT_ID                         "2bed1355"
#define PRODUCT_KEY                        "d98963de-97c9-11e4-ae3a-f23c9150064b"

#define DEFAULT_ROM_VERSION                "v1.0.0"
#define DEFAULT_DEVICE_NAME                "MiCO Wechat"      // device name upload to cloud
#define DEFAULT_MANUFACTURER               "MXCHIP"       // device manufacturer

   
/*******************************************************************************
 *                             CONNECTING
 ******************************************************************************/
/* Wi-Fi configuration mode */
#define MICO_CONFIG_MODE                   CONFIG_MODE_AIRKISS

/* MICO cloud service */
#define MICO_CLOUD_TYPE                    CLOUD_FOGCLOUD
   
// if need to auto activate afger first time configure, comment it out
//#define DISABLE_FOGCLOUD_AUTO_ACTIVATE

// disalbe FogCloud OTA check when system start
#define DISABLE_FOGCLOUD_OTA_CHECK
   
/*******************************************************************************
 *                             RESOURCES
 ******************************************************************************/
#define STACK_SIZE_USER_MAIN_THREAD         0x800
   
/*User provided configurations*/
#define CONFIGURATION_VERSION               0x00000001 // if default configuration is changed, update this number

   
#endif  // __USER_CONFIG_H_
