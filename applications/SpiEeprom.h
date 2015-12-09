/*****************************************************************************************************
* FileName:                    SpiEeprom.h
*
* Description:                 SpiEeprom操作相关头文件
*
* Author:                      YanDengxue, Fiberhome-Fuhua
*
* Rev History:
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Spi_Eeprom_H
#define _Spi_Eeprom_H

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// 宏定义
//====================================================================================================
#define X25650_CLK          3000000u
#define X25650_DATA_LENGTH  8u
#define X25650_PRESCALE     (((FREQ_PLL0_SYSCLK2 / X25650_CLK) - 1u) & 0xffu)

#define X25650_CMD_WREN      0x06u
#define X25650_CMD_WRDI      0x04u
#define X25650_CMD_RDSR      0x05u
#define X25650_CMD_WRSR      0x01u
#define X25650_CMD_READ      0x03u
#define X25650_CMD_WRITE     0x02u

#define X25650_PAGE_SIZE     0x20u

#define X25650_STATUS_WIP    0x01u
#define X25650_STATUS_WEL    0x02u

// Common parameters
#define X25650_W25_CFG_HZ            1000u
#define X25650_PROG_TIMEOUT          ((100000u * X25650_W25_CFG_HZ) / 1000u)

#define pSPIEEPROM pSPI1

//====================================================================================================
// 外部函数声明
//====================================================================================================
extern int32 X25650ClaimBus(void);
extern int32 X25650ReleaseBus(void);
extern int32 X25650Read(int32 addr, int32 length, Uint8 *buffer);
extern int32 X25650Write(int32 addr, int32 length, Uint8 const *buffer);

#ifdef __cplusplus
}
#endif

#endif

