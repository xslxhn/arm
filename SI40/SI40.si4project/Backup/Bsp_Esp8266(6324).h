﻿/**
  ******************************************************************************
  * @file    bsp_Esp8266.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于Esp8266的Wifi驱动.
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
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Exported types ------------------------------------------------------------*/
typedef enum BSP_ESP8266_E_CH
{
	BSP_ESP8266_E_CH_1=0,
	BSP_ESP8266_E_CH_2,
	BSP_ESP8266_E_CH_3,
	BSP_ESP8266_E_CH_4,
	BSP_ESP8266_E_CH_5,
	BSP_ESP8266_E_CH_MAX,
}BSP_ESP8266_E_CH;
//-----信息结构体
typedef struct BSP_ESP8266_S_INFO
{
    uint8_t  LotalIP_AP[4]; 	// 本地IP
    uint8_t  LotalIP_STA[4];	// 本地IP
    uint8_t  MacAddr_AP[6];		// Mac地址
    uint8_t  MacAddr_STA[6];	// Mac地址
    uint8_t  SignalVal;     	// 信号强度
    uint8_t  Flag_Install	:1;	// 0-没有安装此模块,1-安装了此模块
    uint8_t  Flag_OnOff		:1; // ON-启用模块 OFF-关闭模块
    uint8_t  Flag_Connect_1	:1; // 1通道连接状态 0-未连接 1-已连接
	uint8_t  Flag_Connect_2	:1; // 2通道连接状态 0-未连接 1-已连接
	uint8_t  Flag_Connect_3	:1; // 3通道连接状态 0-未连接 1-已连接
	uint8_t  Flag_Connect_4	:1; // 4通道连接状态 0-未连接 1-已连接
	uint8_t  Flag_Connect_5	:1; // 5通道连接状态 0-未连接 1-已连接
    uint8_t  Flag_Res		:1;	
	uint8_t	 Res[2];
} BSP_ESP8266_S_INFO;
extern BSP_ESP8266_S_INFO		BspEsp8266_s_Info;
/* Exported constants --------------------------------------------------------*/
/* Transplant define ---------------------------------------------------------*/
#if   	(defined(XKAP_ICARE_B_M))
#include "nrf_delay.h"
#define BSP_ESP8266_RST_PIN			17
#define BSP_ESP8266_UARTX			1
#elif	(defined(PROJECT_XKAP_V3))
#define	BSP_ESP8266_RST_PORT		GPIOB
#define	BSP_ESP8266_RST_PIN			GPIO_Pin_1
#define BSP_ESP8266_RCC_ENABLE  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB ,ENABLE)
#define BSP_ESP8266_UARTX			3
#define BSP_ESP8266_AUTO_CONNECT_ENABLE	0	
#define BSP_ESP8266_CONNECT_1		"AT+CIPSTART=0,\"UDP\",\"10.10.18.202\",8080,8824,0\r\n"
#define BSP_ESP8266_RX_PARSE_CH1    BspEsp8266_TestParse
//#define BSP_ESP8266_CONNECT_2		NULL		
//#define BSP_ESP8266_RX_PARSE_CH2    
//#define BSP_ESP8266_CONNECT_3		NULL
//#define BSP_ESP8266_RX_PARSE_CH3  
//#define BSP_ESP8266_CONNECT_4		NULL
//#define BSP_ESP8266_RX_PARSE_CH4  
//#define BSP_ESP8266_CONNECT_5		NULL
//#define BSP_ESP8266_RX_PARSE_CH5  
#elif	(defined(PROJECT_ARMFLY_V5_XSL))
#define	BSP_ESP8266_RST_PORT		GPIOB
#define	BSP_ESP8266_RST_PIN			GPIO_Pin_7
#define	BSP_ESP8266_PWR_PORT		GPIOI
#define	BSP_ESP8266_PWR_PIN			GPIO_Pin_0
#define BSP_ESP8266_GPIO0_PORT		GPIOG
#define BSP_ESP8266_GPIO0_PIN		GPIO_Pin_8
#define BSP_ESP8266_GPIO2_PORT		GPIOG
#define BSP_ESP8266_GPIO2_PIN		GPIO_Pin_15
#define BSP_ESP8266_UARTX			6
#else 
#error Please Set Project to Bsp_Esp8266.h
#endif
/* Application define --------------------------------------------------------*/
#if		(defined(STM32F1)||defined(STM32F4))
#ifdef	BSP_ESP8266_PWR_PIN
/* 硬件掉电控制引脚 -- 接 3.3V 开始工作  */
#define BSP_ESP8266_PWR_OFF			GPIO_ResetBits(BSP_ESP8266_PWR_PORT, BSP_ESP8266_PWR_PIN)
#define BSP_ESP8266_PWR_ON			GPIO_SetBits(BSP_ESP8266_PWR_PORT, BSP_ESP8266_PWR_PIN)
#else
#define BSP_ESP8266_PWR_OFF			
#define BSP_ESP8266_PWR_ON			
#endif
#ifdef	BSP_ESP8266_RST_PIN
/* 硬件复位引脚 -- 可以不接 */
#define BSP_ESP8266_RST_L			GPIO_ResetBits(BSP_ESP8266_RST_PORT, BSP_ESP8266_RST_PIN)
#define BSP_ESP8266_RST_H			GPIO_SetBits(BSP_ESP8266_RST_PORT, BSP_ESP8266_RST_PIN)
#else
#define BSP_ESP8266_RST_L			
#define BSP_ESP8266_RST_H			
#endif
#ifdef	BSP_ESP8266_GPIO0_PIN
/* 1表示进入固件升级模式 0表示正常AT指令模式 */
#define BSP_ESP8266_EXIT_ISP		GPIO_ResetBits(BSP_ESP8266_GPIO0_PORT, BSP_ESP8266_GPIO0_PIN)
#define BSP_ESP8266_ENTER_ISP		GPIO_SetBits(BSP_ESP8266_GPIO0_PORT, BSP_ESP8266_GPIO0_PIN)
#else
#define BSP_ESP8266_EXIT_ISP		
#define BSP_ESP8266_ENTER_ISP		
#endif
#ifdef	BSP_ESP8266_GPIO2_PIN
/* 驱动为高，无其他用途 */
#define BSP_ESP8266_GPIO2_L			GPIO_ResetBits(BSP_ESP8266_GPIO2_PORT, BSP_ESP8266_GPIO2_PIN)
#define BSP_ESP8266_GPIO2_H			GPIO_SetBits(BSP_ESP8266_GPIO2_PORT, BSP_ESP8266_GPIO2_PIN)
#else
#define BSP_ESP8266_GPIO2_L			
#define BSP_ESP8266_GPIO2_H			
#endif
#elif	(defined(NRF51)||defined(NRF52))
#ifdef	BSP_ESP8266_PWR_PIN
/* 硬件掉电控制引脚 -- 接 3.3V 开始工作  */
#define BSP_ESP8266_PWR_OFF			nrf_gpio_pin_write(BSP_ESP8266_PWR_PIN, 0)
#define BSP_ESP8266_PWR_ON			nrf_gpio_pin_write(BSP_ESP8266_PWR_PIN, 1)
#else
#define BSP_ESP8266_PWR_OFF			
#define BSP_ESP8266_PWR_ON			
#endif
#ifdef	BSP_ESP8266_RST_PIN
/* 硬件复位引脚 -- 可以不接 */
#define BSP_ESP8266_RST_L			nrf_gpio_pin_write(BSP_ESP8266_RST_PIN, 0)
#define BSP_ESP8266_RST_H			nrf_gpio_pin_write(BSP_ESP8266_RST_PIN, 1)
#else
#define BSP_ESP8266_RST_L			
#define BSP_ESP8266_RST_H			
#endif
#ifdef	BSP_ESP8266_GPIO0_PIN
/* 1表示进入固件升级模式 0表示正常AT指令模式 */
#define BSP_ESP8266_EXIT_ISP		nrf_gpio_pin_write(BSP_ESP8266_GPIO0_PIN, 0)
#define BSP_ESP8266_ENTER_ISP		nrf_gpio_pin_write(BSP_ESP8266_GPIO0_PIN, 1)
#else
#define BSP_ESP8266_EXIT_ISP		
#define BSP_ESP8266_ENTER_ISP		
#endif
#ifdef	BSP_ESP8266_GPIO2_PIN
/* 驱动为高，无其他用途 */
#define BSP_ESP8266_GPIO2_L			nrf_gpio_pin_write(BSP_ESP8266_GPIO2_PIN, 0)
#define BSP_ESP8266_GPIO2_H			nrf_gpio_pin_write(BSP_ESP8266_GPIO2_PIN, 1)
#else
#define BSP_ESP8266_GPIO2_L			
#define BSP_ESP8266_GPIO2_H			
#endif

