/*****************************************************************************************************
* FileName:                    SpiFlash.h
*
* Description:                 SpiFlash操作相关头文件
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Spi_Flash_H
#define _Spi_Flash_H

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// 宏定义
//====================================================================================================
#define SPI_FLASH_STATUS_BUSY_MASK   0x01u// Write-in-Progress

#define SPI_FLASH_CMD_WRENABLE       0x06u
#define SPI_FLASH_CMD_WRDISABLE      0x04u
#define SPI_FLASH_CMD_RD_STATUS      0x05u
#define SPI_FLASH_CMD_WR_STATUS      0x01u
#define SPI_FLASH_CMD_READ           0x03u
#define SPI_FLASH_CMD_WRITE          0x02u
#define SPI_FLASH_CMD_FASTREAD       0x0Bu
#define SPI_FLASH_CMD_FASTREAD_DUAL_OUTPUT 0x3Bu
#define SPI_FLASH_CMD_BLOCKERASE         0xD8u
#define SPI_FLASH_CMD_SECTORERASE        0x20u
#define SPI_FLASH_CMD_CHIPERASE          0xC7u
#define SPI_FLASH_CMD_POWERDOWN          0xB9u
#define SPI_FLASH_CMD_SIMPLE_ID          0x90u
#define SPI_FLASH_CMD_JEDEC_ID           0x9Fu
#define SPI_FLASH_DUMMY_BYTE             0x00u
#define SPI_FLASH_PAGE_SIZE              0x100u
#define SPI_FLASH_SECTOR_SIZE            0x1000u
#define SPI_FLASH_PROG_TIMEOUT          (100000u)
#define SPI_FLASH_PAGE_ERASE_TIMEOUT    (500000u)
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT  (10000000u)
#define SPI_FLASH_BLOCK_ERASE_TIMEOUT   (10000000u)
#define SPI_FLASH_CHIP_ERASE_TIMEOUT    (100000000u)

#define pSPIFLASH_CPU  pSPI1
#define pSPIFLASH_FPGA pSPI2

//====================================================================================================
// 外部函数声明
//====================================================================================================
extern int32 SpiFlashRead(SPI_REGS *p_spiflash, int32 addr, Uint8 *buffer, int32 length);
extern int32 SpiFlashWrite(SPI_REGS *p_spiflash, int32 addr, Uint8 const *buffer, int32 length);
extern int32 SpiFlashErase(SPI_REGS *p_spiflash, int32 addr, int32 length);
extern int32 SpiFlashChipErase(SPI_REGS *p_spiflash);

#ifdef __cplusplus
}
#endif

#endif

