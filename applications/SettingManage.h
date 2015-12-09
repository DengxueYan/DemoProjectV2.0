/*****************************************************************************************************
* FileName:                    SettingManage.h
*
* Description:                 定值管理头文件
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Setting_Manage_H
#define _Setting_Manage_H

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"
#include "BaseData.h"
#include "Setting.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// 结构声明
//====================================================================================================
typedef struct
{
    SIGNAL_DEFINE const *define;
    Uint16  position;
    Uint32  invisible;
    SIGNAL_DATA default_value;
    SIGNAL_DATA min_value;
    SIGNAL_DATA max_value;
    char   unit[UINT_LENGTH_MAX];// if SIGNAL_DATA_TYPE_b, unit[0] is the position of the bit in the varible
    char   const *desc;
    char   const *desc_utf8;
    char   const *remark;
} SETTING;

typedef struct
{
    SETTING *setting;
    int8   need_save_flag;
    Uint8  run_section_no;
    int32  num;
    Uint32 flash_addr;
    Uint32 flash_addr_backup;
    int32  flash_length;
    Uint8  *run_buffer;
    Uint8  *write_buffer;
    Uint8  *history_buffer;
} SETTING_GROUP;

//====================================================================================================
// 外部变量声明
//====================================================================================================
extern SETTING_GROUP setting_system;

//====================================================================================================
// 外部函数声明
//====================================================================================================
extern int32 SettingReadFromFlash(SETTING_GROUP const *p_setting_group);
extern int32 SettingWriteToFlash(SETTING_GROUP const *p_setting_group);
extern int32 RunSettingUpdate(SETTING_GROUP const *p_setting_group);
extern int32 RunSettingUpdateFromWriteSetting(SETTING_GROUP const *p_setting_group);
extern int32 GetValueOfKeyWord(char const *keyword, char const *src, int32 type, SIGNAL_DATA *p_data);
extern int32 EepromSettingInitial(SETTING_GROUP *setting_group, int32 flash_max_length);
extern int32 SettingValueValidationCheck(SETTING_GROUP const *p_setting_group, SIGNAL_DATA value, int32 index);
extern int32 SetSettingBufferFromSettingAddr(SETTING_GROUP const *p_setting_group);
extern int32 SetOneSettingBufferFromSettingAddr(SETTING_GROUP const *p_setting_group, int32 index);
extern int32 SetOneWriteSettingValue(SETTING_GROUP const *p_setting_group, SIGNAL_DATA value, int32 index);
extern int32 SetOneWriteSettingStringValue(SETTING_GROUP const *p_setting_group, char const *p_str, int32 index);
extern int32 CheckRunSettingWithFlash(SETTING_GROUP *p_setting_group);

#ifdef __cplusplus
}
#endif

#endif


