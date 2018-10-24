/**
  ******************************************************************************
  * @file    Bsp_StepperMotor.c
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

/* Includes ------------------------------------------------------------------*/
#include "Bsp_Hs0038.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_ppi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const   nrf_drv_timer_t Capture_Timer = NRF_DRV_TIMER_INSTANCE(BSP_HS0038_TIMER);
/* Extern variables ----------------------------------------------------------*/
bool    Hs0038_Rx_Sem = false;
BSP_HS0038_S_INFO   g_tIR;
/* Private function prototypes -----------------------------------------------*/
static  void Bsp_Cal_Pluse_Time(void);
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   定时器中断函数
 * @note    定时器中断函数
 * @param   None
 * @return  None
 */
static  void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context)
{}
/**
 * @brief   捕捉中断函数
 * @note    捕捉中断函数
 * @param   None
 * @return  None
 */
void Bsp_Gpiote_Event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    Bsp_Cal_Pluse_Time();
}

/**
 * @brief   初始化函数
 * @note    初始化GPIO与定时器等外设
 * @param   None
 * @return  None
 */
void BspHs0038_Init(void)
{
#if     (defined(STM32F1)||defined(STM32F4))
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    // RCC
    BSP_HS0038_RCC_ENABLE;
    // GPIO
#ifdef  STM32F1
    GPIO_InitStructure.GPIO_Mode    =   GPIO_Mode_IPU;
#endif
#ifdef  STM32F4
    GPIO_InitStructure.GPIO_Mode    =   GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType   =   GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =   GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   =   GPIO_Speed_100MHz;
#endif
    GPIO_InitStructure.GPIO_Pin     =   BSP_HS0038_PIN;
    GPIO_Init(BSP_HS0038_PORT, &GPIO_InitStructure);
    // TIM
    {


        /* TIM4 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

        /* GPIOB clock enable */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        /* TIM3 chennel3 configuration : PB.0 */
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;       /* 选择复用功能 */
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;      /* 上拉电阻使能 */
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Connect TIM pin to AF3 */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);

        /* Enable the TIM3 global Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
        TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
        TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
        TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           /* 每次跳变都产生1次捕获事件 */
        TIM_ICInitStructure.TIM_ICFilter = 0x0;

        TIM_ICInit(TIM3, &TIM_ICInitStructure);

        /* 设置分频为 1680/2， 捕获计数器值的单位正好是 10us, 方便脉宽比较 */
        TIM_PrescalerConfig(TIM3, 1680 / 2, TIM_PSCReloadMode_Immediate);

        /* TIM enable counter */
        TIM_Cmd(TIM3, ENABLE);

        /* Enable the CC3 Interrupt Request */
        TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);

        g_tIR.LastCapture = 0;
        g_tIR.Status = 0;
    }
#elif   (defined(NRF51)||defined(NRF52))
    uint32_t capture_task_addr;
    uint32_t gpiote_event_addr;
    nrf_ppi_channel_t   Capture_channel = BSP_HS0038_PPI_CHANNEL;
    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    //初始化定时器
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.frequency = NRF_TIMER_FREQ_1MHz;
    //初始化ppi
    nrf_drv_ppi_init();
    //初始化gpioe
    nrf_drv_gpiote_init();
    nrf_drv_timer_init(&Capture_Timer, &timer_cfg, timer_dummy_handler);
    //设置task和event
    //---初始化GPIO，生成event
    nrf_drv_gpiote_in_init(BSP_HS0038_PIN,&config,Bsp_Gpiote_Event_handler);
    //---分配ppi通道
    nrf_drv_ppi_channel_alloc(&Capture_channel);
    //---得到任务地址
    capture_task_addr = nrf_drv_timer_capture_task_address_get(&Capture_Timer,NRF_TIMER_TASK_CAPTURE3);
    //---得到事件地址
    gpiote_event_addr = nrf_drv_gpiote_in_event_addr_get(BSP_HS0038_PIN);
    //---使用ppi将task和event联系起来
    nrf_drv_ppi_channel_assign(Capture_channel, gpiote_event_addr, capture_task_addr);
    //---使能ppi
    nrf_drv_ppi_channel_enable(Capture_channel);
    //---使能event
    nrf_drv_gpiote_in_event_enable(BSP_HS0038_PIN, 1);
    //使能定时器
    nrf_drv_timer_enable(&Capture_Timer);
#endif
}

