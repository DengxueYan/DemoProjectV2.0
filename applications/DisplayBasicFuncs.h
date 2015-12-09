/*****************************************************************************************************
* FileName:                    DisplayBasicFuncs.h
*
* Description:                 Display related basic functions
*
* Author:                      YanDengxue, SmartSaw
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _DisplayBasicFuncs_H
#define _DisplayBasicFuncs_H

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"
#include "STM32F40xRegister.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// Macro define
//====================================================================================================
#define LCD_TEXT_MEM_START_ADDR 0x0000
#define LCD_GRAPHICS_MEM_START_ADDR  (LCD_TEXT_MEM_START_ADDR + 0x2580)
#define LCD_CONTROLLER_PIXEL_X  320
#define LCD_CONTROLLER_PIXEL_Y  240
#define LCD_PIXEL_X  LCD_CONTROLLER_PIXEL_X
#define LCD_PIXEL_Y  LCD_CONTROLLER_PIXEL_Y

//====================================================================================================
// Types define
//====================================================================================================

//====================================================================================================
// Interface variables
//====================================================================================================

//====================================================================================================
// Interface functions
//====================================================================================================
extern int32 LcdControllerInitial(void);
extern int32 LcdClr(void);
extern int32 LcdDispText(Uint8 const *data, Uint16 offset, Uint8 negative);
extern int32 LcdDispGraphics(Uint8 const *data, Uint16 length, Uint16 offset, Uint8 negative);

#ifdef __cplusplus
}
#endif

#endif
