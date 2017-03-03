/*****************************************************************************************************
* FileName:                    Storage.c
*
* Description:                 Internal flash operation functions
*
* Author:                      YanDengxue, SmartSaw
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
//====================================================================================================
// Header file declaration
//====================================================================================================
// Lib header

// Self-define header
#include "STM32F40xRegister.h"
#include "SystemBase.h"
#include "Storage.h"
#include "SpiFlash.h"
#include "stm32f4xx_flash.h"

//====================================================================================================
// Structure and Macro define
//====================================================================================================
typedef struct
{
    Uint16 sector_no;
    Uint32 start_addr;
    int32  sector_length;
} FLASH_INTERNAL_SECTOR_INFO;

//====================================================================================================
// Local functions decalation, these functions can not be accessed by outside files
//====================================================================================================
static FLASH_INTERNAL_SECTOR_INFO const *FlashInternalGetSectorInfo(Uint32 addr);

//====================================================================================================
// Local variables, these variables can not be accessed by outside files
//====================================================================================================
static const FLASH_INTERNAL_SECTOR_INFO flash_internal_sector_info[12] =
{
    {0x0000u, 0x08000000u, 0x00004000u},
    {0x0008u, 0x08004000u, 0x00004000u},
    {0x0010u, 0x08008000u, 0x00004000u},
    {0x0018u, 0x0800C000u, 0x00004000u},
    {0x0020u, 0x08010000u, 0x00010000u},
    {0x0028u, 0x08020000u, 0x00020000u},
    {0x0030u, 0x08040000u, 0x00020000u},
    {0x0038u, 0x08060000u, 0x00020000u},
    {0x0040u, 0x08080000u, 0x00020000u},
    {0x0048u, 0x080A0000u, 0x00020000u},
    {0x0050u, 0x080C0000u, 0x00020000u},
    {0x0058u, 0x080E0000u, 0x00020000u}
};

static int8 flash_op_flag;

//====================================================================================================
// Functions realize
//====================================================================================================

//----------------------------------------------------------------------------------------------------
// Interface functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: StorageRead
//      Input: Uint32 addr, Uint8 *src, int32 length
//     Output: void
//     Return: int32: successful write length
//Description: Internal flash write
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 StorageRead(Uint8 storage_type, Uint32 addr, Uint8 *dest, int32 length)
{
    Uint32 lv_addr_end;
    Uint32 lv_addr;
    Uint8 const *lv_p_src;
    Uint8 *lv_p_dest;
    int32  lv_length;
    int32  lv_index;

    if (NULL == dest)
    {
        return -2;
    }

    lv_p_dest = dest;
    lv_addr = addr;
    lv_length = length;
    lv_addr_end = lv_addr + lv_length;
    if (STORAGE_TYPE_SPI_FLASH_CPU == storage_type)
    {
        if (0 != flash_op_flag)
        {
            DebugPrintf("Storage is busy!\r\n");
            return RET_STORAGE_BUSY;
        }

        flash_op_flag = 1;
        lv_length = SpiFlashRead(pSPIFLASH_CPU, lv_addr, lv_p_dest, lv_length);
        flash_op_flag = 0;
        return lv_length;
    }
    else
    {
        if ((lv_addr < ADDR_FLASH_SECTOR_00) || (lv_addr_end >= ADDR_FLASH_SECTOR_END))
        {
            return -3;
        }

        if (0 == length)
        {
            return 0;
        }

        lv_p_src = (Uint8 const *)lv_addr;
        for (lv_index = 0; lv_index < lv_length; lv_index++)
        {
            lv_p_dest[lv_index] = lv_p_src[lv_index];
        }

        return length;
    }

}

//----------------------------------------------------------------------------------------------------
//   Function: StorageWrite
//      Input: Uint32 addr, Uint8 const *src, int32 length
//     Output: void
//     Return: int32: successful write length
//Description: Internal flash write
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 StorageWrite(Uint8 storage_type, Uint32 addr, Uint8 const *src, int32 length)
{
    Uint32 lv_addr_end;
    Uint32 lv_addr;
    Uint8 const *lv_p_src;
    int32  lv_addr_word_end;
    int32  lv_addr_halfword_end;
    int32  lv_length;

    if (NULL == src)
    {
        return -2;
    }
    if (0 != flash_op_flag)
    {
        DebugPrintf("Storage is busy!\r\n");
        return RET_STORAGE_BUSY;
    }
    flash_op_flag = 1;

    lv_p_src = src;
    lv_addr = addr;
    lv_length = length;
    lv_addr_end = lv_addr + lv_length;
    if (STORAGE_TYPE_SPI_FLASH_CPU == storage_type)
    {
        lv_length = SpiFlashWrite(pSPIFLASH_CPU, lv_addr, lv_p_src, length);
        flash_op_flag = 0;
        return lv_length;
    }
    else
    {
        if ((lv_addr < ADDR_FLASH_SECTOR_00) || (lv_addr_end >= ADDR_FLASH_SECTOR_END))
        {
            flash_op_flag = 0;
            return -3;
        }

        if (0 == length)
        {
            flash_op_flag = 0;
            return 0;
        }

        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                      FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

        if (0 == (lv_addr & 3))
        {
            lv_addr_word_end = (lv_addr_end & (~3));
            while (lv_addr < lv_addr_word_end)
            {
                if (FLASH_COMPLETE != FLASH_ProgramWord(lv_addr, *(Uint32 *)lv_p_src))
                {
                    FLASH_Lock();
                    DebugPrintf("Internal flash write word at addr %08X failed!\r\n", lv_addr);
                    flash_op_flag = 0;
                    return NORMAL_ERROR;
                }
                lv_addr += 4;
                lv_p_src += 4;
            }
        }

        if (0 == (lv_addr & 1))
        {
            lv_addr_halfword_end = (lv_addr_end & (~1));
            while (lv_addr < lv_addr_halfword_end)
            {
                if (FLASH_COMPLETE != FLASH_ProgramHalfWord(lv_addr, *(Uint16 *)lv_p_src))
                {
                    FLASH_Lock();
                    DebugPrintf("Internal flash write half word at addr %08X failed!\r\n", lv_addr);
                    flash_op_flag = 0;
                    return NORMAL_ERROR;
                }
                lv_addr += 2;
                lv_p_src += 2;
            }

        }

        while (lv_addr < lv_addr_end)
        {
            if (FLASH_COMPLETE != FLASH_ProgramByte(lv_addr, *lv_p_src))
            {
                FLASH_Lock();
                DebugPrintf("Internal flash write byte at addr %08X failed!\r\n", lv_addr);
                flash_op_flag = 0;
                return NORMAL_ERROR;
            }
            lv_addr += 1;
            lv_p_src += 1;
        }

        FLASH_Lock();
        flash_op_flag = 0;

        return length;
    }

}

//----------------------------------------------------------------------------------------------------
//   Function: Storage
//      Input: Uint32 addr
//     Output: void
//     Return: int32: successful erase length
//Description: Internal flash erase
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
int32 StorageErase(Uint8 storage_type, Uint32 addr, int32 length)
{
    FLASH_INTERNAL_SECTOR_INFO const *lv_p_flash_sector_info;
    Uint32 lv_flash_end_addr;
    Uint32 lv_flash_erase_addr;
    int32  lv_length;

    if (0 != flash_op_flag)
    {
        DebugPrintf("Storage is busy!\r\n");
        return RET_STORAGE_BUSY;
    }
    flash_op_flag = 1;

    if (STORAGE_TYPE_SPI_FLASH_CPU == storage_type)
    {
        lv_length = SpiFlashErase(pSPIFLASH_CPU, addr, length);
        flash_op_flag = 0;
        return lv_length;
    }
    else if (STORAGE_TYPE_SPI_FLASH_FPGA == storage_type)
    {
        lv_length = SpiFlashErase(pSPIFLASH_FPGA, addr, length);
        flash_op_flag = 0;
        return lv_length;
    }
    else
    {
        lv_flash_end_addr = addr + length;
        lv_flash_erase_addr = addr;
        FLASH_Unlock();
        while (lv_flash_erase_addr < lv_flash_end_addr)
        {
            lv_p_flash_sector_info = FlashInternalGetSectorInfo(lv_flash_erase_addr);
            if (NULL == lv_p_flash_sector_info)
            {
                FLASH_Lock();
                DebugPrintf("Get flash sector number at address 0x%08X failed!\r\n", addr);
                flash_op_flag = 0;
                return NORMAL_ERROR;
            }
            else if (lv_flash_erase_addr != lv_p_flash_sector_info->start_addr)
            {
                FLASH_Lock();
                DebugPrintf("Flash must be erase from the sector start addr:0x%08X, but addr 0x%08X exceed these scope!\r\n", lv_p_flash_sector_info->start_addr, lv_flash_erase_addr);
                flash_op_flag = 0;
                return NORMAL_ERROR;
            }

            FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                      FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

            if (FLASH_COMPLETE != FLASH_EraseSector(lv_p_flash_sector_info->sector_no, VoltageRange_3))
            {
                FLASH_Lock();
                DebugPrintf("Internal flash sector=%d erase failed!\r\n", lv_p_flash_sector_info->sector_no);
                flash_op_flag = 0;
                return NORMAL_ERROR;
            }

            lv_flash_erase_addr += lv_p_flash_sector_info->sector_length;
        }

        FLASH_Lock();

        flash_op_flag = 0;

        return (lv_flash_erase_addr - addr);
    }
}

//----------------------------------------------------------------------------------------------------
// Local functions
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//   Function: FlashInternalGetSectorInfo
//      Input: Uint32 addr
//     Output: void
//     Return: FLASH_INTERNAL_SECTOR_INFO const *: info related to internal flash
//Description: get internal flash sector information
//    <AUTHOR>        <MODIFYTIME>            <REASON>
//   YanDengxue     2011-03-21 16:30           Create
//----------------------------------------------------------------------------------------------------
static FLASH_INTERNAL_SECTOR_INFO const *FlashInternalGetSectorInfo(Uint32 addr)
{
    if ((addr < ADDR_FLASH_SECTOR_00) || (addr >= ADDR_FLASH_SECTOR_END))
    {
        return NULL;
    }
    else if (addr < ADDR_FLASH_SECTOR_04)
    {
        return &flash_internal_sector_info[((addr >> 14) & 0x3)];
    }
    else if (addr < ADDR_FLASH_SECTOR_05)
    {
        return &flash_internal_sector_info[4];
    }
    else
    {
        return &flash_internal_sector_info[4 + ((addr >> 17) & 0xF)];
    }
}