#endif
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void BspEsp8266_Init(void);
void BspEsp8266_InterruptRx(uint8_t *pbuf,uint16_t len);
void BspEsp8266_100ms(void);
uint8_t BspEsp8266_Tx(BSP_ESP8266_E_CH ch,uint8_t *pbuf,uint16_t len);
// 用于打印出程序操控流程
void BspEsp8266_DebugTestOnOff(uint8_t OnOff);
// 用于完全通过 DEBUG 操控模块
void BspEsp8266_DebugControlOnOff(uint8_t OnOff);
void BspEsp8266_DebugRx(uint8_t *d,uint16_t len);


//#define COM_ESP8266	COM6		/* 选择串口 */

#if 0
/* 串口设备结构体 */
typedef struct
{
	char ssid[33];	/* SSID是一个无线局域网络（WLAN）的名称。SSID是区分大小写的文本字符串，最大长度32个字符 */
	uint8_t ecn;	/* 加密方式 
						0   OPEN
						1   WEP
						2   WPA_PSK
						3   WPA2_PSK
						4   WPA_WPA2_PSK
					*/
	int32_t rssi;		/* 信号强度 */
	uint8_t mac[20];	/* MAC地址字符串*/
	uint8_t ch;			/* 信道 */
}WIFI_AP_T;
/* 加密方式 */
enum
{
	ECN_OPEN = 0,
	ECN_WEP = 1,
	ECN_WPA_PSK = 2,
	ECN_WPA2_PSK = 3,
	ECN_WPA_WPA2_PSK = 4,
};

#endif

/* 定义下面这句话, 将把收到的字符发送到调试串口1 */
//#define ESP8266_TO_COM1_EN

/* 本模块部分函数用到了软件定时器最后1个ID。 因此主程序调用本模块的函数时，请注意回避定时器 TMR_COUNT - 1。
  bsp_StartTimer(3, _usTimeOut);

  TMR_COUNT 在 bsp_timer.h 文件定义
*/
//#define ESP8266_TMR_ID	(TMR_COUNT - 1)

/* 供外部调用的函数声明 */
/*
void bsp_InitESP8266(void);
void ESP8266_Reset(void);
void ESP8266_PowerOn(void);
void ESP8266_PowerOff(void);
void ESP8266_EnterISP(void);
void ESP8266_ExitISP(void);
void ESP8266_SendAT(char *_Cmd);

uint8_t ESP8266_WaitResponse(char *_pAckStr, uint16_t _usTimeOut);
void ESP8266_PrintRxData(uint8_t _ch);
void ESP8266_JoinAP(char *_ssid, char *_pwd);
int16_t ESP8266_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum);
uint16_t ESP8266_RxNew(uint8_t *_pRxBuf);
*/
#endif

/************************ (C) COPYRIGHT XSLXHN *****END OF FILE****************/

