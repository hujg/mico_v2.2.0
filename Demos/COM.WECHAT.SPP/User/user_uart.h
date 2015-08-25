/**
******************************************************************************
* @file    user_uart.h 
* @author  Eshen Wang
* @version V1.0.0
* @date    22-May-2015
* @brief   This header contains the user uart interfaces.
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

#ifndef __MICO_MVDDEVICEINTERFACES_H_
#define __MICO_MVDDEVICEINTERFACES_H_

#include "MICODefine.h"

/*******************************************************************************
 * DEFINES
 ******************************************************************************/
#define UART_FOR_MCU                        MICO_UART_1
#define UART_RECV_TIMEOUT                   100
#define UART_ONE_PACKAGE_LENGTH             1024
#define UART_BUFFER_LENGTH                  2048
   
#define STACK_SIZE_USART_RECV_THREAD        0x500


/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

OSStatus user_uart_init(mico_Context_t* const inContext);
OSStatus user_uart_send(unsigned char *inBuf, unsigned int inBufLen);


#endif
