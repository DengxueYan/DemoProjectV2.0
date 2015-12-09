/*****************************************************************************************************
* FileName     :    Storage.h
*
* Reference    :    NULL
*
* Description  :    Internal flash operation
*
* History      :
*       <Author>        <Data>        <Hardware>     <Version>        <Description>
*     YanDengxue   2011-03-29 15:30       --           1.00             Create
*****************************************************************************************************/
#ifndef _Storage_H_
#define _Storage_H_

//====================================================================================================
// Refer outside header files
//====================================================================================================
#include "UserTypesDef.h"
#include "Debug.h"

#ifdef __cplusplus
extern "C" {
#endif

//====================================================================================================
// Macro
//====================================================================================================
//----------------------------------------------------------------------------------------------------
// flash type define
//----------------------------------------------------------------------------------------------------
#define STORAGE_TYPE_FLASH_INTERNAL    0x01u
#define STORAGE_TYPE_SPI_FLASH_CPU    0x41u
#define STORAGE_TYPE_SPI_FLASH_FPGA   0x81u

#define STORAGE_TYPE_MAIN_STORAGE   STORAGE_TYPE_FLASH_INTERNAL
#define STORAGE_TYPE_STORE_USR_APP  STORAGE_TYPE_FLASH_INTERNAL
#define STORAGE_TYPE_STORE_DEV_CFG  STORAGE_TYPE_FLASH_INTERNAL
#define STORAGE_TYPE_STORE_PRJ_CFG  STORAGE_TYPE_FLASH_INTERNAL

#define STORAGE_TYPE_STORE_SETTINGS STORAGE_TYPE_SPI_FLASH_CPU
#define STORAGE_TYPE_STORE_CALIBRATE_PARAMETERS STORAGE_TYPE_SPI_FLASH_CPU

#define STORAGE_TYPE_STORE_FPGA     STORAGE_TYPE_SPI_FLASH_FPGA


//----------------------------------------------------------------------------------------------------
// flash address
//----------------------------------------------------------------------------------------------------
#define ADDR_FLASH_SECTOR_00     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_01     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_02     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_03     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_04     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_05     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_06     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_07     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_08     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_09     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10     ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11     ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
#define ADDR_FLASH_SECTOR_END    ((uint32_t)0x08100000) /* Base @ of Sector 11, 128 Kbytes */

//----------------------------------------------------------------------------------------------------
// other
//----------------------------------------------------------------------------------------------------
#define RET_STORAGE_BUSY  -5

//====================================================================================================
// External funtion declaration
//====================================================================================================
extern int32 StorageRead(Uint8 storage_type, Uint32 addr, Uint8 *src, int32 length);
extern int32 StorageWrite(Uint8 storage_type, Uint32 addr, Uint8 const *src, int32 length);
extern int32 StorageErase(Uint8 storage_type, Uint32 addr, int32 length);
extern int32 CPUAppDownload(Uint8 const *src, int32 length, Uint32 flash_addr);

#ifdef __cplusplus
}
#endif

#endif

