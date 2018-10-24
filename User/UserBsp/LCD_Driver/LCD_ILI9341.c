/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#include "font.h"
#include "LCD_ILI9341.h"
#include "Bsp_PwmOut.h"
#include "uctsk_Debug.h"

//LCD的画笔颜色和背景色
uint16_t LCD_Ili9341_PointColor=0x0000;
uint16_t LCD_Ili9341_BackColor=0xFFFF;

uint8_t LCD_ILI9341_SleepFlag=0;

//管理LCD重要参数
//默认为竖屏
LCD_ILI9341_S_DEV Lcd_Ili9341_s_dev;
//-------------------------------------------------------------------------------
//static void LCD_ILI9341_WR_REG(uint16_t regval);
//static void LCD_ILI9341_WR_DATA(uint16_t data);
//static uint16_t LCD_ILI9341_RD_DATA(void);
static void LCD_ILI9341_SetCursor(uint16_t Xpos, uint16_t Ypos);
//-------------------------------------------------------------------------------
//                            硬件接口相关(如果用FSMC总线,用宏来替代函数)
//-------------------------------------------------------------------------------
void LCD_ILI9341_HwInit(void)
{
#if   (LCD_ILI9341_MODE == LCD_ILI9341_MODE_FSMC)
    //使能FSMC时钟
    RCC->AHBENR|=1<<8;
    //使能PORTB时钟
    RCC->APB2ENR|=1<<3;
    //使能PORTD时钟
    RCC->APB2ENR|=1<<5;
    //使能PORTE时钟
    RCC->APB2ENR|=1<<6;
    //使能PORTG时钟
    RCC->APB2ENR|=1<<8;
    //PB0 推挽输出 背光
    GPIOB->CRL&=0XFFFFFFF0;
    GPIOB->CRL|=0X00000003;
    //PORTD复用推挽输出
    GPIOD->CRH&=0X00FFF000;
    GPIOD->CRH|=0XBB000BBB;
    GPIOD->CRL&=0XFF00FF00;
    GPIOD->CRL|=0X00BB00BB;
    //PORTE复用推挽输出
    GPIOE->CRH&=0X00000000;
    GPIOE->CRH|=0XBBBBBBBB;
    GPIOE->CRL&=0X0FFFFFFF;
    GPIOE->CRL|=0XB0000000;
    //PORTG12复用推挽输出 A0
    GPIOG->CRH&=0XFFF0FFFF;
    GPIOG->CRH|=0X000B0000;
    //PG0->RS
    GPIOG->CRL&=0XFFFFFFF0;
    GPIOG->CRL|=0X0000000B;

    //寄存器清零
    //bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。
    //这里我们使用NE4 ，也就对应BTCR[6],[7]。
    FSMC_Bank1->BTCR[6]=0X00000000;
    FSMC_Bank1->BTCR[7]=0X00000000;
    FSMC_Bank1E->BWTR[6]=0X00000000;
    //操作BCR寄存器 使用异步模式
    FSMC_Bank1->BTCR[6]|=1<<12;     //存储器写使能
    FSMC_Bank1->BTCR[6]|=1<<14;     //读写使用不同的时序
    FSMC_Bank1->BTCR[6]|=1<<4;      //存储器数据宽度为16bit
    //操作BTR寄存器
    //读时序控制寄存器
    FSMC_Bank1->BTCR[7]|=0<<28;     //模式A
    FSMC_Bank1->BTCR[7]|=1<<0;      //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
    //因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
    FSMC_Bank1->BTCR[7]|=0XF<<8;    //数据保存时间为16个HCLK
    //写时序控制寄存器
    FSMC_Bank1E->BWTR[6]|=0<<28;    //模式A
    FSMC_Bank1E->BWTR[6]|=0<<0;     //地址建立时间（ADDSET）为1个HCLK
    //4个HCLK（HCLK=72M）因为液晶驱动IC的写信号脉宽，最少也得50ns。72M/4=24M=55ns
    FSMC_Bank1E->BWTR[6]|=3<<8;     //数据保存时间为4个HCLK
    //使能BANK1,区域4
    FSMC_Bank1->BTCR[6]|=1<<0;      //使能BANK1，区域4
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_GPIO)
    GPIO_InitTypeDef  GPIO_InitStructure;
    BSP_LCD_ILI9341_RCC_ENABLE;
    //
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    /*
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_IM0;
    GPIO_Init(LCD_ILI9341_PORT_IM0, &GPIO_InitStructure);
    LCD_ILI9341_IM0_SET;
    */
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_WR;
    GPIO_Init(LCD_ILI9341_PORT_WR, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_RS;
    GPIO_Init(LCD_ILI9341_PORT_RS, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_RST;
    GPIO_Init(LCD_ILI9341_PORT_RST, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_RD;
    GPIO_Init(LCD_ILI9341_PORT_RD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_CS;
    GPIO_Init(LCD_ILI9341_PORT_CS, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D0;
    GPIO_Init(LCD_ILI9341_PORT_D0, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D1;
    GPIO_Init(LCD_ILI9341_PORT_D1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D2;
    GPIO_Init(LCD_ILI9341_PORT_D2, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D3;
    GPIO_Init(LCD_ILI9341_PORT_D3, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D4;
    GPIO_Init(LCD_ILI9341_PORT_D4, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D5;
    GPIO_Init(LCD_ILI9341_PORT_D5, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D6;
    GPIO_Init(LCD_ILI9341_PORT_D6, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D7;
    GPIO_Init(LCD_ILI9341_PORT_D7, &GPIO_InitStructure);
    //初始化背光引脚
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    // 时钟
    BSP_LCD_ILI9341_RCC_ENABLE;
    // Remap
    LCD_ILI9341_SPI_MOSI_REMAP;
    LCD_ILI9341_SPI_MISO_REMAP;
    LCD_ILI9341_SPI_SCLK_REMAP;
    // GPIO
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_RST;
    GPIO_Init(LCD_ILI9341_PORT_RST, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_SPI_CS_PIN;
    GPIO_Init(LCD_ILI9341_SPI_CS_PORT, &GPIO_InitStructure);
    LCD_ILI9341_SPI_CS_HIGH;
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_DCX;
    GPIO_Init(LCD_ILI9341_PORT_DCX, &GPIO_InitStructure);
    // SPI - GPIO
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_SPI_MOSI_PIN;
    GPIO_Init(LCD_ILI9341_SPI_MOSI_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_SPI_MISO_PIN;
    GPIO_Init(LCD_ILI9341_SPI_MISO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_SPI_SCLK_PIN;
    GPIO_Init(LCD_ILI9341_SPI_SCLK_PORT, &GPIO_InitStructure);
    // SPI - CONF
    LCD_ILI9341_SPI_RCC_ENABLE;
    SPI_InitStructure.SPI_Direction =  SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      =  SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  =  SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      =  SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      =  SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS       =  SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler  =  SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit  =  SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial      =  7;
    SPI_Init(LCD_ILI9341_SPI_SPIX, &SPI_InitStructure);
    SPI_Cmd(LCD_ILI9341_SPI_SPIX, DISABLE);
    SPI_Cmd(LCD_ILI9341_SPI_SPIX, ENABLE);
#endif
    //执行硬件复位
    LCD_ILI9341_RST_SET;
    MODULE_OS_DELAY_MS(1);
    LCD_ILI9341_RST_CLR;
    MODULE_OS_DELAY_MS(10);
    LCD_ILI9341_RST_SET;
    MODULE_OS_DELAY_MS(120);
}

#if   (LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
uint8_t LCD_ILI9341_WR_Byte(uint8_t data)
{
    //Loop while DR register in not emplty
    while (SPI_I2S_GetFlagStatus(LCD_ILI9341_SPI_SPIX, SPI_I2S_FLAG_TXE) == RESET);
    //Send byte through the SPI2 peripheral
    SPI_I2S_SendData(LCD_ILI9341_SPI_SPIX, data);
    //Wait to receive a byte
    while (SPI_I2S_GetFlagStatus(LCD_ILI9341_SPI_SPIX, SPI_I2S_FLAG_RXNE) == RESET);
    //Return the byte read from the SPI bus
    return SPI_I2S_ReceiveData(LCD_ILI9341_SPI_SPIX);
}
#endif

//写寄存器
//static void LCD_ILI9341_WR_REG(uint16_t regval)
void LCD_ILI9341_WR_REG(uint16_t regval)
{
#if   (LCD_ILI9341_MODE == LCD_ILI9341_MODE_FSMC)
    LCD_ILI9341_FSMC->LCD_ILI9341_REG=regval;
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_GPIO)
    LCD_ILI9341_RD_SET;
    LCD_ILI9341_RS_CLR;
    LCD_ILI9341_CS_CLR;
    //
    LCD_ILI9341_WR_CLR;
#ifdef LCD_ILI9341_PORT_DATA
    //加速算法
#if   (defined(STM32F1))
    regval<<=8;
    LCD_ILI9341_PORT_DATA->BSRR=regval;
    regval^=0xFF00;
    LCD_ILI9341_PORT_DATA->BRR=regval;
#elif (defined(STM32F4))
    regval<<=8;
    LCD_ILI9341_PORT_DATA->BSRRL=regval;
    regval^=0xFF00;
    LCD_ILI9341_PORT_DATA->BSRRH=regval;
#endif
#else
    //普通算法
    if(regval&(1<<0))   LCD_ILI9341_D0_SET;
    else                LCD_ILI9341_D0_CLR;
    if(regval&(1<<1))   LCD_ILI9341_D1_SET;
    else                LCD_ILI9341_D1_CLR;
    if(regval&(1<<2))   LCD_ILI9341_D2_SET;
    else                LCD_ILI9341_D2_CLR;
    if(regval&(1<<3))   LCD_ILI9341_D3_SET;
    else                LCD_ILI9341_D3_CLR;
    if(regval&(1<<4))   LCD_ILI9341_D4_SET;
    else                LCD_ILI9341_D4_CLR;
    if(regval&(1<<5))   LCD_ILI9341_D5_SET;
    else                LCD_ILI9341_D5_CLR;
    if(regval&(1<<6))   LCD_ILI9341_D6_SET;
    else                LCD_ILI9341_D6_CLR;
    if(regval&(1<<7))   LCD_ILI9341_D7_SET;
    else                LCD_ILI9341_D7_CLR;
#endif
    //
    LCD_ILI9341_WR_SET;
    //LCD_ILI9341_CS_SET;
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
    LCD_ILI9341_SPI_CS_LOW;
    LCD_ILI9341_DCX_CLR;
    LCD_ILI9341_WR_Byte(regval);
#endif
}
//写LCD数据
//static void LCD_ILI9341_WR_DATA(uint16_t data)
void LCD_ILI9341_WR_DATA(uint16_t data)
{
#if   (LCD_ILI9341_MODE == LCD_ILI9341_MODE_FSMC)
    LCD_ILI9341_FSMC->LCD_ILI9341_RAM=data;
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_GPIO)
    LCD_ILI9341_CS_CLR;
    LCD_ILI9341_RS_SET;
    LCD_ILI9341_RD_SET;
    //
    LCD_ILI9341_WR_CLR;
#ifdef LCD_ILI9341_PORT_DATA
    //加速算法
#if   (defined(STM32F1))
    data<<=8;
    LCD_ILI9341_PORT_DATA->BSRR=data;
    data^=0xFF00;
    LCD_ILI9341_PORT_DATA->BRR=data;
#elif (defined(STM32F4))
    data<<=8;
    LCD_ILI9341_PORT_DATA->BSRRL=data;
    data^=0xFF00;
    LCD_ILI9341_PORT_DATA->BSRRH=data;
#endif
#else
    //普通算法
    if(data&(1<<0))     LCD_ILI9341_D0_SET;
    else                LCD_ILI9341_D0_CLR;
    if(data&(1<<1))     LCD_ILI9341_D1_SET;
    else                LCD_ILI9341_D1_CLR;
    if(data&(1<<2))     LCD_ILI9341_D2_SET;
    else                LCD_ILI9341_D2_CLR;
    if(data&(1<<3))     LCD_ILI9341_D3_SET;
    else                LCD_ILI9341_D3_CLR;
    if(data&(1<<4))     LCD_ILI9341_D4_SET;
    else                LCD_ILI9341_D4_CLR;
    if(data&(1<<5))     LCD_ILI9341_D5_SET;
    else                LCD_ILI9341_D5_CLR;
    if(data&(1<<6))     LCD_ILI9341_D6_SET;
    else                LCD_ILI9341_D6_CLR;
    if(data&(1<<7))     LCD_ILI9341_D7_SET;
    else                LCD_ILI9341_D7_CLR;
#endif
    //
    LCD_ILI9341_WR_SET;
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
    LCD_ILI9341_SPI_CS_LOW;
    LCD_ILI9341_DCX_SET;
    LCD_ILI9341_WR_Byte(data);
#endif

}
//读LCD数据
uint16_t LCD_ILI9341_RD_DATA(void)
{
#if   (LCD_ILI9341_MODE == LCD_ILI9341_MODE_FSMC)
    return LCD_ILI9341_FSMC->LCD_ILI9341_RAM;
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_GPIO)
    uint16_t i16;
    GPIO_InitTypeDef  GPIO_InitStructure;
    //将GPIO设为输入状态
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IPU;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
#endif
#ifdef LCD_ILI9341_PORT_DATA
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D0\
                                       |LCD_ILI9341_PIN_D1\
                                       |LCD_ILI9341_PIN_D2\
                                       |LCD_ILI9341_PIN_D3\
                                       |LCD_ILI9341_PIN_D4\
                                       |LCD_ILI9341_PIN_D5\
                                       |LCD_ILI9341_PIN_D6\
                                       |LCD_ILI9341_PIN_D7;
    GPIO_Init(LCD_ILI9341_PORT_DATA, &GPIO_InitStructure);
#ifdef STM32F4
    LCD_ILI9341_PORT_DATA->BSRRL    =  LCD_ILI9341_PIN_D0\
                                       |LCD_ILI9341_PIN_D1\
                                       |LCD_ILI9341_PIN_D2\
                                       |LCD_ILI9341_PIN_D3\
                                       |LCD_ILI9341_PIN_D4\
                                       |LCD_ILI9341_PIN_D5\
                                       |LCD_ILI9341_PIN_D6\
                                       |LCD_ILI9341_PIN_D7;
#endif
#ifdef STM32F1
    LCD_ILI9341_PORT_DATA->BSRR         = LCD_ILI9341_PIN_D0\
                                          |LCD_ILI9341_PIN_D1\
                                          |LCD_ILI9341_PIN_D2\
                                          |LCD_ILI9341_PIN_D3\
                                          |LCD_ILI9341_PIN_D4\
                                          |LCD_ILI9341_PIN_D5\
                                          |LCD_ILI9341_PIN_D6\
                                          |LCD_ILI9341_PIN_D7;
#endif
#else
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D0;
    GPIO_Init(LCD_ILI9341_PORT_D0, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D1;
    GPIO_Init(LCD_ILI9341_PORT_D1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D2;
    GPIO_Init(LCD_ILI9341_PORT_D2, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D3;
    GPIO_Init(LCD_ILI9341_PORT_D3, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D4;
    GPIO_Init(LCD_ILI9341_PORT_D4, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D5;
    GPIO_Init(LCD_ILI9341_PORT_D5, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D6;
    GPIO_Init(LCD_ILI9341_PORT_D6, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D7;
    GPIO_Init(LCD_ILI9341_PORT_D7, &GPIO_InitStructure);
    GPIO_SetBits(LCD_ILI9341_PORT_D0,LCD_ILI9341_PIN_D0);
    GPIO_SetBits(LCD_ILI9341_PORT_D1,LCD_ILI9341_PIN_D1);
    GPIO_SetBits(LCD_ILI9341_PORT_D2,LCD_ILI9341_PIN_D2);
    GPIO_SetBits(LCD_ILI9341_PORT_D3,LCD_ILI9341_PIN_D3);
    GPIO_SetBits(LCD_ILI9341_PORT_D4,LCD_ILI9341_PIN_D4);
    GPIO_SetBits(LCD_ILI9341_PORT_D5,LCD_ILI9341_PIN_D5);
    GPIO_SetBits(LCD_ILI9341_PORT_D6,LCD_ILI9341_PIN_D6);
    GPIO_SetBits(LCD_ILI9341_PORT_D7,LCD_ILI9341_PIN_D7);
#endif
    //
    LCD_ILI9341_RS_SET;
    LCD_ILI9341_CS_CLR;
    LCD_ILI9341_RD_CLR;
    i16=GPIO_ReadInputData(LCD_ILI9341_PORT_D0);
    i16>>=8;
    LCD_ILI9341_RD_SET;
    //将GPIO设为输出状态
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
#ifdef LCD_ILI9341_PORT_DATA
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D0\
                                       |LCD_ILI9341_PIN_D1\
                                       |LCD_ILI9341_PIN_D2\
                                       |LCD_ILI9341_PIN_D3\
                                       |LCD_ILI9341_PIN_D4\
                                       |LCD_ILI9341_PIN_D5\
                                       |LCD_ILI9341_PIN_D6\
                                       |LCD_ILI9341_PIN_D7;
    GPIO_Init(LCD_ILI9341_PORT_DATA, &GPIO_InitStructure);
#else
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D0;
    GPIO_Init(LCD_ILI9341_PORT_D0, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D1;
    GPIO_Init(LCD_ILI9341_PORT_D1, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D2;
    GPIO_Init(LCD_ILI9341_PORT_D2, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D3;
    GPIO_Init(LCD_ILI9341_PORT_D3, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D4;
    GPIO_Init(LCD_ILI9341_PORT_D4, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D5;
    GPIO_Init(LCD_ILI9341_PORT_D5, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D6;
    GPIO_Init(LCD_ILI9341_PORT_D6, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  LCD_ILI9341_PIN_D7;
    GPIO_Init(LCD_ILI9341_PORT_D7, &GPIO_InitStructure);
#endif
    return i16;
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
    LCD_ILI9341_SPI_CS_LOW;
    LCD_ILI9341_DCX_SET;
    return (LCD_ILI9341_WR_Byte(0xFF));
#endif
}
void LCD_ILI9341_SetBackLight(uint8_t _bright)
{
    BspPwmOut_Write(1,1000,_bright);
}
//-------------------------------------------------------------------------------
//                            硬件接口无关
//-------------------------------------------------------------------------------
/****************************************************************************
* 功    能：写多字节的16位数据函数
****************************************************************************/
/*
void LCD_ILI9341_WriteMData(uint16_t * pData, int NumWords)
{
    for (; NumWords; NumWords--)
    {
        LCD_ILI9341_WR_DATA(*pData++);
    }
}
*/
/****************************************************************************
* 功    能：读多字节的16位数据函数
****************************************************************************/
/*
void LCD_ILI9341_ReadMData(uint16_t * pData, int NumWords)
{
    uint16_t i16=0;
    for (; NumWords; NumWords--)
    {
        i16 = LCD_ILI9341_RD_DATA();
        i16<<=8;
        i16+= LCD_ILI9341_RD_DATA();
        *pData++ = i16;
    }
}
*/
/****************************************************************************
* 功    能：向寄存器写入一个字节
****************************************************************************/
void LCD_ILI9341_WriteRegData(uint8_t Reg, uint16_t Value)
{
    LCD_ILI9341_WR_REG(Reg);
    LCD_ILI9341_WR_DATA(Value);
}
/****************************************************************************
* 功    能：向寄存器读出一个字节
****************************************************************************/
uint16_t LCD_ILI9341_ReadRegData(uint8_t Reg)
{
    LCD_ILI9341_WR_REG(Reg);
    Count_DelayUs(5);
    return LCD_ILI9341_RD_DATA();
}
/****************************************************************************
* 功    能：颜色格式转换(BGR->RGB)
* 说    明: 从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
****************************************************************************/
uint16_t LCD_ILI9341_BGR2RGB(uint16_t c)
{
    uint16_t  r,g,b,rgb;
    b=(c>>0)&0x1f;
    g=(c>>5)&0x3f;
    r=(c>>11)&0x1f;
    rgb=(b<<11)+(g<<5)+(r<<0);
    return(rgb);
}
/****************************************************************************
* 功    能：读取个某点的颜色值
* 返 回 值: 此点的颜色(RGB)
****************************************************************************/

uint16_t LCD_ILI9341_ReadPoint(uint16_t x,uint16_t y)
{
    if(Lcd_Ili9341_s_dev.id==0X9488)
    {
        uint8_t a=0;
        uint8_t b=0;
        uint8_t c=0;
        uint16_t i16=0;
        LCD_ILI9341_SetCursor(x,y);

        LCD_ILI9341_WR_REG(0X2E);
        a = LCD_ILI9341_RD_DATA();
        a = LCD_ILI9341_RD_DATA();
        b = LCD_ILI9341_RD_DATA();
        c = LCD_ILI9341_RD_DATA();
#ifdef	LCD_ILI9341_SPI_CS_HIGH
        LCD_ILI9341_SPI_CS_HIGH;
			  LCD_ILI9341_SPI_CS_LOW;
#endif			
        a = a>>3;
        b = b>>2;
        c = c>>3;
        i16 = (a<<11)+(b<<5)+c;
        i16 = ~i16;
        return i16;
    }
    else
    {
        uint16_t r=0,g=0,b=0;
        //超过了范围,直接返回
        if(x>=Lcd_Ili9341_s_dev.width||y>=Lcd_Ili9341_s_dev.height)
            return 0;
        LCD_ILI9341_SetCursor(x,y);
        //9341/6804/3510 发送读GRAM指令
        if(Lcd_Ili9341_s_dev.id==0X9341||Lcd_Ili9341_s_dev.id==0X6804||Lcd_Ili9341_s_dev.id==0X5310)
        {
            LCD_ILI9341_WR_REG(0X2E);
        }
        //其他IC发送读GRAM指令
        else
        {
            LCD_ILI9341_WR_REG(0x22);
        }
        //FOR 9320,延时2us
        if(Lcd_Ili9341_s_dev.id==0X9320)
            Count_DelayUs(2);
        //dummy Read
        r = LCD_ILI9341_RD_DATA();
        Count_DelayUs(2);
        //实际坐标颜色
        r=LCD_ILI9341_RD_DATA();
        //-----XSL-----增加
        r<<=8;
        r|=LCD_ILI9341_RD_DATA();
        //-------------
        //9341/NT35310要分2次读出
        if(Lcd_Ili9341_s_dev.id==0X9341||Lcd_Ili9341_s_dev.id==0X5310)
        {
            Count_DelayUs(2);
            b=LCD_ILI9341_RD_DATA();
            //-----XSL-----增加
            b<<=8;
            //-------------
            //对于9341/5310,第一次读取的是RG的值,R在前,G在后,各占8位
            g=r&0XFF;
            g<<=8;
        }
        //6804第二次读取的才是真实值
        else if(Lcd_Ili9341_s_dev.id==0X6804)
        {
            r=LCD_ILI9341_RD_DATA();
        }
        //这几种IC直接返回颜色值
        if(Lcd_Ili9341_s_dev.id==0X9325||Lcd_Ili9341_s_dev.id==0X4535||Lcd_Ili9341_s_dev.id==0X4531||Lcd_Ili9341_s_dev.id==0X8989||Lcd_Ili9341_s_dev.id==0XB505)
        {
            return r;
        }
        //ILI9341/NT35310需要公式转换一下
        else if(Lcd_Ili9341_s_dev.id==0X9341||Lcd_Ili9341_s_dev.id==0X5310)
        {
            return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
        }
        //其他IC
        else
        {
            return LCD_ILI9341_BGR2RGB(r);
        }
    }
}
/****************************************************************************
* 功    能：LCD开启显示
****************************************************************************/
void LCD_ILI9341_DisplayOn(void)
{
    if(Lcd_Ili9341_s_dev.id==0X9341 \
       ||Lcd_Ili9341_s_dev.id==0X6804 \
       ||Lcd_Ili9341_s_dev.id==0X5310 \
       ||Lcd_Ili9341_s_dev.id==0X9488)
    {
        LCD_ILI9341_WR_REG(0X29);
    }
    else
    {
        LCD_ILI9341_WriteRegData(0x07,0x0173);
    }
}
/****************************************************************************
* 功    能：LCD关闭显示
****************************************************************************/
void LCD_ILI9341_DisplayOff(void)
{
    if(Lcd_Ili9341_s_dev.id==0X9341 \
       ||Lcd_Ili9341_s_dev.id==0X6804 \
       ||Lcd_Ili9341_s_dev.id==0X5310 \
       ||Lcd_Ili9341_s_dev.id==0X9488)
    {
        LCD_ILI9341_WR_REG(0X28);
    }
    else
    {
        LCD_ILI9341_WriteRegData(0x07,0x0);
    }
}
/****************************************************************************
* 功    能：设置光标位置
****************************************************************************/
static void LCD_ILI9341_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    if(Lcd_Ili9341_s_dev.id==0X9341||Lcd_Ili9341_s_dev.id==0X5310)
    {
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setxcmd);
        LCD_ILI9341_WR_DATA(Xpos>>8);
        LCD_ILI9341_WR_DATA(Xpos&0XFF);
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setycmd);
        LCD_ILI9341_WR_DATA(Ypos>>8);
        LCD_ILI9341_WR_DATA(Ypos&0XFF);
    }
    else if(Lcd_Ili9341_s_dev.id==0X9488)
    {
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setxcmd);
        LCD_ILI9341_WR_DATA(Xpos>>8);
        LCD_ILI9341_WR_DATA(Xpos&0XFF);
        LCD_ILI9341_WR_DATA(Lcd_Ili9341_s_dev.width>>8);
        LCD_ILI9341_WR_DATA(Lcd_Ili9341_s_dev.width&0XFF);
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setycmd);
        LCD_ILI9341_WR_DATA(Ypos>>8);
        LCD_ILI9341_WR_DATA(Ypos&0XFF);
        LCD_ILI9341_WR_DATA(Lcd_Ili9341_s_dev.height>>8);
        LCD_ILI9341_WR_DATA(Lcd_Ili9341_s_dev.height&0XFF);
    }
    else if(Lcd_Ili9341_s_dev.id==0X6804)
    {
        if(Lcd_Ili9341_s_dev.dir==1)Xpos=Lcd_Ili9341_s_dev.width-1-Xpos;//横屏时处理
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setxcmd);
        LCD_ILI9341_WR_DATA(Xpos>>8);
        LCD_ILI9341_WR_DATA(Xpos&0XFF);
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setycmd);
        LCD_ILI9341_WR_DATA(Ypos>>8);
        LCD_ILI9341_WR_DATA(Ypos&0XFF);
    }
    else
    {
        if(Lcd_Ili9341_s_dev.dir==1)Xpos=Lcd_Ili9341_s_dev.width-1-Xpos;//横屏其实就是调转x,y坐标
        LCD_ILI9341_WriteRegData(Lcd_Ili9341_s_dev.setxcmd, Xpos);
        LCD_ILI9341_WriteRegData(Lcd_Ili9341_s_dev.setycmd, Ypos);
    }
}
/****************************************************************************
* 功    能：设置LCD的自动扫描方向
* 参    数: dir:0~7,代表8个方向(具体定义见LCD_ILI9341.h)
* 说    明: 其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
            所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
            9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310等IC已经实际测试.
****************************************************************************/
static void LCD_ILI9341_ScanDir(uint8_t dir)
{
    uint16_t regval=0;
    uint8_t dirreg=0;
    uint16_t temp;
    //横屏时，对6804不改变扫描方向
    if(Lcd_Ili9341_s_dev.dir==1&&Lcd_Ili9341_s_dev.id!=0X6804)
    {
        switch(dir)//方向转换
        {
            case 0:
                dir=6;
                break;
            case 1:
                dir=7;
                break;
            case 2:
                dir=4;
                break;
            case 3:
                dir=5;
                break;
            case 4:
                dir=1;
                break;
            case 5:
                dir=0;
                break;
            case 6:
                dir=3;
                break;
            case 7:
                dir=2;
                break;
        }
    }
    //9341/6804/5310,很特殊
    if(Lcd_Ili9341_s_dev.id==0x9341\
       ||Lcd_Ili9341_s_dev.id==0X9488\
       ||Lcd_Ili9341_s_dev.id==0X6804\
       ||Lcd_Ili9341_s_dev.id==0X5310)
    {
        switch(dir)
        {
            case LCD_ILI9341_E_SCAN_DIR_L2R_U2D://从左到右,从上到下
                regval|=(0<<7)|(0<<6)|(0<<5);
                break;
            case LCD_ILI9341_E_SCAN_DIR_L2R_D2U://从左到右,从下到上
                regval|=(1<<7)|(0<<6)|(0<<5);
                break;
            case LCD_ILI9341_E_SCAN_DIR_R2L_U2D://从右到左,从上到下
                regval|=(0<<7)|(1<<6)|(0<<5);
                break;
            case LCD_ILI9341_E_SCAN_DIR_R2L_D2U://从右到左,从下到上
                regval|=(1<<7)|(1<<6)|(0<<5);
                break;
            case LCD_ILI9341_E_SCAN_DIR_U2D_L2R://从上到下,从左到右
                regval|=(0<<7)|(0<<6)|(1<<5);
                break;
            case LCD_ILI9341_E_SCAN_DIR_U2D_R2L://从上到下,从右到左
                regval|=(0<<7)|(1<<6)|(1<<5);
                break;
            case LCD_ILI9341_E_SCAN_DIR_D2U_L2R://从下到上,从左到右
                regval|=(1<<7)|(0<<6)|(1<<5);
                break;
            case LCD_ILI9341_E_SCAN_DIR_D2U_R2L://从下到上,从右到左
                regval|=(1<<7)|(1<<6)|(1<<5);
                break;
        }
        dirreg=0X36;
        //5310不需要BGR
        if(Lcd_Ili9341_s_dev.id!=0X5310)regval|=0X08;
        //6804的BIT6和9341的反了
        if(Lcd_Ili9341_s_dev.id==0X6804)regval|=0x02;
        LCD_ILI9341_WriteRegData(dirreg,regval);
        if((regval&0X20)||Lcd_Ili9341_s_dev.dir==1)
        {
            //交换X,Y
            if(Lcd_Ili9341_s_dev.width<Lcd_Ili9341_s_dev.height)
            {
                temp=Lcd_Ili9341_s_dev.width;
                Lcd_Ili9341_s_dev.width=Lcd_Ili9341_s_dev.height;
                Lcd_Ili9341_s_dev.height=temp;
            }
        }
        else
        {
            //交换X,Y
            if(Lcd_Ili9341_s_dev.width>Lcd_Ili9341_s_dev.height)
            {
                temp=Lcd_Ili9341_s_dev.width;
                Lcd_Ili9341_s_dev.width=Lcd_Ili9341_s_dev.height;
                Lcd_Ili9341_s_dev.height=temp;
            }
        }
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setxcmd);
        LCD_ILI9341_WR_DATA(0);
        LCD_ILI9341_WR_DATA(0);
        LCD_ILI9341_WR_DATA((Lcd_Ili9341_s_dev.width-1)>>8);
        LCD_ILI9341_WR_DATA((Lcd_Ili9341_s_dev.width-1)&0XFF);
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setycmd);
        LCD_ILI9341_WR_DATA(0);
        LCD_ILI9341_WR_DATA(0);
        LCD_ILI9341_WR_DATA((Lcd_Ili9341_s_dev.height-1)>>8);
        LCD_ILI9341_WR_DATA((Lcd_Ili9341_s_dev.height-1)&0XFF);
    }
    else
    {
        switch(dir)
        {
            case LCD_ILI9341_E_SCAN_DIR_L2R_U2D://从左到右,从上到下
                regval|=(1<<5)|(1<<4)|(0<<3);
                break;
            case LCD_ILI9341_E_SCAN_DIR_L2R_D2U://从左到右,从下到上
                regval|=(0<<5)|(1<<4)|(0<<3);
                break;
            case LCD_ILI9341_E_SCAN_DIR_R2L_U2D://从右到左,从上到下
                regval|=(1<<5)|(0<<4)|(0<<3);
                break;
            case LCD_ILI9341_E_SCAN_DIR_R2L_D2U://从右到左,从下到上
                regval|=(0<<5)|(0<<4)|(0<<3);
                break;
            case LCD_ILI9341_E_SCAN_DIR_U2D_L2R://从上到下,从左到右
                regval|=(1<<5)|(1<<4)|(1<<3);
                break;
            case LCD_ILI9341_E_SCAN_DIR_U2D_R2L://从上到下,从右到左
                regval|=(1<<5)|(0<<4)|(1<<3);
                break;
            case LCD_ILI9341_E_SCAN_DIR_D2U_L2R://从下到上,从左到右
                regval|=(0<<5)|(1<<4)|(1<<3);
                break;
            case LCD_ILI9341_E_SCAN_DIR_D2U_R2L://从下到上,从右到左
                regval|=(0<<5)|(0<<4)|(1<<3);
                break;
        }
        if(Lcd_Ili9341_s_dev.id==0x8989)//8989 IC
        {
            dirreg=0X11;
            regval|=0X6040; //65K
        }
        else //其他驱动IC
        {
            dirreg=0X03;
            regval|=1<<12;
        }
        LCD_ILI9341_WriteRegData(dirreg,regval);
    }
}
/****************************************************************************
* 功    能：固定颜色画点
* 返 回 值: 此点的颜色(RGB)
* 参    数: 坐标(x,y),颜色(POINT_COLOR)LCD_Ili9341_PointColor
****************************************************************************/
void LCD_ILI9341_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{
    uint8_t color_buf[3];
    if(Lcd_Ili9341_s_dev.colorType==666)
    {
        // 颜色变换
        Count_ColorConver(0,&color,color_buf);
        LCD_ILI9341_SetCursor(x,y);     //设置光标位置
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
        LCD_ILI9341_WR_DATA(color_buf[0]);
        LCD_ILI9341_WR_DATA(color_buf[1]);
        LCD_ILI9341_WR_DATA(color_buf[2]);
    }
    else if(Lcd_Ili9341_s_dev.colorType==565)
    {
        LCD_ILI9341_SetCursor(x,y);     //设置光标位置
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
        LCD_ILI9341_WR_DATA(color>>8);
        LCD_ILI9341_WR_DATA(color);
    }
}
/****************************************************************************
* 功    能：设置LCD显示方向
* 参    数: dir:0,竖屏；1,横屏
****************************************************************************/
void LCD_ILI9341_DisplayDir(uint8_t dir)
{
    //竖屏
    if(dir==0)
    {
        Lcd_Ili9341_s_dev.dir=0;
        Lcd_Ili9341_s_dev.width=LCD_ILI9341_WIDTH;
        Lcd_Ili9341_s_dev.height=LCD_ILI9341_HEIGHT;
        if(Lcd_Ili9341_s_dev.id==0X9341\
           ||Lcd_Ili9341_s_dev.id==0X9488\
           ||Lcd_Ili9341_s_dev.id==0X6804\
           ||Lcd_Ili9341_s_dev.id==0X5310)
        {
            Lcd_Ili9341_s_dev.wramcmd=0X2C;
            Lcd_Ili9341_s_dev.setxcmd=0X2A;
            Lcd_Ili9341_s_dev.setycmd=0X2B;
            if(Lcd_Ili9341_s_dev.id==0X6804\
               ||Lcd_Ili9341_s_dev.id==0X5310)
            {
                Lcd_Ili9341_s_dev.width=320;
                Lcd_Ili9341_s_dev.height=480;
            }
        }
        else if(Lcd_Ili9341_s_dev.id==0X8989)
        {
            Lcd_Ili9341_s_dev.wramcmd=0x22;
            Lcd_Ili9341_s_dev.setxcmd=0X4E;
            Lcd_Ili9341_s_dev.setycmd=0X4F;
        }
        else
        {
            Lcd_Ili9341_s_dev.wramcmd=0x22;
            Lcd_Ili9341_s_dev.setxcmd=0x20;
            Lcd_Ili9341_s_dev.setycmd=0x21;
        }
    }
    //横屏
    else
    {
        Lcd_Ili9341_s_dev.dir=1;
        Lcd_Ili9341_s_dev.width=LCD_ILI9341_WIDTH;
        Lcd_Ili9341_s_dev.height=LCD_ILI9341_HEIGHT;
        if(Lcd_Ili9341_s_dev.id==0X9341 \
           ||Lcd_Ili9341_s_dev.id==0X5310 \
           ||Lcd_Ili9341_s_dev.id==0X9488)
        {
            Lcd_Ili9341_s_dev.wramcmd=0X2C;
            Lcd_Ili9341_s_dev.setxcmd=0X2A;
            Lcd_Ili9341_s_dev.setycmd=0X2B;
        }
        else if(Lcd_Ili9341_s_dev.id==0X6804)
        {
            Lcd_Ili9341_s_dev.wramcmd=0X2C;
            Lcd_Ili9341_s_dev.setxcmd=0X2B;
            Lcd_Ili9341_s_dev.setycmd=0X2A;
        }
        else if(Lcd_Ili9341_s_dev.id==0X8989)
        {
            Lcd_Ili9341_s_dev.wramcmd=0x22;
            Lcd_Ili9341_s_dev.setxcmd=0X4F;
            Lcd_Ili9341_s_dev.setycmd=0X4E;
        }
        else
        {
            Lcd_Ili9341_s_dev.wramcmd=0x22;
            Lcd_Ili9341_s_dev.setxcmd=0x21;
            Lcd_Ili9341_s_dev.setycmd=0x20;
        }
    }
    LCD_ILI9341_ScanDir(DFT_SCAN_DIR);
}
/****************************************************************************
* 功    能：设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
* 参    数: sx,sy:窗口起始坐标(左上角),width,height:窗口宽度和高度
* 注    意: 68042,横屏时不支持窗口设置!!
****************************************************************************/
void LCD_ILI9341_SetWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
    uint8_t hsareg,heareg,vsareg,veareg;
    uint16_t hsaval,heaval,vsaval,veaval;
    width=sx+width-1;
    height=sy+height-1;
    if(Lcd_Ili9341_s_dev.id==0X9341\
       ||Lcd_Ili9341_s_dev.id==0X9488\
       ||Lcd_Ili9341_s_dev.id==0X5310\
       ||Lcd_Ili9341_s_dev.id==0X6804)//6804横屏不支持
    {
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setxcmd);
        LCD_ILI9341_WR_DATA(sx>>8);
        LCD_ILI9341_WR_DATA(sx&0XFF);
        LCD_ILI9341_WR_DATA(width>>8);
        LCD_ILI9341_WR_DATA(width&0XFF);
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.setycmd);
        LCD_ILI9341_WR_DATA(sy>>8);
        LCD_ILI9341_WR_DATA(sy&0XFF);
        LCD_ILI9341_WR_DATA(height>>8);
        LCD_ILI9341_WR_DATA(height&0XFF);
    }
    //其他驱动IC
    else
    {
        //横屏
        if(Lcd_Ili9341_s_dev.dir==1)
        {
            //窗口值
            hsaval=sy;
            heaval=height;
            vsaval=Lcd_Ili9341_s_dev.width-width-1;
            veaval=Lcd_Ili9341_s_dev.width-sx-1;
        }
        else
        {
            hsaval=sx;
            heaval=width;
            vsaval=sy;
            veaval=height;
        }
        //8989 IC
        if(Lcd_Ili9341_s_dev.id==0X8989)
        {
            hsareg=0X44;
            heareg=0X44;//水平方向窗口寄存器 (1289的由一个寄存器控制)
            hsaval|=(heaval<<8);    //得到寄存器值.
            heaval=hsaval;
            vsareg=0X45;
            veareg=0X46;//垂直方向窗口寄存器
        }
        //其他驱动IC
        else
        {
            hsareg=0X50;
            //水平方向窗口寄存器
            heareg=0X51;
            vsareg=0X52;
            //垂直方向窗口寄存器
            veareg=0X53;
        }
        //设置寄存器值
        LCD_ILI9341_WriteRegData(hsareg,hsaval);
        LCD_ILI9341_WriteRegData(heareg,heaval);
        LCD_ILI9341_WriteRegData(vsareg,vsaval);
        LCD_ILI9341_WriteRegData(veareg,veaval);
        //设置光标位置
        LCD_ILI9341_SetCursor(sx,sy);
    }
}
/****************************************************************************
* 功    能：退出窗口模式
****************************************************************************/
void LCD_ILI9341_QuitWinow(void)
{
    LCD_ILI9341_SetWindow(0,0,Lcd_Ili9341_s_dev.width,Lcd_Ili9341_s_dev.height);
}
/****************************************************************************
* 功    能：读取芯片ID
****************************************************************************/
uint16_t LCD_ILI9341_ReadID(void)
{
    Lcd_Ili9341_s_dev.id = LCD_ILI9341_ReadRegData(0x00);
    //读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
    if(Lcd_Ili9341_s_dev.id<0XFF||Lcd_Ili9341_s_dev.id==0XFFFF||Lcd_Ili9341_s_dev.id==0X9300)
    {
        //尝试9341 ID的读取
        LCD_ILI9341_WR_REG(0XD3);
        //dummy read
        Lcd_Ili9341_s_dev.id=LCD_ILI9341_RD_DATA();
        //读到0X00
        Lcd_Ili9341_s_dev.id=LCD_ILI9341_RD_DATA();
        //读取93
        Lcd_Ili9341_s_dev.id=LCD_ILI9341_RD_DATA();
        Lcd_Ili9341_s_dev.id<<=8;
        //读取41
        Lcd_Ili9341_s_dev.id|=LCD_ILI9341_RD_DATA();
        //非9341,尝试是不是6804
        if(Lcd_Ili9341_s_dev.id!=0X9341)
        {
            LCD_ILI9341_WR_REG(0XBF);
            //dummy read
            LCD_ILI9341_RD_DATA();
            //读回0X01
            LCD_ILI9341_RD_DATA();
            //读回0XD0
            LCD_ILI9341_RD_DATA();
            //这里读回0X68
            Lcd_Ili9341_s_dev.id=LCD_ILI9341_RD_DATA();
            Lcd_Ili9341_s_dev.id<<=8;
            //这里读回0X04
            Lcd_Ili9341_s_dev.id|=LCD_ILI9341_RD_DATA();
        }
        //不是9341也不是6804，尝试看看是不是NT35310
        if(Lcd_Ili9341_s_dev.id!=0X9341&&Lcd_Ili9341_s_dev.id!=0X6804)
        {
            LCD_ILI9341_WR_REG(0XD4);
            //dummy read
            LCD_ILI9341_RD_DATA();
            //读回0X01
            LCD_ILI9341_RD_DATA();
            //读回0X53
            Lcd_Ili9341_s_dev.id=LCD_ILI9341_RD_DATA();
            Lcd_Ili9341_s_dev.id<<=8;
            //这里读回0X10
            Lcd_Ili9341_s_dev.id|=LCD_ILI9341_RD_DATA();
        }
        // 尝试查看是不是9488(0x9488)
        if(Lcd_Ili9341_s_dev.id!=0X9341\
           &&Lcd_Ili9341_s_dev.id!=0X6804)
        {
            LCD_ILI9341_WR_REG(0xFB);
            LCD_ILI9341_WR_DATA(0x80+2);
            LCD_ILI9341_WR_REG(0xD3);
            Lcd_Ili9341_s_dev.id=LCD_ILI9341_RD_DATA();
            Lcd_Ili9341_s_dev.id<<=8;
            //
            LCD_ILI9341_WR_REG(0xFB);
            LCD_ILI9341_WR_DATA(0x80+3);
            LCD_ILI9341_WR_REG(0xD3);
            Lcd_Ili9341_s_dev.id|=LCD_ILI9341_RD_DATA();
            //
            LCD_ILI9341_WR_REG(0xFB);
            LCD_ILI9341_WR_DATA(0x00);
        }
    }
    return (Lcd_Ili9341_s_dev.id);
}
/****************************************************************************
* 功    能：初始化lcd
* 注    意: 该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI932
            在其他型号的驱动芯片上没有测试!
****************************************************************************/
void LCD_ILI9341_Init(void)
{
    LCD_ILI9341_HwInit();
    MODULE_OS_DELAY_MS(50);
    LCD_ILI9341_WriteRegData(0x0000,0x0001);
    MODULE_OS_DELAY_MS(50);
    Lcd_Ili9341_s_dev.id = LCD_ILI9341_ReadID();
    //9341初始化
    if(Lcd_Ili9341_s_dev.id==0X9341)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WR_REG(0xCF);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD9);
        LCD_ILI9341_WR_DATA(0X30);

        LCD_ILI9341_WR_REG(0xED);
        LCD_ILI9341_WR_DATA(0x64);
        LCD_ILI9341_WR_DATA(0x03);
        LCD_ILI9341_WR_DATA(0X12);
        LCD_ILI9341_WR_DATA(0X81);

        LCD_ILI9341_WR_REG(0xE8);
        LCD_ILI9341_WR_DATA(0x85);
        LCD_ILI9341_WR_DATA(0x10);
        LCD_ILI9341_WR_DATA(0x78);

        LCD_ILI9341_WR_REG(0xCB);
        LCD_ILI9341_WR_DATA(0x39);
        LCD_ILI9341_WR_DATA(0x2C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x34);
        LCD_ILI9341_WR_DATA(0x02);

        LCD_ILI9341_WR_REG(0xF7);
        LCD_ILI9341_WR_DATA(0x20);

        LCD_ILI9341_WR_REG(0xEA);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC0);    //Power control
        LCD_ILI9341_WR_DATA(0x21);   //VRH[5:0]

        LCD_ILI9341_WR_REG(0xC1);    //Power control
        LCD_ILI9341_WR_DATA(0x12);   //SAP[2:0];BT[3:0]

        LCD_ILI9341_WR_REG(0xC5);    //VCM control
        LCD_ILI9341_WR_DATA(0x32);
        LCD_ILI9341_WR_DATA(0x3C);

        LCD_ILI9341_WR_REG(0xC7);    //VCM control2
        LCD_ILI9341_WR_DATA(0XC1);

        LCD_ILI9341_WR_REG(0x36);    // Memory Access Control
        LCD_ILI9341_WR_DATA(0x08);

        LCD_ILI9341_WR_REG(0x3A);
        LCD_ILI9341_WR_DATA(0x55); //RGB

        LCD_ILI9341_WR_REG(0xB1);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x18);

        LCD_ILI9341_WR_REG(0xB6);    // Display Function Control
        LCD_ILI9341_WR_DATA(0x0A);
        LCD_ILI9341_WR_DATA(0xA2);

        LCD_ILI9341_WR_REG(0x36);       // Memory Access Control
        LCD_ILI9341_WR_DATA (0xa8);     // ???? my,mx,mv,ml,BGR,mh,0.0
        //           1 0  1  0   1  0  0 0
        //if MADCTL BIT5=1, EC[15:0]=0x013f,28/68/A8/E8
        //if MADCTL BIT5=0, EC[15:0]=0x00ef,08/48/88/c8


        LCD_ILI9341_WR_REG(0xF2);    // 3Gamma Function Disable
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0x26);    //Gamma curve selected
        LCD_ILI9341_WR_DATA(0x01);

        LCD_ILI9341_WR_REG(0xE0);    //Set Gamma
        LCD_ILI9341_WR_DATA(0x0F);
        LCD_ILI9341_WR_DATA(0x20);
        LCD_ILI9341_WR_DATA(0x1E);
        LCD_ILI9341_WR_DATA(0x09);
        LCD_ILI9341_WR_DATA(0x12);
        LCD_ILI9341_WR_DATA(0x0B);
        LCD_ILI9341_WR_DATA(0x50);
        LCD_ILI9341_WR_DATA(0XBA);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x09);
        LCD_ILI9341_WR_DATA(0x14);
        LCD_ILI9341_WR_DATA(0x05);
        LCD_ILI9341_WR_DATA(0x23);
        LCD_ILI9341_WR_DATA(0x21);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0XE1);    //Set Gamma
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x19);
        LCD_ILI9341_WR_DATA(0x19);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x12);
        LCD_ILI9341_WR_DATA(0x07);
        LCD_ILI9341_WR_DATA(0x2D);
        LCD_ILI9341_WR_DATA(0x28);
        LCD_ILI9341_WR_DATA(0x3F);
        LCD_ILI9341_WR_DATA(0x02);
        LCD_ILI9341_WR_DATA(0x0A);
        LCD_ILI9341_WR_DATA(0x08);
        LCD_ILI9341_WR_DATA(0x25);
        LCD_ILI9341_WR_DATA(0x2D);
        LCD_ILI9341_WR_DATA(0x0F);

        LCD_ILI9341_WR_REG(0x11);    //Exit Sleep
        MODULE_OS_DELAY_MS(120);
        LCD_ILI9341_WR_REG(0x29);    //Display on
    }
    //6804初始化
    else if(Lcd_Ili9341_s_dev.id==0x6804)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WR_REG(0X11);
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WR_REG(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
        LCD_ILI9341_WR_DATA(0X07);
        LCD_ILI9341_WR_DATA(0X42);
        LCD_ILI9341_WR_DATA(0X1D);
        LCD_ILI9341_WR_REG(0XD1);//VCOMH VCOM_AC amplitude setting
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X1a);
        LCD_ILI9341_WR_DATA(0X09);
        LCD_ILI9341_WR_REG(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
        LCD_ILI9341_WR_DATA(0X01);
        LCD_ILI9341_WR_DATA(0X22);
        LCD_ILI9341_WR_REG(0XC0);//REV SM GS
        LCD_ILI9341_WR_DATA(0X10);
        LCD_ILI9341_WR_DATA(0X3B);
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X02);
        LCD_ILI9341_WR_DATA(0X11);

        LCD_ILI9341_WR_REG(0XC5);// Frame rate setting = 72HZ  when setting 0x03
        LCD_ILI9341_WR_DATA(0X03);

        LCD_ILI9341_WR_REG(0XC8);//Gamma setting
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X25);
        LCD_ILI9341_WR_DATA(0X21);
        LCD_ILI9341_WR_DATA(0X05);
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X0a);
        LCD_ILI9341_WR_DATA(0X65);
        LCD_ILI9341_WR_DATA(0X25);
        LCD_ILI9341_WR_DATA(0X77);
        LCD_ILI9341_WR_DATA(0X50);
        LCD_ILI9341_WR_DATA(0X0f);
        LCD_ILI9341_WR_DATA(0X00);

        LCD_ILI9341_WR_REG(0XF8);
        LCD_ILI9341_WR_DATA(0X01);

        LCD_ILI9341_WR_REG(0XFE);
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X02);

        LCD_ILI9341_WR_REG(0X20);//Exit invert mode

        LCD_ILI9341_WR_REG(0X36);
        LCD_ILI9341_WR_DATA(0X08);//原来是a

        LCD_ILI9341_WR_REG(0X3A);
        LCD_ILI9341_WR_DATA(0X55);//16位模式
        LCD_ILI9341_WR_REG(0X2B);
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X01);
        LCD_ILI9341_WR_DATA(0X3F);

        LCD_ILI9341_WR_REG(0X2A);
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X00);
        LCD_ILI9341_WR_DATA(0X01);
        LCD_ILI9341_WR_DATA(0XDF);
        MODULE_OS_DELAY_MS(120);
        LCD_ILI9341_WR_REG(0X29);
    }
    else if(Lcd_Ili9341_s_dev.id==0x5310)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WR_REG(0xED);
        LCD_ILI9341_WR_DATA(0x01);
        LCD_ILI9341_WR_DATA(0xFE);

        LCD_ILI9341_WR_REG(0xEE);
        LCD_ILI9341_WR_DATA(0xDE);
        LCD_ILI9341_WR_DATA(0x21);

        LCD_ILI9341_WR_REG(0xF1);
        LCD_ILI9341_WR_DATA(0x01);
        LCD_ILI9341_WR_REG(0xDF);
        LCD_ILI9341_WR_DATA(0x10);

        //VCOMvoltage//
        LCD_ILI9341_WR_REG(0xC4);
        LCD_ILI9341_WR_DATA(0x8F);    //5f

        LCD_ILI9341_WR_REG(0xC6);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xE2);
        LCD_ILI9341_WR_DATA(0xE2);
        LCD_ILI9341_WR_DATA(0xE2);
        LCD_ILI9341_WR_REG(0xBF);
        LCD_ILI9341_WR_DATA(0xAA);

        LCD_ILI9341_WR_REG(0xB0);
        LCD_ILI9341_WR_DATA(0x0D);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x0D);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x11);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x19);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x21);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x2D);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x3D);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x5D);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x5D);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB1);
        LCD_ILI9341_WR_DATA(0x80);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x8B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x96);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB2);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x02);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x03);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB4);
        LCD_ILI9341_WR_DATA(0x8B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x96);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA1);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB5);
        LCD_ILI9341_WR_DATA(0x02);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x03);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x04);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB6);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB7);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x3F);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x5E);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x64);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x8C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xAC);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xDC);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x70);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x90);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xEB);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xDC);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB8);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xBA);
        LCD_ILI9341_WR_DATA(0x24);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC1);
        LCD_ILI9341_WR_DATA(0x20);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x54);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xFF);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC2);
        LCD_ILI9341_WR_DATA(0x0A);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x04);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC3);
        LCD_ILI9341_WR_DATA(0x3C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x3A);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x39);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x37);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x3C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x36);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x32);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x2F);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x2C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x29);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x26);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x24);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x24);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x23);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x3C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x36);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x32);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x2F);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x2C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x29);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x26);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x24);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x24);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x23);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC4);
        LCD_ILI9341_WR_DATA(0x62);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x05);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x84);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF0);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x18);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA4);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x18);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x50);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x0C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x17);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x95);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xE6);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC5);
        LCD_ILI9341_WR_DATA(0x32);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x65);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x76);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x88);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC6);
        LCD_ILI9341_WR_DATA(0x20);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x17);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x01);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC7);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC8);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xC9);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE0);
        LCD_ILI9341_WR_DATA(0x16);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x1C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x21);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x36);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x46);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x52);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x64);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x7A);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x8B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA8);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xB9);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC4);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xCA);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD2);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD9);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xE0);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF3);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE1);
        LCD_ILI9341_WR_DATA(0x16);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x1C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x22);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x36);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x45);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x52);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x64);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x7A);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x8B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA8);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xB9);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC4);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xCA);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD2);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD8);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xE0);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF3);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE2);
        LCD_ILI9341_WR_DATA(0x05);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x0B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x1B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x34);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x4F);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x61);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x79);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x88);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x97);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA6);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xB7);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC2);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC7);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD1);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD6);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xDD);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_REG(0xE3);
        LCD_ILI9341_WR_DATA(0x05);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x1C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x33);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x50);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x62);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x78);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x88);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x97);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA6);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xB7);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC2);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC7);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD1);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD5);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xDD);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF3);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE4);
        LCD_ILI9341_WR_DATA(0x01);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x01);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x02);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x2A);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x3C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x4B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x5D);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x74);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x84);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x93);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA2);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xB3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xBE);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC4);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xCD);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xDD);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_REG(0xE5);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x02);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x29);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x3C);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x4B);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x5D);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x74);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x84);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x93);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xA2);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xB3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xBE);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xC4);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xCD);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xD3);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xDC);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xF3);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE6);
        LCD_ILI9341_WR_DATA(0x11);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x34);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x56);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x76);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x77);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x66);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x88);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xBB);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x66);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x55);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x55);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x45);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x43);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE7);
        LCD_ILI9341_WR_DATA(0x32);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x55);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x76);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x66);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x67);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x67);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x87);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xBB);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x77);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x56);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x23);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x33);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x45);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE8);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x87);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x88);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x77);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x66);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x88);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xAA);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0xBB);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x99);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x66);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x55);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x55);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x44);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x55);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xE9);
        LCD_ILI9341_WR_DATA(0xAA);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0x00);
        LCD_ILI9341_WR_DATA(0xAA);

        LCD_ILI9341_WR_REG(0xCF);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xF0);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x50);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xF3);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xF9);
        LCD_ILI9341_WR_DATA(0x06);
        LCD_ILI9341_WR_DATA(0x10);
        LCD_ILI9341_WR_DATA(0x29);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0x3A);
        LCD_ILI9341_WR_DATA(0x55);  //66

        LCD_ILI9341_WR_REG(0x11);
        MODULE_OS_DELAY_MS(100);
        LCD_ILI9341_WR_REG(0x29);
        LCD_ILI9341_WR_REG(0x35);
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0x51);
        LCD_ILI9341_WR_DATA(0xFF);
        LCD_ILI9341_WR_REG(0x53);
        LCD_ILI9341_WR_DATA(0x2C);
        LCD_ILI9341_WR_REG(0x55);
        LCD_ILI9341_WR_DATA(0x82);
        LCD_ILI9341_WR_REG(0x2c);
    }
    //9325
    else if(Lcd_Ili9341_s_dev.id==0x9325)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x00E5,0x78F0);
        LCD_ILI9341_WriteRegData(0x0001,0x0100);
        LCD_ILI9341_WriteRegData(0x0002,0x0700);
        LCD_ILI9341_WriteRegData(0x0003,0x1030);
        LCD_ILI9341_WriteRegData(0x0004,0x0000);
        LCD_ILI9341_WriteRegData(0x0008,0x0202);
        LCD_ILI9341_WriteRegData(0x0009,0x0000);
        LCD_ILI9341_WriteRegData(0x000A,0x0000);
        LCD_ILI9341_WriteRegData(0x000C,0x0000);
        LCD_ILI9341_WriteRegData(0x000D,0x0000);
        LCD_ILI9341_WriteRegData(0x000F,0x0000);
        //power on sequence VGHVGL
        LCD_ILI9341_WriteRegData(0x0010,0x0000);
        LCD_ILI9341_WriteRegData(0x0011,0x0007);
        LCD_ILI9341_WriteRegData(0x0012,0x0000);
        LCD_ILI9341_WriteRegData(0x0013,0x0000);
        LCD_ILI9341_WriteRegData(0x0007,0x0000);
        //vgh
        LCD_ILI9341_WriteRegData(0x0010,0x1690);
        LCD_ILI9341_WriteRegData(0x0011,0x0227);
        //delayms(100);
        //vregiout
        LCD_ILI9341_WriteRegData(0x0012,0x009D); //0x001b
        //delayms(100);
        //vom amplitude
        LCD_ILI9341_WriteRegData(0x0013,0x1900);
        //delayms(100);
        //vom H
        LCD_ILI9341_WriteRegData(0x0029,0x0025);
        LCD_ILI9341_WriteRegData(0x002B,0x000D);
        //gamma
        LCD_ILI9341_WriteRegData(0x0030,0x0007);
        LCD_ILI9341_WriteRegData(0x0031,0x0303);
        LCD_ILI9341_WriteRegData(0x0032,0x0003);// 0006
        LCD_ILI9341_WriteRegData(0x0035,0x0206);
        LCD_ILI9341_WriteRegData(0x0036,0x0008);
        LCD_ILI9341_WriteRegData(0x0037,0x0406);
        LCD_ILI9341_WriteRegData(0x0038,0x0304);//0200
        LCD_ILI9341_WriteRegData(0x0039,0x0007);
        LCD_ILI9341_WriteRegData(0x003C,0x0602);// 0504
        LCD_ILI9341_WriteRegData(0x003D,0x0008);
        //ram
        LCD_ILI9341_WriteRegData(0x0050,0x0000);
        LCD_ILI9341_WriteRegData(0x0051,0x00EF);
        LCD_ILI9341_WriteRegData(0x0052,0x0000);
        LCD_ILI9341_WriteRegData(0x0053,0x013F);
        LCD_ILI9341_WriteRegData(0x0060,0xA700);
        LCD_ILI9341_WriteRegData(0x0061,0x0001);
        LCD_ILI9341_WriteRegData(0x006A,0x0000);
        //
        LCD_ILI9341_WriteRegData(0x0080,0x0000);
        LCD_ILI9341_WriteRegData(0x0081,0x0000);
        LCD_ILI9341_WriteRegData(0x0082,0x0000);
        LCD_ILI9341_WriteRegData(0x0083,0x0000);
        LCD_ILI9341_WriteRegData(0x0084,0x0000);
        LCD_ILI9341_WriteRegData(0x0085,0x0000);
        //
        LCD_ILI9341_WriteRegData(0x0090,0x0010);
        LCD_ILI9341_WriteRegData(0x0092,0x0600);

        LCD_ILI9341_WriteRegData(0x0007,0x0133);
        LCD_ILI9341_WriteRegData(0x00,0x0022);//
    }
    //ILI9328   OK
    else if(Lcd_Ili9341_s_dev.id==0x9328)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x00EC,0x108F);// internal timeing
        LCD_ILI9341_WriteRegData(0x00EF,0x1234);// ADD
        //LCD_WriteReg(0x00e7,0x0010);
        //LCD_WriteReg(0x0000,0x0001);//开启内部时钟
        LCD_ILI9341_WriteRegData(0x0001,0x0100);
        LCD_ILI9341_WriteRegData(0x0002,0x0700);//电源开启
        //LCD_WriteReg(0x0003,(1<<3)|(1<<4) );  //65K  RGB
        //DRIVE TABLE(寄存器 03H)
        //BIT3=AM BIT4:5=ID0:1
        //AM ID0 ID1   FUNCATION
        // 0  0   0    R->L D->U
        // 1  0   0    D->U R->L
        // 0  1   0    L->R D->U
        // 1  1   0    D->U L->R
        // 0  0   1    R->L U->D
        // 1  0   1    U->D R->L
        // 0  1   1    L->R U->D 正常就用这个.
        // 1  1   1    U->D L->R
        LCD_ILI9341_WriteRegData(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K
        LCD_ILI9341_WriteRegData(0x0004,0x0000);
        LCD_ILI9341_WriteRegData(0x0008,0x0202);
        LCD_ILI9341_WriteRegData(0x0009,0x0000);
        LCD_ILI9341_WriteRegData(0x000a,0x0000);//display setting
        LCD_ILI9341_WriteRegData(0x000c,0x0001);//display setting
        LCD_ILI9341_WriteRegData(0x000d,0x0000);//0f3c
        LCD_ILI9341_WriteRegData(0x000f,0x0000);
        //电源配置
        LCD_ILI9341_WriteRegData(0x0010,0x0000);
        LCD_ILI9341_WriteRegData(0x0011,0x0007);
        LCD_ILI9341_WriteRegData(0x0012,0x0000);
        LCD_ILI9341_WriteRegData(0x0013,0x0000);
        LCD_ILI9341_WriteRegData(0x0007,0x0001);
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x0010,0x1490);
        LCD_ILI9341_WriteRegData(0x0011,0x0227);
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x0012,0x008A);
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x0013,0x1a00);
        LCD_ILI9341_WriteRegData(0x0029,0x0006);
        LCD_ILI9341_WriteRegData(0x002b,0x000d);
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x0020,0x0000);
        LCD_ILI9341_WriteRegData(0x0021,0x0000);
        MODULE_OS_DELAY_MS(50);
        //伽马校正
        LCD_ILI9341_WriteRegData(0x0030,0x0000);
        LCD_ILI9341_WriteRegData(0x0031,0x0604);
        LCD_ILI9341_WriteRegData(0x0032,0x0305);
        LCD_ILI9341_WriteRegData(0x0035,0x0000);
        LCD_ILI9341_WriteRegData(0x0036,0x0C09);
        LCD_ILI9341_WriteRegData(0x0037,0x0204);
        LCD_ILI9341_WriteRegData(0x0038,0x0301);
        LCD_ILI9341_WriteRegData(0x0039,0x0707);
        LCD_ILI9341_WriteRegData(0x003c,0x0000);
        LCD_ILI9341_WriteRegData(0x003d,0x0a0a);
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x0050,0x0000); //水平GRAM起始位置
        LCD_ILI9341_WriteRegData(0x0051,0x00ef); //水平GRAM终止位置
        LCD_ILI9341_WriteRegData(0x0052,0x0000); //垂直GRAM起始位置
        LCD_ILI9341_WriteRegData(0x0053,0x013f); //垂直GRAM终止位置


        LCD_ILI9341_WriteRegData(0x0060,0xa700);
        LCD_ILI9341_WriteRegData(0x0061,0x0001);
        LCD_ILI9341_WriteRegData(0x006a,0x0000);
        LCD_ILI9341_WriteRegData(0x0080,0x0000);
        LCD_ILI9341_WriteRegData(0x0081,0x0000);
        LCD_ILI9341_WriteRegData(0x0082,0x0000);
        LCD_ILI9341_WriteRegData(0x0083,0x0000);
        LCD_ILI9341_WriteRegData(0x0084,0x0000);
        LCD_ILI9341_WriteRegData(0x0085,0x0000);

        LCD_ILI9341_WriteRegData(0x0090,0x0010);
        LCD_ILI9341_WriteRegData(0x0092,0x0600);
        //开启显示设置
        LCD_ILI9341_WriteRegData(0x0007,0x0133);
    }
    //测试OK.
    else if(Lcd_Ili9341_s_dev.id==0x9320)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x00,0x0000);
        LCD_ILI9341_WriteRegData(0x01,0x0100);  //Driver Output Contral.
        LCD_ILI9341_WriteRegData(0x02,0x0700);  //LCD Driver Waveform Contral.
        LCD_ILI9341_WriteRegData(0x03,0x1030);//Entry Mode Set.
        //LCD_WriteReg(0x03,0x1018);    //Entry Mode Set.

        LCD_ILI9341_WriteRegData(0x04,0x0000);  //Scalling Contral.
        LCD_ILI9341_WriteRegData(0x08,0x0202);  //Display Contral 2.(0x0207)
        LCD_ILI9341_WriteRegData(0x09,0x0000);  //Display Contral 3.(0x0000)
        LCD_ILI9341_WriteRegData(0x0a,0x0000);  //Frame Cycle Contal.(0x0000)
        LCD_ILI9341_WriteRegData(0x0c,(1<<0));  //Extern Display Interface Contral 1.(0x0000)
        LCD_ILI9341_WriteRegData(0x0d,0x0000);  //Frame Maker Position.
        LCD_ILI9341_WriteRegData(0x0f,0x0000);  //Extern Display Interface Contral 2.
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x07,0x0101);  //Display Contral.
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4)); //Power Control 1.(0x16b0)
        LCD_ILI9341_WriteRegData(0x11,0x0007);                              //Power Control 2.(0x0001)
        LCD_ILI9341_WriteRegData(0x12,(1<<8)|(1<<4)|(0<<0));                //Power Control 3.(0x0138)
        LCD_ILI9341_WriteRegData(0x13,0x0b00);                              //Power Control 4.
        LCD_ILI9341_WriteRegData(0x29,0x0000);                              //Power Control 7.

        LCD_ILI9341_WriteRegData(0x2b,(1<<14)|(1<<4));
        LCD_ILI9341_WriteRegData(0x50,0);   //Set X Star
        //水平GRAM终止位置Set X End.
        LCD_ILI9341_WriteRegData(0x51,239); //Set Y Star
        LCD_ILI9341_WriteRegData(0x52,0);   //Set Y End.t.
        LCD_ILI9341_WriteRegData(0x53,319); //

        LCD_ILI9341_WriteRegData(0x60,0x2700);  //Driver Output Control.
        LCD_ILI9341_WriteRegData(0x61,0x0001);  //Driver Output Control.
        LCD_ILI9341_WriteRegData(0x6a,0x0000);  //Vertical Srcoll Control.

        LCD_ILI9341_WriteRegData(0x80,0x0000);  //Display Position? Partial Display 1.
        LCD_ILI9341_WriteRegData(0x81,0x0000);  //RAM Address Start? Partial Display 1.
        LCD_ILI9341_WriteRegData(0x82,0x0000);  //RAM Address End-Partial Display 1.
        LCD_ILI9341_WriteRegData(0x83,0x0000);  //Displsy Position? Partial Display 2.
        LCD_ILI9341_WriteRegData(0x84,0x0000);  //RAM Address Start? Partial Display 2.
        LCD_ILI9341_WriteRegData(0x85,0x0000);  //RAM Address End? Partial Display 2.

        LCD_ILI9341_WriteRegData(0x90,(0<<7)|(16<<0));  //Frame Cycle Contral.(0x0013)
        LCD_ILI9341_WriteRegData(0x92,0x0000);  //Panel Interface Contral 2.(0x0000)
        LCD_ILI9341_WriteRegData(0x93,0x0001);  //Panel Interface Contral 3.
        LCD_ILI9341_WriteRegData(0x95,0x0110);  //Frame Cycle Contral.(0x0110)
        LCD_ILI9341_WriteRegData(0x97,(0<<8));  //
        LCD_ILI9341_WriteRegData(0x98,0x0000);  //Frame Cycle Contral.
        LCD_ILI9341_WriteRegData(0x07,0x0173);  //(0x0173)
    }
    else if(Lcd_Ili9341_s_dev.id==0X9331) //OK |/|/|
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x00E7, 0x1014);
        LCD_ILI9341_WriteRegData(0x0001, 0x0100); // set SS and SM bit
        LCD_ILI9341_WriteRegData(0x0002, 0x0200); // set 1 line inversion
        LCD_ILI9341_WriteRegData(0x0003,(1<<12)|(3<<4)|(1<<3));//65K
        //LCD_WriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
        LCD_ILI9341_WriteRegData(0x0008, 0x0202); // set the back porch and front porch
        LCD_ILI9341_WriteRegData(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
        LCD_ILI9341_WriteRegData(0x000A, 0x0000); // FMARK function
        LCD_ILI9341_WriteRegData(0x000C, 0x0000); // RGB interface setting
        LCD_ILI9341_WriteRegData(0x000D, 0x0000); // Frame marker Position
        LCD_ILI9341_WriteRegData(0x000F, 0x0000); // RGB interface polarity
        //*************Power On sequence ****************//
        LCD_ILI9341_WriteRegData(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_ILI9341_WriteRegData(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
        LCD_ILI9341_WriteRegData(0x0012, 0x0000); // VREG1OUT voltage
        LCD_ILI9341_WriteRegData(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
        MODULE_OS_DELAY_MS(200); // Dis-charge capacitor power voltage
        LCD_ILI9341_WriteRegData(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_ILI9341_WriteRegData(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
        MODULE_OS_DELAY_MS(50); // Delay 50ms
        LCD_ILI9341_WriteRegData(0x0012, 0x000C); // Internal reference voltage= Vci;
        MODULE_OS_DELAY_MS(50); // Delay 50ms
        LCD_ILI9341_WriteRegData(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
        LCD_ILI9341_WriteRegData(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
        LCD_ILI9341_WriteRegData(0x002B, 0x000B); // Set Frame Rate
        MODULE_OS_DELAY_MS(50); // Delay 50ms
        LCD_ILI9341_WriteRegData(0x0020, 0x0000); // GRAM horizontal Address
        LCD_ILI9341_WriteRegData(0x0021, 0x013f); // GRAM Vertical Address
        // ----------- Adjust the Gamma Curve ----------//
        LCD_ILI9341_WriteRegData(0x0030, 0x0000);
        LCD_ILI9341_WriteRegData(0x0031, 0x0106);
        LCD_ILI9341_WriteRegData(0x0032, 0x0000);
        LCD_ILI9341_WriteRegData(0x0035, 0x0204);
        LCD_ILI9341_WriteRegData(0x0036, 0x160A);
        LCD_ILI9341_WriteRegData(0x0037, 0x0707);
        LCD_ILI9341_WriteRegData(0x0038, 0x0106);
        LCD_ILI9341_WriteRegData(0x0039, 0x0707);
        LCD_ILI9341_WriteRegData(0x003C, 0x0402);
        LCD_ILI9341_WriteRegData(0x003D, 0x0C0F);
        //------------------ Set GRAM area ---------------//
        LCD_ILI9341_WriteRegData(0x0050, 0x0000); // Horizontal GRAM Start Address
        LCD_ILI9341_WriteRegData(0x0051, 0x00EF); // Horizontal GRAM End Address
        LCD_ILI9341_WriteRegData(0x0052, 0x0000); // Vertical GRAM Start Address
        LCD_ILI9341_WriteRegData(0x0053, 0x013F); // Vertical GRAM Start Address
        LCD_ILI9341_WriteRegData(0x0060, 0x2700); // Gate Scan Line
        LCD_ILI9341_WriteRegData(0x0061, 0x0001); // NDL,VLE, REV
        LCD_ILI9341_WriteRegData(0x006A, 0x0000); // set scrolling line
        //-------------- Partial Display Control ---------//
        LCD_ILI9341_WriteRegData(0x0080, 0x0000);
        LCD_ILI9341_WriteRegData(0x0081, 0x0000);
        LCD_ILI9341_WriteRegData(0x0082, 0x0000);
        LCD_ILI9341_WriteRegData(0x0083, 0x0000);
        LCD_ILI9341_WriteRegData(0x0084, 0x0000);
        LCD_ILI9341_WriteRegData(0x0085, 0x0000);
        //-------------- Panel Control -------------------//
        LCD_ILI9341_WriteRegData(0x0090, 0x0010);
        LCD_ILI9341_WriteRegData(0x0092, 0x0600);
        LCD_ILI9341_WriteRegData(0x0007, 0x0133); // 262K color and display ON
    }
    //
    else if(Lcd_Ili9341_s_dev.id==0x5408)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x01,0x0100);
        LCD_ILI9341_WriteRegData(0x02,0x0700);//LCD Driving Waveform Contral
        LCD_ILI9341_WriteRegData(0x03,0x1030);//Entry Mode设置
        //指针从左至右自上而下的自动增模式
        //Normal Mode(Window Mode disable)
        //RGB格式
        //16位数据2次传输的8总线设置
        LCD_ILI9341_WriteRegData(0x04,0x0000); //Scalling Control register
        LCD_ILI9341_WriteRegData(0x08,0x0207); //Display Control 2
        LCD_ILI9341_WriteRegData(0x09,0x0000); //Display Control 3
        LCD_ILI9341_WriteRegData(0x0A,0x0000); //Frame Cycle Control
        LCD_ILI9341_WriteRegData(0x0C,0x0000); //External Display Interface Control 1
        LCD_ILI9341_WriteRegData(0x0D,0x0000); //Frame Maker Position
        LCD_ILI9341_WriteRegData(0x0F,0x0000); //External Display Interface Control 2
        MODULE_OS_DELAY_MS(20);
        //TFT 液晶彩色图像显示方法14
        LCD_ILI9341_WriteRegData(0x10,0x16B0); //0x14B0 //Power Control 1
        LCD_ILI9341_WriteRegData(0x11,0x0001); //0x0007 //Power Control 2
        LCD_ILI9341_WriteRegData(0x17,0x0001); //0x0000 //Power Control 3
        LCD_ILI9341_WriteRegData(0x12,0x0138); //0x013B //Power Control 4
        LCD_ILI9341_WriteRegData(0x13,0x0800); //0x0800 //Power Control 5
        LCD_ILI9341_WriteRegData(0x29,0x0009); //NVM read data 2
        LCD_ILI9341_WriteRegData(0x2a,0x0009); //NVM read data 3
        LCD_ILI9341_WriteRegData(0xa4,0x0000);
        LCD_ILI9341_WriteRegData(0x50,0x0000); //设置操作窗口的X轴开始列
        LCD_ILI9341_WriteRegData(0x51,0x00EF); //设置操作窗口的X轴结束列
        LCD_ILI9341_WriteRegData(0x52,0x0000); //设置操作窗口的Y轴开始行
        LCD_ILI9341_WriteRegData(0x53,0x013F); //设置操作窗口的Y轴结束行
        LCD_ILI9341_WriteRegData(0x60,0x2700); //Driver Output Control
        //设置屏幕的点数以及扫描的起始行
        LCD_ILI9341_WriteRegData(0x61,0x0001); //Driver Output Control
        LCD_ILI9341_WriteRegData(0x6A,0x0000); //Vertical Scroll Control
        LCD_ILI9341_WriteRegData(0x80,0x0000); //Display Position – Partial Display 1
        LCD_ILI9341_WriteRegData(0x81,0x0000); //RAM Address Start – Partial Display 1
        LCD_ILI9341_WriteRegData(0x82,0x0000); //RAM address End - Partial Display 1
        LCD_ILI9341_WriteRegData(0x83,0x0000); //Display Position – Partial Display 2
        LCD_ILI9341_WriteRegData(0x84,0x0000); //RAM Address Start – Partial Display 2
        LCD_ILI9341_WriteRegData(0x85,0x0000); //RAM address End – Partail Display2
        LCD_ILI9341_WriteRegData(0x90,0x0013); //Frame Cycle Control
        LCD_ILI9341_WriteRegData(0x92,0x0000);  //Panel Interface Control 2
        LCD_ILI9341_WriteRegData(0x93,0x0003); //Panel Interface control 3
        LCD_ILI9341_WriteRegData(0x95,0x0110);  //Frame Cycle Control
        LCD_ILI9341_WriteRegData(0x07,0x0173);
        MODULE_OS_DELAY_MS(50);
    }
    //OK
    else if(Lcd_Ili9341_s_dev.id==0x1505)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        // second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
        LCD_ILI9341_WriteRegData(0x0007,0x0000);
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x0012,0x011C);//0x011A   why need to set several times?
        LCD_ILI9341_WriteRegData(0x00A4,0x0001);//NVM
        LCD_ILI9341_WriteRegData(0x0008,0x000F);
        LCD_ILI9341_WriteRegData(0x000A,0x0008);
        LCD_ILI9341_WriteRegData(0x000D,0x0008);
        //伽马校正
        LCD_ILI9341_WriteRegData(0x0030,0x0707);
        LCD_ILI9341_WriteRegData(0x0031,0x0007); //0x0707
        LCD_ILI9341_WriteRegData(0x0032,0x0603);
        LCD_ILI9341_WriteRegData(0x0033,0x0700);
        LCD_ILI9341_WriteRegData(0x0034,0x0202);
        LCD_ILI9341_WriteRegData(0x0035,0x0002); //?0x0606
        LCD_ILI9341_WriteRegData(0x0036,0x1F0F);
        LCD_ILI9341_WriteRegData(0x0037,0x0707); //0x0f0f  0x0105
        LCD_ILI9341_WriteRegData(0x0038,0x0000);
        LCD_ILI9341_WriteRegData(0x0039,0x0000);
        LCD_ILI9341_WriteRegData(0x003A,0x0707);
        LCD_ILI9341_WriteRegData(0x003B,0x0000); //0x0303
        LCD_ILI9341_WriteRegData(0x003C,0x0007); //?0x0707
        LCD_ILI9341_WriteRegData(0x003D,0x0000); //0x1313//0x1f08
        MODULE_OS_DELAY_MS(50);
        LCD_ILI9341_WriteRegData(0x0007,0x0001);
        LCD_ILI9341_WriteRegData(0x0017,0x0001);//开启电源
        MODULE_OS_DELAY_MS(50);
        //电源配置
        LCD_ILI9341_WriteRegData(0x0010,0x17A0);
        LCD_ILI9341_WriteRegData(0x0011,0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
        LCD_ILI9341_WriteRegData(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
        LCD_ILI9341_WriteRegData(0x0013,0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
        LCD_ILI9341_WriteRegData(0x002A,0x0000);
        LCD_ILI9341_WriteRegData(0x0029,0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
        LCD_ILI9341_WriteRegData(0x0012,0x013E);// 0x013C  power supply on
        //Coordinates Control//
        LCD_ILI9341_WriteRegData(0x0050,0x0000);//0x0e00
        LCD_ILI9341_WriteRegData(0x0051,0x00EF);
        LCD_ILI9341_WriteRegData(0x0052,0x0000);
        LCD_ILI9341_WriteRegData(0x0053,0x013F);
        //Pannel Image Control//
        LCD_ILI9341_WriteRegData(0x0060,0x2700);
        LCD_ILI9341_WriteRegData(0x0061,0x0001);
        LCD_ILI9341_WriteRegData(0x006A,0x0000);
        LCD_ILI9341_WriteRegData(0x0080,0x0000);
        //Partial Image Control//
        LCD_ILI9341_WriteRegData(0x0081,0x0000);
        LCD_ILI9341_WriteRegData(0x0082,0x0000);
        LCD_ILI9341_WriteRegData(0x0083,0x0000);
        LCD_ILI9341_WriteRegData(0x0084,0x0000);
        LCD_ILI9341_WriteRegData(0x0085,0x0000);
        //Panel Interface Control//
        LCD_ILI9341_WriteRegData(0x0090,0x0013);//0x0010 frenqucy
        LCD_ILI9341_WriteRegData(0x0092,0x0300);
        LCD_ILI9341_WriteRegData(0x0093,0x0005);
        LCD_ILI9341_WriteRegData(0x0095,0x0000);
        LCD_ILI9341_WriteRegData(0x0097,0x0000);
        LCD_ILI9341_WriteRegData(0x0098,0x0000);

        LCD_ILI9341_WriteRegData(0x0001,0x0100);
        LCD_ILI9341_WriteRegData(0x0002,0x0700);
        LCD_ILI9341_WriteRegData(0x0003,0x1038);//扫描方向 上->下  左->右
        LCD_ILI9341_WriteRegData(0x0004,0x0000);
        LCD_ILI9341_WriteRegData(0x000C,0x0000);
        LCD_ILI9341_WriteRegData(0x000F,0x0000);
        LCD_ILI9341_WriteRegData(0x0020,0x0000);
        LCD_ILI9341_WriteRegData(0x0021,0x0000);
        LCD_ILI9341_WriteRegData(0x0007,0x0021);
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WriteRegData(0x0007,0x0061);
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WriteRegData(0x0007,0x0173);
        MODULE_OS_DELAY_MS(20);
    }
    //
    else if(Lcd_Ili9341_s_dev.id==0xB505)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);

        LCD_ILI9341_WriteRegData(0x00a4,0x0001);
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WriteRegData(0x0060,0x2700);
        LCD_ILI9341_WriteRegData(0x0008,0x0202);

        LCD_ILI9341_WriteRegData(0x0030,0x0214);
        LCD_ILI9341_WriteRegData(0x0031,0x3715);
        LCD_ILI9341_WriteRegData(0x0032,0x0604);
        LCD_ILI9341_WriteRegData(0x0033,0x0e16);
        LCD_ILI9341_WriteRegData(0x0034,0x2211);
        LCD_ILI9341_WriteRegData(0x0035,0x1500);
        LCD_ILI9341_WriteRegData(0x0036,0x8507);
        LCD_ILI9341_WriteRegData(0x0037,0x1407);
        LCD_ILI9341_WriteRegData(0x0038,0x1403);
        LCD_ILI9341_WriteRegData(0x0039,0x0020);

        LCD_ILI9341_WriteRegData(0x0090,0x001a);
        LCD_ILI9341_WriteRegData(0x0010,0x0000);
        LCD_ILI9341_WriteRegData(0x0011,0x0007);
        LCD_ILI9341_WriteRegData(0x0012,0x0000);
        LCD_ILI9341_WriteRegData(0x0013,0x0000);
        MODULE_OS_DELAY_MS(20);

        LCD_ILI9341_WriteRegData(0x0010,0x0730);
        LCD_ILI9341_WriteRegData(0x0011,0x0137);
        MODULE_OS_DELAY_MS(20);

        LCD_ILI9341_WriteRegData(0x0012,0x01b8);
        MODULE_OS_DELAY_MS(20);

        LCD_ILI9341_WriteRegData(0x0013,0x0f00);
        LCD_ILI9341_WriteRegData(0x002a,0x0080);
        LCD_ILI9341_WriteRegData(0x0029,0x0048);
        MODULE_OS_DELAY_MS(20);

        LCD_ILI9341_WriteRegData(0x0001,0x0100);
        LCD_ILI9341_WriteRegData(0x0002,0x0700);
        LCD_ILI9341_WriteRegData(0x0003,0x1038);//扫描方向 上->下  左->右
        LCD_ILI9341_WriteRegData(0x0008,0x0202);
        LCD_ILI9341_WriteRegData(0x000a,0x0000);
        LCD_ILI9341_WriteRegData(0x000c,0x0000);
        LCD_ILI9341_WriteRegData(0x000d,0x0000);
        LCD_ILI9341_WriteRegData(0x000e,0x0030);
        LCD_ILI9341_WriteRegData(0x0050,0x0000);
        LCD_ILI9341_WriteRegData(0x0051,0x00ef);
        LCD_ILI9341_WriteRegData(0x0052,0x0000);
        LCD_ILI9341_WriteRegData(0x0053,0x013f);
        LCD_ILI9341_WriteRegData(0x0060,0x2700);
        LCD_ILI9341_WriteRegData(0x0061,0x0001);
        LCD_ILI9341_WriteRegData(0x006a,0x0000);
        //LCD_WriteReg(0x0080,0x0000);
        //LCD_WriteReg(0x0081,0x0000);
        LCD_ILI9341_WriteRegData(0x0090,0X0011);
        LCD_ILI9341_WriteRegData(0x0092,0x0600);
        LCD_ILI9341_WriteRegData(0x0093,0x0402);
        LCD_ILI9341_WriteRegData(0x0094,0x0002);
        MODULE_OS_DELAY_MS(20);

        LCD_ILI9341_WriteRegData(0x0007,0x0001);
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WriteRegData(0x0007,0x0061);
        LCD_ILI9341_WriteRegData(0x0007,0x0173);

        LCD_ILI9341_WriteRegData(0x0020,0x0000);
        LCD_ILI9341_WriteRegData(0x0021,0x0000);
        LCD_ILI9341_WriteRegData(0x00,0x22);
    }
    //
    else if(Lcd_Ili9341_s_dev.id==0xC505)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x0000,0x0000);
        LCD_ILI9341_WriteRegData(0x00a4,0x0001);
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WriteRegData(0x0060,0x2700);
        LCD_ILI9341_WriteRegData(0x0008,0x0806);

        LCD_ILI9341_WriteRegData(0x0030,0x0703);//gamma setting
        LCD_ILI9341_WriteRegData(0x0031,0x0001);
        LCD_ILI9341_WriteRegData(0x0032,0x0004);
        LCD_ILI9341_WriteRegData(0x0033,0x0102);
        LCD_ILI9341_WriteRegData(0x0034,0x0300);
        LCD_ILI9341_WriteRegData(0x0035,0x0103);
        LCD_ILI9341_WriteRegData(0x0036,0x001F);
        LCD_ILI9341_WriteRegData(0x0037,0x0703);
        LCD_ILI9341_WriteRegData(0x0038,0x0001);
        LCD_ILI9341_WriteRegData(0x0039,0x0004);



        LCD_ILI9341_WriteRegData(0x0090, 0x0015);   //80Hz
        LCD_ILI9341_WriteRegData(0x0010, 0X0410);   //BT,AP
        LCD_ILI9341_WriteRegData(0x0011,0x0247);    //DC1,DC0,VC
        LCD_ILI9341_WriteRegData(0x0012, 0x01BC);
        LCD_ILI9341_WriteRegData(0x0013, 0x0e00);
        MODULE_OS_DELAY_MS(120);
        LCD_ILI9341_WriteRegData(0x0001, 0x0100);
        LCD_ILI9341_WriteRegData(0x0002, 0x0200);
        LCD_ILI9341_WriteRegData(0x0003, 0x1030);

        LCD_ILI9341_WriteRegData(0x000A, 0x0008);
        LCD_ILI9341_WriteRegData(0x000C, 0x0000);

        LCD_ILI9341_WriteRegData(0x000E, 0x0020);
        LCD_ILI9341_WriteRegData(0x000F, 0x0000);
        LCD_ILI9341_WriteRegData(0x0020, 0x0000);   //H Start
        LCD_ILI9341_WriteRegData(0x0021, 0x0000);   //V Start
        LCD_ILI9341_WriteRegData(0x002A,0x003D);    //vcom2
        MODULE_OS_DELAY_MS(20);
        LCD_ILI9341_WriteRegData(0x0029, 0x002d);
        LCD_ILI9341_WriteRegData(0x0050, 0x0000);
        LCD_ILI9341_WriteRegData(0x0051, 0xD0EF);
        LCD_ILI9341_WriteRegData(0x0052, 0x0000);
        LCD_ILI9341_WriteRegData(0x0053, 0x013F);
        LCD_ILI9341_WriteRegData(0x0061, 0x0000);
        LCD_ILI9341_WriteRegData(0x006A, 0x0000);
        LCD_ILI9341_WriteRegData(0x0092,0x0300);

        LCD_ILI9341_WriteRegData(0x0093, 0x0005);
        LCD_ILI9341_WriteRegData(0x0007, 0x0100);
    }
    //OK |/|/|
    else if(Lcd_Ili9341_s_dev.id==0x8989)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0x0000,0x0001);//打开晶振
        LCD_ILI9341_WriteRegData(0x0003,0xA8A4);//0xA8A4
        LCD_ILI9341_WriteRegData(0x000C,0x0000);
        LCD_ILI9341_WriteRegData(0x000D,0x080C);
        LCD_ILI9341_WriteRegData(0x000E,0x2B00);
        LCD_ILI9341_WriteRegData(0x001E,0x00B0);
        LCD_ILI9341_WriteRegData(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
        LCD_ILI9341_WriteRegData(0x0002,0x0600);
        LCD_ILI9341_WriteRegData(0x0010,0x0000);
        LCD_ILI9341_WriteRegData(0x0011,0x6078); //定义数据格式  16位色         横屏 0x6058
        LCD_ILI9341_WriteRegData(0x0005,0x0000);
        LCD_ILI9341_WriteRegData(0x0006,0x0000);
        LCD_ILI9341_WriteRegData(0x0016,0xEF1C);
        LCD_ILI9341_WriteRegData(0x0017,0x0003);
        LCD_ILI9341_WriteRegData(0x0007,0x0233); //0x0233
        LCD_ILI9341_WriteRegData(0x000B,0x0000);
        LCD_ILI9341_WriteRegData(0x000F,0x0000); //扫描开始地址
        LCD_ILI9341_WriteRegData(0x0041,0x0000);
        LCD_ILI9341_WriteRegData(0x0042,0x0000);
        LCD_ILI9341_WriteRegData(0x0048,0x0000);
        LCD_ILI9341_WriteRegData(0x0049,0x013F);
        LCD_ILI9341_WriteRegData(0x004A,0x0000);
        LCD_ILI9341_WriteRegData(0x004B,0x0000);
        LCD_ILI9341_WriteRegData(0x0044,0xEF00);
        LCD_ILI9341_WriteRegData(0x0045,0x0000);
        LCD_ILI9341_WriteRegData(0x0046,0x013F);
        LCD_ILI9341_WriteRegData(0x0030,0x0707);
        LCD_ILI9341_WriteRegData(0x0031,0x0204);
        LCD_ILI9341_WriteRegData(0x0032,0x0204);
        LCD_ILI9341_WriteRegData(0x0033,0x0502);
        LCD_ILI9341_WriteRegData(0x0034,0x0507);
        LCD_ILI9341_WriteRegData(0x0035,0x0204);
        LCD_ILI9341_WriteRegData(0x0036,0x0204);
        LCD_ILI9341_WriteRegData(0x0037,0x0502);
        LCD_ILI9341_WriteRegData(0x003A,0x0302);
        LCD_ILI9341_WriteRegData(0x003B,0x0302);
        LCD_ILI9341_WriteRegData(0x0023,0x0000);
        LCD_ILI9341_WriteRegData(0x0024,0x0000);
        LCD_ILI9341_WriteRegData(0x0025,0x8000);
        LCD_ILI9341_WriteRegData(0x004f,0);        //行首址0
        LCD_ILI9341_WriteRegData(0x004e,0);        //列首址0
    }
    //OK |/|/|
    else if(Lcd_Ili9341_s_dev.id==0x4531)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0X00,0X0001);
        MODULE_OS_DELAY_MS(10);
        LCD_ILI9341_WriteRegData(0X10,0X1628);
        LCD_ILI9341_WriteRegData(0X12,0X000e);//0x0006
        LCD_ILI9341_WriteRegData(0X13,0X0A39);
        MODULE_OS_DELAY_MS(10);
        LCD_ILI9341_WriteRegData(0X11,0X0040);
        LCD_ILI9341_WriteRegData(0X15,0X0050);
        MODULE_OS_DELAY_MS(10);
        LCD_ILI9341_WriteRegData(0X12,0X001e);//16
        MODULE_OS_DELAY_MS(10);
        LCD_ILI9341_WriteRegData(0X10,0X1620);
        LCD_ILI9341_WriteRegData(0X13,0X2A39);
        MODULE_OS_DELAY_MS(10);
        LCD_ILI9341_WriteRegData(0X01,0X0100);
        LCD_ILI9341_WriteRegData(0X02,0X0300);
        LCD_ILI9341_WriteRegData(0X03,0X1038);//改变方向的
        LCD_ILI9341_WriteRegData(0X08,0X0202);
        LCD_ILI9341_WriteRegData(0X0A,0X0008);
        LCD_ILI9341_WriteRegData(0X30,0X0000);
        LCD_ILI9341_WriteRegData(0X31,0X0402);
        LCD_ILI9341_WriteRegData(0X32,0X0106);
        LCD_ILI9341_WriteRegData(0X33,0X0503);
        LCD_ILI9341_WriteRegData(0X34,0X0104);
        LCD_ILI9341_WriteRegData(0X35,0X0301);
        LCD_ILI9341_WriteRegData(0X36,0X0707);
        LCD_ILI9341_WriteRegData(0X37,0X0305);
        LCD_ILI9341_WriteRegData(0X38,0X0208);
        LCD_ILI9341_WriteRegData(0X39,0X0F0B);
        LCD_ILI9341_WriteRegData(0X41,0X0002);
        LCD_ILI9341_WriteRegData(0X60,0X2700);
        LCD_ILI9341_WriteRegData(0X61,0X0001);
        LCD_ILI9341_WriteRegData(0X90,0X0210);
        LCD_ILI9341_WriteRegData(0X92,0X010A);
        LCD_ILI9341_WriteRegData(0X93,0X0004);
        LCD_ILI9341_WriteRegData(0XA0,0X0100);
        LCD_ILI9341_WriteRegData(0X07,0X0001);
        LCD_ILI9341_WriteRegData(0X07,0X0021);
        LCD_ILI9341_WriteRegData(0X07,0X0023);
        LCD_ILI9341_WriteRegData(0X07,0X0033);
        LCD_ILI9341_WriteRegData(0X07,0X0133);
        LCD_ILI9341_WriteRegData(0XA0,0X0000);
    }
    //
    else if(Lcd_Ili9341_s_dev.id==0x4535)
    {
        Lcd_Ili9341_s_dev.colorType=565;
        LCD_ILI9341_WriteRegData(0X15,0X0030);
        LCD_ILI9341_WriteRegData(0X9A,0X0010);
        LCD_ILI9341_WriteRegData(0X11,0X0020);
        LCD_ILI9341_WriteRegData(0X10,0X3428);
        LCD_ILI9341_WriteRegData(0X12,0X0002);//16
        LCD_ILI9341_WriteRegData(0X13,0X1038);
        MODULE_OS_DELAY_MS(40);
        LCD_ILI9341_WriteRegData(0X12,0X0012);//16
        MODULE_OS_DELAY_MS(40);
        LCD_ILI9341_WriteRegData(0X10,0X3420);
        LCD_ILI9341_WriteRegData(0X13,0X3038);
        MODULE_OS_DELAY_MS(70);
        LCD_ILI9341_WriteRegData(0X30,0X0000);
        LCD_ILI9341_WriteRegData(0X31,0X0402);
        LCD_ILI9341_WriteRegData(0X32,0X0307);
        LCD_ILI9341_WriteRegData(0X33,0X0304);
        LCD_ILI9341_WriteRegData(0X34,0X0004);
        LCD_ILI9341_WriteRegData(0X35,0X0401);
        LCD_ILI9341_WriteRegData(0X36,0X0707);
        LCD_ILI9341_WriteRegData(0X37,0X0305);
        LCD_ILI9341_WriteRegData(0X38,0X0610);
        LCD_ILI9341_WriteRegData(0X39,0X0610);

        LCD_ILI9341_WriteRegData(0X01,0X0100);
        LCD_ILI9341_WriteRegData(0X02,0X0300);
        LCD_ILI9341_WriteRegData(0X03,0X1030);//改变方向的
        LCD_ILI9341_WriteRegData(0X08,0X0808);
        LCD_ILI9341_WriteRegData(0X0A,0X0008);
        LCD_ILI9341_WriteRegData(0X60,0X2700);
        LCD_ILI9341_WriteRegData(0X61,0X0001);
        LCD_ILI9341_WriteRegData(0X90,0X013E);
        LCD_ILI9341_WriteRegData(0X92,0X0100);
        LCD_ILI9341_WriteRegData(0X93,0X0100);
        LCD_ILI9341_WriteRegData(0XA0,0X3000);
        LCD_ILI9341_WriteRegData(0XA3,0X0010);
        LCD_ILI9341_WriteRegData(0X07,0X0001);
        LCD_ILI9341_WriteRegData(0X07,0X0021);
        LCD_ILI9341_WriteRegData(0X07,0X0023);
        LCD_ILI9341_WriteRegData(0X07,0X0033);
        LCD_ILI9341_WriteRegData(0X07,0X0133);
    }
    else if(Lcd_Ili9341_s_dev.id==0x9488)
    {
        Lcd_Ili9341_s_dev.colorType=666;
        LCD_ILI9341_WR_REG(0XC0);   //Power Control 1
        LCD_ILI9341_WR_DATA(0x17);  //Vreg1out
        LCD_ILI9341_WR_DATA(0x15);  //Verg2out

        LCD_ILI9341_WR_REG(0xC1);   //Power Control 2
        LCD_ILI9341_WR_DATA(0x41);  //VGH,VGL

        LCD_ILI9341_WR_REG(0xC5);   //Power Control 3
        LCD_ILI9341_WR_DATA(0x00);
        LCD_ILI9341_WR_DATA(0x12);  //Vcom
        LCD_ILI9341_WR_DATA(0x80);

        LCD_ILI9341_WR_REG(0x36);   //Memory Access
        LCD_ILI9341_WR_DATA(0x48);

        LCD_ILI9341_WR_REG(0x3A);   // Interface Pixel Format
        LCD_ILI9341_WR_DATA(0x66);  //18 bit

        LCD_ILI9341_WR_REG(0XB0);   // Interface Mode Control
        LCD_ILI9341_WR_DATA(0x00);

        LCD_ILI9341_WR_REG(0xB1);   //Frame rate
        LCD_ILI9341_WR_DATA(0xA0);  //60Hz

        LCD_ILI9341_WR_REG(0xB4);   //Display Inversion Control
        LCD_ILI9341_WR_DATA(0x00);  // -dot

        LCD_ILI9341_WR_REG(0XB6);   //Display Function Control  RGB/MCU Interface Control
        LCD_ILI9341_WR_DATA(0x02);  //MCU
        LCD_ILI9341_WR_DATA(0x02);  //Source,Gate scan dieection

        LCD_ILI9341_WR_REG(0XE9);   // Set Image Functio
        LCD_ILI9341_WR_DATA(0x00);  // Disable 24 bit data

        LCD_ILI9341_WR_REG(0xF7);   // Adjust Control
        LCD_ILI9341_WR_DATA(0xA9);
        LCD_ILI9341_WR_DATA(0x51);
        LCD_ILI9341_WR_DATA(0x2C);
        LCD_ILI9341_WR_DATA(0x82);  // D7 stream, loose

        LCD_ILI9341_WR_REG(0x11);   //Sleep out
        MODULE_OS_DELAY_MS(120);
        LCD_ILI9341_WR_REG(0x29);
        // -----XSL-----TEST
        LCD_ILI9341_SetBackLight(100);
        // -------------
    }
    //屏方向
    LCD_ILI9341_DisplayDir(1);
    //点亮背光
    //LCD6804_LED_ON;
    LCD_ILI9341_Clear(LCD_ILI9341_E_COLOUR_BLACK);
    //
}
//清屏函数
//color:要清屏的填充色
void LCD_ILI9341_Clear(uint32_t color)
{
    u32 index=0;
    uint8_t color_buf[3];
    //得到总点数
    u32 totalpoint=Lcd_Ili9341_s_dev.width;
    totalpoint*=Lcd_Ili9341_s_dev.height;
    //6804横屏的时候特殊处理
    if((Lcd_Ili9341_s_dev.id==0X6804)&&(Lcd_Ili9341_s_dev.dir==1))
    {
        Lcd_Ili9341_s_dev.dir=0;
        Lcd_Ili9341_s_dev.setxcmd=0X2A;
        Lcd_Ili9341_s_dev.setycmd=0X2B;
        LCD_ILI9341_SetCursor(0x00,0x0000);
        Lcd_Ili9341_s_dev.dir=1;
        Lcd_Ili9341_s_dev.setxcmd=0X2B;
        Lcd_Ili9341_s_dev.setycmd=0X2A;
    }
    else
    {
        LCD_ILI9341_SetCursor(0x00,0x0000);
    }
    LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
    if(Lcd_Ili9341_s_dev.colorType==666)
    {
        // 颜色变换
        Count_ColorConver(0,&color,color_buf);
        //
        for(index=0; index<totalpoint; index++)
        {
            LCD_ILI9341_WR_DATA(color_buf[0]);
            LCD_ILI9341_WR_DATA(color_buf[1]);
            LCD_ILI9341_WR_DATA(color_buf[2]);
        }
    }
    else
    {
        for(index=0; index<totalpoint; index++)
        {
            LCD_ILI9341_WR_DATA(color>>8);
            LCD_ILI9341_WR_DATA(color);
        }
    }
}
/****************************************************************************
* 功    能：在指定区域内填充单个颜色
* 参    数: (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
            color:要填充的颜色
* 注    意: 里面包含重入函数,不知结果会怎么样
****************************************************************************/
void LCD_ILI9341_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color)
{
    uint16_t i,j;
    uint16_t xlen=0;
    uint16_t temp;
    //6804横屏的时候特殊处理
    if((Lcd_Ili9341_s_dev.id==0X6804)&&(Lcd_Ili9341_s_dev.dir==1))
    {
        temp=sx;
        sx=sy;
        sy=Lcd_Ili9341_s_dev.width-ex-1;
        ex=ey;
        ey=Lcd_Ili9341_s_dev.width-temp-1;
        Lcd_Ili9341_s_dev.dir=0;
        Lcd_Ili9341_s_dev.setxcmd=0X2A;
        Lcd_Ili9341_s_dev.setycmd=0X2B;
        LCD_ILI9341_Fill(sx,sy,ex,ey,color);
        Lcd_Ili9341_s_dev.dir=1;
        Lcd_Ili9341_s_dev.setxcmd=0X2B;
        Lcd_Ili9341_s_dev.setycmd=0X2A;
    }
    else if(Lcd_Ili9341_s_dev.id==0x9488 && Lcd_Ili9341_s_dev.colorType==666)
    {
        uint8_t color_buf[3];
        // 颜色变换
        Count_ColorConver(0,&color,color_buf);
        //
        xlen=ex-sx+1;
        for(i=sy; i<=ey; i++)
        {
            LCD_ILI9341_SetCursor(sx,i);
            LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
            for(j=0; j<xlen; j++)
            {
                LCD_ILI9341_WR_DATA(color_buf[0]);
                LCD_ILI9341_WR_DATA(color_buf[1]);
                LCD_ILI9341_WR_DATA(color_buf[2]);
            }
        }
    }
    else
    {
        xlen=ex-sx+1;
        for(i=sy; i<=ey; i++)
        {
            LCD_ILI9341_SetCursor(sx,i);
            LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
            for(j=0; j<xlen; j++)
            {
                //LCD_ILI9341_WR_DATA(color);
                LCD_ILI9341_WR_DATA(color>>8);
                LCD_ILI9341_WR_DATA(color);
            }
        }
    }
}
/****************************************************************************
* 功    能：在指定区域内填充单个颜色
* 参    数: (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
            color:要填充的颜色
****************************************************************************/
/*
void LCD_ILI9341_ColorFill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
    uint16_t height,width;
    uint16_t i,j;
    width=ex-sx+1;
    height=ey-sy+1;
    for(i=0; i<height; i++)
    {
        LCD_ILI9341_SetCursor(sx,sy+i);
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
        for(j=0; j<width; j++)
        {
            LCD_ILI9341_WR_DATA(color[i*height+j]);
        }
    }
}
*/
/****************************************************************************
* 功    能：画线
* 参    数: x1,y1:起点坐标    x2,y2:终点坐标
****************************************************************************/
void LCD_ILI9341_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    uint16_t t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1;
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    //垂直线
    if(delta_x>0)
        incx=1;
    else if(delta_x==0)
        incx=0;
    else
    {
        incx=-1;
        delta_x=-delta_x;
    }
    //水平线
    if(delta_y>0)
        incy=1;
    else if(delta_y==0)
        incy=0;
    else
    {
        incy=-1;
        delta_y=-delta_y;
    }
    //选取基本增量坐标轴
    if( delta_x>delta_y)distance=delta_x;
    else distance=delta_y;
    //画线输出
    for(t=0; t<=distance+1; t++ )
    {
        //画点
        LCD_ILI9341_DrawPoint(uRow,uCol,color);
        xerr+=delta_x ;
        yerr+=delta_y ;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}
