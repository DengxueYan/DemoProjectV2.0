/*****************************************************************************************************
* FileName:                    Display.c
*
* Description:                 Human-Machine basic interface Functions
*
* Author:                      YanDengxue, SmartSaw
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
//====================================================================================================
// Header files declaration
//====================================================================================================
// Lib headers
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Self-define headers
#include "SystemBase.h"
#include "UserApp.h"
#include "Uart.h"
#include "TaskServices.h"
#include "SpiFlash.h"
#include "Storage.h"
#include "DisplayBasicFuncs.h"
#include "Display.h"
#include "RtcBasicFuncs.h"

//====================================================================================================
// Macro defnie
//====================================================================================================
#define DISPLAY_PERIOD 500
#define DISPLAY_SCROLL_PERIOD 20
#define DISPLAY_FLASH_PERIOD 4

//====================================================================================================
// Local functions, these functions can not be accessed outside
//====================================================================================================
static int32 DisplayRectangle(Uint32 disp_pos, Uint16 length, Uint16 width, Uint8 solid, Uint8 negative);

//====================================================================================================
// Outside functions reference
//====================================================================================================

//====================================================================================================
// Global variables
//====================================================================================================

//====================================================================================================
// Local variables, these variables can not be accessed outside

//====================================================================================================
static Uint32 display_iterant_count;
static LED_DEF leds;

//====================================================================================================
// Functions realize
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: DisplayTask0Service
//      Input: void
//     Output: void
//     Return: void
//Description: Display task0 handle function
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
void DisplayTask0Service(void)
{
    if (0 == (system_running_status & CPU_EXIT_USR_APP_FLAG))
    {
        leds.led1_toggle_count++;
        if ((COMM_STATUS_SUCCESS == uart4.comm_status) && (COMM_STATUS_SUCCESS == uart2.comm_status))
        {
            if (leds.led1_toggle_count > LED_FLASH_LEVEL1_COUNT_MAX)
            {
                pGPIOC->ODR ^= 0x0004u;
                leds.led1_toggle_count = 0;
            }
        }
        else if (COMM_STATUS_SUCCESS == uart4.comm_status)
        {
            if (leds.led1_toggle_count > LED_FLASH_LEVEL2_COUNT_MAX)
            {
                pGPIOC->ODR ^= 0x0004u;
                leds.led1_toggle_count = 0;
            }
        }
        else if (COMM_STATUS_SUCCESS == uart2.comm_status)
        {
            if (leds.led1_toggle_count > LED_FLASH_LEVEL3_COUNT_MAX)
            {
                pGPIOC->ODR ^= 0x0004u;
                leds.led1_toggle_count = 0;
            }
        }
        else
        {
            pGPIOC->BSRRH = 0x0004u;
        }
        pGPIOC->BSRRL = 0x0002u;
    }
    else
    {
        pGPIOC->BSRRH = 0x0002u;
        pGPIOC->BSRRL = 0x0004u;
    }

    if (display_iterant_count < DISPLAY_PERIOD)
    {
        display_iterant_count++;
    }
    else
    {
        if (0 == task1_service_request_no)
        {
            display_iterant_count = 0;
            task1_service_request_no = 3;
        }
    }
}

//----------------------------------------------------------------------------------------------------
//   Function: DisplayTask1Service
//      Input: void
//     Output: void
//     Return: void
//Description: Display interrupt handle function
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static Uint8 display_run_first_time_finish;
static Uint32 display_scroll_count;
static Uint32 display_flash_count;
static Uint8  display_flash_toggle;
int32 DisplayTask1Service(void)
{
    Uint8 lv_buffer_rtc[7];
    int8 lv_buffer[320 / 8 + 1];
    Uint32 lv_disp_pos;

    if (0 == display_run_first_time_finish)
    {
        LcdClr();
        display_run_first_time_finish = 1;

        // Display scroll bar
        lv_disp_pos = (320 * 17 + (320 - 7));
        DisplayRectangle(lv_disp_pos, 7, (240 - 17) / 3 + 1, 1, 0);
        lv_disp_pos += (240 - 17) / 3 * 320;
        DisplayRectangle(lv_disp_pos, 7, (240 - 17) / 3 + 1, 0, 0);
        lv_disp_pos += (240 - 17) / 3 * 320;
        DisplayRectangle(lv_disp_pos, 7, (240 - 17) - (240 - 17) / 3 * 2, 0, 0);

        // Display table

        DisplayRectangle(320 * 17 + 0 * 8,  2, 240 - 17, 1, 0);
        DisplayRectangle(320 * 17  + 5 * 8 + 3,  1, 240 - 17, 1, 0);
        DisplayRectangle(320 * 17  + 12 * 8 + 3, 1, 240 - 17, 1, 0);
        DisplayRectangle(320 * 17  + 24 * 8 + 3, 1, 240 - 17, 1, 0);
        DisplayRectangle(320 * 17  + 33 * 8 + 3, 1, 240 - 17, 1, 0);
        DisplayRectangle(320 * 17  + 38 * 8 + 6, 2, 240 - 17, 1, 0);

        lv_disp_pos =  0;
        lv_disp_pos =  320 * 16;
        lv_disp_pos += 320;
        DisplayRectangle(lv_disp_pos, 320 - 8, 1, 1, 0);
        lv_disp_pos += 320;
        lv_disp_pos += 320 * 16;

        DisplayRectangle(lv_disp_pos, 320 - 8, 1, 1, 0);
        lv_disp_pos += 320;
        DisplayRectangle(lv_disp_pos, 320 - 8, 1, 1, 0);


        // Display initial characters
        snprintf(lv_buffer, sizeof(lv_buffer), "          2000-01-01 00:00:00           ");
        if (1 == sys_cfg.language)
        {
            LcdDispText((Uint8 const *)lv_buffer, 0, 1);
            lv_disp_pos =  320 / 8 * (16 + 2);
            snprintf(lv_buffer, sizeof(lv_buffer), " 天线 传感器    温度 (%c) 强度(dB) 状态 ", 127);
            LcdDispText((Uint8 const *)lv_buffer, lv_disp_pos, 0);
            lv_disp_pos += 320 / 8 * 17;
        }
        else
        {
            LcdDispText((Uint8 const *)lv_buffer, 0, 1);
            lv_disp_pos =  320 / 8 * (16 + 2);
            snprintf(lv_buffer, sizeof(lv_buffer), "  ANT SENSOR      T  (%c)   Q (dB)  ST  ", 127);
            LcdDispText((Uint8 const *)lv_buffer, lv_disp_pos, 0);
            lv_disp_pos += 320 / 8 * 17;
        }
    }

    if (display_scroll_count < DISPLAY_SCROLL_PERIOD)
    {
        display_scroll_count++;
    }
    else
    {
        display_scroll_count = 0;
    }

    if (display_flash_count < DISPLAY_FLASH_PERIOD)
    {
        display_flash_count++;
    }
    else
    {
        display_flash_count = 0;
        display_flash_toggle = (display_flash_toggle + 1) & 1;
        if (0 != display_flash_toggle)
        {
            if ((COMM_STATUS_SUCCESS == uart4.comm_status) && (COMM_STATUS_SUCCESS == uart2.comm_status))
            {
                lv_buffer[0] = '*';
            }
            else if (COMM_STATUS_SUCCESS == uart4.comm_status)
            {
                lv_buffer[0] = '+';
            }
            else if (COMM_STATUS_SUCCESS == uart2.comm_status)
            {
                lv_buffer[0] = '-';
            }
            else
            {
                lv_buffer[0] = ' ';
            }
        }
        else
        {
            lv_buffer[0] = ' ';
        }
        lv_buffer[1] = '\0';
        LcdDispText((Uint8 const *)lv_buffer, 1, 1);
    }

    RtcRead(31, lv_buffer_rtc, sizeof(lv_buffer_rtc));
    snprintf(lv_buffer, sizeof(lv_buffer), "%02X-%02X-%02X %02X:%02X:%02X",
                                           lv_buffer_rtc[6],
                                           lv_buffer_rtc[4],
                                           lv_buffer_rtc[3],
                                           lv_buffer_rtc[2] & 0x7Fu,
                                           lv_buffer_rtc[1],
                                           lv_buffer_rtc[0] & 0x7Fu);
    LcdDispText((Uint8 const *)lv_buffer, 12, 1);

    lv_disp_pos =  320 / 8 * (16 + 2);
    lv_disp_pos += 320 / 8 * 17;

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
// Local functions realize
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: DisplayRectangle
//      Input:
//     Output: void
//     Return: int32
//Description: Display Rectangle
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 DisplayRectangle(Uint32 disp_pos, Uint16 length, Uint16 hight, Uint8 solid, Uint8 negative)
{
    int32 lv_index_x;
    int32 lv_index_y;
    Uint8  lv_byte;
    Uint16 lv_byte_index_start;
    Uint8  lv_byte_start_offset;
    Uint8  lv_byte_start;
    Uint16 lv_byte_index_end;
    Uint8  lv_byte_end_offset;
    Uint8  lv_byte_end;

    if ((0 == length) || (0 == hight))
    {
        return NORMAL_SUCCESS;
    }

    lv_byte_index_start  = disp_pos / 8;
    lv_byte_start_offset = disp_pos % 8;

    lv_byte_index_end  = (disp_pos + length - 1) / 8;
    lv_byte_end_offset = (disp_pos + length - 1) % 8;

    if (lv_byte_index_start == lv_byte_index_end)
    {
        lv_byte = 0;
        for (lv_index_x = lv_byte_start_offset; lv_index_x <= lv_byte_end_offset; lv_index_x++)
        {
            lv_byte |= (0x80u >> lv_index_x);
        }
        LcdDispGraphics(&lv_byte, 1, lv_byte_index_start, negative);
        if (hight > 1)
        {
            LcdDispGraphics(&lv_byte, 1, lv_byte_index_start + (320 / 8) * (hight - 1), negative);

            if (0 == solid)
            {
                lv_byte = ((0x80u >> lv_byte_start_offset) | (0x80u >> (lv_byte_end_offset)));
            }

            for (lv_index_y = 1; lv_index_y < (hight - 1); lv_index_y++)
            {
                LcdDispGraphics(&lv_byte, 1, lv_byte_index_start + (320 / 8) * lv_index_y, negative);
            }
        }
    }
    else
    {
        lv_byte_start = (0xFFu >> lv_byte_start_offset);
        lv_byte_end = (0xFFu << (7 - lv_byte_end_offset));

        LcdDispGraphics(&lv_byte_start, 1, lv_byte_index_start, negative);
        for (lv_index_x = lv_byte_index_start + 1; lv_index_x < lv_byte_index_end; lv_index_x++)
        {
            lv_byte = 0xFFu;
            LcdDispGraphics(&lv_byte, 1, lv_index_x, negative);
        }
        LcdDispGraphics(&lv_byte_end, 1, lv_byte_index_end, 0);

        if (hight > 1)
        {
            LcdDispGraphics(&lv_byte_start, 1, lv_byte_index_start + (320 / 8) * (hight - 1), negative);
            for (lv_index_x = lv_byte_index_start + 1; lv_index_x < lv_byte_index_end; lv_index_x++)
            {
                lv_byte = 0xFFu;
                LcdDispGraphics(&lv_byte, 1, lv_index_x + (320 / 8) * (hight - 1), negative);
            }
            LcdDispGraphics(&lv_byte_end, 1, lv_byte_index_end + (320 / 8) * (hight - 1), negative);

            if (0 == solid)
            {
                lv_byte_start = (0x80u >> lv_byte_start_offset);
                lv_byte_end = (0x01u << (7 - lv_byte_end_offset));
            }

            for (lv_index_y = 1; lv_index_y < (hight - 1); lv_index_y++)
            {
                LcdDispGraphics(&lv_byte_start, 1, lv_byte_index_start + (320 / 8) * lv_index_y, negative);
                if (0 != solid)
                {
                    lv_byte = 0xFFu;
                    for (lv_index_x = lv_byte_index_start + 1; lv_index_x < lv_byte_index_end; lv_index_x++)
                    {
                        LcdDispGraphics(&lv_byte, 1, lv_index_x + (320 / 8) * lv_index_y, negative);
                    }
                }

                LcdDispGraphics(&lv_byte_end, 1, lv_byte_index_end + (320 / 8) * lv_index_y, negative);
            }
        }
    }
    return NORMAL_SUCCESS;
};


