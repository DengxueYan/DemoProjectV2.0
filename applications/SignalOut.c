/*****************************************************************************************************
* FileName:                    SignalOut.c
*
* Description:                 Output related functions
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

// 自定义头文件
#include "SystemBase.h"
#include "TaskServices.h"
#include "Uart.h"
#include "SignalOut.h"
#include "UserApp.h"

//====================================================================================================
// 本文件定义的与外部的接口变量
//====================================================================================================
SIGNAL_DEFINE const analog_define[] =
{
    {"test0",         &task0_max_run_time,                         SIGNAL_DATA_TYPE_uc},
};

SIGNAL_DEFINE const self_diagnose_define[] =
{
    {"system_running_status",   &system_running_status_disp,     SIGNAL_DATA_TYPE_uc},
    {"system_run_time_seconds", &system_s_count_disp,            SIGNAL_DATA_TYPE_ul},
    {"task0_max_run_time",      &task0_max_run_time,             SIGNAL_DATA_TYPE_ul},
    {"task0_run_time",          &task0_run_time,                 SIGNAL_DATA_TYPE_ul},
    {"task1_max_run_time",      &task1_max_run_time,             SIGNAL_DATA_TYPE_ul},
    {"task1_run_time",          &task1_run_time,                 SIGNAL_DATA_TYPE_ul},
    {"task1_interrupt_count",   &task1_interrupt_count,          SIGNAL_DATA_TYPE_ui},
    {"uart2_max_run_time",      &uart2_max_run_time,             SIGNAL_DATA_TYPE_ul},
    {"uart2_run_time",          &uart2_run_time,                 SIGNAL_DATA_TYPE_ul},
    {"uart4_max_run_time",      &uart4_max_run_time,             SIGNAL_DATA_TYPE_ul},
    {"uart4_run_time",          &uart4_run_time,                 SIGNAL_DATA_TYPE_ul},
    {"uart6_max_run_time",      &uart6_max_run_time,             SIGNAL_DATA_TYPE_ul},
    {"uart6_run_time",          &uart6_run_time,                 SIGNAL_DATA_TYPE_ul},

};

SIGNAL_BIN_DEFINE const self_diagnose_bin_define[] =
{
    {"test0",         &task0_max_run_time,                         0x40u},
};

SIGNAL_BIN_DEFINE const bin_input_define[] =
{
    {"test0",         &task0_max_run_time,                         0x40u},
};

SIGNAL_BIN_DEFINE const bin_output_define[] =
{
    {"test0",         &task0_max_run_time,                         0x40u},
};

int32 const analog_define_num        = sizeof(analog_define) / sizeof(analog_define[0]);
int32 const self_diagnose_define_num = sizeof(self_diagnose_define) / sizeof(self_diagnose_define[0]);
int32 const self_diagnose_bin_define_num = sizeof(self_diagnose_bin_define) / sizeof(self_diagnose_bin_define[0]);
int32 const bin_input_define_num     = sizeof(bin_input_define) / sizeof(bin_input_define[0]);
int32 const bin_output_define_num    = sizeof(bin_output_define) / sizeof(bin_output_define[0]);

