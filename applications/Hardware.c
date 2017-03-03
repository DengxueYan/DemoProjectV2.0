/*****************************************************************************************************
* FileName:                    Hardware.c
*
* Description:                 Hardware related functions
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
//====================================================================================================
// 本文件使用的头文件
//====================================================================================================
// 库头文件
#include <string.h>
#include <stdlib.h>

// 自定义头文件
#include "STM32F40xRegister.h"
#include "SystemBase.h"
#include "Debug.h"
#include "Uart.h"
#include "stm32f4xx.h"
#include "Display.h"
#include "RtcBasicFuncs.h"

//====================================================================================================
// 本地函数声明,此处声明的函数不与外部接口
//====================================================================================================
static int32 DebugUARTInitial(void);
static int32 TimerInitial(void);
static int32 InterruptInitial(void);
static int32 WatchDogInitial(void);
static int32 DisplayInitial(void);
static int32 RtcInitial(void);
static int32 CPUSPIFlashInitial(void);

//====================================================================================================
// 本文件定义的与外部的接口变量
//====================================================================================================

//====================================================================================================
// 本地变量声明,此处声明的变量不与外部接口
//====================================================================================================
extern Uint8 const system_start_logo[320 * 240/8];

//====================================================================================================
// 函数实现
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// 接口函数
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: HardwareInitial
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: hardware initial function
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 HardwareInitial(void)
{
    if (    (NORMAL_SUCCESS != SystemBaseInitial())
         || (NORMAL_SUCCESS != DebugUARTInitial())
         || (NORMAL_SUCCESS != TimerInitial())
         || (NORMAL_SUCCESS != InterruptInitial())
         || (NORMAL_SUCCESS != WatchDogInitial())
         || (NORMAL_SUCCESS != DisplayInitial())
         || (NORMAL_SUCCESS != RtcInitial())
         || (NORMAL_SUCCESS != CPUSPIFlashInitial())
       )
    {
        DebugPrintf("Hardware initial failed!\r\n");
        return NORMAL_ERROR;
    }

    // enable global interrupt after hardware initial
    __enable_irq();

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
// 本地函数
//----------------------------------------------------------------------------------------------------
static int8 const uart_initial_string[] = "    \r\n\r\n";
//----------------------------------------------------------------------------------------------------
//   Function: DebugUARInitial
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: Debug串口初始化函数
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 DebugUARTInitial(void)
{
    Uint32 lv_ulong;
    Uint32 i;

    debug_rx_buffer = MallocLowSpeedMem(DEBUG_RX_BUF_SIZE);
    if (NULL == debug_rx_buffer)
    {
        TRACE("debug_rx_buffer MallocLowSpeedMem failed\r\n");
        return NORMAL_ERROR;
    }

    pRCC->AHB1ENR |= 0x00400001u;
    pRCC->APB2ENR |= 0x00000010u;

    lv_ulong = pGPIOA->MODER;
    lv_ulong &= 0xFFC3FFFFu;
    lv_ulong |= 0x00280000u;
    pGPIOA->MODER = lv_ulong;

    lv_ulong = pGPIOA->OTYPER;
    lv_ulong &= 0xFFFFF9FFu;
    lv_ulong |= 0x00000000u;
    pGPIOA->OTYPER = lv_ulong;

    lv_ulong = pGPIOA->OSPEEDR;
    lv_ulong &= 0xFFC3FFFFu;
    lv_ulong |= 0x00000000u;
    pGPIOA->OSPEEDR = lv_ulong;

    lv_ulong = pGPIOA->AFRH;
    lv_ulong &= 0xFFFFF00Fu;
    lv_ulong |= 0x00000770u;
    pGPIOA->AFRH = lv_ulong;

    while (0 != (pDMA2->STREAM[5].CR & 1))
    {
        pDMA2->STREAM[5].CR &= (~1);
    }

    pDMA2->STREAM[5].PAR = (Uint32)&(pDEBUG->DR);
    pDMA2->STREAM[5].M0AR = (Uint32)debug_rx_buffer;
    pDMA2->STREAM[5].NDTR = DEBUG_RX_BUF_SIZE;
    pDMA2->STREAM[5].CR  =  0x08030500u;
    pDMA2->STREAM[5].FCR =  0x03u;
    pDMA2->HIFCR |= 0x00000F40u;

    pDEBUG->BRR = 0x02D9u;
    pDEBUG->CR3 |= 0x0040u;// RX DMA
    pDEBUG->SR  = 0;
    pDEBUG->CR1 = 0x200Cu;
    pDMA2->STREAM[5].CR |= 1;

    pDEBUG->DR = '\r';
    for (i = 0; i < sizeof(uart_initial_string); i++)
    {
        DebugSendByte(uart_initial_string[i]);
    }

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: TimerInitial
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: Timer初始化函数
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 TimerInitial(void)
{
    // Timer2
    pRCC->APB1ENR |= 0x00000001u;

    pTIMER2->PSC = 0;
    pTIMER2->ARR = (~0);
    pTIMER2->EGR |= 1;
    pTIMER2->CR1 = 0x0007u;

    // Timer14
    pRCC->APB1ENR |= 0x00000100u;

    pTIMER14->PSC = 83;
    pTIMER14->ARR = 500;
    pTIMER14->CCR1 = 0xFFFFu;
    pTIMER14->SR &= (~0x0003u);
    pTIMER14->DIER|= 0x0001u;// interrupt enable
    pTIMER14->CR1 = 0x0005u;

    pNVIC->IP[45]  = 0x40u;
    pNVIC->ISER[1] |= 0x00002000u;

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: WatchDogInitial
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: Watchdog初始化函数
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 WatchDogInitial(void)
{
    Uint32 lv_ulong;

    pRCC->AHB1ENR |= 0x00000100u;

    lv_ulong = pGPIOI->MODER;
    lv_ulong &= 0xFFF3FFFFu;
    lv_ulong |= 0x00040000u;
    pGPIOI->MODER = lv_ulong;

    lv_ulong = pGPIOI->OTYPER;
    lv_ulong &= 0xFFFFFDFFu;
    lv_ulong |= 0x00000000u;
    pGPIOI->OTYPER = lv_ulong;

    lv_ulong = pGPIOI->OSPEEDR;
    lv_ulong &= 0xFFF3FFFFu;
    lv_ulong |= 0x000C0000u;
    pGPIOI->OSPEEDR = lv_ulong;

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: DisplayInitial
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Display inital function
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 DisplayInitial(void)
{
    Uint32 lv_ulong;

    // LEDs
    pRCC->AHB1ENR |= 0x00000004u;

    pGPIOC->BSRRH = 0x0002u;
    pGPIOC->BSRRL = 0x0004u;

    lv_ulong = pGPIOC->MODER;
    lv_ulong &= 0xFFFFFFC3u;
    lv_ulong |= 0x00000014u;
    pGPIOC->MODER = lv_ulong;

    lv_ulong = pGPIOC->OTYPER;
    lv_ulong &= 0xFFFFFFF9u;
    lv_ulong |= 0x00000006u;
    pGPIOC->OTYPER = lv_ulong;

    lv_ulong = pGPIOC->OSPEEDR;
    lv_ulong &= 0xFFFFFFC3u;
    lv_ulong |= 0x0000003Cu;
    pGPIOC->OSPEEDR = lv_ulong;

    // LCD
    pRCC->AHB1ENR |= 0x00000180u;

    pGPIOH->BSRRL = 0xE018u;

    lv_ulong = pGPIOH->MODER;
    lv_ulong &= 0x03FFFC3Fu;
    lv_ulong |= 0x54000140u;
    pGPIOH->MODER = lv_ulong;

    lv_ulong = pGPIOH->OTYPER;
    lv_ulong &= 0xFFFF1FE7u;
    lv_ulong |= 0x00000000u;
    pGPIOH->OTYPER = lv_ulong;

    lv_ulong = pGPIOH->OSPEEDR;
    lv_ulong &= 0x03FFFC3Fu;
    lv_ulong |= 0xFC0003C0u;
    pGPIOH->OSPEEDR = lv_ulong;

    lv_ulong = pGPIOI->MODER;
    lv_ulong &= 0xFFFF0000u;
    lv_ulong |= 0x00005555u;
    pGPIOI->MODER = lv_ulong;

    lv_ulong = pGPIOI->OTYPER;
    lv_ulong &= 0xFFFFFF00u;
    lv_ulong |= 0x00000000u;
    pGPIOI->OTYPER = lv_ulong;

    lv_ulong = pGPIOI->OSPEEDR;
    lv_ulong &= 0xFFFF0000u;
    lv_ulong |= 0x0000FFFFu;
    pGPIOI->OSPEEDR = lv_ulong;

    LcdControllerInitial();
    LcdDispGraphics(system_start_logo, sizeof(system_start_logo), 0, 0);

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: RtcInitial
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Display inital function
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 RtcInitial(void)
{
    Uint32 lv_ulong;
    Uint8 lv_byte;

    pRCC->AHB1ENR |= 0x00000002u;

    pGPIOB->BSRRH = 0x00C0u;
//    pGPIOB->BSRRL = 0x0080u;

    lv_ulong = pGPIOB->MODER;
    lv_ulong &= 0xFFFC0FFFu;
    lv_ulong |= 0x00005000u;
    pGPIOB->MODER = lv_ulong;

    lv_ulong = pGPIOB->OTYPER;
    lv_ulong &= 0xFFFFFE3Fu;
    lv_ulong |= 0x00000000u;
    pGPIOB->OTYPER = lv_ulong;

    lv_ulong = pGPIOB->OSPEEDR;
    lv_ulong &= 0xFFFC0FFFu;
    lv_ulong |= 0x0003F000u;
    pGPIOB->OSPEEDR = lv_ulong;

    RtcRead(0, &lv_byte, 1);
    if (0 != (lv_byte & 0x80u))
    {
        lv_byte &= 0x7Fu;
        RtcWrite(0, &lv_byte, 1);
    }

    RtcRead(2, &lv_byte, 1);
    if (0 != (lv_byte & 0x80u))
    {
        lv_byte &= 0x7Fu;
        RtcWrite(2, &lv_byte, 1);
    }

    lv_byte = 0x00;
    RtcWrite(7, &lv_byte, 1);

    lv_byte = 0xA5;
    RtcWrite(8, &lv_byte, 1);

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: InterruptInitial
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: interrupt inital function
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 InterruptInitial(void)
{
    Uint32 lv_ulong;

    pRCC->AHB1ENR |= 0x00000040u;
    pRCC->APB2ENR |= 0x00004000u;

    lv_ulong = pGPIOG->MODER;
    lv_ulong &= 0xFFFFCFFFu;
    lv_ulong |= 0x00000000u;
    pGPIOG->MODER = lv_ulong;

    lv_ulong = pGPIOG->OTYPER;
    lv_ulong &= 0xFFFFFFBFu;
    lv_ulong |= 0x00000040u;
    pGPIOG->OTYPER = lv_ulong;

    lv_ulong = pGPIOG->OSPEEDR;
    lv_ulong &= 0xFFFFCFFFu;
    lv_ulong |= 0x00003000u;
    pGPIOG->OSPEEDR = lv_ulong;

    lv_ulong = pGPIOG->PUPDR;
    lv_ulong &= 0xFFFFCFFFu;
    lv_ulong |= 0x00000000u;
    pGPIOG->PUPDR = lv_ulong;

    pSYSCFG->EXTICR[1] = 0x00000600u;
    pEXTI->RTSR = 0x00000040u;
    pEXTI->PR   = 0x00000040u;
    pNVIC->IP[23]  = 0x30u;
    pNVIC->ICPR[0] |= 0x00800000u;

    // interrupt service for level1 enable
    pNVIC->IP[71]  = 0x80u;
    pNVIC->ICPR[2] |= 0x00000080u;
    pNVIC->ISER[2] |= 0x00000080u;

    // interrupt service for level2 enable
    pNVIC->IP[70]  = 0x90u;
    pNVIC->ICPR[2] |= 0x00000040u;
    pNVIC->ISER[2] |= 0x00000040u;

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: CPUSPIFlashInitial
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: CPU Cfg SPI initial inital function
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 CPUSPIFlashInitial(void)
{
    Uint32 lv_ulong;

    pRCC->AHB1ENR |= 0x00000111u;
    pRCC->APB2ENR |= 0x00001000u;


    pGPIOA->BSRRL = 0x0010u;

    lv_ulong = pGPIOA->MODER;
    lv_ulong &= 0xFFFF00FFu;
    lv_ulong |= 0x0000A900u;
    pGPIOA->MODER = lv_ulong;

    lv_ulong = pGPIOA->OTYPER;
    lv_ulong &= 0xFFFFFF0Fu;
    lv_ulong |= 0x00000000u;
    pGPIOA->OTYPER = lv_ulong;

    lv_ulong = pGPIOA->OSPEEDR;
    lv_ulong &= 0xFFFF00FFu;
    lv_ulong |= 0x0000FF00u;
    pGPIOA->OSPEEDR = lv_ulong;

    lv_ulong = pGPIOA->AFRL;
    lv_ulong &= 0x000FFFFFu;
    lv_ulong |= 0x55500000u;
    pGPIOA->AFRL = lv_ulong;


    lv_ulong = pGPIOE->MODER;
    lv_ulong &= 0xFFFFF3FFu;
    lv_ulong |= 0x00000000u;
    pGPIOE->MODER = lv_ulong;

    lv_ulong = pGPIOE->OTYPER;
    lv_ulong &= 0xFFFFFFDFu;
    lv_ulong |= 0x00000000u;
    pGPIOE->OTYPER = lv_ulong;

    lv_ulong = pGPIOE->OSPEEDR;
    lv_ulong &= 0xFFFFF3FFu;
    lv_ulong |= 0x00000C00u;
    pGPIOE->OSPEEDR = lv_ulong;


    lv_ulong = pGPIOI->MODER;
    lv_ulong &= 0xFFFCFFFFu;
    lv_ulong |= 0x00000000u;
    pGPIOI->MODER = lv_ulong;

    lv_ulong = pGPIOI->OTYPER;
    lv_ulong &= 0xFFFFFEFFu;
    lv_ulong |= 0x00000000u;
    pGPIOI->OTYPER = lv_ulong;

    lv_ulong = pGPIOI->OSPEEDR;
    lv_ulong &= 0xFFFCFFFFu;
    lv_ulong |= 0x00000C00u;
    pGPIOI->OSPEEDR = lv_ulong;

    pSPI1->I2SCFGR = 0x0000u;
    pSPI1->CR2  = 0x0004u;
    pSPI1->CR1  = 0x030Cu;
    pSPI1->CR1 |= 0x0040u;

    return NORMAL_SUCCESS;
}



