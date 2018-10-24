﻿/**
  ******************************************************************************
  * @file    Can.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于STM内部的Can总线操作.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */

#ifndef __CAN_H 
#define __CAN_H
//-------------------------------------------------------------------------------宏定义
//-------------------------------------------------------------------------------接口函数
void Can_Test_100ms(void);
void CAN_RX_IRQHandler(void);
//
#endif
/********************************************************************************
      END FILE
********************************************************************************/
