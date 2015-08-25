/**
******************************************************************************
* @file    DHT11.c
* @author  Eshen Wang
* @version V1.0.0
* @date    1-May-2015
* @brief   dc motor operation. 
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDATAG CUSTOMERS
* WITH CODATAG INFORMATION REGARDATAG THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODATAG INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
******************************************************************************
*/ 

#include "DHT11.h"

/*------------------------------ delay function ------------------------------*/

void Delay_us(uint32_t nus)
{		
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  // TIM1 configuration    10ms
  TIM_DeInit(TIM1);
  TIM_TimeBaseStructure.TIM_Period = nus;          
  TIM_TimeBaseStructure.TIM_Prescaler = (100-1);       
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  
  // Clear TIM1 update pending flag
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);
  // Disable TIM1 Update interrupt 
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  // TIM1 enable counter 
  TIM_Cmd(TIM1, ENABLE);
  
  while(TIM_GetITStatus(TIM1, TIM_IT_Update) == RESET);
  
  // stop TIM1
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
  TIM_Cmd(TIM1, DISABLE);
}

void Delay_ms(uint16_t nms)
{	  	  
  mico_thread_msleep(nms);	  	    
}

/*--------------------------------- DHT11 Operations -------------------------*/

//Reset DHT11
void DHT11_Rst(void)	   
{                 
  DHT11_IO_OUT(); 											//SET OUTPUT
  DHT11_DATA_Clr(); 											//GPIOA.0=0
  Delay_ms(20);    											//Pull down Least 18ms
  DHT11_DATA_Set(); 											//GPIOA.0=1 
  Delay_us(30);     										//Pull up 20~40us
}

u8 DHT11_Check(void) 	   
{   
  u8 retry=0;
  DHT11_IO_IN();												//SET INPUT	 
  while (DHT11_DQ_IN&&retry<100)				//DHT11 Pull down 40~80us
  {
    retry++;
    Delay_us(1);
  }	 
  
  if(retry>=100)
    return 1;
  else 
    retry=0;
  
  while (!DHT11_DQ_IN&&retry<100)				//DHT11 Pull up 40~80us
  {
    retry++;
    Delay_us(1);
  }
  
  if(retry>=100)
    return 1;														//chack error	    
  
  return 0;
}

u8 DHT11_Read_Bit(void) 			 
{
  u8 retry=0;
  while(DHT11_DQ_IN&&retry<100)					//wait become Low level
  {
    retry++;
    Delay_us(1);
  }
  
  retry=0;
  while(!DHT11_DQ_IN&&retry<100)				//wait become High level
  {
    retry++;
    Delay_us(1);
  }
  
  Delay_us(40);//wait 40us
  
  if(DHT11_DQ_IN)
    return 1;
  else 
    return 0;		   
}

u8 DHT11_Read_Byte(void)    
{        
  u8 i,dat;
  dat=0;
  for (i=0;i<8;i++) 
  {
    dat<<=1; 
    dat|=DHT11_Read_Bit();
  }						    
  
  return dat;
}

u8 DHT11_Read_Data(u8 *temperature,u8 *humidity)    
{        
  u8 buf[5];
  u8 i;
  DHT11_Rst();
  if(DHT11_Check()==0)
  {
    for(i=0;i<5;i++)
    {
      buf[i]=DHT11_Read_Byte();
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
    {
      *humidity=buf[0];
      *temperature=buf[2];
    }
  }
  else {
    return 1;
  }
  
  return 0;	    
}

uint8_t DHT11_Init(void)
{	 
  DHT11_IO_OUT();
  
  DHT11_Rst();  
  return DHT11_Check();
} 
