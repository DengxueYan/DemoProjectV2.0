/*****************************************************************************************************
* FileName:                    UserApp.h
*
* Description:                 �û�����ͷ�ļ�
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _User_App_H
#define _User_App_H

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// �궨��
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// Other
//----------------------------------------------------------------------------------------------------
#define FLASH_CALIBRATE_PARAMETER_START_ADDR  0x00010000u
#define FLASH_CALIBRATE_PARAMETER_SPACE_LENTH 0x00004000u

#define LED_FLASH_LEVEL1_COUNT_MAX  100
#define LED_FLASH_LEVEL2_COUNT_MAX  1000
#define LED_FLASH_LEVEL3_COUNT_MAX  4000

//====================================================================================================
// Types define
//====================================================================================================

//====================================================================================================
// �ⲿ��������
//====================================================================================================
extern Uint32 measure_delay;

//====================================================================================================
// �ⲿ��������
//====================================================================================================
extern int32 NewUserApp(void);
extern int32 UserAppInitial(void);
extern void  UserAppTask0Service(void);
extern void  UserAppTask1Service(Uint8 service_no);
extern void  UserAppTask2Service(Uint8 service_no);
extern int32 UserAppSettingHandle(void *addr, int32 set_num);

#ifdef __cplusplus
}
#endif

#endif
