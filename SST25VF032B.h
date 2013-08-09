#ifndef _SST25_H
#define _SST25_H

/************************************************************************
* EEPROM Commands                                                       
************************************************************************/
#define SST25_PAGE_SIZE    (unsigned)64
#define SST25_PAGE_MASK    (unsigned)0x003f
#define SST25_CMD_READ     (unsigned)0x03
#define SST25_CMD_WRITE    (unsigned)0x02
#define SST25_CMD_WRDI     (unsigned)0x04
#define SST25_CMD_WREN     (unsigned)0x06
#define SST25_CMD_RDSR     (unsigned)0x05
#define SST25_CMD_WRSR     (unsigned)0x01
#define SST25_CMD_ERASE     (unsigned)0x60
#define SST25_CMD_EWSR      (unsigned)0x50
#define SST25_CMD_SER       (unsigned)0x20

/************************************************************************
* Aliases for IOs registers related to SPI connected to EEPROM
************************************************************************/

#define SST25_SS_TRIS       TRISCbits.TRISC13
#define SST25_SS_PORT       PORTCbits.RC13

#define EEPROM_SCK_TRIS      TRISGbits.TRISG6
#define EEPROM_SDO_TRIS      TRISGbits.TRISG8
#define EEPROM_SDI_TRIS      TRISGbits.TRISG7

#define EEPROM_SCK_PORT      PORTGbits.RG6
#define EEPROM_SDO_PORT      PORTGbits.RG8
#define EEPROM_SDI_PORT      PORTGbits.RG7


void SST25Init(void);

void SST25WriteByte(BYTE data, DWORD address);

BYTE SST25ReadByte(DWORD address);

void SST25WriteWord(WORD data, DWORD address);

WORD SST25ReadWord(DWORD address);

BYTE SST25IsWriteBusy(void);

BYTE SST25WriteArray(DWORD address, BYTE *pData, WORD nCount);

void SST25ReadArray(DWORD address, BYTE *pData, WORD nCount);

void SST25ChipErase(void);

void SST25SectorErase(DWORD address);











#endif
