/*****************************************************************************************************
* FileName:                    SettingManage.c
*
* Description:                 定值管理文件
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
#include "STM32F40xRegister.h"
#include "BaseData.h"
#include "SystemBase.h"
#include "SettingManage.h"
#include "Storage.h"

//====================================================================================================
// Macro
//====================================================================================================

//====================================================================================================
// Local functions, these functions can not be accessed outside
//====================================================================================================
static int32 SettingToDefault(SETTING_GROUP const *p_setting_group);
static int32 SettingBackup(SETTING_GROUP const *p_setting_group);
static int32 RestoreSettingFromBackup(SETTING_GROUP const *p_setting_group);
static int32 BackupSettingCheck(SETTING_GROUP const *p_setting_group);
static int8  setting_op_flag;

//====================================================================================================
// 本文件定义的与外部的接口变量
//====================================================================================================
SETTING_GROUP setting_system;

//====================================================================================================
// 函数实现
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// 接口函数
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: SettingReadFromFlash
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 从flash中读出定值
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 SettingReadFromFlash(SETTING_GROUP const *p_setting_group)
{
    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot read settings from flash!\r\n");
        return NORMAL_ERROR;
    }

    StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->run_buffer, p_setting_group->flash_length);
    if (0 != DefaultCrcCal(p_setting_group->run_buffer, p_setting_group->flash_length, 0xA5A5u))
    {
        TRACE("Active setting section check crc failed! Try backup!\r\n");
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup, p_setting_group->run_buffer, p_setting_group->flash_length);
        if (0 != DefaultCrcCal(p_setting_group->run_buffer, p_setting_group->flash_length, 0xA5A5u))
        {
            TRACE("Backup setting check crc also failed!\r\n");
            return NORMAL_ERROR;
        }
        else
        {
            if (NORMAL_SUCCESS != RestoreSettingFromBackup(p_setting_group))
            {
                TRACE("Restore settings from backup failed!\r\n");
                return NORMAL_ERROR;
            }
            TRACE("Successfully restore settings from backup!\r\n");
        }
    }

    if (NORMAL_SUCCESS != RunSettingUpdate(p_setting_group))
    {
        TRACE("Run setting update error!\r\n");
        return NORMAL_ERROR;
    }

    BackupSettingCheck(p_setting_group);
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: SettingWriteToFlash
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 将定值写入flash
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 SettingWriteToFlash(SETTING_GROUP const *p_setting_group)
{
    int32  lv_index;
    Uint16 lv_crc;
    Uint8 lv_ubyte;
    Uint8 lv_ubyte_backup;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (0 != setting_op_flag)
    {
        DebugPrintf("Storage is busy!\r\n");
        return RET_STORAGE_BUSY;
    }
    setting_op_flag = 0;

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot save!\r\n");
        setting_op_flag = 0;
        return NORMAL_ERROR;
    }
    StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->history_buffer, p_setting_group->flash_length);

    lv_crc = DefaultCrcCal(p_setting_group->write_buffer, p_setting_group->flash_length - 2, 0xA5A5u);
    p_setting_group->write_buffer[p_setting_group->flash_length - 2] = LLSB(lv_crc);
    p_setting_group->write_buffer[p_setting_group->flash_length - 1] = LHSB(lv_crc);

    StorageErase(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->flash_length);
    StorageWrite(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->write_buffer, p_setting_group->flash_length);
    StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->run_buffer, p_setting_group->flash_length);
    for (lv_index = 0; lv_index < p_setting_group->flash_length; lv_index++)
    {
        if (p_setting_group->run_buffer[lv_index] != p_setting_group->write_buffer[lv_index])
        {
            TRACE("parameter write error!\r\n");
            setting_op_flag = 0;
            return NORMAL_ERROR;
        }
    }

    // Backup
    StorageErase(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup, p_setting_group->flash_length);
    StorageWrite(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup, p_setting_group->write_buffer, p_setting_group->flash_length);
    StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup, p_setting_group->run_buffer, p_setting_group->flash_length);

    for (lv_index = 0; lv_index < p_setting_group->flash_length; lv_index++)
    {
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr + lv_index, &lv_ubyte, 1);
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup + lv_index, &lv_ubyte_backup, 1);
        if (lv_ubyte != lv_ubyte_backup)
        {
            TRACE("Settings backup failed! Backup byte at addr 0x%08X is 0x%02X, but byte in the active section at addr 0x%08X is 0x%02X.\r\n"
            , p_setting_group->flash_addr_backup + lv_index
            , lv_ubyte_backup
            , p_setting_group->flash_addr + lv_index
            , lv_ubyte);
            setting_op_flag = 0;
            return NORMAL_ERROR;
        }
    }

    RunSettingUpdate(p_setting_group);

    setting_op_flag = 0;
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: RunSettingUpdate
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 运行定值更新
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 RunSettingUpdate(SETTING_GROUP const *p_setting_group)
{
    int32  i;
    Uint8 const *lv_p_stream;
    int32 lv_position;
    SETTING const *lv_p_setting;
    SIGNAL_DATA lv_signal_data;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot update run settings!\r\n");
        return NORMAL_ERROR;
    }

    lv_p_stream = p_setting_group->run_buffer;
    lv_signal_data.ul = 0;
    for (i = 0; i < p_setting_group->num; i++)
    {
        lv_p_setting = &p_setting_group->setting[i];
        lv_position = lv_p_setting->position;
        switch (lv_p_setting->define->type)
        {
            //字符串定长SET_VAL_LEN_MAX字节
            case SIGNAL_DATA_TYPE_b:
            case SIGNAL_DATA_TYPE_c:
            {
                lv_signal_data.c =   lv_p_stream[lv_position];
                if ((lv_signal_data.c > lv_p_setting->max_value.c) || (lv_signal_data.c < lv_p_setting->min_value.c))
                {
                    TRACE("setting value=%d, exceed scope [%d,%d]\r\n", lv_signal_data.c, lv_p_setting->min_value.c, lv_p_setting->max_value.c);
                    return NORMAL_ERROR;
                }
                *((char *)lv_p_setting->define->addr) = lv_signal_data.c;
                break;
            }
            case SIGNAL_DATA_TYPE_uc:
            {
                lv_signal_data.uc =   lv_p_stream[lv_position];
                if ((lv_signal_data.uc > lv_p_setting->max_value.uc) || (lv_signal_data.uc < lv_p_setting->min_value.uc))
                {
                    TRACE("setting value=%u, exceed scope [%u,%u]\r\n", lv_signal_data.uc, lv_p_setting->min_value.uc, lv_p_setting->max_value.uc);
                    return NORMAL_ERROR;
                }
                *((Uint8 *)lv_p_setting->define->addr) = lv_signal_data.uc;
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            {
                lv_signal_data.i =   lv_p_stream[lv_position]
                                    | (lv_p_stream[lv_position + 1] << 8);
                if ((lv_signal_data.i > lv_p_setting->max_value.i) || (lv_signal_data.i < lv_p_setting->min_value.i))
                {
                    TRACE("setting value=%d, exceed scope [%d,%d]\r\n", lv_signal_data.i, lv_p_setting->min_value.i, lv_p_setting->max_value.i);
                    return NORMAL_ERROR;
                }
                *((int16 *)lv_p_setting->define->addr) = lv_signal_data.i;
                break;
            }
            case SIGNAL_DATA_TYPE_ui:
            {
                lv_signal_data.ui =   lv_p_stream[lv_position]
                                     | (lv_p_stream[lv_position + 1] << 8);
                if ((lv_signal_data.ui > lv_p_setting->max_value.ui) || (lv_signal_data.ui < lv_p_setting->min_value.ui))
                {
                    TRACE("setting value=%u, exceed scope [%u,%u]\r\n", lv_signal_data.ui, lv_p_setting->min_value.ui, lv_p_setting->max_value.ui);
                    return NORMAL_ERROR;
                }
                *((Uint16 *)lv_p_setting->define->addr) = lv_signal_data.ui;
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            {
                lv_signal_data.l =   lv_p_stream[lv_position]
                                    | (lv_p_stream[lv_position + 1] << 8)
                                    | (lv_p_stream[lv_position + 2] << 16)
                                    | (lv_p_stream[lv_position + 3] << 24);
                if ((lv_signal_data.l > lv_p_setting->max_value.l) || (lv_signal_data.l < lv_p_setting->min_value.l))
                {
                    TRACE("setting value=%d, exceed scope [%d,%d]\r\n", lv_signal_data.l, lv_p_setting->min_value.l, lv_p_setting->max_value.l);
                    return NORMAL_ERROR;
                }
                *((int32 *)lv_p_setting->define->addr) = lv_signal_data.l;
                break;
            }
            case SIGNAL_DATA_TYPE_ul:
            {
                lv_signal_data.ul =   lv_p_stream[lv_position]
                                     | (lv_p_stream[lv_position + 1] << 8)
                                     | (lv_p_stream[lv_position + 2] << 16)
                                     | (lv_p_stream[lv_position + 3] << 24);

                if ((lv_signal_data.ul > lv_p_setting->max_value.ul) || (lv_signal_data.ul < lv_p_setting->min_value.ul))
                {
                    TRACE("setting value=%u, exceed scope [%u,%u]\r\n", lv_signal_data.ul, lv_p_setting->min_value.ul, lv_p_setting->max_value.ul);
                    return NORMAL_ERROR;
                }
                *((Uint32 *)lv_p_setting->define->addr) = lv_signal_data.ul;
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                lv_signal_data.ul =   lv_p_stream[lv_position]
                                     | (lv_p_stream[lv_position + 1] << 8)
                                     | (lv_p_stream[lv_position + 2] << 16)
                                     | (lv_p_stream[lv_position + 3] << 24);
                if ((lv_signal_data.f > lv_p_setting->max_value.f) || (lv_signal_data.f < lv_p_setting->min_value.f))
                {
                    TRACE("setting value=%f, exceed scope [%f,%f]\r\n", lv_signal_data.f, lv_p_setting->min_value.f, lv_p_setting->max_value.f);
                    return NORMAL_ERROR;
                }
                *((float *)lv_p_setting->define->addr) = lv_signal_data.f;
                break;
            }
            //两字节长度定值
            default:
            {
                TRACE("unknow type=%d\r\n", lv_p_setting->define->type);
                return NORMAL_ERROR;
            }
        }
    }
    memcpy(p_setting_group->write_buffer, lv_p_stream, p_setting_group->flash_length);

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: RunSettingUpdateFromWriteSetting
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 将定值写入flash
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 RunSettingUpdateFromWriteSetting(SETTING_GROUP const *p_setting_group)
{
    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot update run settings!\r\n");
        return NORMAL_ERROR;
    }

    memcpy(p_setting_group->run_buffer, p_setting_group->write_buffer, p_setting_group->flash_length);

    return RunSettingUpdate(p_setting_group);
}

//----------------------------------------------------------------------------------------------------
//   Function: GetValueOfKeyWord
//      Input: char const *keyword: 关键字
//             char const *src: 待转换字符串
//             int32 type: 转换类型
//             SIGNAL_DATA: 数据输出
//     Output: void
//     Return: int32: 函数执行情况
//Description: 运行定值更新
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 GetValueOfKeyWord(char const *keyword, char const *src, int32 type, SIGNAL_DATA *p_data)
{
    int32  lv_long_temp;
    char  lv_buffer[64];

    if ((NULL == keyword) || (NULL == src) || (NULL == p_data))
    {
        TRACE("function entry error\r\n");
        return NORMAL_ERROR;
    }

    if (NORMAL_SUCCESS != GetContentOfKeyword(keyword, src, lv_buffer, 64))
    {
        return NORMAL_ERROR;
    }

    switch (type)
    {
        case SIGNAL_DATA_TYPE_b :
        {
            lv_long_temp = strtol(lv_buffer, NULL, 0);
            if ((lv_long_temp < 0) || (lv_long_temp > 1))
            {
                TRACE("type=b scope is [0,1], but here is %d\r\n", lv_long_temp);
                return NORMAL_ERROR;
            }
            p_data->c = lv_long_temp;
            break;
        }
        case SIGNAL_DATA_TYPE_c :
        {
            lv_long_temp = strtol(lv_buffer, NULL, 0);
            if ((lv_long_temp < -128) || (lv_long_temp > 127))
            {
                TRACE("type=c scope is [-128,127], but here is %d\r\n", lv_long_temp);
                return NORMAL_ERROR;
            }
            p_data->c = lv_long_temp;
            break;
        }
        case SIGNAL_DATA_TYPE_uc :
        {
            lv_long_temp = strtol(lv_buffer, NULL, 0);
            if ((lv_long_temp < 0) || (lv_long_temp > 255))
            {
                TRACE("type=uc scope is [0,255], but here is %u\r\n", lv_long_temp);
                return NORMAL_ERROR;
            }
            p_data->uc = lv_long_temp;
            break;
        }
        case SIGNAL_DATA_TYPE_i :
        {
            lv_long_temp = strtol(lv_buffer, NULL,0);
            if ((lv_long_temp < -32768) || (lv_long_temp > 32767))
            {
                TRACE("type=i scope is [-32768,32767], but here is %d\r\n", lv_long_temp);
                return NORMAL_ERROR;
            }
            p_data->i = lv_long_temp;
            break;
        }
        case SIGNAL_DATA_TYPE_ui :
        {
            lv_long_temp = strtol(lv_buffer, NULL, 0);
            if ((lv_long_temp < 0) || (lv_long_temp > 65535))
            {
                TRACE("type=ui scope is [0,65535], but here is %u\r\n", lv_long_temp);
                return NORMAL_ERROR;
            }
            p_data->ui = lv_long_temp;
            break;
        }
        case SIGNAL_DATA_TYPE_l :
        {
            p_data->l = strtol(lv_buffer, NULL, 0);
            break;
        }
        case SIGNAL_DATA_TYPE_ul :
        {
            p_data->ul = strtoul(lv_buffer, NULL, 0);
            break;
        }
        case SIGNAL_DATA_TYPE_f :
        {
            p_data->f = strtod(lv_buffer, NULL);
            break;
        }
        default :
        {
            TRACE("unknow type=%d\r\n", type);
            return NORMAL_ERROR;
        }
    }
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: EepromSettingSpaceAlloc
//      Input: SETTING_GROUP *setting_group
//     Output: SETTING_GROUP *setting_group
//     Return: int32: 函数执行情况
//Description: 定值存储位置初始化
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2010-01-07 10:30           Create
//----------------------------------------------------------------------------------------------------
int32 EepromSettingInitial(SETTING_GROUP *p_setting_group, int32 flash_max_length)
{
    int32  lv_index;
    int32  lv_current_position;
    SETTING *lv_p_setting;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    // 定位定值在EEPROM中的字位置和位偏移位置
    lv_current_position = 4;
    for (lv_index = 0; lv_index < p_setting_group->num; lv_index++)
    {
        lv_p_setting = &p_setting_group->setting[lv_index];
        lv_p_setting->position = lv_current_position;
        switch (lv_p_setting->define->type)
        {
            //字符串定长SET_VAL_LEN_MAX字节
            case SIGNAL_DATA_TYPE_b:
            case SIGNAL_DATA_TYPE_c:
            case SIGNAL_DATA_TYPE_uc:
            {
                lv_current_position += 1;
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            case SIGNAL_DATA_TYPE_ui:
            {
                lv_current_position += 2;
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            case SIGNAL_DATA_TYPE_ul:
            {
                lv_current_position += 4;
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                lv_current_position += 4;
                break;
            }
            //两字节长度定值
            default:
            {
                TRACE("unknow type=%d", lv_p_setting->define->type);
                return NORMAL_ERROR;
            }
        }
    }

    lv_current_position += 2;
    if (lv_current_position > flash_max_length)
    {
        TRACE("Overall settings length, which is %d, at flash addr 0x%08X exceed the max allowed length %d!\r\n"
        , lv_current_position
        , p_setting_group->flash_addr
        , flash_max_length);
        return NORMAL_ERROR;
    }
    p_setting_group->flash_length = lv_current_position;

    p_setting_group->run_buffer = CallocLowSpeedMem(1, lv_current_position);
    if (NULL == p_setting_group->run_buffer)
    {
        TRACE("setting run buffer CallocLowSpeedMem failed");
        return NORMAL_ERROR;
    }

    p_setting_group->write_buffer = CallocLowSpeedMem(1, lv_current_position);
    if (NULL == p_setting_group->write_buffer)
    {
        TRACE("setting write buffer CallocLowSpeedMem failed\r\n");
        return NORMAL_ERROR;
    }

    p_setting_group->history_buffer = CallocLowSpeedMem(1, lv_current_position);
    if (NULL == p_setting_group->history_buffer)
    {
        TRACE("setting history buffer CallocLowSpeedMem failed\r\n");
        return NORMAL_ERROR;
    }

    if (NORMAL_SUCCESS != SettingToDefault(p_setting_group))
    {
        TRACE("setting to default value failed\r\n");
        return NORMAL_ERROR;
    }

    p_setting_group->write_buffer[0] = lv_current_position;
    p_setting_group->write_buffer[1] = (lv_current_position >> 8);
    p_setting_group->write_buffer[2] = (lv_current_position >> 16);
    p_setting_group->write_buffer[3] = FILE_TYPE_SETTING;

    p_setting_group->run_buffer[0] = lv_current_position;
    p_setting_group->run_buffer[1] = (lv_current_position >> 8);
    p_setting_group->run_buffer[2] = (lv_current_position >> 16);
    p_setting_group->run_buffer[3] = FILE_TYPE_SETTING;

    StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->history_buffer, p_setting_group->flash_length);

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: SetOneWriteSettingValueCheck
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 运行定值更新
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 SettingValueValidationCheck(SETTING_GROUP const *p_setting_group, SIGNAL_DATA value, int32 index)
{
    SETTING const *lv_p_setting;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (index >= p_setting_group->num)
    {
        TRACE("setting[%d] exceed max setting num %d\r\n", index, p_setting_group->num);
        return NORMAL_ERROR;
    }

    lv_p_setting = &p_setting_group->setting[index];
    switch (lv_p_setting->define->type)
    {
        //字符串定长SET_VAL_LEN_MAX字节
        case SIGNAL_DATA_TYPE_b:
        case SIGNAL_DATA_TYPE_c:
        {
            if ((value.c > lv_p_setting->max_value.c) || (value.c < lv_p_setting->min_value.c))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.c, lv_p_setting->min_value.c, lv_p_setting->max_value.c);
                return NORMAL_ERROR;
            }
            break;
        }
        case SIGNAL_DATA_TYPE_uc:
        {
            if ((value.uc > lv_p_setting->max_value.uc) || (value.uc < lv_p_setting->min_value.uc))
            {
                TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", index, value.uc, lv_p_setting->min_value.uc, lv_p_setting->max_value.uc);
                return NORMAL_ERROR;
            }
            break;
        }
        case SIGNAL_DATA_TYPE_i:
        {
            if ((value.i > lv_p_setting->max_value.i) || (value.i < lv_p_setting->min_value.i))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.i, lv_p_setting->min_value.i, lv_p_setting->max_value.i);
                return NORMAL_ERROR;
            }
            break;
        }
        case SIGNAL_DATA_TYPE_ui:
        {
            if ((value.ui > lv_p_setting->max_value.ui) || (value.ui < lv_p_setting->min_value.ui))
            {
                TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", index, value.ui, lv_p_setting->min_value.ui, lv_p_setting->max_value.ui);
                return NORMAL_ERROR;
            }
            break;
        }
        case SIGNAL_DATA_TYPE_l:
        {
            if ((value.l > lv_p_setting->max_value.l) || (value.l < lv_p_setting->min_value.l))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.l, lv_p_setting->min_value.l, lv_p_setting->max_value.l);
                return NORMAL_ERROR;
            }
            break;
        }
        case SIGNAL_DATA_TYPE_ul:
        {
            if ((value.ul > lv_p_setting->max_value.ul) || (value.ul < lv_p_setting->min_value.ul))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.ul, lv_p_setting->min_value.ul, lv_p_setting->max_value.ul);
                return NORMAL_ERROR;
            }
            break;
        }
        case SIGNAL_DATA_TYPE_f:
        {
            if ((value.f > lv_p_setting->max_value.f) || (value.f < lv_p_setting->min_value.f))
            {
                TRACE("setting[%d] value=%f, exceed scope [%f,%f]\r\n", index, value.f, lv_p_setting->min_value.f, lv_p_setting->max_value.f);
                return NORMAL_ERROR;
            }
            break;
        }
        //两字节长度定值
        default:
        {
            TRACE("unknow type=%d", lv_p_setting->define->type);
            return NORMAL_ERROR;
        }
    }

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: SetOneSettingBufferFromSettingAddr
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 运行定值更新
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 SetSettingBufferFromSettingAddr(SETTING_GROUP const *p_setting_group)
{
    Uint8 *lv_p_stream;
    int32  lv_position;
    int32  lv_index;
    SETTING const *lv_p_setting;
    SIGNAL_DATA lv_value;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot save settings!\r\n");
        return NORMAL_ERROR;
    }

    lv_p_stream = p_setting_group->write_buffer;
    for (lv_index = 0; lv_index < p_setting_group->num; lv_index++)
    {
        lv_p_setting = &p_setting_group->setting[lv_index];
        lv_position = lv_p_setting->position;
        switch (lv_p_setting->define->type)
        {
            //字符串定长SET_VAL_LEN_MAX字节
            case SIGNAL_DATA_TYPE_b:
            case SIGNAL_DATA_TYPE_c:
            {
                lv_value.c = *(int8 *)lv_p_setting->define->addr;
                if ((lv_value.c > lv_p_setting->max_value.c) || (lv_value.c < lv_p_setting->min_value.c))
                {
                    TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", lv_index, lv_value.c, lv_p_setting->min_value.c, lv_p_setting->max_value.c);
                    return NORMAL_ERROR;
                }

                lv_p_stream[lv_position] = lv_value.c;
                break;
            }
            case SIGNAL_DATA_TYPE_uc:
            {
                lv_value.uc = *(Uint8 *)lv_p_setting->define->addr;
                if ((lv_value.uc > lv_p_setting->max_value.uc) || (lv_value.uc < lv_p_setting->min_value.uc))
                {
                    TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", lv_index, lv_value.uc, lv_p_setting->min_value.uc, lv_p_setting->max_value.uc);
                    return NORMAL_ERROR;
                }

                lv_p_stream[lv_position] = lv_value.uc;
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            {
                lv_value.i = *(int16 *)lv_p_setting->define->addr;
                if ((lv_value.i > lv_p_setting->max_value.i) || (lv_value.i < lv_p_setting->min_value.i))
                {
                    TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", lv_index, lv_value.i, lv_p_setting->min_value.i, lv_p_setting->max_value.i);
                    return NORMAL_ERROR;
                }

                lv_p_stream[lv_position] = LLSB(lv_value.i);
                lv_p_stream[lv_position + 1] = LHSB(lv_value.i);
                break;
            }
            case SIGNAL_DATA_TYPE_ui:
            {
                lv_value.ui = *(Uint16 *)lv_p_setting->define->addr;
                if ((lv_value.ui > lv_p_setting->max_value.ui) || (lv_value.ui < lv_p_setting->min_value.ui))
                {
                    TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", lv_index, lv_value.ui, lv_p_setting->min_value.ui, lv_p_setting->max_value.ui);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = LLSB(lv_value.ui);
                lv_p_stream[lv_position + 1] = LHSB(lv_value.ui);
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            {
                lv_value.l = *(int32 *)lv_p_setting->define->addr;
                if ((lv_value.l > lv_p_setting->max_value.l) || (lv_value.l < lv_p_setting->min_value.l))
                {
                    TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", lv_index, lv_value.l, lv_p_setting->min_value.l, lv_p_setting->max_value.l);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = LLSB(lv_value.l);
                lv_p_stream[lv_position + 1] = LHSB(lv_value.l);
                lv_p_stream[lv_position + 2] = HLSB(lv_value.l);
                lv_p_stream[lv_position + 3] = HHSB(lv_value.l);
                break;
            }
            case SIGNAL_DATA_TYPE_ul:
            {
                lv_value.ul = *(int32 *)lv_p_setting->define->addr;
                if ((lv_value.ul > lv_p_setting->max_value.ul) || (lv_value.ul < lv_p_setting->min_value.ul))
                {
                    TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", lv_index, lv_value.ul, lv_p_setting->min_value.ul, lv_p_setting->max_value.ul);
                    return NORMAL_ERROR;
                }

                lv_p_stream[lv_position] = LLSB(lv_value.ul);
                lv_p_stream[lv_position + 1] = LHSB(lv_value.ul);
                lv_p_stream[lv_position + 2] = HLSB(lv_value.ul);
                lv_p_stream[lv_position + 3] = HHSB(lv_value.ul);
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                lv_value.f = *(float *)lv_p_setting->define->addr;
                if ((lv_value.f > lv_p_setting->max_value.f) || (lv_value.f < lv_p_setting->min_value.f))
                {
                    TRACE("setting[%d] value=%f, exceed scope [%f,%f]\r\n", lv_index, lv_value.f, lv_p_setting->min_value.f, lv_p_setting->max_value.f);
                    return NORMAL_ERROR;
                }

                lv_p_stream[lv_position] = LLSB(lv_value.l);
                lv_p_stream[lv_position + 1] = LHSB(lv_value.l);
                lv_p_stream[lv_position + 2] = HLSB(lv_value.l);
                lv_p_stream[lv_position + 3] = HHSB(lv_value.l);
                break;
            }
            //两字节长度定值
            default:
            {
                TRACE("unknow type=%d", lv_p_setting->define->type);
                return NORMAL_ERROR;
            }
        }
    }
    memcpy(p_setting_group->run_buffer, p_setting_group->write_buffer, p_setting_group->flash_length);

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: SetOneWriteSettingValue
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 运行定值更新
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 SetOneWriteSettingValue(SETTING_GROUP const *p_setting_group, SIGNAL_DATA value, int32 index)
{
    Uint8 *lv_p_stream;
    int32  lv_position;
    SETTING const *lv_p_setting;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot save settings!\r\n");
        return NORMAL_ERROR;
    }

    if (index >= p_setting_group->num)
    {
        TRACE("setting[%d] exceed max setting num %d\r\n", index, p_setting_group->num);
        return NORMAL_ERROR;
    }

    lv_p_stream = p_setting_group->write_buffer;
    lv_p_setting = &p_setting_group->setting[index];
    lv_position = lv_p_setting->position;
    switch (lv_p_setting->define->type)
    {
        //字符串定长SET_VAL_LEN_MAX字节
        case SIGNAL_DATA_TYPE_b:
        case SIGNAL_DATA_TYPE_c:
        {
            if ((value.c > lv_p_setting->max_value.c) || (value.c < lv_p_setting->min_value.c))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.c, lv_p_setting->min_value.c, lv_p_setting->max_value.c);
                return NORMAL_ERROR;
            }

            lv_p_stream[lv_position] = value.c;
            break;
        }
        case SIGNAL_DATA_TYPE_uc:
        {
            if ((value.uc > lv_p_setting->max_value.uc) || (value.uc < lv_p_setting->min_value.uc))
            {
                TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", index, value.uc, lv_p_setting->min_value.uc, lv_p_setting->max_value.uc);
                return NORMAL_ERROR;
            }

            lv_p_stream[lv_position] = value.uc;
            break;
        }
        case SIGNAL_DATA_TYPE_i:
        {
            if ((value.i > lv_p_setting->max_value.i) || (value.i < lv_p_setting->min_value.i))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.i, lv_p_setting->min_value.i, lv_p_setting->max_value.i);
                return NORMAL_ERROR;
            }

            lv_p_stream[lv_position] = LLSB(value.i);
            lv_p_stream[lv_position + 1] = LHSB(value.i);
            break;
        }
        case SIGNAL_DATA_TYPE_ui:
        {
            if ((value.ui > lv_p_setting->max_value.ui) || (value.ui < lv_p_setting->min_value.ui))
            {
                TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", index, value.ui, lv_p_setting->min_value.ui, lv_p_setting->max_value.ui);
                return NORMAL_ERROR;
            }
            lv_p_stream[lv_position] = LLSB(value.ui);
            lv_p_stream[lv_position + 1] = LHSB(value.ui);
            break;
        }
        case SIGNAL_DATA_TYPE_l:
        {
            if ((value.l > lv_p_setting->max_value.l) || (value.l < lv_p_setting->min_value.l))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.l, lv_p_setting->min_value.l, lv_p_setting->max_value.l);
                return NORMAL_ERROR;
            }

            lv_p_stream[lv_position] = LLSB(value.l);
            lv_p_stream[lv_position + 1] = LHSB(value.l);
            lv_p_stream[lv_position + 2] = HLSB(value.l);
            lv_p_stream[lv_position + 3] = HHSB(value.l);
            break;
        }
        case SIGNAL_DATA_TYPE_ul:
        {
            if ((value.ul > lv_p_setting->max_value.ul) || (value.ul < lv_p_setting->min_value.ul))
            {
                TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, value.ul, lv_p_setting->min_value.ul, lv_p_setting->max_value.ul);
                return NORMAL_ERROR;
            }

            lv_p_stream[lv_position] = LLSB(value.ul);
            lv_p_stream[lv_position + 1] = LHSB(value.ul);
            lv_p_stream[lv_position + 2] = HLSB(value.ul);
            lv_p_stream[lv_position + 3] = HHSB(value.ul);
            break;
        }
        case SIGNAL_DATA_TYPE_f:
        {
            if ((value.f > lv_p_setting->max_value.f) || (value.f < lv_p_setting->min_value.f))
            {
                TRACE("setting[%d] value=%f, exceed scope [%f,%f]\r\n", index, value.f, lv_p_setting->min_value.f, lv_p_setting->max_value.f);
                return NORMAL_ERROR;
            }

            lv_p_stream[lv_position] = LLSB(value.l);
            lv_p_stream[lv_position + 1] = LHSB(value.l);
            lv_p_stream[lv_position + 2] = HLSB(value.l);
            lv_p_stream[lv_position + 3] = HHSB(value.l);
            break;
        }
        //两字节长度定值
        default:
        {
            TRACE("unknow type=%d", lv_p_setting->define->type);
            return NORMAL_ERROR;
        }
    }

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: SetOneWriteSettingStringValue
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 运行定值更新
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 SetOneWriteSettingStringValue(SETTING_GROUP const *p_setting_group, char const *p_str, int32 index)
{
    int32  lv_long_temp;
    Uint8 *lv_p_stream;
    int32  lv_position;
    int8   lv_default_flag;
    SIGNAL_DATA lv_signal_data;
    SETTING const *lv_p_setting;

    if ((NULL == p_setting_group) || (NULL == p_str))
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot save settings!\r\n");
        return NORMAL_ERROR;
    }

    if (index >= p_setting_group->num)
    {
        TRACE("setting index=%d exceed max setting num %d\r\n", index, p_setting_group->num);
        return NORMAL_ERROR;
    }

    if (0 == strcmp(p_str, "default"))
    {
        lv_default_flag = 1;
    }
    else
    {
        lv_default_flag = 0;
    }

    lv_p_stream = p_setting_group->write_buffer;
    lv_p_setting = &p_setting_group->setting[index];
    lv_position = lv_p_setting->position;
    lv_signal_data.l = 0;
    switch (lv_p_setting->define->type)
    {
        //字符串定长SET_VAL_LEN_MAX字节
        case SIGNAL_DATA_TYPE_b:
        case SIGNAL_DATA_TYPE_c:
        {
            if (0 == lv_default_flag)
            {
                lv_long_temp = strtol(p_str, NULL, 0);
                if ((lv_long_temp > lv_p_setting->max_value.c) || (lv_long_temp < lv_p_setting->min_value.c))
                {
                    TRACE("set[%d] value=%d, exceed scope [%d,%d]\r\n", index, lv_long_temp, lv_p_setting->min_value.c, lv_p_setting->max_value.c);
                    return NORMAL_ERROR;
                }
                lv_signal_data.c = lv_long_temp;
            }
            else
            {
                lv_signal_data.c = lv_p_setting->default_value.c;
            }
            lv_p_stream[lv_position] = lv_signal_data.c;
            break;
        }
        case SIGNAL_DATA_TYPE_uc:
        {
            if (0 == lv_default_flag)
            {
                lv_long_temp = strtol(p_str, NULL, 0);
                if ((lv_long_temp > lv_p_setting->max_value.uc) || (lv_long_temp < lv_p_setting->min_value.uc))
                {
                    TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", index, lv_long_temp, lv_p_setting->min_value.uc, lv_p_setting->max_value.uc);
                    return NORMAL_ERROR;
                }
                lv_signal_data.uc = lv_long_temp;
            }
            else
            {
                lv_signal_data.uc = lv_p_setting->default_value.uc;
            }
            lv_p_stream[lv_position] = lv_signal_data.uc;
            break;
        }
        case SIGNAL_DATA_TYPE_i:
        {
            if (0 == lv_default_flag)
            {
                lv_long_temp = strtol(p_str, NULL, 0);
                if ((lv_long_temp > lv_p_setting->max_value.i) || (lv_long_temp < lv_p_setting->min_value.i))
                {
                    TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, lv_long_temp, lv_p_setting->min_value.i, lv_p_setting->max_value.i);
                    return NORMAL_ERROR;
                }
                lv_signal_data.i = lv_long_temp;
            }
            else
            {
                lv_signal_data.i = lv_p_setting->default_value.i;
            }
            lv_p_stream[lv_position] = LLSB(lv_signal_data.i);
            lv_p_stream[lv_position + 1] = LHSB(lv_signal_data.i);
            break;
        }
        case SIGNAL_DATA_TYPE_ui:
        {
            if (0 == lv_default_flag)
            {
                lv_long_temp = strtol(p_str, NULL, 0);
                if ((lv_long_temp > lv_p_setting->max_value.ui) || (lv_long_temp < lv_p_setting->min_value.ui))
                {
                    TRACE("setting[%d] value=%u, exceed scope [%u,%u]\r\n", index, lv_long_temp, lv_p_setting->min_value.ui, lv_p_setting->max_value.ui);
                    return NORMAL_ERROR;
                }
                lv_signal_data.ui = lv_long_temp;
            }
            else
            {
                lv_signal_data.ui = lv_p_setting->default_value.ui;
            }
            lv_p_stream[lv_position] = LLSB(lv_signal_data.ui);
            lv_p_stream[lv_position + 1] = LHSB(lv_signal_data.ui);
            break;
        }
        case SIGNAL_DATA_TYPE_l:
        {
            if (0 == lv_default_flag)
            {
                lv_signal_data.l = strtol(p_str, NULL, 0);
                if ((lv_signal_data.l > lv_p_setting->max_value.l) || (lv_signal_data.l < lv_p_setting->min_value.l))
                {
                    TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, lv_signal_data.l, lv_p_setting->min_value.l, lv_p_setting->max_value.l);
                    return NORMAL_ERROR;
                }
            }
            else
            {
                lv_signal_data.l = lv_p_setting->default_value.l;
            }
            lv_p_stream[lv_position] = LLSB(lv_signal_data.l);
            lv_p_stream[lv_position + 1] = LHSB(lv_signal_data.l);
            lv_p_stream[lv_position + 2] = HLSB(lv_signal_data.l);
            lv_p_stream[lv_position + 3] = HHSB(lv_signal_data.l);
            break;
        }
        case SIGNAL_DATA_TYPE_ul:
        {
            if (0 == lv_default_flag)
            {
                lv_signal_data.ul = strtoul(p_str, NULL, 0);
                if ((lv_signal_data.ul > lv_p_setting->max_value.ul) || (lv_signal_data.ul < lv_p_setting->min_value.ul))
                {
                    TRACE("setting[%d] value=%d, exceed scope [%d,%d]\r\n", index, lv_signal_data.ul, lv_p_setting->min_value.ul, lv_p_setting->max_value.ul);
                    return NORMAL_ERROR;
                }
            }
            else
            {
                lv_signal_data.ul = lv_p_setting->default_value.ul;
            }
            lv_p_stream[lv_position] = LLSB(lv_signal_data.ul);
            lv_p_stream[lv_position + 1] = LHSB(lv_signal_data.ul);
            lv_p_stream[lv_position + 2] = HLSB(lv_signal_data.ul);
            lv_p_stream[lv_position + 3] = HHSB(lv_signal_data.ul);
            break;
        }
        case SIGNAL_DATA_TYPE_f:
        {
            if (0 == lv_default_flag)
            {
                lv_signal_data.f = strtod(p_str, NULL);
                if ((lv_signal_data.f > lv_p_setting->max_value.f) || (lv_signal_data.f < lv_p_setting->min_value.f))
                {
                    TRACE("setting[%d] value=%f, exceed scope [%f,%f]\r\n", index, lv_signal_data.f, lv_p_setting->min_value.f, lv_p_setting->max_value.f);
                    return NORMAL_ERROR;
                }
            }
            else
            {
                lv_signal_data.f = lv_p_setting->default_value.f;
            }
            lv_p_stream[lv_position] = LLSB(lv_signal_data.l);
            lv_p_stream[lv_position + 1] = LHSB(lv_signal_data.l);
            lv_p_stream[lv_position + 2] = HLSB(lv_signal_data.l);
            lv_p_stream[lv_position + 3] = HHSB(lv_signal_data.l);
            break;
        }
        //两字节长度定值
        default:
        {
            TRACE("unknow type=%d", lv_p_setting->define->type);
            return NORMAL_ERROR;
        }
    }

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: CheckRunSettingWithFlash
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 检测运行定值与flash定值是否有差别
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 CheckRunSettingWithFlash(SETTING_GROUP *p_setting_group)
{
    int32 lv_index;
    Uint8 lv_ubyte;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot check settings!\r\n");
        return NORMAL_ERROR;
    }

    for (lv_index = 0; lv_index < p_setting_group->flash_length; lv_index++)
    {
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, (p_setting_group->flash_addr + lv_index), &lv_ubyte, 1);
        if (p_setting_group->write_buffer[lv_index] != lv_ubyte)
        {
            p_setting_group->need_save_flag = 1;
            return NORMAL_ERROR;
        }
    }

    p_setting_group->need_save_flag = 0;
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
// Local functions realize
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: SettingToDefault
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 运行定值更新
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 SettingToDefault(SETTING_GROUP const *p_setting_group)
{
    int32  i;
    Uint8  *lv_p_stream;
    int32  lv_position;
    SETTING const *lv_p_setting;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (    (NULL == p_setting_group->history_buffer)
         || (NULL == p_setting_group->write_buffer)
         || (NULL == p_setting_group->run_buffer))
    {
        TRACE("Settings' buffers are not initialed, cannot set settings!\r\n");
        return NORMAL_ERROR;
    }

    lv_p_stream = p_setting_group->run_buffer;
    for (i = 0; i < p_setting_group->num; i++)
    {
        lv_p_setting = &p_setting_group->setting[i];
        lv_position = lv_p_setting->position;
        switch (lv_p_setting->define->type)
        {
            //字符串定长SET_VAL_LEN_MAX字节
            case SIGNAL_DATA_TYPE_b:
            case SIGNAL_DATA_TYPE_c:
            {
                if ((lv_p_setting->default_value.c > lv_p_setting->max_value.c) || (lv_p_setting->default_value.c < lv_p_setting->min_value.c))
                {
                    TRACE("default value=%d, exceed scope [%d,%d]\r\n", lv_p_setting->default_value.c, lv_p_setting->min_value.c, lv_p_setting->max_value.c);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = lv_p_setting->default_value.c;
                break;
            }
            case SIGNAL_DATA_TYPE_uc:
            {
                if ((lv_p_setting->default_value.uc > lv_p_setting->max_value.uc) || (lv_p_setting->default_value.uc < lv_p_setting->min_value.uc))
                {
                    TRACE("default  value=%u, exceed scope [%u,%u]\r\n", lv_p_setting->default_value.uc, lv_p_setting->min_value.uc, lv_p_setting->max_value.uc);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = lv_p_setting->default_value.uc;
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            {
                if ((lv_p_setting->default_value.i > lv_p_setting->max_value.i) || (lv_p_setting->default_value.i < lv_p_setting->min_value.i))
                {
                    TRACE("default  value=%d, exceed scope [%d,%d]\r\n", lv_p_setting->default_value.i, lv_p_setting->min_value.i, lv_p_setting->max_value.i);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = LLSB(lv_p_setting->default_value.i);
                lv_p_stream[lv_position + 1] = LHSB(lv_p_setting->default_value.i);
                break;
            }
            case SIGNAL_DATA_TYPE_ui:
            {
                if ((lv_p_setting->default_value.ui > lv_p_setting->max_value.ui) || (lv_p_setting->default_value.ui < lv_p_setting->min_value.ui))
                {
                    TRACE("default  value=%u, exceed scope [%u,%u]\r\n", lv_p_setting->default_value.ui, lv_p_setting->min_value.ui, lv_p_setting->max_value.ui);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = LLSB(lv_p_setting->default_value.ui);
                lv_p_stream[lv_position + 1] = LHSB(lv_p_setting->default_value.ui);
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            {
                if ((lv_p_setting->default_value.l > lv_p_setting->max_value.l) || (lv_p_setting->default_value.l < lv_p_setting->min_value.l))
                {
                    TRACE("default  value=%d, exceed scope [%d,%d]\r\n", lv_p_setting->default_value.l, lv_p_setting->min_value.l, lv_p_setting->max_value.l);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = LLSB(lv_p_setting->default_value.l);
                lv_p_stream[lv_position + 1] = LHSB(lv_p_setting->default_value.l);
                lv_p_stream[lv_position + 2] = HLSB(lv_p_setting->default_value.l);
                lv_p_stream[lv_position + 3] = HHSB(lv_p_setting->default_value.l);
                break;
            }
            case SIGNAL_DATA_TYPE_ul:
            {
                if ((lv_p_setting->default_value.ul > lv_p_setting->max_value.ul) || (lv_p_setting->default_value.ul < lv_p_setting->min_value.ul))
                {
                    TRACE("default  value=%d, exceed scope [%d,%d]\r\n", lv_p_setting->default_value.ul, lv_p_setting->min_value.ul, lv_p_setting->max_value.ul);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = LLSB(lv_p_setting->default_value.ul);
                lv_p_stream[lv_position + 1] = LHSB(lv_p_setting->default_value.ul);
                lv_p_stream[lv_position + 2] = HLSB(lv_p_setting->default_value.ul);
                lv_p_stream[lv_position + 3] = HHSB(lv_p_setting->default_value.ul);
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                if ((lv_p_setting->default_value.f > lv_p_setting->max_value.f) || (lv_p_setting->default_value.f < lv_p_setting->min_value.f))
                {
                    TRACE("default  value=%f, exceed scope [%f,%f]\r\n", lv_p_setting->default_value.f, lv_p_setting->min_value.f, lv_p_setting->max_value.f);
                    return NORMAL_ERROR;
                }
                lv_p_stream[lv_position] = LLSB(lv_p_setting->default_value.l);
                lv_p_stream[lv_position + 1] = LHSB(lv_p_setting->default_value.l);
                lv_p_stream[lv_position + 2] = HLSB(lv_p_setting->default_value.l);
                lv_p_stream[lv_position + 3] = HHSB(lv_p_setting->default_value.l);
                break;
            }
            //两字节长度定值
            default:
            {
                TRACE("unknow type=%d\r\n", lv_p_setting->define->type);
                return NORMAL_ERROR;
            }
        }
    }

    if (NORMAL_SUCCESS != RunSettingUpdate(p_setting_group))
    {
        TRACE("Run setting update to default error!\r\n");
        return NORMAL_ERROR;
    }

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: SettingBackup
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 定值备份
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 SettingBackup(SETTING_GROUP const *p_setting_group)
{
    int32 lv_index;
    Uint8 lv_ubyte;
    Uint8 lv_ubyte_backup;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    if (0 != setting_op_flag)
    {
        DebugPrintf("Storage is busy!\r\n");
        return RET_STORAGE_BUSY;
    }
    setting_op_flag = 1;


    StorageErase(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup, p_setting_group->flash_length);
    StorageWrite(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup, p_setting_group->write_buffer, p_setting_group->flash_length);

    for (lv_index = 0; lv_index < p_setting_group->flash_length; lv_index++)
    {
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr + lv_index, &lv_ubyte, 1);
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup + lv_index, &lv_ubyte_backup, 1);
        if (lv_ubyte != lv_ubyte_backup)
        {
            TRACE("Settings backup failed! Backup byte at addr 0x%08X is 0x%02X, but byte in the active section at addr 0x%08X is 0x%02X.\r\n"
            , p_setting_group->flash_addr_backup + lv_index
            , lv_ubyte_backup
            , p_setting_group->flash_addr + lv_index
            , lv_ubyte);
            setting_op_flag = 0;
            return NORMAL_ERROR;
        }
    }

    setting_op_flag = 0;

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: RestoreSettingFromBackup
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 从backup中恢复定值
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 RestoreSettingFromBackup(SETTING_GROUP const *p_setting_group)
{
    int32 lv_index;
    Uint8 lv_ubyte;
    Uint8 lv_ubyte_backup;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup, p_setting_group->history_buffer, p_setting_group->flash_length);
    if (0 != DefaultCrcCal(p_setting_group->history_buffer, p_setting_group->flash_length, 0xA5A5u))
    {
        TRACE("Backup crc error!\r\n");
        return NORMAL_ERROR;
    }

    StorageErase(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->flash_length);
    StorageWrite(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr, p_setting_group->history_buffer, p_setting_group->flash_length);

    for (lv_index = 0; lv_index < p_setting_group->flash_length; lv_index++)
    {
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr + lv_index, &lv_ubyte, 1);
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup + lv_index, &lv_ubyte_backup, 1);
        if (lv_ubyte != lv_ubyte_backup)
        {
            TRACE("Restore settings from backup failed! Backup byte at addr 0x%08X is 0x%02X, but byte in the active section at addr 0x%08X is 0x%02X.\r\n"
            , p_setting_group->flash_addr_backup + lv_index
            , lv_ubyte_backup
            , p_setting_group->flash_addr + lv_index
            , lv_ubyte);
            return NORMAL_ERROR;
        }
    }

    TRACE("Successfully restore settings from backup!\r\n");
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: BackupSettingCheck
//      Input: SETTING_GROUP const *p_setting_group: 定值组
//     Output: void
//     Return: int32: 函数执行情况
//Description: 定值备份
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 BackupSettingCheck(SETTING_GROUP const *p_setting_group)
{
    int32 lv_index;
    Uint8 lv_ubyte;
    Uint8 lv_ubyte_backup;

    if (NULL == p_setting_group)
    {
        TRACE("Function entry error!\r\n");
        return NORMAL_ERROR;
    }

    for (lv_index = 0; lv_index < p_setting_group->flash_length; lv_index++)
    {
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr + lv_index, &lv_ubyte, 1);
        StorageRead(STORAGE_TYPE_STORE_SETTINGS, p_setting_group->flash_addr_backup + lv_index, &lv_ubyte_backup, 1);
        if (lv_ubyte != lv_ubyte_backup)
        {
            TRACE("Settings backup failed! Backup byte at addr 0x%08X is 0x%02X, but byte in the active section at addr 0x%08X is 0x%02X.\r\n"
            , p_setting_group->flash_addr_backup + lv_index
            , lv_ubyte_backup
            , p_setting_group->flash_addr + lv_index
            , lv_ubyte);
            TRACE("Try to backup again!\r\n");
            return SettingBackup(p_setting_group);
        }
    }

    return NORMAL_SUCCESS;
}

