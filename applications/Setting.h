/*****************************************************************************************************
* FileName:                    Setting.h
*
* Description:                 Setting pub header
*                              
* Author:                      YanDengxue, Fiberhome-Fuhua
*                              
* Rev History:  
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Setting_H
#define _Setting_H

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
extern SIGNAL_DEFINE const setting_system_define[];
extern int32 const setting_system_define_num;

#ifdef __cplusplus
}
#endif

#endif

