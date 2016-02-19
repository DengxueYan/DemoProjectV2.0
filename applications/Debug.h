/*****************************************************************************************************
* FileName:                    Debug.h
*
* Description:                 调试用定义
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Debug_H
#define _Debug_H

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// Refer outside header file
//====================================================================================================
#include "UserTypesDef.h"
#include "stdio.h"

//====================================================================================================
// structure define
//====================================================================================================

//====================================================================================================
// 宏定义
//====================================================================================================
#define pDEBUG pUART1

//#define DEBUG_PORT 2

//#if (0 == DEBUG_PORT)
//#define pDEBUG pUART0
//#elif (1 == DEBUG_PORT)
//#define pDEBUG pUART1
//#else
//#define pDEBUG pUART2
//#endif

#define XMODEM_SOH 0x01u
#define XMODEM_EOT 0x04u
#define XMODEM_ACK 0x06u
#define XMODEM_NAK 0x15u
#define XMODEM_CAN 0x18u

#define DebugPrintf printf
#define TRACE DebugPrintf

#define DEBUG_UART_DESIRED_BAUD   (115200u)
#define DEBUG_UART_OSM_SEL   0
#if (0 == DEBUG_UART_OSM_SEL)
#define DEBUG_UART_OVERSAMPLE_CNT (16u)
#elif (1 == DEBUG_UART_OSM_SEL)
#define DEBUG_UART_OVERSAMPLE_CNT (13u)
#else
#error "DEBUG_UART_OSM_SEL difine error!"
#endif

#define DEBUG_FRAME_BUFFER_SIZE       512u
#define DEBUG_FRAME_BUFFER_MASK         (DEBUG_FRAME_BUFFER_SIZE - 1u)// 串口接收缓存长度MASK
#define DEBUG_FRAME_BUFFER_ADDR(addr)   ((Uint16)((addr) & (DEBUG_FRAME_BUFFER_MASK)))// 串口接收缓存长度有效寻址

#define DEBUG_RX_BUF_SIZE         512u// 串口接收缓存长度
#define DEBUG_RX_BUF_MASK         (DEBUG_RX_BUF_SIZE - 1u)// 串口接收缓存长度MASK
#define DEBUG_RX_BUF_ADDR(addr)   ((Uint16)((addr) & (DEBUG_RX_BUF_MASK)))// 串口接收缓存长度有效寻址

//====================================================================================================
// 外部接口变量
//====================================================================================================
extern VUint16 debug_rx_buffer_rd_ptr;
extern VUint16 debug_rx_buffer_wr_ptr;
extern int8 const *debug_rx_buffer;

//====================================================================================================
// 外部函数声明
//====================================================================================================
extern int32 DebugSendByte(int8 byte);
extern int32 DebugSendByteWithDelay(int8 byte, Uint32 delay_us);
extern void HmiService(void);

#ifdef __cplusplus
}
#endif

#endif

