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
    {"antenna0_sensor0_measure_T",                 &antenna[0].temperature_sensor[0].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna0_sensor0_measure_fft",               &antenna[0].temperature_sensor[0].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna0_sensor0_measure_dbm",               &antenna[0].temperature_sensor[0].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor0_measure_freq_tab_index",    &antenna[0].temperature_sensor[0].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor0_status",                    &antenna[0].temperature_sensor[0].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor1_measure_T",                 &antenna[0].temperature_sensor[1].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna0_sensor1_measure_fft",               &antenna[0].temperature_sensor[1].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna0_sensor1_measure_dbm",               &antenna[0].temperature_sensor[1].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor1_measure_freq_tab_index",    &antenna[0].temperature_sensor[1].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor1_status",                    &antenna[0].temperature_sensor[1].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor2_measure_T",                 &antenna[0].temperature_sensor[2].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna0_sensor2_measure_fft",               &antenna[0].temperature_sensor[2].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna0_sensor2_measure_dbm",               &antenna[0].temperature_sensor[2].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor2_measure_freq_tab_index",    &antenna[0].temperature_sensor[2].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor2_status",                    &antenna[0].temperature_sensor[2].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor3_measure_T",                 &antenna[0].temperature_sensor[3].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna0_sensor3_measure_fft",               &antenna[0].temperature_sensor[3].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna0_sensor3_measure_dbm",               &antenna[0].temperature_sensor[3].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor3_measure_freq_tab_index",    &antenna[0].temperature_sensor[3].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor3_status",                    &antenna[0].temperature_sensor[3].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor4_measure_T",                 &antenna[0].temperature_sensor[4].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna0_sensor4_measure_fft",               &antenna[0].temperature_sensor[4].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna0_sensor4_measure_dbm",               &antenna[0].temperature_sensor[4].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor4_measure_freq_tab_index",    &antenna[0].temperature_sensor[4].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor4_status",                    &antenna[0].temperature_sensor[4].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor5_measure_T",                 &antenna[0].temperature_sensor[5].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna0_sensor5_measure_fft",               &antenna[0].temperature_sensor[5].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna0_sensor5_measure_dbm",               &antenna[0].temperature_sensor[5].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor5_measure_freq_tab_index",    &antenna[0].temperature_sensor[5].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna0_sensor5_status",                    &antenna[0].temperature_sensor[5].measure_status,                       SIGNAL_DATA_TYPE_ui},
                                                                                                                           
    {"antenna1_sensor0_measure_T",                 &antenna[1].temperature_sensor[0].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna1_sensor0_measure_fft",               &antenna[1].temperature_sensor[0].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna1_sensor0_measure_dbm",               &antenna[1].temperature_sensor[0].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor0_measure_freq_tab_index",    &antenna[1].temperature_sensor[0].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor0_status",                    &antenna[1].temperature_sensor[0].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor1_measure_T",                 &antenna[1].temperature_sensor[1].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna1_sensor1_measure_fft",               &antenna[1].temperature_sensor[1].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna1_sensor1_measure_dbm",               &antenna[1].temperature_sensor[1].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor1_measure_freq_tab_index",    &antenna[1].temperature_sensor[1].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor1_status",                    &antenna[1].temperature_sensor[1].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor2_measure_T",                 &antenna[1].temperature_sensor[2].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna1_sensor2_measure_fft",               &antenna[1].temperature_sensor[2].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna1_sensor2_measure_dbm",               &antenna[1].temperature_sensor[2].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor2_measure_freq_tab_index",    &antenna[1].temperature_sensor[2].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor2_status",                    &antenna[1].temperature_sensor[2].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor3_measure_T",                 &antenna[1].temperature_sensor[3].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna1_sensor3_measure_fft",               &antenna[1].temperature_sensor[3].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna1_sensor3_measure_dbm",               &antenna[1].temperature_sensor[3].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor3_measure_freq_tab_index",    &antenna[1].temperature_sensor[3].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor3_status",                    &antenna[1].temperature_sensor[3].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor4_measure_T",                 &antenna[1].temperature_sensor[4].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna1_sensor4_measure_fft",               &antenna[1].temperature_sensor[4].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna1_sensor4_measure_dbm",               &antenna[1].temperature_sensor[4].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor4_measure_freq_tab_index",    &antenna[1].temperature_sensor[4].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor4_status",                    &antenna[1].temperature_sensor[4].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor5_measure_T",                 &antenna[1].temperature_sensor[5].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna1_sensor5_measure_fft",               &antenna[1].temperature_sensor[5].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna1_sensor5_measure_dbm",               &antenna[1].temperature_sensor[5].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor5_measure_freq_tab_index",    &antenna[1].temperature_sensor[5].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna1_sensor5_status",                    &antenna[1].temperature_sensor[5].measure_status,                       SIGNAL_DATA_TYPE_ui},
                                                                                                                           
    {"antenna2_sensor0_measure_T",                 &antenna[2].temperature_sensor[0].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna2_sensor0_measure_fft",               &antenna[2].temperature_sensor[0].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna2_sensor0_measure_dbm",               &antenna[2].temperature_sensor[0].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor0_measure_freq_tab_index",    &antenna[2].temperature_sensor[0].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor0_status",                    &antenna[2].temperature_sensor[0].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor1_measure_T",                 &antenna[2].temperature_sensor[1].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna2_sensor1_measure_fft",               &antenna[2].temperature_sensor[1].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna2_sensor1_measure_dbm",               &antenna[2].temperature_sensor[1].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor1_measure_freq_tab_index",    &antenna[2].temperature_sensor[1].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor1_status",                    &antenna[2].temperature_sensor[1].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor2_measure_T",                 &antenna[2].temperature_sensor[2].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna2_sensor2_measure_fft",               &antenna[2].temperature_sensor[2].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna2_sensor2_measure_dbm",               &antenna[2].temperature_sensor[2].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor2_measure_freq_tab_index",    &antenna[2].temperature_sensor[2].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor2_status",                    &antenna[2].temperature_sensor[2].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor3_measure_T",                 &antenna[2].temperature_sensor[3].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna2_sensor3_measure_fft",               &antenna[2].temperature_sensor[3].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna2_sensor3_measure_dbm",               &antenna[2].temperature_sensor[3].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor3_measure_freq_tab_index",    &antenna[2].temperature_sensor[3].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor3_status",                    &antenna[2].temperature_sensor[3].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor4_measure_T",                 &antenna[2].temperature_sensor[4].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna2_sensor4_measure_fft",               &antenna[2].temperature_sensor[4].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna2_sensor4_measure_dbm",               &antenna[2].temperature_sensor[4].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor4_measure_freq_tab_index",    &antenna[2].temperature_sensor[4].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor4_status",                    &antenna[2].temperature_sensor[4].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor5_measure_T",                 &antenna[2].temperature_sensor[5].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna2_sensor5_measure_fft",               &antenna[2].temperature_sensor[5].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna2_sensor5_measure_dbm",               &antenna[2].temperature_sensor[5].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor5_measure_freq_tab_index",    &antenna[2].temperature_sensor[5].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna2_sensor5_status",                    &antenna[2].temperature_sensor[5].measure_status,                       SIGNAL_DATA_TYPE_ui},
                                                                                                                           
    {"antenna3_sensor0_measure_T",                 &antenna[3].temperature_sensor[0].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna3_sensor0_measure_fft",               &antenna[3].temperature_sensor[0].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna3_sensor0_measure_dbm",               &antenna[3].temperature_sensor[0].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor0_measure_freq_tab_index",    &antenna[3].temperature_sensor[0].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor0_status",                    &antenna[3].temperature_sensor[0].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor1_measure_T",                 &antenna[3].temperature_sensor[1].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna3_sensor1_measure_fft",               &antenna[3].temperature_sensor[1].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna3_sensor1_measure_dbm",               &antenna[3].temperature_sensor[1].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor1_measure_freq_tab_index",    &antenna[3].temperature_sensor[1].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor1_status",                    &antenna[3].temperature_sensor[1].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor2_measure_T",                 &antenna[3].temperature_sensor[2].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna3_sensor2_measure_fft",               &antenna[3].temperature_sensor[2].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna3_sensor2_measure_dbm",               &antenna[3].temperature_sensor[2].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor2_measure_freq_tab_index",    &antenna[3].temperature_sensor[2].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor2_status",                    &antenna[3].temperature_sensor[2].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor3_measure_T",                 &antenna[3].temperature_sensor[3].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna3_sensor3_measure_fft",               &antenna[3].temperature_sensor[3].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna3_sensor3_measure_dbm",               &antenna[3].temperature_sensor[3].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor3_measure_freq_tab_index",    &antenna[3].temperature_sensor[3].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor3_status",                    &antenna[3].temperature_sensor[3].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor4_measure_T",                 &antenna[3].temperature_sensor[4].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna3_sensor4_measure_fft",               &antenna[3].temperature_sensor[4].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna3_sensor4_measure_dbm",               &antenna[3].temperature_sensor[4].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor4_measure_freq_tab_index",    &antenna[3].temperature_sensor[4].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor4_status",                    &antenna[3].temperature_sensor[4].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor5_measure_T",                 &antenna[3].temperature_sensor[5].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna3_sensor5_measure_fft",               &antenna[3].temperature_sensor[5].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna3_sensor5_measure_dbm",               &antenna[3].temperature_sensor[5].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor5_measure_freq_tab_index",    &antenna[3].temperature_sensor[5].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna3_sensor5_status",                    &antenna[3].temperature_sensor[5].measure_status,                       SIGNAL_DATA_TYPE_ui},

    {"antenna4_sensor0_measure_T",                 &antenna[4].temperature_sensor[0].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna4_sensor0_measure_fft",               &antenna[4].temperature_sensor[0].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna4_sensor0_measure_dbm",               &antenna[4].temperature_sensor[0].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor0_measure_freq_tab_index",    &antenna[4].temperature_sensor[0].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor0_status",                    &antenna[4].temperature_sensor[0].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor1_measure_T",                 &antenna[4].temperature_sensor[1].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna4_sensor1_measure_fft",               &antenna[4].temperature_sensor[1].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna4_sensor1_measure_dbm",               &antenna[4].temperature_sensor[1].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor1_measure_freq_tab_index",    &antenna[4].temperature_sensor[1].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor1_status",                    &antenna[4].temperature_sensor[1].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor2_measure_T",                 &antenna[4].temperature_sensor[2].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna4_sensor2_measure_fft",               &antenna[4].temperature_sensor[2].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna4_sensor2_measure_dbm",               &antenna[4].temperature_sensor[2].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor2_measure_freq_tab_index",    &antenna[4].temperature_sensor[2].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor2_status",                    &antenna[4].temperature_sensor[2].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor3_measure_T",                 &antenna[4].temperature_sensor[3].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna4_sensor3_measure_fft",               &antenna[4].temperature_sensor[3].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna4_sensor3_measure_dbm",               &antenna[4].temperature_sensor[3].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor3_measure_freq_tab_index",    &antenna[4].temperature_sensor[3].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor3_status",                    &antenna[4].temperature_sensor[3].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor4_measure_T",                 &antenna[4].temperature_sensor[4].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna4_sensor4_measure_fft",               &antenna[4].temperature_sensor[4].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna4_sensor4_measure_dbm",               &antenna[4].temperature_sensor[4].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor4_measure_freq_tab_index",    &antenna[4].temperature_sensor[4].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor4_status",                    &antenna[4].temperature_sensor[4].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor5_measure_T",                 &antenna[4].temperature_sensor[5].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna4_sensor5_measure_fft",               &antenna[4].temperature_sensor[5].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna4_sensor5_measure_dbm",               &antenna[4].temperature_sensor[5].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor5_measure_freq_tab_index",    &antenna[4].temperature_sensor[5].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna4_sensor5_status",                    &antenna[4].temperature_sensor[5].measure_status,                       SIGNAL_DATA_TYPE_ui},

    {"antenna5_sensor0_measure_T",                 &antenna[5].temperature_sensor[0].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna5_sensor0_measure_fft",               &antenna[5].temperature_sensor[0].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna5_sensor0_measure_dbm",               &antenna[5].temperature_sensor[0].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor0_measure_freq_tab_index",    &antenna[5].temperature_sensor[0].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor0_status",                    &antenna[5].temperature_sensor[0].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor1_measure_T",                 &antenna[5].temperature_sensor[1].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna5_sensor1_measure_fft",               &antenna[5].temperature_sensor[1].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna5_sensor1_measure_dbm",               &antenna[5].temperature_sensor[1].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor1_measure_freq_tab_index",    &antenna[5].temperature_sensor[1].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor1_status",                    &antenna[5].temperature_sensor[1].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor2_measure_T",                 &antenna[5].temperature_sensor[2].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna5_sensor2_measure_fft",               &antenna[5].temperature_sensor[2].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna5_sensor2_measure_dbm",               &antenna[5].temperature_sensor[2].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor2_measure_freq_tab_index",    &antenna[5].temperature_sensor[2].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor2_status",                    &antenna[5].temperature_sensor[2].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor3_measure_T",                 &antenna[5].temperature_sensor[3].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna5_sensor3_measure_fft",               &antenna[5].temperature_sensor[3].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna5_sensor3_measure_dbm",               &antenna[5].temperature_sensor[3].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor3_measure_freq_tab_index",    &antenna[5].temperature_sensor[3].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor3_status",                    &antenna[5].temperature_sensor[3].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor4_measure_T",                 &antenna[5].temperature_sensor[4].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna5_sensor4_measure_fft",               &antenna[5].temperature_sensor[4].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna5_sensor4_measure_dbm",               &antenna[5].temperature_sensor[4].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor4_measure_freq_tab_index",    &antenna[5].temperature_sensor[4].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor4_status",                    &antenna[5].temperature_sensor[4].measure_status,                       SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor5_measure_T",                 &antenna[5].temperature_sensor[5].measure_temperature_disp,             SIGNAL_DATA_TYPE_i},
    {"antenna5_sensor5_measure_fft",               &antenna[5].temperature_sensor[5].measure_fft,                          SIGNAL_DATA_TYPE_ul},
    {"antenna5_sensor5_measure_dbm",               &antenna[5].temperature_sensor[5].measure_dbm,                          SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor5_measure_freq_tab_index",    &antenna[5].temperature_sensor[5].measure_freq_tab_index_center_3db,    SIGNAL_DATA_TYPE_ui},
    {"antenna5_sensor5_status",                    &antenna[5].temperature_sensor[5].measure_status,                       SIGNAL_DATA_TYPE_ui},
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

    {"antenna0_sensor0_rx_attenuator",            &antenna[0].temperature_sensor[0].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor0_tx_attenuator",            &antenna[0].temperature_sensor[0].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor1_rx_attenuator",            &antenna[0].temperature_sensor[1].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor1_tx_attenuator",            &antenna[0].temperature_sensor[1].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor2_rx_attenuator",            &antenna[0].temperature_sensor[2].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor2_tx_attenuator",            &antenna[0].temperature_sensor[2].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor3_rx_attenuator",            &antenna[0].temperature_sensor[3].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor3_tx_attenuator",            &antenna[0].temperature_sensor[3].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor4_rx_attenuator",            &antenna[0].temperature_sensor[4].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor4_tx_attenuator",            &antenna[0].temperature_sensor[4].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor5_rx_attenuator",            &antenna[0].temperature_sensor[5].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna0_sensor5_tx_attenuator",            &antenna[0].temperature_sensor[5].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    
    {"antenna1_sensor0_rx_attenuator",            &antenna[1].temperature_sensor[0].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor0_tx_attenuator",            &antenna[1].temperature_sensor[0].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor1_rx_attenuator",            &antenna[1].temperature_sensor[1].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor1_tx_attenuator",            &antenna[1].temperature_sensor[1].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor2_rx_attenuator",            &antenna[1].temperature_sensor[2].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor2_tx_attenuator",            &antenna[1].temperature_sensor[2].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor3_rx_attenuator",            &antenna[1].temperature_sensor[3].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor3_tx_attenuator",            &antenna[1].temperature_sensor[3].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor4_rx_attenuator",            &antenna[1].temperature_sensor[4].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor4_tx_attenuator",            &antenna[1].temperature_sensor[4].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor5_rx_attenuator",            &antenna[1].temperature_sensor[5].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna1_sensor5_tx_attenuator",            &antenna[1].temperature_sensor[5].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    
    {"antenna2_sensor0_rx_attenuator",            &antenna[2].temperature_sensor[0].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor0_tx_attenuator",            &antenna[2].temperature_sensor[0].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor1_rx_attenuator",            &antenna[2].temperature_sensor[1].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor1_tx_attenuator",            &antenna[2].temperature_sensor[1].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor2_rx_attenuator",            &antenna[2].temperature_sensor[2].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor2_tx_attenuator",            &antenna[2].temperature_sensor[2].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor3_rx_attenuator",            &antenna[2].temperature_sensor[3].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor3_tx_attenuator",            &antenna[2].temperature_sensor[3].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor4_rx_attenuator",            &antenna[2].temperature_sensor[4].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor4_tx_attenuator",            &antenna[2].temperature_sensor[4].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor5_rx_attenuator",            &antenna[2].temperature_sensor[5].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna2_sensor5_tx_attenuator",            &antenna[2].temperature_sensor[5].tx_attenuator,    SIGNAL_DATA_TYPE_uc},

    {"antenna3_sensor0_rx_attenuator",            &antenna[3].temperature_sensor[0].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor0_tx_attenuator",            &antenna[3].temperature_sensor[0].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor1_rx_attenuator",            &antenna[3].temperature_sensor[1].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor1_tx_attenuator",            &antenna[3].temperature_sensor[1].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor2_rx_attenuator",            &antenna[3].temperature_sensor[2].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor2_tx_attenuator",            &antenna[3].temperature_sensor[2].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor3_rx_attenuator",            &antenna[3].temperature_sensor[3].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor3_tx_attenuator",            &antenna[3].temperature_sensor[3].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor4_rx_attenuator",            &antenna[3].temperature_sensor[4].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor4_tx_attenuator",            &antenna[3].temperature_sensor[4].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor5_rx_attenuator",            &antenna[3].temperature_sensor[5].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna3_sensor5_tx_attenuator",            &antenna[3].temperature_sensor[5].tx_attenuator,    SIGNAL_DATA_TYPE_uc},

    {"antenna4_sensor0_rx_attenuator",            &antenna[4].temperature_sensor[0].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor0_tx_attenuator",            &antenna[4].temperature_sensor[0].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor1_rx_attenuator",            &antenna[4].temperature_sensor[1].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor1_tx_attenuator",            &antenna[4].temperature_sensor[1].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor2_rx_attenuator",            &antenna[4].temperature_sensor[2].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor2_tx_attenuator",            &antenna[4].temperature_sensor[2].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor3_rx_attenuator",            &antenna[4].temperature_sensor[3].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor3_tx_attenuator",            &antenna[4].temperature_sensor[3].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor4_rx_attenuator",            &antenna[4].temperature_sensor[4].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor4_tx_attenuator",            &antenna[4].temperature_sensor[4].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor5_rx_attenuator",            &antenna[4].temperature_sensor[5].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna4_sensor5_tx_attenuator",            &antenna[4].temperature_sensor[5].tx_attenuator,    SIGNAL_DATA_TYPE_uc},

    {"antenna5_sensor0_rx_attenuator",            &antenna[5].temperature_sensor[0].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor0_tx_attenuator",            &antenna[5].temperature_sensor[0].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor1_rx_attenuator",            &antenna[5].temperature_sensor[1].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor1_tx_attenuator",            &antenna[5].temperature_sensor[1].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor2_rx_attenuator",            &antenna[5].temperature_sensor[2].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor2_tx_attenuator",            &antenna[5].temperature_sensor[2].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor3_rx_attenuator",            &antenna[5].temperature_sensor[3].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor3_tx_attenuator",            &antenna[5].temperature_sensor[3].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor4_rx_attenuator",            &antenna[5].temperature_sensor[4].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor4_tx_attenuator",            &antenna[5].temperature_sensor[4].tx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor5_rx_attenuator",            &antenna[5].temperature_sensor[5].rx_attenuator,    SIGNAL_DATA_TYPE_uc},
    {"antenna5_sensor5_tx_attenuator",            &antenna[5].temperature_sensor[5].tx_attenuator,    SIGNAL_DATA_TYPE_uc},

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

