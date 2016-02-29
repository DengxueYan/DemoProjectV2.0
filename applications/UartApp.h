/*****************************************************************************************************
* FileName:                    UartApp.h
*
* Description:                 Uart communication related functions
*
* Author:                      Dengxue Yan
*
* Rev History:
*       <Author>        <Date>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _UartApp_H
#define _UartApp_H

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
// Interface functions
//====================================================================================================
extern void IEC103FrameProcess(UART_DEF *p_uart);
extern void UartDefaultFrameProcess(UART_DEF *p_uart);
extern int32 UART6HardwareIntial(UART_DEF *p_uart);

#ifdef __cplusplus
}
#endif

#endif


