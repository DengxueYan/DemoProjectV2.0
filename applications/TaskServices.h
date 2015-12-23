/*****************************************************************************************************
* FileName     :    TaskServices.h
*
* Reference    :    NULL
*
* Description  :    Task0 header
*
* History      :
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Task_Services_H_
#define _Task_Services_H_

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// 外部变量声明
//====================================================================================================
extern VUint8 cpu_poweron_initial_flag;
extern VUint8 task1_service_request_no;
extern VUint8 task2_service_request_no;

extern Uint8  system_running_status_disp;
extern Uint32 system_s_count_disp;
extern Uint32 task0_max_run_time;
extern Uint32 task1_max_run_time;
extern Uint32 task2_run_time;
extern Uint32 task0_run_time;
extern Uint32 task1_run_time;
extern Uint32 task2_max_run_time;
extern Uint16 task1_interrupt_count;
extern Uint16 task2_interrupt_count;

//====================================================================================================
// 外部函数声明
//====================================================================================================
extern int32 LowLevelInterruptLock(void);
extern int32 LowLevelInterruptUnlock(void);

#ifdef __cplusplus
}
#endif

#endif

