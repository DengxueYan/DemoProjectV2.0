/*****************************************************************************************************
* FileName:                    Startup.c
*
* Description:                 调试相关程序
*
* Author:                      YanDengxue, SmartSaw
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
//====================================================================================================
// Header files declaration
//====================================================================================================
// Lib headers
#include <stdio.h>

#include "stm32f4xx.h"

// Self-defined headers
#include "FileTypesDef.h"
#include "SystemBase.h"
#include "TaskServices.h"
#include "Hardware.h"
#include "STM32F40xRegister.h"
#include "Debug.h"
#include "App.h"

static int32 RoutineServices(void);

//====================================================================================================
// Functions realize
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: StartUp
//      Input: void
//     Output: void
//     Return: int32: 函数执行情况
//Description: 系统主函数入口
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 Startup(void)
{
    // 初始化硬件
    if (NORMAL_SUCCESS != HardwareInitial())
    {
        DebugPrintf("System hardware initial failed!");
        return RoutineServices();
    }

    DebugPrintf("Hardware ready!\r\n");
    DebugPrintf("#---------------------------------------------------------------------------\r\n");
    DebugPrintf("# System Compile Time : %s %s\r\n", __DATE__, __TIME__);
    DebugPrintf("# System Author       : Dengxue Yan\r\n");
    DebugPrintf("#---------------------------------------------------------------------------\r\n");

    if (NORMAL_SUCCESS != NewApp())
    {
        DebugPrintf("Application NEW failed!\r\n");
        return RoutineServices();
    }

    if (NORMAL_SUCCESS != ParseDevConfig())
    {
        DebugPrintf("R&D Confing parse failed!\r\n");
        return RoutineServices();
    }
    DebugPrintf("R&D Confing ready!\r\n");

    if (NORMAL_SUCCESS != AppInitial())
    {
        DebugPrintf("Application NEW failed!\r\n");
        return RoutineServices();
    }

    DebugPrintf("Application ready!\r\n");
    DebugPrintf("\r\n");
    DebugPrintf("Enter user application!\r\n\r\n");

    pGPIOB->BSRRH = 0x0200u;
    while (0 == cpu_poweron_initial_flag);
    system_running_status |= CPU_RUNNING_FLAG;

    pEXTI->PR  = 0x00000040u;
    pNVIC->ICPR[0] |= 0x00800000u;
    pNVIC->ISER[0] |= 0x00800000u;
    pEXTI->IMR = 0x00000040u;

    return RoutineServices();
}

//----------------------------------------------------------------------------------------------------
// Local functions realize
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: RoutineServices
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: routine funciton
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 RoutineServices(void)
{
    int32 i;

    DebugPrintf("[App@Dengxue Yan] => ");

    i = 1;
    while (0 != i)
    {
        HmiService();
    }

    return NORMAL_SUCCESS;
}

/*@}*/
