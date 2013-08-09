#include "MainDemo.h"
/************************************************************************
* Function: SST25Init                                                  
*                                                                       
* Overview: this function setup SPI and IOs connected to SST25
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void SST25Init(void)
{

    // Initialize SPI2

/*   
    SPI2STAT = 0;
    SPI2CON1 = 0x001b;
    SPI2CON1bits.MSTEN = 1;
    SPI2CON2 = 0;
    SPI2CON1bits.MODE16 = 0;
    SPI2CON1bits.CKE = 0;
    SPI2CON1bits.CKP = 1;
    SPI2CON1bits.SMP = 1;
    SPI2STATbits.SPIEN = 1;
*/
    // Set IOs directions for EEPROM SPI
    SST25_SS_TRIS = 0;
    SST25_SS_PORT = 1;
	Nop();
    Nop();
	Nop();
    EEPROM_SCK_TRIS = 0;
	Nop();
    EEPROM_SDO_TRIS = 0;
    Nop();
	Nop();
    EEPROM_SDI_TRIS = 1;

    SST25ResetWriteProtection();
}


/************************************************************************
* Macro: EEPROMSSLow()                                                   
*                                                                       
* Preconditions: SS IO must be configured as output
*                                                                       
* Overview: this macro pulls down SS line                    
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
#define SST25SSLow()      SST25_SS_PORT=0;


/************************************************************************
* Macro: EEPROMSSHigh()
*                                                                       
* Preconditions: SS IO must be configured as output
*                                                                       
* Overview: this macro set SS line to high level
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
#define SST25SSHigh()     SST25_SS_PORT=1;
/************************************************************************
* Function: void SST25WriteByte(BYTE data, DWORD address)                                           
*                                                                       
* Overview: this function writes a byte to the address specified
*                                                                       
* Input: data to be written and address
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void SST25WriteByte(BYTE data, DWORD address)
{
   // SST25SectorErase(address);
	//DelayMs(100);	
    SST25WriteEnable();
    SST25SSLow();

    SPIPut(SST25_CMD_WRITE);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[0]);
    SPIGet();

    SPIPut(data);
    SPIGet();

    SST25SSHigh();

    // Wait for write end
    while(SST25IsWriteBusy());
}

/************************************************************************
* Function: BYTE SST25ReadByte(DWORD address)             
*                                                                       
* Overview: this function reads a byte from the address specified         
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
BYTE SST25ReadByte(DWORD address)
{
    BYTE    temp;
    SST25SSLow();

    SPIPut(SST25_CMD_READ);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[0]);
    SPIGet();

    SPIPut(0);
    temp = SPIGet();

    SST25SSHigh();
    return (temp);
}

/************************************************************************
* Function: void SST25WriteWord(WODR data, DWORD address)                                           
*                                                                       
* Overview: this function writes a 16-bit word to the address specified
*                                                                       
* Input: data to be written and address
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
void SST25WriteWord(WORD data, DWORD address)
{

    SST25WriteByte(((WORD_VAL) data).v[0], address);
    SST25WriteByte(((WORD_VAL) data).v[1], address + 1);
}

/************************************************************************
* Function: WORD SST25ReadWord(DWORD address)             
*                                                                       
* Overview: this function reads a 16-bit word from the address specified         
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
WORD SST25ReadWord(DWORD address)
{
    WORD_VAL    temp;

    temp.v[0] = SST25ReadByte(address);
    temp.v[1] = SST25ReadByte(address + 1);

    return (temp.Val);
}

/************************************************************************
* Function: SST25WriteEnable()                                         
*                                                                       
* Overview: this function allows write/erase SST25. Must be called  
* before every write/erase command.                                         
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void SST25WriteEnable(void)
{
    SST25SSLow();
    SPIPut(SST25_CMD_WREN);
    SPIGet();
    SST25SSHigh();
}

/************************************************************************
* Function: BYTE SST25IsWriteBusy(void)  
*                                                                       
* Overview: this function reads status register and chek BUSY bit for write operation
*                                                                       
* Input: none                                                          
*                                                                       
* Output: non zero if busy
*                                                                       
************************************************************************/
BYTE SST25IsWriteBusy(void)
{
    BYTE    temp;

    SST25SSLow();
    SPIPut(SST25_CMD_RDSR);
    SPIGet();

    SPIPut(0);
    temp = SPIGet();
    SST25SSHigh();

    return (temp & 0x01);
}

/************************************************************************
* Function: BYTE SST25WriteArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function writes a data array at the address specified
*                                                                       
* Input: flash memory address, pointer to the data array, data number
*                                                                       
* Output: return 1 if the operation was successfull
*                                                                     
************************************************************************/
BYTE SST25WriteArray(DWORD address, BYTE *pData, WORD nCount)
{
    DWORD   addr;
    BYTE    *pD;
    WORD    counter;

    addr = address;
    pD = pData;

    // WRITE
    for(counter = 0; counter < nCount; counter++)
    {
        SST25WriteByte(*pD++, addr++);
    }

    // VERIFY
    for(counter = 0; counter < nCount; counter++)
    {
        if(*pData != SST25ReadByte(address))
            return (0);
        pData++;
        address++;
    }

    return (1);
}

/************************************************************************
* Function: void SST25ReadArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function reads data into buffer specified
*                                                                       
* Input: flash memory address, pointer to the data buffer, data number
*                                                                       
************************************************************************/
void SST25ReadArray(DWORD address, BYTE *pData, WORD nCount)
{
    SST25SSLow();

    SPIPut(SST25_CMD_READ);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[0]);
    SPIGet();

    while(nCount--)
    {
        SPIPut(0);
        *pData++ = SPIGet();
    }

    SST25SSHigh();
}

/************************************************************************
* Function: void SST25ChipErase(void)
*                                                                       
* Overview: chip erase
*                                                                       
* Input: none
*                                                                       
************************************************************************/
void SST25ChipErase(void)
{
    SST25WriteEnable();

    SST25SSLow();

    SPIPut(SST25_CMD_ERASE);
    SPIGet();

    SST25SSHigh();

    // Wait for write end
    while(SST25IsWriteBusy());
}

/************************************************************************
* Function: void SST25ResetWriteProtection()
*                                                                       
* Overview: this function reset write protection bits
*                                                                       
* Input: none                                                     
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void SST25ResetWriteProtection(void)
{
    SST25SSLow();

    SPIPut(SST25_CMD_EWSR);
    SPIGet();

    SST25SSHigh();

    SST25SSLow();

    SPIPut(SST25_CMD_WRSR);
    SPIGet();

    SPIPut(0);
    SPIGet();

    SST25SSHigh();
}

/************************************************************************
* Function: void SST25SectorErase(DWORD address)                                           
*                                                                       
* Overview: this function erases a 4Kb sector
*                                                                       
* Input: address within sector to be erased
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void SST25SectorErase(DWORD address)
{
    SST25WriteEnable();
    SST25SSLow();

    SPIPut(SST25_CMD_SER);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL) address).v[0]);
    SPIGet();

    SST25SSHigh();

    // Wait for write end
    DelayMs(100);
    while(SST25IsWriteBusy());
}