/*
*********************************************************************************************************
*   函 数 名: BspHs0038_IrdDecodeNec
*   功能说明: 按照NEC编码格式实时解码
*   形    参: _width 脉冲宽度，单位 10us
*   返 回 值: 无
*********************************************************************************************************
*/
static void BspHs0038_IrdDecodeNec(uint16_t _width)
{
    static uint16_t s_LowWidth;
    static uint8_t s_Byte;
    static uint8_t s_Bit;
    uint16_t TotalWitdh;

    /* NEC 格式 （5段）
        1、引导码  9ms低 + 4.5ms高
        2、低8位地址码  0=1.125ms  1=2.25ms    bit0先传
        3、高8位地址码  0=1.125ms  1=2.25ms
        4、8位数据      0=1.125ms  1=2.25ms
        5、8为数码反码  0=1.125ms  1=2.25ms
    */

loop1:
    switch (g_tIR.Status)
    {
        case 0:         /* 使用三星手机作为红外发送源， */
            if (((_width > 400) && (_width < 500)) ||((_width > 700) && (_width < 1100)))/* 同步码 4.5ms */
            {
                g_tIR.Status = 1;
                s_Byte = 0;
                s_Bit = 0;
            }
            break;

        case 1:         /* 413 判断引导码高信号  3ms - 6ms */
            if ((_width > 313) && (_width < 600))   /* 引导码 4.5ms */
            {
                g_tIR.Status = 2;
            }
            else if ((_width > 150) && (_width < 250))  /* 2.25ms */
            {
#ifdef IR_REPEAT_SEND_EN
                if (g_tIR.RepeatCount >= IR_REPEAT_FILTER)
                {
                    bsp_PutKey(g_tIR.RxBuf[2] + IR_KEY_STRAT);  /* 连发码 */
                }
                else
                {
                    g_tIR.RepeatCount++;
                }
#endif
                g_tIR.Status = 0;   /* 复位解码状态 */
            }
            else
            {
                /* 异常脉宽 */
                g_tIR.Status = 0;   /* 复位解码状态 */
            }
            break;

        case 2:         /* 低电平期间 0.56ms */
            if ((_width > 10) && (_width < 100))
            {
                g_tIR.Status = 3;
                s_LowWidth = _width;    /* 保存低电平宽度 */
            }
            else    /* 异常脉宽 */
            {
                /* 异常脉宽 */
                g_tIR.Status = 0;   /* 复位解码器状态 */
                goto loop1;     /* 继续判断同步信号 */
            }
            break;

        case 3:         /* 85+25, 64+157 开始连续解码32bit */
            TotalWitdh = s_LowWidth + _width;
            /* 0的宽度为1.125ms，1的宽度为2.25ms */
            s_Byte >>= 1;
            if ((TotalWitdh > 92) && (TotalWitdh < 132))
            {
                ;                   /* bit = 0 */
            }
            else if ((TotalWitdh > 205) && (TotalWitdh < 245))
            {
                s_Byte += 0x80;     /* bit = 1 */
            }
            else
            {
                /* 异常脉宽 */
                g_tIR.Status = 0;   /* 复位解码器状态 */
                goto loop1;     /* 继续判断同步信号 */
            }

            s_Bit++;
            if (s_Bit == 8) /* 收齐8位 */
            {
                g_tIR.RxBuf[0] = s_Byte;
                s_Byte = 0;
            }
            else if (s_Bit == 16)   /* 收齐16位 */
            {
                g_tIR.RxBuf[1] = s_Byte;
                s_Byte = 0;
            }
            else if (s_Bit == 24)   /* 收齐24位 */
            {
                g_tIR.RxBuf[2] = s_Byte;
                s_Byte = 0;
            }
            else if (s_Bit == 32)   /* 收齐32位 */
            {
                g_tIR.RxBuf[3] = s_Byte;

                if (g_tIR.RxBuf[2] + g_tIR.RxBuf[3] == 255) /* 检查校验 */
                {
//                  bsp_PutKey(g_tIR.RxBuf[2] + IR_KEY_STRAT);  /* 将键值放入KEY FIFO */
                    g_tIR.RepeatCount = 0;  /* 重发计数器 */
                }

                g_tIR.Status = 0;   /* 等待下一组编码 */
//              g_tIR.LastCapture = 0;  /*新一组数据，本次清零*/
                Hs0038_Rx_Sem = true;
                break;
            }
            g_tIR.Status = 2;   /* 继续下一个bit */
            break;
    }
}

/*
*********************************************************************************************************
*   函 数 名: Capture_Handler
*   功能说明: 捕获动作出现是的中断函数，计算脉冲宽度
*   形    参: pin使用的GPIO引脚；action GPIO的动作类型
*   返 回 值: 无
*********************************************************************************************************
*/
void Bsp_Cal_Pluse_Time(void)
{
    uint32_t NowCapture;
    uint32_t Width;

    NowCapture = nrf_drv_timer_capture(&Capture_Timer,NRF_TIMER_CC_CHANNEL3);
    if (NowCapture >= g_tIR.LastCapture)
    {
        Width = NowCapture - g_tIR.LastCapture;
    }
    else if (NowCapture < g_tIR.LastCapture)    /* 计数器抵达最大并翻转 */
    {
        Width = ((0xFFFFFFFF - g_tIR.LastCapture) + NowCapture);
    }

    Width = Width/10;

    if(g_tIR.LastCapture == 0)
    {
        g_tIR.LastCapture = NowCapture;
        return;
    }

    if(g_tIR.Status == 0)
    {
        ;
    }

    g_tIR.LastCapture = NowCapture; /* 保存当前计数器，用于下次计算差值 */

    BspHs0038_IrdDecodeNec(Width);       /* 解码 */
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/