/*****************************************************************************************************
* FileName:                    UartApp.c
*
* Description:                 Uart communication
*
* Author:                      Dengxue Yan
*
* Rev History:
*       <Author>        <Date>        <Hardware>     <Version>        <Description>
*     Dengxue Yan   2011-03-21 15:30       --           1.00             Create
*****************************************************************************************************/
//====================================================================================================
// Header file declaration
//====================================================================================================
// Lib header
#include <string.h>
#include <stdlib.h>

// Self-define header
#include "SystemBase.h"
#include "TaskServices.h"
#include "Uart.h"
#include "UartApp.h"
#include "UserApp.h"
#include "SettingManage.h"
#include "SignalOutManage.h"

//====================================================================================================
// Functions realize
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: IEC103FrameProcess
//      Input: UART_DEF *p_uart
//     Output: void
//     Return: void
//Description: IEC103 frame process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan    2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
void IEC103FrameProcess(UART_DEF *p_uart)
{
    
}

//----------------------------------------------------------------------------------------------------
//   Function: UartDefaultFrameProcess
//      Input: void
//     Output: void
//     Return: void
//Description: Uart Default frame process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan    2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
void UartDefaultFrameProcess(UART_DEF *p_uart)
{
    Uint32 lv_index;
    
    if (0 != p_uart->rx_frame_length)
    {
        DebugPrintf("RX:\r\n");
        for (lv_index = 0; lv_index < p_uart->rx_frame_length; lv_index++)
        {
            DebugPrintf("%02X ", p_uart->rx_frame_buffer[lv_index]);
        }
        DebugPrintf("\r\n");
    }
    
}


//----------------------------------------------------------------------------------------------------
//   Function: UART6HardwareIntial
//      Input: UART_DEF *p_uart
//     Output: void
//     Return: int32: function status
//Description: Uart6 hardware initial
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   Dengxue Yan    2016-02-28 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 UART6HardwareIntial(UART_DEF *p_uart)
{
    return NORMAL_SUCCESS;
}
