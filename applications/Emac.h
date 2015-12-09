/*****************************************************************************************************
* FileName:                    Emac.h
*
* Description:                 Emac相关定义
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Emac_H
#define _Emac_H

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// Refer outside header file
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"

//====================================================================================================
// 外部变量声明
//====================================================================================================
extern EMAC_DESCRIPTOR_REGS *p_emac_rx_desc_head;
extern EMAC_DESCRIPTOR_REGS *p_emac_rx_desc_tail;
extern Vint8 emac_queue_active;

#ifdef __cplusplus
}
#endif

#endif// _System_Base_H_