/****************************************************************************
* 功    能：画矩形框
* 参    数: (x1,y1),(x2,y2):矩形的对角坐标
****************************************************************************/
/*
void LCD_ILI9341_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    LCD_ILI9341_DrawHLine(x1,y1,x2,color);
    LCD_ILI9341_DrawVLine(x2,y1,y2,color);
    LCD_ILI9341_DrawHLine(x1,y2,x2,color);
    LCD_ILI9341_DrawVLine(x1,y1,y2,color);
}
*/
void LCD_ILI9341_DrawRect(uint16_t _usX, uint16_t _usY, uint8_t _usHeight, uint16_t _usWidth, uint32_t _usColor)
{
    LCD_ILI9341_DrawHLine(_usX,_usY,_usX + _usWidth - 1,_usColor);
    LCD_ILI9341_DrawVLine(_usX + _usWidth - 1,_usY,_usY + _usHeight - 1,_usColor);
    LCD_ILI9341_DrawHLine(_usX,_usY + _usHeight - 1,_usX + _usWidth - 1,_usColor);
    LCD_ILI9341_DrawVLine(_usX,_usY,_usY + _usHeight - 1,_usColor);
}
/****************************************************************************
* 功    能：在指定位置画一个指定大小的圆
* 参    数: (x,y):中心点   r:半径
****************************************************************************/
void LCD_ILI9341_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r,uint32_t color)
{
    int a,b;
    int di;
    a=0;
    b=r;
    //判断下个点位置的标志
    di=3-(r<<1);
    while(a<=b)
    {
        LCD_ILI9341_DrawPoint(x0+a,y0-b,color);
        LCD_ILI9341_DrawPoint(x0+b,y0-a,color);
        LCD_ILI9341_DrawPoint(x0+b,y0+a,color);
        LCD_ILI9341_DrawPoint(x0+a,y0+b,color);
        LCD_ILI9341_DrawPoint(x0-a,y0+b,color);
        LCD_ILI9341_DrawPoint(x0-b,y0+a,color);
        LCD_ILI9341_DrawPoint(x0-a,y0-b,color);
        LCD_ILI9341_DrawPoint(x0-b,y0-a,color);
        a++;
        //使用Bresenham算法画圆
        if(di<0)di +=4*a+6;
        else
        {
            di+=10+4*(a-b);
            b--;
        }
    }
}
/*
*********************************************************************************************************
*   功能说明: 在LCD上显示一个BMP位图，位图点阵扫描次序：从左到右，从上到下
*   形    参:
*           _usX, _usY : 图片的坐标
*           _usHeight  ：图片高度
*           _usWidth   ：图片宽度
*           _ptr       ：图片点阵指针
*   返 回 值: 无
*********************************************************************************************************
*/
void LCD_ILI9341_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
    uint32_t index = 0;
    const uint16_t *p;

    /* 设置图片的位置和大小， 即设置显示窗口 */
    LCD_ILI9341_SetWindow(_usX, _usY,_usWidth,_usHeight);

    p = _ptr;
    for (index = 0; index < _usHeight * _usWidth; index++)
    {
        LCD_ILI9341_DrawPoint(_usX, _usY, *p++);
    }

    /* 退出窗口绘图模式 */
    LCD_ILI9341_QuitWinow();
}
//-----XSL-----增加
void LCD_ILI9341_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint32_t _usColor)
{
    uint8_t color_buf[3];
    LCD_ILI9341_SetCursor(_usX1,_usY1);
    LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
    if(Lcd_Ili9341_s_dev.colorType==565)
    {
        for(; _usX1<=_usX2; _usX1++)
        {
            LCD_ILI9341_WR_DATA(_usColor>>8);
            LCD_ILI9341_WR_DATA(_usColor);
        }
    }
    else if(Lcd_Ili9341_s_dev.colorType==666)
    {
        // 颜色变换
        Count_ColorConver(0,&_usColor,color_buf);
        for(; _usX1<=_usX2; _usX1++)
        {
            LCD_ILI9341_WR_DATA(color_buf[0]);
            LCD_ILI9341_WR_DATA(color_buf[1]);
            LCD_ILI9341_WR_DATA(color_buf[2]);
        }
    }
}
void LCD_ILI9341_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor)
{
    uint16_t i16;
    uint32_t color;
    uint8_t color_buf[3];
    LCD_ILI9341_SetCursor(_usX1,_usY1);
    LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
    if(Lcd_Ili9341_s_dev.colorType==565)
    {
        for(i16=0; i16<_usWidth; i16++)
        {
            LCD_ILI9341_WR_DATA(_pColor[i16]>>8);
            LCD_ILI9341_WR_DATA(_pColor[i16]);
        }
    }
    else if(Lcd_Ili9341_s_dev.colorType==666)
    {
        //
        for(i16=0; i16<_usWidth; i16++)
        {
            // 颜色变换
            color = _pColor[i16];
            Count_ColorConver(0,&color,color_buf);
            LCD_ILI9341_WR_DATA(color_buf[0]);
            LCD_ILI9341_WR_DATA(color_buf[1]);
            LCD_ILI9341_WR_DATA(color_buf[2]);
        }
    }
}
void LCD_ILI9341_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint32_t _usColor)
{
    /*
    for(i16 = 1; i16< _usY2-_usY1+1; i16++)
    {
        LCD_ILI9341_WR_REG(Lcd_Ili9341_s_dev.wramcmd);
        LCD_ILI9341_WR_DATA(_usColor>>8);
        LCD_ILI9341_WR_DATA(_usColor);
        LCD_ILI9341_SetCursor(_usX1,_usY1+i16);
    }
    */
    uint16_t i16;
    if(_usY1>_usY2)
    {
        i16   =  _usY1;
        _usY1 =  _usY2;
        _usY2 =  i16;
    }
    while(_usY1<=_usY2)
    {
        LCD_ILI9341_DrawPoint(_usX1,_usY1++,_usColor);
    }
}
/****************************************************************************
* 功    能：芯片休眠
****************************************************************************/
void LCD_ILI9341_EnterSleep(void)
{
    LCD_ILI9341_WR_REG(0X10);
    LCD_ILI9341_SleepFlag=1;
}
/****************************************************************************
* 功    能：芯片唤醒
****************************************************************************/
void LCD_ILI9341_EnterWork(void)
{
    LCD_ILI9341_WR_REG(0X11);
    LCD_ILI9341_SleepFlag=0;
}
/****************************************************************************
* 功    能：读取寄存器值
****************************************************************************/
void LCD_ILI9341_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf;
    uint8_t i;
    //申请缓存
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    DebugOutStr((int8_t*)"ILI9341 REG READ:\r\n");
    //
    // 04
    i=0x04;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    pbuf[102]=LCD_ILI9341_RD_DATA();
    pbuf[103]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X %02X %02X\r\n",i,pbuf[100],pbuf[101],pbuf[102],pbuf[103]);
    DebugOutStr((int8_t*)pbuf);
    // 09
    i=0x09;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    pbuf[102]=LCD_ILI9341_RD_DATA();
    pbuf[103]=LCD_ILI9341_RD_DATA();
    pbuf[104]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X %02X %02X %02X\r\n",i,pbuf[100],pbuf[101],pbuf[102],pbuf[103],pbuf[104]);
    DebugOutStr((int8_t*)pbuf);
    // 0A
    i=0x0A;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X\r\n",i,pbuf[100],pbuf[101]);
    DebugOutStr((int8_t*)pbuf);
    // 0B
    i=0x0B;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X\r\n",i,pbuf[100],pbuf[101]);
    DebugOutStr((int8_t*)pbuf);
    // 0C
    i=0x0C;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X\r\n",i,pbuf[100],pbuf[101]);
    DebugOutStr((int8_t*)pbuf);
    // 0D
    i=0x0D;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X\r\n",i,pbuf[100],pbuf[101]);
    DebugOutStr((int8_t*)pbuf);
    // 0E
    i=0x0E;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X\r\n",i,pbuf[100],pbuf[101]);
    DebugOutStr((int8_t*)pbuf);
    // 0F
    i=0x0F;
    LCD_ILI9341_WR_REG(i);
    pbuf[100]=LCD_ILI9341_RD_DATA();
    pbuf[101]=LCD_ILI9341_RD_DATA();
    sprintf((char*)pbuf,"REG-%02X Value-%02X %02X\r\n",i,pbuf[100],pbuf[101]);
    DebugOutStr((int8_t*)pbuf);
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
