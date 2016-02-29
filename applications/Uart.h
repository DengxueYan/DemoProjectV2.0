/*****************************************************************************************************
* FileName:                    Uart.h
*
* Description:                 Uart communication related functions
*
* Author:                      YanDengxue, SmartSaw
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Uart_H
#define _Uart_H

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"
#include "STM32F40xRegister.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// Macro define
//====================================================================================================
#define UART_RX_BUF_SIZE         512u// 串口接收缓存长度
#define UART_RX_BUF_MASK         (UART_RX_BUF_SIZE - 1u)// 串口接收缓存长度MASK
#define UART_RX_BUF_ADDR(addr)   ((Uint16)((addr) & (UART_RX_BUF_MASK)))// 串口接收缓存长度有效寻址

#define UART_TX_BUF_SIZE         512u// 串口发送缓存长度
#define UART_TX_BUF_MASK         (UART_TX_BUF_SIZE - 1u)// 串口发送缓存长度MASK
#define UART_TX_BUF_ADDR(addr)   ((Uint16)((addr) & (UART_TX_BUF_MASK)))// 串口发送缓存长度有效寻址

#define UART_FRAME_BUFFER_SIZE         256u
#define UART_FRAME_BUFFER_MASK         (UART_FRAME_BUFFER_SIZE - 1u)// 串口接收缓存长度MASK
#define UART_FRAME_BUFFER_ADDR(addr)   ((Uint16)((addr) & (UART_FRAME_BUFFER_MASK)))// 串口接收缓存长度有效寻址

#define COMM_STATUS_FAILED              0u
#define COMM_STATUS_SUCCESS             1u

#define COMM_FRAME_STATUS_INITIAL       0u
#define COMM_FRAME_STATUS_WAIT_FRAME_COMPLETE 1u
#define COMM_FRAME_STATUS_FRAME_COMPLETE      2u
#define COMM_TIMEOUT_MS                 2000
#define UART_RX_MIN_TIMEOUT_MS          5
#define UART_RX_MAX_TIMEOUT_MS          600000

#define UART_RX_2_TX_SWITCH_WAIT_DOTS   8u// Wait for the master to switch from TX to RX
#define UART_TX_2_RX_SWITCH_WAIT_DOTS   16u
#define UART_ST_RX              0u
#define UART_ST_TX_READY        1u
#define UART_ST_TX              (UART_ST_TX_READY + UART_RX_2_TX_SWITCH_WAIT_DOTS)
#define UART_ST_TX_WAIT_FINISH  (UART_ST_TX + 1u)
#define UART_ST_TX_FINISH       (UART_ST_TX_WAIT_FINISH + UART_TX_2_RX_SWITCH_WAIT_DOTS)

//====================================================================================================
// Types define
//====================================================================================================
typedef struct UART_DEF
{
    UART_REGS *regs;
    VUint8  status;

    Uint16  comm_addr;//
    Uint8   comm_mode;// 0: RS485 1: RS232
    Uint8   comm_protocol;// 0: IEC103 1: Modbus
    Uint8   comm_status;
    Uint8   comm_frame_status;
    Uint32  comm_frame_rx_time;

    Uint8   parity;// 0: no parity, 1: Odd, 2: Even
    Uint8   stop_bits;
    Uint8   baud_rate;

    VUint16 rx_buffer_wr_ptr;
    VUint16 rx_buffer_rd_ptr;
    Uint32  rx_timeout_ms;
    int8    *rx_buffer;
    int8    *tx_buffer;

    Uint16  rx_frame_length;
    Uint16  tx_frame_length;
    Uint8   *rx_frame_buffer;
    Uint8   *tx_frame_buffer;
    Uint32  temp[8];

} UART_DEF;

//====================================================================================================
// Interface variables
//====================================================================================================
extern UART_DEF uart2;
extern Uint32 uart2_max_run_time;
extern Uint32 uart2_run_time;
extern UART_DEF uart4;
extern Uint32 uart4_max_run_time;
extern Uint32 uart4_run_time;
extern UART_DEF uart6;
extern Uint32 uart6_max_run_time;
extern Uint32 uart6_run_time;

//====================================================================================================
// Interface functions
//====================================================================================================
extern void  UartTask0Service(void);
extern int32 UartTask1Service(void);
extern int32 NewUart(void);
extern int32 UARTInitial(void);
extern int32 UARTSettingHandle(void);

#ifdef __cplusplus
}
#endif

#endif

