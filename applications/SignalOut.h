/*****************************************************************************************************
* FileName:                    SignalOut.h
*
* Description:                 SignalOut pub header
*                              
* Author:                      YanDengxue, Fiberhome-Fuhua
*                              
* Rev History:  
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Signal_Out_H
#define _Signal_Out_H

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"
#include "BaseData.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// 外部变量声明
//====================================================================================================
extern SIGNAL_DEFINE const analog_define[];
extern SIGNAL_DEFINE const self_diagnose_define[];
extern SIGNAL_BIN_DEFINE const self_diagnose_bin_define[];
extern SIGNAL_BIN_DEFINE const bin_input_define[];
extern SIGNAL_BIN_DEFINE const bin_output_define[];
extern int32 const analog_define_num;
extern int32 const self_diagnose_define_num;
extern int32 const self_diagnose_bin_define_num;
extern int32 const bin_input_define_num;
extern int32 const bin_output_define_num;

#ifdef __cplusplus
}
#endif

#endif


