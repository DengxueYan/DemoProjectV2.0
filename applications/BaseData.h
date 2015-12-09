/*****************************************************************************************************
* FileName:                    BaseData.h
*
* Description:                 基本数据结构头文件
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Base_Data_H
#define _Base_Data_H

#include "UserTypesDef.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// 宏定义
//====================================================================================================
#define FH_BIN_HEADER_LENGTH         16u
#define SIGNAL_BIN_POS_BITS          5u
#define SIGNAL_BIN_POS_MASK          0x1Fu

//====================================================================================================
// enum定义
//====================================================================================================
typedef enum
{
    SIGNAL_DATA_TYPE_b   = 0,
    SIGNAL_DATA_TYPE_c   = 1,
    SIGNAL_DATA_TYPE_uc  = 2,
    SIGNAL_DATA_TYPE_i   = 3,
    SIGNAL_DATA_TYPE_ui  = 4,
    SIGNAL_DATA_TYPE_l   = 5,
    SIGNAL_DATA_TYPE_ul  = 6,
    SIGNAL_DATA_TYPE_f   = 7
} SIGNAL_DATA_TYPE;

typedef enum
{
    PROJECT_DATA_TYPE_BOOLEAN    = 0,
    PROJECT_DATA_TYPE_DBPOS      = 1,
    PROJECT_DATA_TYPE_INT32      = 2,
    PROJECT_DATA_TYPE_UINT32     = 3,
    PROJECT_DATA_TYPE_ENTRYTIME  = 4,
    PROJECT_DATA_TYPE_TIMESTAMP  = 5,
    PROJECT_DATA_TYPE_QUALITY    = 6,
    PROJECT_DATA_TYPE_FLOAT      = 7
} PROJECT_DATA_TYPE;

//====================================================================================================
// 结构声明
//====================================================================================================
typedef struct
{
    char   const *name;
    void   *addr;
    SIGNAL_DATA_TYPE  type;
} SIGNAL_DEFINE;

typedef struct
{
    char   const *name;
    void   *addr;
    Uint8  position;
} SIGNAL_BIN_DEFINE;

typedef union
{
    Uint8  uc;
    int8   c;
    Uint16 ui;
    int16  i;
    Uint32 ul;
    int32  l;
    float  f;
    char const *str;
} SIGNAL_DATA;

#ifdef __cplusplus
}
#endif

#endif

