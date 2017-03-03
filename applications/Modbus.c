/*****************************************************************************************************
* FileName:                    Modbus.c
*
* Description:                 Uart communication
*
* Author:                      YanDengxue, SmartSaw
*
* Rev History:
*       <Author>        <Date>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
//====================================================================================================
// Header file declaration
//====================================================================================================
// Lib header
#include <string.h>
#include <stdlib.h>

// Self-define header
#include "SystemBase.h"
#include "TaskServices.h"
#include "Uart.h"
#include "Modbus.h"
#include "UserApp.h"
#include "SettingManage.h"
#include "SignalOutManage.h"

//====================================================================================================
// Macro define
//====================================================================================================

//====================================================================================================
// Local functions, these functions can not be accessed outside
//====================================================================================================
static int32 ModbusAppProc(UART_DEF *uart);
static int32 ModbusFunc01Proc(UART_DEF *uart);
static int32 ModbusFunc03Proc(UART_DEF *uart);
static int32 ModbusFunc04Proc(UART_DEF *uart);
static int32 ModbusFunc06Proc(UART_DEF *uart);
static int32 ModbusFunc16Proc(UART_DEF *uart);
static int32 ModusSendExceptionFrame(UART_DEF *uart, Uint8 func_code, Uint8 reason);
static int32 ModbusWriteSettings(SETTING_GROUP *p_setting_group, Uint8 const *value_src, Uint16 set_start_addr, Uint16 quantity);

//====================================================================================================
// Global variables
//====================================================================================================

//====================================================================================================
// Local variables, these variables can not be accessed outside
//====================================================================================================

//====================================================================================================
// Functions realize
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: ModbusFrameProc
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus frame process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 ModbusFrameProc(UART_DEF *uart)
{
    int32  lv_index;
    int32  lv_ret;
    Uint8  lv_addr;
    Uint8  lv_byte_num;
    Uint16 lv_quantity;
    Uint8  lv_func_code;
    UART_DEF *lv_p_uart;
    Uint16 lv_rx_buffer_wr_ptr_temp;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }

    lv_p_uart = uart;
    while (0 != UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_wr_ptr - lv_p_uart->rx_buffer_rd_ptr))
    {
        if (COMM_FRAME_STATUS_INITIAL == lv_p_uart->comm_frame_status)
        {
            lv_addr = lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr)];
            if ((lv_addr != lv_p_uart->comm_addr) && (0 != lv_addr))
            {
                lv_p_uart->rx_buffer_rd_ptr++;
                continue;
            }
            lv_p_uart->rx_frame_length = 8;
            lv_p_uart->temp[0] = lv_p_uart->rx_buffer_wr_ptr;
            lv_p_uart->temp[1] = system_ms_count;
            lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_WAIT_FRAME_COMPLETE;
        }
        else
        {
            lv_rx_buffer_wr_ptr_temp = lv_p_uart->rx_buffer_wr_ptr;
            if (UART_RX_BUF_ADDR(lv_rx_buffer_wr_ptr_temp - lv_p_uart->rx_buffer_rd_ptr) < lv_p_uart->rx_frame_length)
            {
                if (lv_p_uart->temp[0] == lv_rx_buffer_wr_ptr_temp)
                {
                    if ((system_ms_count - lv_p_uart->temp[1]) >= lv_p_uart->rx_timeout_ms)
                    {
                        lv_p_uart->rx_buffer_rd_ptr = lv_rx_buffer_wr_ptr_temp;
                        lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                    }
                }
                else
                {
                    lv_p_uart->temp[0] = lv_p_uart->rx_buffer_wr_ptr;
                    lv_p_uart->temp[1] = system_ms_count;
                }
                return NORMAL_SUCCESS;
            }
            else
            {
                if (8 == lv_p_uart->rx_frame_length)
                {
                    lv_func_code = lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 1)];
                    switch (lv_func_code)
                    {
                        case 0x01u:
                        case 0x02u:
                        {
                            lv_quantity  =   (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 4)] << 8)
                                           | (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 5)]);
                            if ((0 == lv_quantity) || (lv_quantity > 0x07D0u))
                            {
                                lv_p_uart->rx_buffer_rd_ptr++;
                                lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                                continue;
                            }
                            break;
                        }
                        case 0x03u:
                        case 0x04u:
                        {
                            lv_quantity  =   (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 4)] << 8)
                                           | (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 5)]);
                            if ((0 == lv_quantity) || (lv_quantity > 0x007Du))
                            {
                                lv_p_uart->rx_buffer_rd_ptr++;
                                lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                                continue;
                            }
                            break;
                        }
                        case 0x05u:
                        case 0x06u:
                        {
                            break;
                        }
                        case 0x0Fu:
                        {
                            lv_quantity  =   (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 4)] << 8)
                                           | (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 5)]);
                            lv_byte_num = lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 6)];
                            if (    (((lv_quantity + 7) >> 3) != lv_byte_num)
                                 || ((0 == lv_quantity) || (lv_quantity > 0x07B0u)))
                            {
                                lv_p_uart->rx_buffer_rd_ptr++;
                                lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                                continue;
                            }
                            else
                            {
                                lv_p_uart->rx_frame_length = lv_byte_num + 9u;
                                continue;
                            }
                        }
                        case 0x10u:
                        {
                            lv_quantity  =   (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 4)] << 8)
                                           | (lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 5)]);
                            lv_byte_num = lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + 6)];
                            if (    ((lv_quantity << 1) != lv_byte_num)
                                 || ((0 == lv_quantity) || (lv_quantity > 0x007Bu)))
                            {
                                lv_p_uart->rx_buffer_rd_ptr++;
                                lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                                continue;
                            }
                            else
                            {
                                lv_p_uart->rx_frame_length = lv_byte_num + 9u;
                                continue;
                            }
                        }
                        case 0x14u:
                        case 0x15u:
                        case 0x16u:
                        case 0x17u:
                        case 0x2Bu:
                        default:
                        {
                            lv_p_uart->rx_buffer_rd_ptr++;
                            lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                            continue;
                        }
                    }
                }

                for (lv_index = 0; lv_index < lv_p_uart->rx_frame_length; lv_index++)
                {
                    lv_p_uart->rx_frame_buffer[lv_index] = lv_p_uart->rx_buffer[UART_RX_BUF_ADDR(lv_p_uart->rx_buffer_rd_ptr + lv_index)];
                }
                if (0 != DefaultCrcCal(lv_p_uart->rx_frame_buffer, lv_p_uart->rx_frame_length, 0xFFFFu))
                {
                    lv_p_uart->rx_buffer_rd_ptr++;
                    lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                    continue;
                }
                lv_p_uart->rx_buffer_rd_ptr += lv_p_uart->rx_frame_length;
                lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_FRAME_COMPLETE;
                lv_ret = ModbusAppProc(lv_p_uart);
                if (NORMAL_SUCCESS == lv_ret)
                {
                    lv_p_uart->comm_frame_rx_time = system_ms_count;
                    lv_p_uart->comm_status = COMM_STATUS_SUCCESS;
                }
                lv_p_uart->comm_frame_status = COMM_FRAME_STATUS_INITIAL;
                return lv_ret;
            }
        }
    }

    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: ModbusAppProc
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus application process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModbusAppProc(UART_DEF *uart)
{
    UART_DEF *lv_p_uart;
    Uint8 lv_func_code;
    Uint8 lv_addr;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }

    lv_p_uart = uart;
    lv_addr = lv_p_uart->rx_frame_buffer[0];
    if (0 == lv_addr)
    {
    }
    else if (lv_p_uart->comm_addr == lv_addr)
    {
        lv_func_code = lv_p_uart->rx_frame_buffer[1];
        switch (lv_func_code)
        {
            case 0x01u:
            {
                return ModbusFunc01Proc(lv_p_uart);
            }
            case 0x03u:
            {
                return ModbusFunc03Proc(lv_p_uart);
            }
            case 0x04u:
            {
                return ModbusFunc04Proc(lv_p_uart);
            }
            case 0x06u:
            {
                return ModbusFunc06Proc(lv_p_uart);
            }
            case 0x10u:
            {
                return ModbusFunc16Proc(lv_p_uart);
            }
            case 0x02u:
            case 0x05u:
            case 0x0Fu:
            case 0x17u:
            default:
            {
                ModusSendExceptionFrame(lv_p_uart, lv_func_code, MODBUS_EXCEPTION_CODE_ILLEGAL_FUNCTION);
                break;
            }
        }
    }

    return NORMAL_ERROR;
}

//----------------------------------------------------------------------------------------------------
// Local functions realize
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: ModbusFunc01Proc
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus func code 1 process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModbusFunc01Proc(UART_DEF *uart)
{
    UART_DEF *lv_p_uart;
    Uint8 *lv_buffer;
    Uint8 const *lv_p_rx_frame_buffer;
    Uint8 lv_send_length;
    Uint16 lv_crc;
    Uint16 lv_send_start_addr;
    Uint16 lv_send_end_addr;
    Uint16 lv_search_addr;
    Uint16 lv_quantity;
    Uint8  lv_bit_index;
    Uint8 *lv_p_send_buffer;
    Uint8  lv_modbus_bin_index;
    Uint8  lv_bin;
    Uint8  lv_type;
    Uint8  lv_bin_pos;
    Uint8  lv_byte;
    SIGNAL_BINARY const *lv_p_binary;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }

    lv_p_uart = uart;
    lv_p_rx_frame_buffer = lv_p_uart->rx_frame_buffer;
    lv_buffer = lv_p_uart->tx_frame_buffer;

    lv_send_start_addr =   (lv_p_rx_frame_buffer[2] << 8)
                         | (lv_p_rx_frame_buffer[3]);
    lv_quantity   =   (lv_p_rx_frame_buffer[4] << 8)
                    | (lv_p_rx_frame_buffer[5]);
    if ((lv_quantity < 1) || (lv_quantity > 2000u))
    {
        DebugPrintf("modbus func 1 quanity=%d exceeds the scope [1,2000]\n\r", lv_quantity);
        return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_ILLEGAL_ADDRESS);
    }
    lv_send_end_addr = lv_send_start_addr + lv_quantity;

    lv_buffer[0] = lv_p_rx_frame_buffer[0];
    lv_buffer[1] = lv_p_rx_frame_buffer[1];

    lv_p_send_buffer = &lv_buffer[3];
    lv_search_addr = 0;
    lv_modbus_bin_index = 0;
    lv_byte = 0;
    for (lv_bit_index = 0; lv_bit_index < bin_input_group.num; lv_bit_index++)
    {
        lv_p_binary = &bin_input_group.binary[lv_bit_index];
        if (0 != (lv_p_binary->invisible & ITEM_DEBUG_INVISIBLE))
        {
             continue;
        }

        if (0 != (lv_p_binary->invisible & ITEM_UART_INVISIBLE))
        {
            continue;
        }

        lv_search_addr++;
        if (lv_search_addr <= lv_send_start_addr)
        {
            continue;
        }
        else
        {
            lv_type = ((lv_p_binary->define->position >> SIGNAL_BIN_POS_BITS) & 0x07u);
            lv_bin_pos = (lv_p_binary->define->position & SIGNAL_BIN_POS_MASK);
            if (0 == lv_type)
            {
                lv_bin = (((*(VUint8 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
            }
            else if (1 == lv_type)
            {
                lv_bin = (((*(VUint16 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
            }
            else if (2 == lv_type)
            {
                lv_bin = (((*(VUint32 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
            }
            else
            {
                DebugPrintf("unknow bin_input type=%d\n\r", lv_type);
                return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_DEVICE_FAILURE);;
            }
            lv_byte |= (lv_bin << lv_modbus_bin_index);
            if (lv_modbus_bin_index >= 7u)
            {
                *lv_p_send_buffer++ = lv_byte;
                lv_modbus_bin_index = 0;
                lv_byte = 0;
            }
            else
            {
                lv_modbus_bin_index++;
            }
        }

        if (lv_search_addr >= lv_send_end_addr)
        {
            break;
        }
    }

    if (lv_search_addr < lv_send_end_addr)
    {
        for (lv_bit_index = 0; lv_bit_index < bin_output_group.num; lv_bit_index++)
        {
            lv_p_binary = &bin_output_group.binary[lv_bit_index];
            if (0 != (lv_p_binary->invisible & ITEM_DEBUG_INVISIBLE))
            {
                 continue;
            }

            if (0 != (lv_p_binary->invisible & ITEM_UART_INVISIBLE))
            {
                continue;
            }

            lv_search_addr++;
            if (lv_search_addr <= lv_send_start_addr)
            {
                continue;
            }
            else
            {
                lv_type = ((lv_p_binary->define->position >> SIGNAL_BIN_POS_BITS) & 0x07u);
                lv_bin_pos = (lv_p_binary->define->position & SIGNAL_BIN_POS_MASK);
                if (0 == lv_type)
                {
                    lv_bin = (((*(VUint8 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
                }
                else if (1 == lv_type)
                {
                    lv_bin = (((*(VUint16 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
                }
                else if (2 == lv_type)
                {
                    lv_bin = (((*(VUint32 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
                }
                else
                {
                    DebugPrintf("unknow bin_output type=%d\n\r", lv_type);
                    return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_DEVICE_FAILURE);;
                }
                lv_byte |= (lv_bin << lv_modbus_bin_index);
                if (lv_modbus_bin_index >= 7u)
                {
                    *lv_p_send_buffer++ = lv_byte;
                    lv_modbus_bin_index = 0;
                    lv_byte = 0;
                }
                else
                {
                    lv_modbus_bin_index++;
                }
            }

            if (lv_search_addr >= lv_send_end_addr)
            {
                break;
            }
        }
    }

    if (lv_search_addr < lv_send_end_addr)
    {
        for (lv_bit_index = 0; lv_bit_index < self_diagnose_bin_group.num; lv_bit_index++)
        {
            lv_p_binary = &self_diagnose_bin_group.binary[lv_bit_index];
            if (0 != (lv_p_binary->invisible & ITEM_DEBUG_INVISIBLE))
            {
                 continue;
            }

            if (0 != (lv_p_binary->invisible & ITEM_UART_INVISIBLE))
            {
                continue;
            }

            lv_search_addr++;
            if (lv_search_addr <= lv_send_start_addr)
            {
                continue;
            }
            else
            {
                lv_type = ((lv_p_binary->define->position >> SIGNAL_BIN_POS_BITS) & 0x07u);
                lv_bin_pos = (lv_p_binary->define->position & SIGNAL_BIN_POS_MASK);
                if (0 == lv_type)
                {
                    lv_bin = (((*(VUint8 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
                }
                else if (1 == lv_type)
                {
                    lv_bin = (((*(VUint16 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
                }
                else if (2 == lv_type)
                {
                    lv_bin = (((*(VUint32 const *)lv_p_binary->define->addr) >> lv_bin_pos) & 1);
                }
                else
                {
                    DebugPrintf("unknow self_diagnose type=%d\n\r", lv_type);
                    return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_DEVICE_FAILURE);;
                }
                lv_byte |= (lv_bin << lv_modbus_bin_index);
                if (lv_modbus_bin_index >= 7u)
                {
                    *lv_p_send_buffer++ = lv_byte;
                    lv_modbus_bin_index = 0;
                    lv_byte = 0;
                }
                else
                {
                    lv_modbus_bin_index++;
                }
            }

            if (lv_search_addr >= lv_send_end_addr)
            {
                break;
            }
        }
    }

    if (0 != lv_modbus_bin_index)
    {
        *lv_p_send_buffer++ = (lv_byte);
        lv_search_addr = ((lv_search_addr + 7u) & 0xFFF8u);
    }

    // Pad not used bytes
    while (lv_search_addr < lv_send_end_addr)
    {
        *lv_p_send_buffer++ = 0;
        lv_search_addr += 8;
    }

    lv_send_length = ((lv_search_addr - lv_send_start_addr + 7u) >> 3u) + 5u;
    lv_buffer[2] = lv_send_length - 5u;

    lv_crc = DefaultCrcCal(lv_buffer, (lv_send_length - 2), 0xFFFFu);
    lv_buffer[lv_send_length - 2] = lv_crc;
    lv_buffer[lv_send_length - 1] = (lv_crc >> 8);

    lv_p_uart->tx_frame_length = lv_send_length;
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: ModbusFunc03Proc
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus func code 3 process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModbusFunc03Proc(UART_DEF *uart)
{
    UART_DEF *lv_p_uart;
    Uint8 *lv_buffer;
    Uint8 const *lv_p_rx_frame_buffer;
    Uint8 lv_send_length;
    Uint16 lv_crc;
    Uint16 lv_send_start_addr;
    Uint16 lv_send_end_addr;
    Uint16 lv_search_addr;
    Uint16 lv_quantity;
    int32  lv_set_index;
    Uint8  lv_16b_num;
    Uint8 *lv_p_send_buffer;
    SETTING const *lv_p_setting;
    SIGNAL_DATA lv_signal_data;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }

    if (NULL == setting_system.setting)
    {
        return NORMAL_ERROR;
    }

    lv_p_uart = uart;
    lv_p_rx_frame_buffer = lv_p_uart->rx_frame_buffer;
    lv_buffer = lv_p_uart->tx_frame_buffer;

    lv_send_start_addr =   (lv_p_rx_frame_buffer[2] << 8)
                         | (lv_p_rx_frame_buffer[3]);
    lv_quantity   =   (lv_p_rx_frame_buffer[4] << 8)
                    | (lv_p_rx_frame_buffer[5]);
    if ((lv_quantity < 1) || (lv_quantity > 125u))
    {
        DebugPrintf("modbus func 3 quanity=%d exceeds the scope [1,2000]\n\r", lv_quantity);
        return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_ILLEGAL_ADDRESS);
    }
    lv_send_end_addr = lv_send_start_addr + lv_quantity;

    lv_buffer[0] = lv_p_rx_frame_buffer[0];
    lv_buffer[1] = lv_p_rx_frame_buffer[1];

    lv_p_send_buffer = &lv_buffer[3];
    for (lv_set_index = 0, lv_search_addr = 0; lv_set_index < setting_system.num; lv_set_index++)
    {
        lv_p_setting = &setting_system.setting[lv_set_index];

        if (0 != (lv_p_setting->invisible & ITEM_DEBUG_INVISIBLE))
        {
             continue;
        }

        if (0 != (lv_p_setting->invisible & ITEM_UART_INVISIBLE))
        {
            continue;
        }
        lv_16b_num = 1;
        lv_signal_data.l = 0;
        switch (lv_p_setting->define->type)
        {
            case SIGNAL_DATA_TYPE_b:
            {
                lv_signal_data.c = *((Vint8 *)lv_p_setting->define->addr);
                lv_signal_data.c = (lv_signal_data.c & 1);
                break;
            }
            case SIGNAL_DATA_TYPE_c:
            {
                lv_signal_data.c = *((Vint8 *)lv_p_setting->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_uc:
            {
                lv_signal_data.uc = *((VUint8 *)lv_p_setting->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            {
                lv_signal_data.i = *((Vint16 *)lv_p_setting->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_ui:
            {
                lv_signal_data.ui = *((VUint16 *)lv_p_setting->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            {
                lv_signal_data.l = *((Vint32 *)lv_p_setting->define->addr);
                lv_16b_num = 2;
                break;
            }
            case SIGNAL_DATA_TYPE_ul:
            {
                lv_signal_data.ul = *((VUint32 *)lv_p_setting->define->addr);
                lv_16b_num = 2;
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                lv_signal_data.f = *((Vfloat *)lv_p_setting->define->addr);
                lv_16b_num = 2;
                break;
            }
            default:
            {
                DebugPrintf("unknow setting type=%d\n\r", lv_p_setting->define->type);
                return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_DEVICE_FAILURE);;
            }
        }

        if (lv_search_addr < lv_send_start_addr)
        {
            lv_search_addr += lv_16b_num;
            if (lv_search_addr > lv_send_start_addr)
            {
                *lv_p_send_buffer++ = (lv_signal_data.ul >> 8u);
                *lv_p_send_buffer++ = (lv_signal_data.ul);
            }
            else
            {
                continue;
            }
        }
        else
        {
            lv_search_addr += lv_16b_num;
            if (2 == lv_16b_num)
            {
                *lv_p_send_buffer++ = (lv_signal_data.ul >> 24u);
                *lv_p_send_buffer++ = (lv_signal_data.ul >> 16u);
                if (lv_search_addr > lv_send_end_addr)
                {
                    lv_search_addr--;
                }
                else
                {
                    *lv_p_send_buffer++ = (lv_signal_data.ul >> 8u);
                    *lv_p_send_buffer++ = (lv_signal_data.ul);
                }
            }
            else
            {
                *lv_p_send_buffer++ = (lv_signal_data.ui >> 8u);
                *lv_p_send_buffer++ = (lv_signal_data.ui);
            }
        }
        if (lv_search_addr >= lv_send_end_addr)
        {
            break;
        }
    }

    // Pad not used bytes
    while (lv_search_addr < lv_send_end_addr)
    {
        *lv_p_send_buffer++ = 0;
        *lv_p_send_buffer++ = 0;
        lv_search_addr++;
    }

    lv_send_length = ((lv_search_addr - lv_send_start_addr) << 1) + 5;
    lv_buffer[2] = lv_send_length - 5u;

    lv_crc = DefaultCrcCal(lv_buffer, (lv_send_length - 2), 0xFFFFu);
    lv_buffer[lv_send_length - 2] = lv_crc;
    lv_buffer[lv_send_length - 1] = (lv_crc >> 8);

    lv_p_uart->tx_frame_length = lv_send_length;
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: ModbusFunc04Proc
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus func code 4 process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModbusFunc04Proc(UART_DEF *uart)
{
    UART_DEF *lv_p_uart;
    Uint8 *lv_buffer;
    Uint8 const *lv_p_rx_frame_buffer;
    Uint8 lv_send_length;
    Uint16 lv_crc;
    Uint16 lv_send_start_addr;
    Uint16 lv_send_end_addr;
    Uint16 lv_search_addr;
    Uint16 lv_quantity;
    int32  lv_ana_index;
    Uint8  lv_16b_num;
    Uint8 *lv_p_send_buffer;
    SIGNAL_ANALOG const *lv_p_analog;
    SIGNAL_DATA lv_signal_data;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }

    lv_p_uart = uart;
    lv_p_rx_frame_buffer = lv_p_uart->rx_frame_buffer;
    lv_buffer = lv_p_uart->tx_frame_buffer;

    lv_send_start_addr =   (lv_p_rx_frame_buffer[2] << 8)
                         | (lv_p_rx_frame_buffer[3]);
    lv_quantity   =   (lv_p_rx_frame_buffer[4] << 8)
                    | (lv_p_rx_frame_buffer[5]);
    if ((lv_quantity < 1) || (lv_quantity > 125u))
    {
        DebugPrintf("modbus func 3 quanity=%d exceeds the scope [1,2000]\n\r", lv_quantity);
        return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_ILLEGAL_ADDRESS);
    }
    lv_send_end_addr = lv_send_start_addr + lv_quantity;

    lv_buffer[0] = lv_p_rx_frame_buffer[0];
    lv_buffer[1] = lv_p_rx_frame_buffer[1];

    lv_p_send_buffer = &lv_buffer[3];
    lv_search_addr = 0;
    for (lv_ana_index = 0; lv_ana_index < analog_group.num; lv_ana_index++)
    {
        lv_p_analog = &analog_group.analog[lv_ana_index];

        if (0 != (lv_p_analog->invisible & ITEM_DEBUG_INVISIBLE))
        {
             continue;
        }

        if (0 != (lv_p_analog->invisible & ITEM_UART_INVISIBLE))
        {
            continue;
        }
        lv_16b_num = 1;
        lv_signal_data.l = 0;
        switch (lv_p_analog->define->type)
        {
            case SIGNAL_DATA_TYPE_b:
            {
                lv_signal_data.c = *((Vint8 *)lv_p_analog->define->addr);
                lv_signal_data.c = (lv_signal_data.c & 1);
                break;
            }
            case SIGNAL_DATA_TYPE_c:
            {
                lv_signal_data.c = *((Vint8 *)lv_p_analog->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_uc:
            {
                lv_signal_data.uc = *((VUint8 *)lv_p_analog->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            {
                lv_signal_data.i = *((Vint16 *)lv_p_analog->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_ui:
            {
                lv_signal_data.ui = *((VUint16 *)lv_p_analog->define->addr);
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            {
                lv_signal_data.l = *((Vint32 *)lv_p_analog->define->addr);
                lv_16b_num = 2;
                break;
            }
            case SIGNAL_DATA_TYPE_ul:
            {
                lv_signal_data.ul = *((VUint32 *)lv_p_analog->define->addr);
                lv_16b_num = 2;
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                lv_signal_data.f = *((Vfloat *)lv_p_analog->define->addr);
                lv_16b_num = 2;
                break;
            }
            default:
            {
                DebugPrintf("unknow analog type=%d\n\r", lv_p_analog->define->type);
                return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_DEVICE_FAILURE);;
            }
        }

        if (lv_search_addr < lv_send_start_addr)
        {
            lv_search_addr += lv_16b_num;
            if (lv_search_addr > lv_send_start_addr)
            {
                *lv_p_send_buffer++ = (lv_signal_data.ul >> 8u);
                *lv_p_send_buffer++ = (lv_signal_data.ul);
            }
            else
            {
                continue;
            }
        }
        else
        {
            lv_search_addr += lv_16b_num;
            if (2 == lv_16b_num)
            {
                *lv_p_send_buffer++ = (lv_signal_data.ul >> 24u);
                *lv_p_send_buffer++ = (lv_signal_data.ul >> 16u);
                if (lv_search_addr > lv_send_end_addr)
                {
                    lv_search_addr--;
                }
                else
                {
                    *lv_p_send_buffer++ = (lv_signal_data.ul >> 8u);
                    *lv_p_send_buffer++ = (lv_signal_data.ul);
                }
            }
            else
            {
                *lv_p_send_buffer++ = (lv_signal_data.ui >> 8u);
                *lv_p_send_buffer++ = (lv_signal_data.ui);
            }
        }
        if (lv_search_addr >= lv_send_end_addr)
        {
            break;
        }
    }

    if (lv_search_addr < lv_send_end_addr)
    {
        for (lv_ana_index = 0; lv_ana_index < self_diagnose_group.num; lv_ana_index++)
        {
            lv_p_analog = &self_diagnose_group.analog[lv_ana_index];

            if (0 != (lv_p_analog->invisible & ITEM_DEBUG_INVISIBLE))
            {
                 continue;
            }

            if (0 != (lv_p_analog->invisible & ITEM_UART_INVISIBLE))
            {
                continue;
            }
            lv_16b_num = 1;
            lv_signal_data.l = 0;
            switch (lv_p_analog->define->type)
            {
                case SIGNAL_DATA_TYPE_b:
                {
                    lv_signal_data.c = *((Vint8 *)lv_p_analog->define->addr);
                    lv_signal_data.c = (lv_signal_data.c & 1);
                    break;
                }
                case SIGNAL_DATA_TYPE_c:
                {
                    lv_signal_data.c = *((Vint8 *)lv_p_analog->define->addr);
                    break;
                }
                case SIGNAL_DATA_TYPE_uc:
                {
                    lv_signal_data.uc = *((VUint8 *)lv_p_analog->define->addr);
                    break;
                }
                case SIGNAL_DATA_TYPE_i:
                {
                    lv_signal_data.i = *((Vint16 *)lv_p_analog->define->addr);
                    break;
                }
                case SIGNAL_DATA_TYPE_ui:
                {
                    lv_signal_data.ui = *((VUint16 *)lv_p_analog->define->addr);
                    break;
                }
                case SIGNAL_DATA_TYPE_l:
                {
                    lv_signal_data.l = *((Vint32 *)lv_p_analog->define->addr);
                    lv_16b_num = 2;
                    break;
                }
                case SIGNAL_DATA_TYPE_ul:
                {
                    lv_signal_data.ul = *((VUint32 *)lv_p_analog->define->addr);
                    lv_16b_num = 2;
                    break;
                }
                case SIGNAL_DATA_TYPE_f:
                {
                    lv_signal_data.f = *((Vfloat *)lv_p_analog->define->addr);
                    lv_16b_num = 2;
                    break;
                }
                default:
                {
                    DebugPrintf("unknow self_diagnose type=%d\n\r", lv_p_analog->define->type);
                    return ModusSendExceptionFrame(lv_p_uart, 0x03u, MODBUS_EXCEPTION_CODE_DEVICE_FAILURE);;
                }
            }

            if (lv_search_addr < lv_send_start_addr)
            {
                lv_search_addr += lv_16b_num;
                if (lv_search_addr > lv_send_start_addr)
                {
                    *lv_p_send_buffer++ = (lv_signal_data.ul >> 8u);
                    *lv_p_send_buffer++ = (lv_signal_data.ul);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                lv_search_addr += lv_16b_num;
                if (2 == lv_16b_num)
                {
                    *lv_p_send_buffer++ = (lv_signal_data.ul >> 24u);
                    *lv_p_send_buffer++ = (lv_signal_data.ul >> 16u);
                    if (lv_search_addr > lv_send_end_addr)
                    {
                        lv_search_addr--;
                    }
                    else
                    {
                        *lv_p_send_buffer++ = (lv_signal_data.ul >> 8u);
                        *lv_p_send_buffer++ = (lv_signal_data.ul);
                    }
                }
                else
                {
                    *lv_p_send_buffer++ = (lv_signal_data.ui >> 8u);
                    *lv_p_send_buffer++ = (lv_signal_data.ui);
                }
            }
            if (lv_search_addr >= lv_send_end_addr)
            {
                break;
            }
        }
    }
    // Pad not used bytes
    while (lv_search_addr < lv_send_end_addr)
    {
        *lv_p_send_buffer++ = 0;
        *lv_p_send_buffer++ = 0;
        lv_search_addr++;
    }

    lv_send_length = ((lv_search_addr - lv_send_start_addr) << 1) + 5;
    lv_buffer[2] = lv_send_length - 5u;

    lv_crc = DefaultCrcCal(lv_buffer, (lv_send_length - 2), 0xFFFFu);
    lv_buffer[lv_send_length - 2] = lv_crc;
    lv_buffer[lv_send_length - 1] = (lv_crc >> 8);

    lv_p_uart->tx_frame_length = lv_send_length;
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: ModbusFunc06Proc
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus func code 6 process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModbusFunc06Proc(UART_DEF *uart)
{
    UART_DEF *lv_p_uart;
    Uint8 const *lv_p_rx_frame_buffer;
    Uint16 lv_write_addr;
    int32  lv_ret;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }

    if (NULL == setting_system.setting)
    {
        return NORMAL_ERROR;
    }

    lv_p_uart = uart;
    lv_p_rx_frame_buffer = lv_p_uart->rx_frame_buffer;

    lv_write_addr =   (lv_p_rx_frame_buffer[2] << 8)
                    | (lv_p_rx_frame_buffer[3]);

    lv_ret = ModbusWriteSettings(&setting_system, &lv_p_rx_frame_buffer[4], lv_write_addr, 1);
    if (NORMAL_SUCCESS != lv_ret)
    {
        return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], lv_ret);
    }
    else
    {
        memcpy(lv_p_uart->tx_frame_buffer, lv_p_uart->rx_frame_buffer, lv_p_uart->rx_frame_length);
        lv_p_uart->tx_frame_length = lv_p_uart->rx_frame_length;
        return NORMAL_SUCCESS;
    }
}



//----------------------------------------------------------------------------------------------------
//   Function: ModbusFunc16Proc
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus func code 16 process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModbusFunc16Proc(UART_DEF *uart)
{
    Uint8 *lv_buffer;
    UART_DEF *lv_p_uart;
    Uint8 const *lv_p_rx_frame_buffer;
    Uint16 lv_write_start_addr;
    Uint16 lv_quantity;
    Uint16 lv_crc;
    int32  lv_ret;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }

    if (NULL == setting_system.setting)
    {
        return NORMAL_ERROR;
    }


    lv_p_uart = uart;
    lv_p_rx_frame_buffer = lv_p_uart->rx_frame_buffer;
    lv_buffer = lv_p_uart->tx_frame_buffer;

    lv_write_start_addr = (lv_p_rx_frame_buffer[2] << 8)
                    | (lv_p_rx_frame_buffer[3]);
    lv_quantity   =   (lv_p_rx_frame_buffer[4] << 8)
                    | (lv_p_rx_frame_buffer[5]);
    if ((lv_quantity < 1) || (lv_quantity > 125u))
    {
        DebugPrintf("modbus func 3 quanity=%d exceeds the scope [1,2000]\n\r", lv_quantity);
        return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], MODBUS_EXCEPTION_CODE_ILLEGAL_ADDRESS);
    }


    lv_ret = ModbusWriteSettings(&setting_system, &lv_p_rx_frame_buffer[7], lv_write_start_addr, lv_quantity);
    if (NORMAL_SUCCESS != lv_ret)
    {
        return ModusSendExceptionFrame(lv_p_uart, lv_p_rx_frame_buffer[1], lv_ret);
    }
    else
    {
        memcpy(lv_buffer, lv_p_rx_frame_buffer, 6u);
        lv_crc = DefaultCrcCal(lv_buffer, 6, 0xFFFFu);
        lv_buffer[6] = lv_crc;
        lv_buffer[7] = (lv_crc >> 8u);
    }

    lv_p_uart->tx_frame_length = 8;
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: ModusSendExceptionFrame
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus application process
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModusSendExceptionFrame(UART_DEF *uart, Uint8 func_code, Uint8 reason)
{
    Uint8 *lv_buffer;
    Uint16 lv_crc;
    UART_DEF *lv_p_uart;

    if (NULL == uart)
    {
        DebugPrintf("function entrys error!\r\n");
        return NORMAL_ERROR;
    }
    lv_p_uart = uart;
    lv_buffer = lv_p_uart->tx_frame_buffer;

    lv_buffer[0] = uart->comm_addr;
    lv_buffer[1] = 0x80u | func_code;
    lv_buffer[2] = reason;

    lv_crc = DefaultCrcCal(lv_buffer, 3, 0xFFFFu);
    lv_buffer[3] = lv_crc;
    lv_buffer[4] = (lv_crc >> 8);

    lv_p_uart->tx_frame_length = 5;
    return NORMAL_SUCCESS;
}

//----------------------------------------------------------------------------------------------------
//   Function: ModbusWriteSettings
//      Input: void
//     Output: void
//     Return: int32: function status
//Description: Modbus set settings
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static int32 ModbusWriteSettings(SETTING_GROUP *p_setting_group, Uint8 const *value_src, Uint16 set_start_addr, Uint16 quantity)
{
    Uint8 const *lv_p_value_buffer;
    Uint16 lv_write_start_addr;
    Uint16 lv_write_end_addr;
    Uint16 lv_search_addr;
    Uint16 lv_current_addr;
    Uint16 lv_quantity;
    int32  lv_set_index;
    int32  lv_valid_set_index;
    SETTING const *lv_p_setting;
    SIGNAL_DATA lv_value;

    if ((NULL == p_setting_group) || (NULL == value_src))
    {
        DebugPrintf("Function entry error!\r\n");
        return MODBUS_EXCEPTION_CODE_DEVICE_FAILURE;
    }

    if (0 == quantity)
    {
        DebugPrintf("No setting quantity\n\r");
        return MODBUS_EXCEPTION_CODE_ILLEGAL_VALUE;
    }

    lv_write_start_addr = set_start_addr;
    lv_quantity = quantity;
    lv_write_end_addr = lv_write_start_addr + lv_quantity;

    lv_value.ul = 0;
    lv_p_value_buffer = value_src;
    lv_valid_set_index = 0;
    lv_search_addr = 0;
    for (lv_set_index = 0; lv_set_index < p_setting_group->num; lv_set_index++)
    {
        lv_p_setting = &p_setting_group->setting[lv_set_index];

        if (0 != (lv_p_setting->invisible & ITEM_DEBUG_INVISIBLE))
        {
             continue;
        }
        lv_valid_set_index++;

        if (0 != (lv_p_setting->invisible & ITEM_UART_INVISIBLE))
        {
            continue;
        }

        lv_value.ul = 0;
        lv_current_addr = lv_search_addr;
        switch (lv_p_setting->define->type)
        {
            case SIGNAL_DATA_TYPE_b:
            case SIGNAL_DATA_TYPE_c:
            case SIGNAL_DATA_TYPE_uc:
            {
                if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.uc = (lv_p_value_buffer[0] << 8) | (lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                lv_search_addr += 1;
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            case SIGNAL_DATA_TYPE_ui:
            {
                if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.ui = (lv_p_value_buffer[0] << 8) | (lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                lv_search_addr += 1;
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            case SIGNAL_DATA_TYPE_ul:
            {

                if ((lv_search_addr + 1) == lv_write_start_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0xFFFF0000u)
                                   | ((lv_p_value_buffer[0] << 8) | lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                else if ((lv_search_addr + 1) == lv_write_end_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0x0000FFFFu)
                                  | ((lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16));
                    lv_p_value_buffer += 2;
                }
                else if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.ul =   (lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16)
                                  | (lv_p_value_buffer[2] << 8) | (lv_p_value_buffer[3]);
                    lv_p_value_buffer += 4;
                }
                lv_search_addr += 2;
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                if ((lv_search_addr + 1) == lv_write_start_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0xFFFF0000u)
                                   | ((lv_p_value_buffer[0] << 8) | lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                else if ((lv_search_addr + 1) == lv_write_end_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0x0000FFFFu)
                                  | ((lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16));
                    lv_p_value_buffer += 2;
                }
                else if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.ul =   (lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16)
                                  | (lv_p_value_buffer[2] << 8) | (lv_p_value_buffer[3]);
                    lv_p_value_buffer += 4;
                }
                lv_search_addr += 2;
                break;

            }
            default:
            {
                DebugPrintf("unknow setting type=%d\n\r", lv_p_setting->define->type);
                return MODBUS_EXCEPTION_CODE_DEVICE_FAILURE;
            }
        }
        if (lv_search_addr > lv_write_start_addr)
        {
            if (NORMAL_SUCCESS != SettingValueValidationCheck(&setting_system, lv_value, lv_set_index))
            {
                DebugPrintf("Modbus addr=%d, set index=%d value validation check failed\r\n", lv_current_addr, lv_valid_set_index);
                return MODBUS_EXCEPTION_CODE_ILLEGAL_VALUE;
            }
        }
        if (lv_search_addr >= lv_write_end_addr)
        {
            break;
        }
    }

    if (lv_search_addr < lv_write_end_addr)
    {
        return MODBUS_EXCEPTION_CODE_ILLEGAL_ADDRESS;
    }

    lv_value.ul = 0;
    lv_p_value_buffer = value_src;
    lv_valid_set_index = 0;
    lv_search_addr = 0;
    lv_p_setting = &setting_system.setting[0];
    for (lv_set_index = 0; lv_set_index < setting_system.num; lv_set_index++)
    {
        lv_p_setting = &setting_system.setting[lv_set_index];

        if (0 != (lv_p_setting->invisible & ITEM_DEBUG_INVISIBLE))
        {
             continue;
        }
        lv_valid_set_index++;

        if (0 != (lv_p_setting->invisible & ITEM_UART_INVISIBLE))
        {
            continue;
        }

        lv_value.ul = 0;
        lv_current_addr = lv_search_addr;
        switch (lv_p_setting->define->type)
        {
            case SIGNAL_DATA_TYPE_b:
            case SIGNAL_DATA_TYPE_c:
            case SIGNAL_DATA_TYPE_uc:
            {
                if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.uc = (lv_p_value_buffer[0] << 8) | (lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                lv_search_addr += 1;
                break;
            }
            case SIGNAL_DATA_TYPE_i:
            case SIGNAL_DATA_TYPE_ui:
            {
                if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.ui = (lv_p_value_buffer[0] << 8) | (lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                lv_search_addr += 1;
                break;
            }
            case SIGNAL_DATA_TYPE_l:
            case SIGNAL_DATA_TYPE_ul:
            {

                if ((lv_search_addr + 1) == lv_write_start_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0xFFFF0000u)
                                   | ((lv_p_value_buffer[0] << 8) | lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                else if ((lv_search_addr + 1) == lv_write_end_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0x0000FFFFu)
                                  | ((lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16));
                    lv_p_value_buffer += 2;
                }
                else if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.ul =   (lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16)
                                  | (lv_p_value_buffer[2] << 8) | (lv_p_value_buffer[3]);
                    lv_p_value_buffer += 4;
                }
                lv_search_addr += 2;
                break;
            }
            case SIGNAL_DATA_TYPE_f:
            {
                if ((lv_search_addr + 1) == lv_write_start_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0xFFFF0000u)
                                   | ((lv_p_value_buffer[0] << 8) | lv_p_value_buffer[1]);
                    lv_p_value_buffer += 2;
                }
                else if ((lv_search_addr + 1) == lv_write_end_addr)
                {
                    lv_value.ul = *((VUint32 *)lv_p_setting->define->addr);
                    lv_value.ul =    (lv_value.ul & 0x0000FFFFu)
                                  | ((lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16));
                    lv_p_value_buffer += 2;
                }
                else if (lv_search_addr >= lv_write_start_addr)
                {
                    lv_value.ul =   (lv_p_value_buffer[0] << 24) | (lv_p_value_buffer[1] << 16)
                                  | (lv_p_value_buffer[2] << 8) | (lv_p_value_buffer[3]);
                    lv_p_value_buffer += 4;
                }
                lv_search_addr += 2;
                break;

            }
            default:
            {
                DebugPrintf("unknow setting type=%d\n\r", lv_p_setting->define->type);
                return MODBUS_EXCEPTION_CODE_DEVICE_FAILURE;
            }
        }
        if (lv_search_addr > lv_write_start_addr)
        {
            if (NORMAL_SUCCESS != SetOneWriteSettingValue(&setting_system, lv_value, lv_set_index))
            {
                DebugPrintf("Modbus addr=%d, set index=%d set value failed\r\n", lv_current_addr, lv_valid_set_index);
                return MODBUS_EXCEPTION_CODE_ILLEGAL_VALUE;
            }
        }
        if (lv_search_addr >= lv_write_end_addr)
        {
            break;
        }
    }

    LowLevelInterruptLock();
    if (    (NORMAL_SUCCESS != RunSettingUpdateFromWriteSetting(&setting_system))
         || (NORMAL_SUCCESS != UserAppSettingHandle(lv_p_setting->define->addr, lv_quantity))
         || (NORMAL_SUCCESS != SetSettingBufferFromSettingAddr(&setting_system))
         || (NORMAL_SUCCESS != SettingWriteToFlash(&setting_system))
         || (NORMAL_SUCCESS != CheckRunSettingWithFlash(&setting_system))
       )
    {
        LowLevelInterruptUnlock();
        DebugPrintf("Device settings write failed!\r\n");
        return MODBUS_EXCEPTION_CODE_DEVICE_FAILURE;
    }
    LowLevelInterruptUnlock();

    return NORMAL_SUCCESS;
}


