/*****************************************************************************************************
* FileName:                    SingalOutManage.h
*
* Description:                 Signal out manage header
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Singal_Out_Manage_H
#define _Singal_Out_Manage_H

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"
#include "BaseData.h"
#include "SignalOut.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// Structure declaration
//====================================================================================================
typedef struct
{
    SIGNAL_DEFINE const *define;
    Uint32 invisible;
    char   unit[UINT_LENGTH_MAX];// if SIGNAL_DATA_TYPE_b, unit[0] is the position of the bit in the varible
    char   const *desc;
    char   const *desc_utf8;
    char   const *remark;
} SIGNAL_ANALOG;

typedef struct
{
    SIGNAL_BIN_DEFINE const *define;
    Uint8  last_value;
    Uint32 invisible;
    char   const *desc;
    char   const *desc_utf8;
    char   const *remark;
} SIGNAL_BINARY;

typedef struct
{
    SIGNAL_ANALOG *analog;
    int32  num;
} ANALOG_GROUP;

typedef struct
{
    SIGNAL_BINARY *binary;
    int32  num;
} BINARY_GROUP;

//====================================================================================================
// External variables
//====================================================================================================
extern ANALOG_GROUP analog_group;
extern ANALOG_GROUP self_diagnose_group;
extern BINARY_GROUP self_diagnose_bin_group;
extern BINARY_GROUP bin_input_group;
extern BINARY_GROUP bin_output_group;

//====================================================================================================
// External functions
//====================================================================================================

#ifdef __cplusplus
}
#endif

#endif



