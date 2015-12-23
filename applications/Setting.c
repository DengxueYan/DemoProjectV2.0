/*****************************************************************************************************
* FileName:                    Setting.c
*
* Description:                 定值文件
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
// 自定义头文件
#include "STM32F40xRegister.h"
#include "SystemBase.h"
#include "Setting.h"
#include "Uart.h"
#include "UserApp.h"

//====================================================================================================
// 本文件定义的与外部的接口变量
//====================================================================================================
SIGNAL_DEFINE const setting_system_define[] =
{
    {"uart2_comm_protocol",             &uart2.comm_protocol,                 SIGNAL_DATA_TYPE_uc},
    {"uart2_comm_addr",                 &uart2.comm_addr,                     SIGNAL_DATA_TYPE_ui},
    {"uart2_baud_rate",                 &uart2.baud_rate,                     SIGNAL_DATA_TYPE_uc},
    {"uart2_parity",                    &uart2.parity,                        SIGNAL_DATA_TYPE_uc},
    {"uart2_stop_bits",                 &uart2.stop_bits,                     SIGNAL_DATA_TYPE_uc},
    {"uart2_rx_timeout_ms",             &uart2.rx_timeout_ms,                 SIGNAL_DATA_TYPE_ul},
    {"uart2_comm_mode",                 &uart2.comm_mode,                     SIGNAL_DATA_TYPE_uc},

    {"uart4_comm_protocol",             &uart4.comm_protocol,                 SIGNAL_DATA_TYPE_uc},
    {"uart4_comm_addr",                 &uart4.comm_addr,                     SIGNAL_DATA_TYPE_ui},
    {"uart4_baud_rate",                 &uart4.baud_rate,                     SIGNAL_DATA_TYPE_uc},
    {"uart4_parity",                    &uart4.parity,                        SIGNAL_DATA_TYPE_uc},
    {"uart4_stop_bits",                 &uart4.stop_bits,                     SIGNAL_DATA_TYPE_uc},
    {"uart4_rx_timeout_ms",             &uart4.rx_timeout_ms,                 SIGNAL_DATA_TYPE_ul},
    {"uart4_comm_mode",                 &uart4.comm_mode,                     SIGNAL_DATA_TYPE_uc},
};

int32 const setting_system_define_num = sizeof(setting_system_define) / sizeof(setting_system_define[0]);

