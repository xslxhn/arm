﻿/**
  ******************************************************************************
  * @file    Bsp_HBridge.c 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
//------------------------------- Includes -----------------------------------
#include "Bsp_HBridge.h"
#include "Bsp_Led.h"
//------------------------------- 常量 ------------------------
GPIO_TypeDef* HBRIDGE_P_GPIOX_BUF[HBRIDGE_MAX_NUM]={HBRIDGE_1_P_GPIOX,HBRIDGE_2_P_GPIOX,HBRIDGE_3_P_GPIOX,HBRIDGE_4_P_GPIOX};
GPIO_TypeDef* HBRIDGE_N_GPIOX_BUF[HBRIDGE_MAX_NUM]={HBRIDGE_1_N_GPIOX,HBRIDGE_2_N_GPIOX,HBRIDGE_3_N_GPIOX,HBRIDGE_4_N_GPIOX};
const uint16_t HBRIDGE_P_PINX_BUF[HBRIDGE_MAX_NUM]={HBRIDGE_1_P_PINX,HBRIDGE_2_P_PINX,HBRIDGE_3_P_PINX,HBRIDGE_4_P_PINX};
const uint16_t HBRIDGE_N_PINX_BUF[HBRIDGE_MAX_NUM]={HBRIDGE_1_N_PINX,HBRIDGE_2_N_PINX,HBRIDGE_3_N_PINX,HBRIDGE_4_N_PINX};

long BspHBridge_1msTimerBuf[HBRIDGE_MAX_NUM]; 

static void BspHBridge_Test_1ms(void);
/*
******************************************************************************
* 函数功能: 初始化
******************************************************************************
*/
void BspHBridge_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //时钟初始化
    BSP_HBRIDGE_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
#endif    
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_1_P_PINX ;
    GPIO_Init(HBRIDGE_1_P_GPIOX, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_1_N_PINX ;
    GPIO_Init(HBRIDGE_1_N_GPIOX, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_2_P_PINX ;
    GPIO_Init(HBRIDGE_2_P_GPIOX, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_2_N_PINX ;
    GPIO_Init(HBRIDGE_2_N_GPIOX, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_3_P_PINX ;
    GPIO_Init(HBRIDGE_3_P_GPIOX, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_3_N_PINX ;
    GPIO_Init(HBRIDGE_3_N_GPIOX, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_4_P_PINX ;
    GPIO_Init(HBRIDGE_4_P_GPIOX, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_4_N_PINX ;
    GPIO_Init(HBRIDGE_4_N_GPIOX, &GPIO_InitStructure);
}
//--------------------------------------------------------------应用函数
void BspHBridge_1ms(void)
{
   uint8_t i;
   for(i=0;i<HBRIDGE_MAX_NUM;i++)
   {
      if(BspHBridge_1msTimerBuf[i]==0)
      {
         //停止
         GPIO_ResetBits(HBRIDGE_P_GPIOX_BUF[i] , HBRIDGE_P_PINX_BUF[i]);
         GPIO_ResetBits(HBRIDGE_N_GPIOX_BUF[i] , HBRIDGE_N_PINX_BUF[i]);
      }
      else if(BspHBridge_1msTimerBuf[i]>0)
      {
         BspHBridge_1msTimerBuf[i]--;
         //正转
         GPIO_ResetBits(HBRIDGE_P_GPIOX_BUF[i] , HBRIDGE_P_PINX_BUF[i]);
         GPIO_SetBits(HBRIDGE_N_GPIOX_BUF[i] , HBRIDGE_N_PINX_BUF[i]);
      }
      else
      {
         BspHBridge_1msTimerBuf[i]++;
         //反转
         GPIO_SetBits(HBRIDGE_P_GPIOX_BUF[i] , HBRIDGE_P_PINX_BUF[i]);
         GPIO_ResetBits(HBRIDGE_N_GPIOX_BUF[i] , HBRIDGE_N_PINX_BUF[i]);
      }
   }
	 BspHBridge_Test_1ms();
}
//--------------------------------------------------------------测试函数
static void BspHBridge_Test_1ms(void)
{
   static uint16_t stimer=0;
   stimer++;
   if(stimer==2500)
   {
      //正转
      BspHBridge_1msTimerBuf[0]=1000;
      BspHBridge_1msTimerBuf[1]=1000;
      BspHBridge_1msTimerBuf[2]=1000;
      BspHBridge_1msTimerBuf[3]=1000;
      //开启前轮灯
      BSP_LED_LED2_ON;
      //关闭后轮灯
      BSP_LED_LED3_OFF;
   }
   else if(stimer==5000)
   {
      //反转
      BspHBridge_1msTimerBuf[0]=-1000;
      BspHBridge_1msTimerBuf[1]=-1000;
      BspHBridge_1msTimerBuf[2]=-1000;
      BspHBridge_1msTimerBuf[3]=-1000;
      //开启后轮灯
      BSP_LED_LED3_ON;
      //关闭前轮灯
      BSP_LED_LED2_OFF;
   }
   else if(stimer>5000)
   {
      stimer=0;
   }
}
//------------------------------------------------------------

