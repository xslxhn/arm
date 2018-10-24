﻿/**
  ******************************************************************************
  * @file    Bsp_BuzzerMusic.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于蜂鸣器的单弦音乐.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
         谱曲存储格式：const u8 musicname{音高，音长，音高，音长...，0，0};
         末尾：0,0表示结束(Important)
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
//------------------------------- Includes -----------------------------------
#include "includes.h"
//------------------------------- 预编译 -------------------------------------
#ifdef BSP_BUZZER_MUSIC_GLOBAL
#define BSP_BUZZER_MUSIC_EXT
#undef BSP_BUZZER_MUSIC_GLOBAL
//------------------------------- 音乐库 --------------------
//早上好
const uint8_t BSP_BUZZER_MUSIC_LIB_MORNING[]=
{
    0x17,0x03, 0x19,0x67, 0x17,0x04, 0x18,0x03, 0x17,0x03,
    0x16,0x03, 0x15,0x04, 0x16,0x04, 0x17,0x03, 0x16,0x03,
    0x15,0x03, 0x17,0x03, 0x16,0x15, 0x17,0x03, 0x19,0x67,
    0x17,0x04, 0x18,0x03, 0x17,0x03, 0x16,0x03, 0x15,0x04,
    0x16,0x04, 0x17,0x03, 0x16,0x03, 0x15,0x03, 0x16,0x03,
    0x0F,0x15, 0x15,0x03, 0x17,0x67, 0x15,0x04, 0x16,0x03,
    0x18,0x02, 0x19,0x03, 0x17,0x03, 0x15,0x03, 0x17,0x03,
    0x15,0x03, 0x16,0x15, 0x17,0x03, 0x19,0x67, 0x17,0x04,
    0x18,0x03, 0x17,0x03, 0x16,0x03, 0x15,0x04, 0x16,0x04,
    0x17,0x03, 0x16,0x03, 0x15,0x03, 0x17,0x03, 0x16,0x03,
    0x16,0x7A, 0x17,0x03, 0x19,0x67, 0x17,0x04, 0x18,0x03,
    0x17,0x03, 0x16,0x03, 0x15,0x04, 0x16,0x04, 0x17,0x03,
    0x16,0x03, 0x15,0x03, 0x17,0x03, 0x16,0x15, 0x17,0x03,
    0x19,0x67, 0x17,0x04, 0x18,0x03, 0x17,0x03, 0x16,0x03,
    0x15,0x04, 0x16,0x04, 0x17,0x03, 0x16,0x03, 0x15,0x03,
    0x16,0x03, 0x0F,0x15, 0x15,0x03, 0x17,0x03, 0x15,0x03,
    0x16,0x03, 0x18,0x17, 0x17,0x03, 0x17,0x03, 0x15,0x03,
    0x17,0x03, 0x15,0x03, 0x16,0x15, 0x15,0x03, 0x17,0x03,
    0x15,0x03, 0x16,0x03, 0x18,0x17, 0x19,0x03, 0x19,0x03,
    0x17,0x03, 0x19,0x03, 0x17,0x03, 0x16,0x02, 0x15,0x16,
    0x17,0x03, 0x19,0x67, 0x17,0x04, 0x18,0x03, 0x17,0x03,
    0x16,0x03, 0x15,0x04, 0x16,0x04, 0x17,0x03, 0x16,0x03,
    0x15,0x03, 0x17,0x03, 0x16,0x15, 0x17,0x03, 0x19,0x67,
    0x17,0x04, 0x18,0x03, 0x17,0x03, 0x16,0x03, 0x15,0x04,
    0x16,0x04, 0x17,0x03, 0x16,0x03, 0x15,0x03, 0x16,0x03,
    0x0F,0x03, 0x0F,0x7A, 0x15,0x03, 0x17,0x03, 0x15,0x03,
    0x16,0x03, 0x18,0x17, 0x17,0x03, 0x17,0x03, 0x15,0x03,
    0x17,0x03, 0x15,0x03, 0x16,0x15, 0x15,0x03, 0x17,0x03,
    0x15,0x03, 0x16,0x03, 0x18,0x17, 0x19,0x03, 0x19,0x03,
    0x17,0x03, 0x19,0x03, 0x17,0x03, 0x16,0x02, 0x15,0x02,
    0x15,0x02, 0x00,0x00
};
//荷塘月色
const uint8_t BSP_BUZZER_MUSIC_LIB_HTYS[]=
{
    0x16,0x02, 0x16,0x03, 0x17,0x03, 0x15,0x02, 0x0F,0x03,
    0x0F,0x03, 0x16,0x03, 0x15,0x03, 0x16,0x03, 0x17,0x03,
    0x15,0x16, 0x17,0x03, 0x18,0x03, 0x17,0x03, 0x15,0x03,
    0x15,0x04, 0x16,0x04, 0x16,0x16, 0x17,0x03, 0x18,0x03,
    0x17,0x03, 0x15,0x03, 0x15,0x03, 0x15,0x03, 0x16,0x04,
    0x15,0x04, 0x11,0x02, 0x16,0x03, 0x16,0x03, 0x16,0x03,
    0x17,0x03, 0x15,0x02, 0x0F,0x03, 0x0F,0x03, 0x16,0x03,
    0x15,0x03, 0x16,0x03, 0x17,0x03, 0x15,0x16, 0x17,0x03,
    0x18,0x03, 0x17,0x03, 0x15,0x03, 0x15,0x04, 0x16,0x04,
    0x16,0x16, 0x17,0x03, 0x18,0x03, 0x17,0x03, 0x18,0x03,
    0x19,0x03, 0x19,0x02, 0x00,0x00,
    0x0F,0x03, 0x10,0x03, 0x15,0x02, 0x16,0x03, 0x17,0x03,
    0x19,0x02, 0x17,0x03, 0x19,0x03, 0x1A,0x03, 0x1B,0x03,
    0x1A,0x03, 0x19,0x03, 0x17,0x02, 0x16,0x03, 0x17,0x03,
    0x19,0x03, 0x17,0x03, 0x17,0x03, 0x16,0x03, 0x17,0x03,
    0x16,0x03, 0x15,0x03, 0x0F,0x01, 0x00,0x00
};

//挥着翅膀的女孩
const uint8_t BSP_BUZZER_MUSIC_LIB_HZCBDNH[]=
{
    0x17,0x02, 0x17,0x03, 0x18,0x03, 0x19,0x02, 0x15,0x03,
    0x16,0x03, 0x17,0x03, 0x17,0x03, 0x17,0x03, 0x18,0x03,
    0x19,0x02, 0x16,0x03, 0x17,0x03, 0x18,0x02, 0x18,0x03,
    0x17,0x03, 0x15,0x02, 0x18,0x03, 0x17,0x03, 0x18,0x02,
    0x10,0x03, 0x15,0x03, 0x16,0x02, 0x15,0x03, 0x16,0x03,
    0x17,0x02, 0x17,0x03, 0x18,0x03, 0x19,0x02, 0x1A,0x03,
    0x1B,0x03, 0x1F,0x03, 0x1F,0x03, 0x17,0x03, 0x18,0x03,
    0x19,0x02, 0x16,0x03, 0x17,0x03, 0x18,0x03, 0x17,0x03,
    0x18,0x03, 0x1F,0x03, 0x1F,0x02, 0x16,0x03, 0x17,0x03,
    0x18,0x03, 0x17,0x03, 0x18,0x03, 0x20,0x03, 0x20,0x02,
    0x1F,0x03, 0x1B,0x03, 0x1F,0x66, 0x20,0x03, 0x21,0x03,
    0x20,0x03, 0x1F,0x03, 0x1B,0x03, 0x1F,0x66, 0x1F,0x03,
    0x1B,0x03, 0x19,0x03, 0x19,0x03, 0x15,0x03, 0x1A,0x66,
    0x1A,0x03, 0x19,0x03, 0x15,0x03, 0x15,0x03, 0x17,0x03,
    0x16,0x66, 0x17,0x04, 0x18,0x04, 0x18,0x03, 0x19,0x03,
    0x1F,0x03, 0x1B,0x03, 0x1F,0x66, 0x20,0x03, 0x21,0x03,
    0x20,0x03, 0x1F,0x03, 0x1B,0x03, 0x1F,0x66, 0x1F,0x03,
    0x1B,0x03, 0x19,0x03, 0x19,0x03, 0x15,0x03, 0x1A,0x66,
    0x1A,0x03, 0x19,0x03, 0x19,0x03, 0x1F,0x03, 0x1B,0x03,
    0x1F,0x00, 0x1A,0x03, 0x1A,0x03, 0x1A,0x03, 0x1B,0x03,
    0x1B,0x03, 0x1A,0x03, 0x19,0x03, 0x19,0x02, 0x17,0x03,
    0x15,0x17, 0x15,0x03, 0x16,0x03, 0x17,0x03, 0x18,0x03,
    0x17,0x04, 0x18,0x0E, 0x18,0x03, 0x17,0x04, 0x18,0x0E,
    0x18,0x66, 0x17,0x03, 0x18,0x03, 0x17,0x03, 0x18,0x03,
    0x20,0x03, 0x20,0x02, 0x1F,0x03, 0x1B,0x03, 0x1F,0x66,
    0x20,0x03, 0x21,0x03, 0x20,0x03, 0x1F,0x03, 0x1B,0x03,
    0x1F,0x66, 0x1F,0x04, 0x1B,0x0E, 0x1B,0x03, 0x19,0x03,
    0x19,0x03, 0x15,0x03, 0x1A,0x66, 0x1A,0x03, 0x19,0x03,
    0x15,0x03, 0x15,0x03, 0x17,0x03, 0x16,0x66, 0x17,0x04,
    0x18,0x04, 0x18,0x03, 0x19,0x03, 0x1F,0x03, 0x1B,0x03,
    0x1F,0x66, 0x20,0x03, 0x21,0x03, 0x20,0x03, 0x1F,0x03,
    0x1B,0x03, 0x1F,0x66, 0x1F,0x03, 0x1B,0x03, 0x19,0x03,
    0x19,0x03, 0x15,0x03, 0x1A,0x66, 0x1A,0x03, 0x19,0x03,
    0x19,0x03, 0x1F,0x03, 0x1B,0x03, 0x1F,0x00, 0x18,0x02,
    0x18,0x03, 0x1A,0x03, 0x19,0x0D, 0x15,0x03, 0x15,0x02,
    0x18,0x66, 0x16,0x02, 0x17,0x02, 0x15,0x00, 
    0,0
};

//两只蝴蝶
const uint8_t BSP_BUZZER_MUSIC_LIB_LZHD[] =
{
    0x17,0x03, 0x16,0x03, 0x17,0x01, 0x16,0x03, 0x17,0x03,
    0x16,0x03, 0x15,0x01, 0x10,0x03, 0x15,0x03, 0x16,0x02,
    0x16,0x0D, 0x17,0x03, 0x16,0x03, 0x15,0x03, 0x10,0x03,
    0x10,0x0E, 0x15,0x04, 0x0F,0x01, 0x17,0x03, 0x16,0x03,
    0x17,0x01, 0x16,0x03, 0x17,0x03, 0x16,0x03, 0x15,0x01,
    0x10,0x03, 0x15,0x03, 0x16,0x02, 0x16,0x0D, 0x17,0x03,
    0x16,0x03, 0x15,0x03, 0x10,0x03, 0x15,0x03, 0x16,0x01,
    0x17,0x03, 0x16,0x03, 0x17,0x01, 0x16,0x03, 0x17,0x03,
    0x16,0x03, 0x15,0x01, 0x10,0x03, 0x15,0x03, 0x16,0x02,
    0x16,0x0D, 0x17,0x03, 0x16,0x03, 0x15,0x03, 0x10,0x03,
    0x10,0x0E, 0x15,0x04, 0x0F,0x01, 0x17,0x03, 0x19,0x03,
    0x19,0x01, 0x19,0x03, 0x1A,0x03, 0x19,0x03, 0x17,0x01,
    0x16,0x03, 0x16,0x03, 0x16,0x02, 0x16,0x0D, 0x17,0x03,
    0x16,0x03, 0x15,0x03, 0x10,0x03, 0x10,0x0D, 0x15,0x00,
    0x19,0x03, 0x19,0x03, 0x1A,0x03, 0x1F,0x03, 0x1B,0x03,
    0x1B,0x03, 0x1A,0x03, 0x17,0x0D, 0x16,0x03, 0x16,0x03,
    0x16,0x0D, 0x17,0x01, 0x17,0x03, 0x17,0x03, 0x19,0x03,
    0x1A,0x02, 0x1A,0x02, 0x10,0x03, 0x17,0x0D, 0x16,0x03,
    0x16,0x01, 0x17,0x03, 0x19,0x03, 0x19,0x03, 0x17,0x03,
    0x19,0x02, 0x1F,0x02, 0x1B,0x03, 0x1A,0x03, 0x1A,0x0E,
    0x1B,0x04, 0x17,0x02, 0x1A,0x03, 0x1A,0x03, 0x1A,0x0E,
    0x1B,0x04, 0x1A,0x03, 0x19,0x03, 0x17,0x03, 0x16,0x03,
    0x17,0x0D, 0x16,0x03, 0x17,0x03, 0x19,0x01, 0x19,0x03,
    0x19,0x03, 0x1A,0x03, 0x1F,0x03, 0x1B,0x03, 0x1B,0x03,
    0x1A,0x03, 0x17,0x0D, 0x16,0x03, 0x16,0x03, 0x16,0x03,
    0x17,0x01, 0x17,0x03, 0x17,0x03, 0x19,0x03, 0x1A,0x02,
    0x1A,0x02, 0x10,0x03, 0x17,0x0D, 0x16,0x03, 0x16,0x01,
    0x17,0x03, 0x19,0x03, 0x19,0x03, 0x17,0x03, 0x19,0x03,
    0x1F,0x02, 0x1B,0x03, 0x1A,0x03, 0x1A,0x0E, 0x1B,0x04,
    0x17,0x02, 0x1A,0x03, 0x1A,0x03, 0x1A,0x0E, 0x1B,0x04,
    0x17,0x16, 0x1A,0x03, 0x1A,0x03, 0x1A,0x0E, 0x1B,0x04,
    0x1A,0x03, 0x19,0x03, 0x17,0x03, 0x16,0x03, 0x0F,0x02,
    0x10,0x03, 0x15,0x00, 
    0,0
};
//童话
const uint8_t BSP_BUZZER_MUSIC_LIB_TH[]=
{
    0x0F,0x67, 0x19,0x03, 0x18,0x03, 0x17,0x02, 0x17,0x03,
    0x18,0x04, 0x17,0x0E, 0x17,0x02, 0x17,0x03, 0x18,0x03,
    0x17,0x03, 0x18,0x03, 0x17,0x03, 0x16,0x04, 0x15,0x18,
    0x15,0x03, 0x17,0x03, 0x19,0x03, 0x1A,0x02, 0x1A,0x03,
    0x1A,0x04, 0x19,0x0E, 0x19,0x03, 0x16,0x03, 0x16,0x03,
    0x18,0x04, 0x17,0x0E, 0x17,0x15, 0x15,0x03, 0x17,0x03,
    0x19,0x03, 0x1A,0x02, 0x1A,0x03, 0x1A,0x04, 0x19,0x0E,
    0x19,0x03, 0x16,0x03, 0x16,0x03, 0x18,0x03, 0x17,0x03,
    0x18,0x03, 0x17,0x03, 0x16,0x04, 0x15,0x0E, 0x15,0x02,
    0x16,0x03, 0x17,0x03, 0x10,0x02, 0x10,0x03, 0x15,0x03,
    0x15,0x02, 0x11,0x02, 0x15,0x14, 0x16,0x03, 0x17,0x03,
    0x10,0x02, 0x10,0x03, 0x15,0x03, 0x15,0x0D, 0x11,0x03,
    0x11,0x0D, 0x15,0x03, 0x15,0x00, 
    0,0
};

//同一首歌
const uint8_t BSP_BUZZER_MUSIC_LIB_TYSG[]=
{
    0x0F,0x01, 0x15,0x02, 0x16,0x02, 0x17,0x66, 0x18,0x03,
    0x17,0x02, 0x15,0x02, 0x16,0x01, 0x15,0x02, 0x10,0x02,
    0x15,0x00, 0x0F,0x01, 0x15,0x02, 0x16,0x02, 0x17,0x02,
    0x17,0x03, 0x18,0x03, 0x19,0x02, 0x15,0x02, 0x18,0x66,
    0x17,0x03, 0x19,0x02, 0x16,0x03, 0x17,0x03, 0x16,0x00,
    0x17,0x01, 0x19,0x02, 0x1B,0x02, 0x1B,0x70, 0x1A,0x03,
    0x1A,0x01, 0x19,0x02, 0x19,0x03, 0x1A,0x03, 0x1B,0x02,
    0x1A,0x0D, 0x19,0x03, 0x17,0x00, 0x18,0x66, 0x18,0x03,
    0x19,0x02, 0x1A,0x02, 0x19,0x0C, 0x18,0x0D, 0x17,0x03,
    0x16,0x01, 0x11,0x02, 0x11,0x03, 0x10,0x03, 0x0F,0x0C,
    0x10,0x02, 0x15,0x00, 0x1F,0x01, 0x1A,0x01, 0x18,0x66,
    0x19,0x03, 0x1A,0x01, 0x1B,0x02, 0x1B,0x03, 0x1B,0x03,
    0x1B,0x0C, 0x1A,0x0D, 0x19,0x03, 0x17,0x00, 0x1F,0x01,
    0x1A,0x01, 0x18,0x66, 0x19,0x03, 0x1A,0x01, 0x10,0x02,
    0x10,0x03, 0x10,0x03, 0x1A,0x0C, 0x18,0x0D, 0x17,0x03,
    0x16,0x00, 0x0F,0x01, 0x15,0x02, 0x16,0x02, 0x17,0x70,
    0x18,0x03, 0x17,0x02, 0x15,0x03, 0x15,0x03, 0x16,0x66,
    0x16,0x03, 0x16,0x02, 0x16,0x03, 0x15,0x03, 0x10,0x02,
    0x10,0x01, 0x11,0x01, 0x11,0x66, 0x10,0x03, 0x0F,0x0C,
    0x1A,0x02, 0x19,0x02, 0x16,0x03, 0x16,0x03, 0x18,0x66,
    0x18,0x03, 0x18,0x02, 0x17,0x03, 0x16,0x03, 0x19,0x00,
    0x00,0x00
};
//bibi
const uint8_t BSP_BUZZER_MUSIC_LIB_B[]=
{
    0x17,(10+0x80),
    0,0
};
const uint8_t BSP_BUZZER_MUSIC_LIB_L[]=
{
    0x17,(100+0x80),
    0,0
};
const uint8_t BSP_BUZZER_MUSIC_LIB_BB[]=
{
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0,0
};
const uint8_t BSP_BUZZER_MUSIC_LIB_BBB[]=
{
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0,0
};
const uint8_t BSP_BUZZER_MUSIC_LIB_BBBB[]=
{
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0,0
};
	const uint8_t BSP_BUZZER_MUSIC_LIB_BBBBB[]=
{
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0,0
};
const uint8_t BSP_BUZZER_MUSIC_LIB_BLB[]=
{
    0x17,(50+0x80),
    0x01,(5+0x80),
    0x17,(10+0x80),
    0,0
};
const uint8_t BSP_BUZZER_MUSIC_LIB_BL[]=
{
    0x17,(10+0x80),
    0x01,(5+0x80),
    0x17,(50+0x80),
    0,0
};
//"鹤拳"前奏(功夫游戏)
const uint8_t BSP_BUZZER_MUSIC_LIB_GF[]=
{
    0x1A,(20+0x80),
    0x01,(4+0x80),
    0x1A,(10+0x80),
    0x01,(4+0x80),
    0x1A,(10+0x80),
    0x01,(4+0x80),
    0x19,(20+0x80),
    0x01,(4+0x80),
    0x19,(20+0x80),
    0x01,(4+0x80),
    0x17,(20+0x80),
    0x01,(4+0x80),
    0x17,(20+0x80),
    0x01,(4+0x80),
    0x15,(40+0x80),
    0x01,(4+0x80),
    0x16,(10+0x80),
    0x01,(4+0x80),
    0x17,(10+0x80),
    0x01,(4+0x80),
    0x16,(10+0x80),
    0x01,(4+0x80),
    0x15,(10+0x80),
    0x01,(4+0x80),
    0x10,(20+0x80),
    0x01,(4+0x80),
    0x15,(20+0x80),
    0x01,(4+0x80),
    0x10,(20+0x80),
    0,0
};
#else
#define BSP_BUZZER_MUSIC_EXT extern
#endif
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_BUZZER_MUSIC_GPIO_TX_PORT     GPIOD
#define BSP_BUZZER_MUSIC_GPIO_TX_PIN      GPIO_Pin_12
#define BSP_BUZZER_MUSIC_GPIO_TX_SOURCE   GPIO_PinSource12
#define BSP_BUZZER_MUSIC_TIMX             TIM4
#define BSP_BUZZER_MUSIC_TIM_RCC_ENABLE   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)
#define BSP_BUZZER_MUSIC_TIM_RCC_DISABLE  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE)
#if   (defined(STM32F1))
#define BSP_BUZZER_MUSIC_GPIO_RCC_ENABLE  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE)
#define BSP_BUZZER_MUSIC_GPIO_REMAP       
#elif (defined(STM32F4))
#define BSP_BUZZER_MUSIC_GPIO_RCC_ENABLE  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)
#define BSP_BUZZER_MUSIC_GPIO_REMAP       GPIO_PinAFConfig(BSP_BUZZER_MUSIC_GPIO_TX_PORT,BSP_BUZZER_MUSIC_GPIO_TX_SOURCE,GPIO_AF_TIM4)
#endif
#elif (defined(NRF51)||defined(NRF52))
#if   (defined(XKAP_ICARE_B_M))
#if   (HARDWARE_SUB_VER==1)
#define BSP_BUZZER_MUSIC_GPIO_TX_PIN      14
#elif (HARDWARE_SUB_VER==2)
#define BSP_BUZZER_MUSIC_GPIO_TX_PIN      7
#endif
#endif
#endif
//-------------------接口宏定义(硬件无关)--------------------
//-------------------加载库函数------------------------------
//
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_MORNING[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_HTYS[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_HZCBDNH[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_LZHD[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_TH[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_TYSG[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_B[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_L[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_BB[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_BBB[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_BBBB[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_BBBBB[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_BLB[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_BL[];
BSP_BUZZER_MUSIC_EXT const uint8_t BSP_BUZZER_MUSIC_LIB_GF[];
//-------------------接口宏定义(硬件相关)--------------------
BSP_BUZZER_MUSIC_EXT void Bsp_BuzzerMusic_PlayMusic_10ms(void);
BSP_BUZZER_MUSIC_EXT uint8_t Bsp_BuzzerMusic_PlayStop(uint8_t OnOff,const uint8_t *p_musicLib,uint16_t maxtime_s);
BSP_BUZZER_MUSIC_EXT uint8_t *Bsp_BuzzerMusic_GetState(void);
BSP_BUZZER_MUSIC_EXT void BspBuzzerMusic_DebugTestOnOff(uint8_t OnOff);
//