/*****************************************************************************************************
* FileName:                    UserApp.c
*
* Description:                 用户程序主程序
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 自定义头文件
#include "SystemBase.h"
#include "UserApp.h"
#include "Uart.h"
#include "TaskServices.h"
#include "SpiFlash.h"
#include "Storage.h"
#include "Display.h"

//====================================================================================================
// Macro defnie
//====================================================================================================
#define FPGA_BUFFER_RD_LENGTH 300
#define FPGA_BUFFER_WR_LENGTH 100
#define FPGA_BASE_WR_ADDR 0x60000000u;
#define FPGA_BASE_RD_ADDR 0x60002000u;

//====================================================================================================
// Local functions, these functions can not be accessed outside
//====================================================================================================

//====================================================================================================
// Outside functions reference
//====================================================================================================

//====================================================================================================
// Global variables
//====================================================================================================

//====================================================================================================
// Local variables, these variables can not be accessed outside
//====================================================================================================
static Uint8  setting_change_flag;

//====================================================================================================
// Functions realize
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// Interface functions realize
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: NewUserApp
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: 应用程序初始化函数
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 NewUserApp(void)
{
    if (NORMAL_SUCCESS != NewUart())
    {
        DebugPrintf("Uart create failed!\r\n");
        return NORMAL_ERROR;
    }
    DebugPrintf("Uart create successfully!\r\n");

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: UserApplicationInitial
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: 应用程序初始化函数
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 UserAppInitial(void)
{
    if (NORMAL_SUCCESS != UARTInitial())
    {
        DebugPrintf("Uart initial failed!\r\n");
        return NORMAL_ERROR;
    }
    DebugPrintf("Uart initial successfully!\r\n");

    return NORMAL_SUCCESS;
}

static Uint8 feed_watchdog_count;
void FeedWatchdog(void)
{
    feed_watchdog_count = (feed_watchdog_count + 1) & 1;
    if (0 == feed_watchdog_count)
    {
        pGPIOI->BSRRL = 0x0200u;
    }
    else
    {
        pGPIOI->BSRRH = 0x0200u;
    }
}

void SystemReboot(void)
{
    system_running_status |= CPU_RESET_FLAG;
    while (1);
}

//----------------------------------------------------------------------------------------------------
//   Function: UserApplicationSettingHandle
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: 应用程序参数处理文件
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 UserAppSettingHandle(void *addr, int32 set_num)
{
    if (NORMAL_SUCCESS != UARTSettingHandle())
    {
        DebugPrintf("Uart settings handle error!\r\n");
        return NORMAL_ERROR;
    }

    setting_change_flag = 1;

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: UpdateUserRunningSetting
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: 用户定值更新函数
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
void UpdateUserRunningSetting(void)
{
}

//----------------------------------------------------------------------------------------------------
//   Function: UserApplication
//      Input: void
//     Output: void
//     Return: void
//Description: 应用程序
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
void UserAppTask0Service(void)
{

    if (0 != setting_change_flag)
    {
        setting_change_flag = 0;
        UpdateUserRunningSetting();
    }

    UartTask0Service();
    DisplayTask0Service();
}

//----------------------------------------------------------------------------------------------------
//   Function: UserAppTask1Service
//      Input: Uint16 interrupt_no
//     Output: void
//     Return: void
//Description:
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
void UserAppTask1Service(Uint8 service_no)
{
    switch (service_no)
    {
        case 1:
        {
            UartTask1Service();
            break;
        }
        case 2:
        {
            break;
        }
        case 3:
        {
            DisplayTask1Service();
            break;
        }
        default:
        {
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
// Local functions realize
//----------------------------------------------------------------------------------------------------


