/* ****************************************************************************
 * Author    Spring.zheng           Date   2008/07/31     Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Define for the line position		 	...
 *****************************************************************************/
#ifndef _DEFINEXY_H
#define _DEFINEXY_H

#define positionax	2//20
#define positionay	50
#define positionsizex	16
#define positionsizey	32
#define positionmaxx	32
#define positionmaxy	16
#define position		0
#define positionItem	22//20


#define SubjectNameMax		16
#define PlaceMax				16
#define VehicleNoMax			16
#define BadgeNoMax			18	// ��ʻ֤����
#define OperatorNameMax	16
#define DepartmentMax		16
#define ItemMax				16

#define	RecordSizeMax	(256)

#define USE_BACKLIGHT
//#define	USE_SLEEP_TIME
#define	DISPLAY_BATTERY
#define	USE_PIC24FJ256GB110
#define	USE_SPI_DataFlash
#define	USE_EEPROM_RECORD	// For the EEOROM Record
#define SPFD5408B	//
#define T5

#define SaveState	// for the Check box
//#define Use_Print
extern volatile DWORD tick;                     // tick counter
extern volatile XCHAR Flashbuff[RecordSizeMax];


// FOR the ID ������
#define ID_JIQIADDRESS	(0X6A00)
#define DebugADDRESS  (0x6A01)
// FOR the checkbox select
#define ID_CHECKBOXADDRESS	(0X7E00)

// for the RadioButton select
#define RButtonAddress		(0x7F18)
#define BackLightAddress		(0x7F18)
#define PrintSelectAddress	(0x7F1A)
#define ThicknessAddress		(0x7F1C)
#define DownloadAddress		(0x7F1E)
#define AdjustminAddress		(0x7F1F)
#define AdjustAddress		(0x7F20)
#define KSelectAddress		(0x7F22)
#define BlowTimeAddress		(0x7F24)
#define BlowPressAddress		(0x7F26)
//#define BackLightAddress		(0x7F18)

#define BackLightTimeAddress	(0x7F30)
#define BlowTimeDataAddress	(0x7F32)
#define BlowPressDataAddress	(0x7F34)
#define KSelectDataAddress	(0x7F36)
#define AdjustTimesDataAddress	(0x7F38)			//	
#define AdjustYrDataAddress		(0x7F3A)		//	/����У׼��ַ
#define AdjustMthDataAddress		(0x7F3B)		//	/����У׼��ַ
#define AdjustDayDataAddress		(0x7F3C)		//	/����У׼��ַ
#define MarkYrDataAddress	(0x7F3D)
#define MarkMthDataAddress		(0x7F3E)// ʵ��У׼���ݵ�ַ
#define MarkDayDataAddress	(0x7F3F)
#define TestTimesDataAddress	(0x7F40)// ʵ��У׼���ݵ�ַ0x7F40-0x7f41

#define SettingPasswordAddress  (0x7F42)//0x7f42  0x7f43  0x7f44
#define AdvanceSettingPasswordAddress  (0x7F45)//0x7f45  0x7f46  0x7f47
#define PasswordsettedAddress  (0x7F48)

#define inspectYearAddress  (0x7F49)
#define inspectMonthAddress  (0x7F4a)
#define inspectDayAddress  (0x7F4b)
#define inspectdateSettedAddress  (0x7F4c)

#ifdef Argentina
#define DeleteSettingPasswordAddress  (0x7F4e)  //0x7F4e  0x7F4f 0x7F50
#endif

#define updateVision  (0x7FD9)

#define ID_RADIOBUTTONStar	0
#define BackLightSelectMax	(4)						//	4
#define PrintSelectMax		(BackLightSelectMax+3)		//	7
#define ThicknessSelectMax	(PrintSelectMax+9)			//	16
#define DownloadSelcetMax	(ThicknessSelectMax+2)	//	18
#define AdjustSelectMin		(DownloadSelcetMax+4)	//	22
#define AdjustSelectMax		(AdjustSelectMin+4)	//	26
#define KSelectMax			(AdjustSelectMax+9)//+1)		//	35	// ���һ��������һ
#define BlowTimeMax			(KSelectMax + 6)	// 41
#define BlowPressMax		(BlowTimeMax + 6)	//47 ���һ��������һ
#define PeriodsMax			(BlowPressMax + 6)	// 53
#define NumberMax			(PeriodsMax + 6 + 1)	//59 ���һ��������һ

//For the Print times
#define PrintTimesAddress		(0x7F10)

// for the DemarcateAddress	// �궨�����ַ
#define	DemarcateAddress	(0x6000)

#define ChinaPageMax		(5)
#define	USE_China
#define	DelayRefussTime		(30)//�ܾ����� ��ʱʱ��

// for the Item Byte
#define USE_ITEM
#define Item0Address		(0x7040)
#define Item1Address		(0x7050)
#define Item2Address		(0x7060)
#define Item3Address		(0x7070)
#define Item4Address		(0x7080)
#define Item5Address		(0x7090)
#define Item6Address		(0x70A0)
#define Item7Address		(0x70B0)
#define Item8Address		(0x70C0)
#define Item9Address		(0x70D0)

#define Item0DataAddress	49
#define Item1DataAddress	68
#define Item2DataAddress	87
#define Item3DataAddress	106
#define Item4DataAddress	125
#define Item5DataAddress	144
#define Item6DataAddress	163
#define Item7DataAddress	182
#define Item8DataAddress	201
#define Item9DataAddress	220

#define pump_pwr      PORTFbits.RF0

//#define	 USE_MDY
# define uch 	unsigned char	// BYTE
# define unint unsigned int	// WORD
# define SDA PORTAbits.RA3	//RC4
# define SCL PORTAbits.RA2	//RC3
/*
struct tagRecord_Flag
{
 unsigned char b__Effective:1;  // 1��FLASHû�м�¼��0��FLASH�м�¼
 unsigned char b__g100ml:1;
 unsigned char b__mgml:1;
 unsigned char b__mgL_mgml:1;
 unsigned char b_RefuseTest:1; // �ܾ����Ա�־λ 1���ܾ�����
 unsigned char b_AutoTest:1;   // 1:�Զ����ԣ�0:��������
 unsigned char b__Excess:1;   // 1:�ƾ�Ũ����0-20%��0���ƾ�Ũ����20%����
 unsigned char b__Drunk:1;    // 1:�ƾ�Ũ����80%����(���Ƽ�ʻ);0:�ƾ�Ũ����20%-80%(���Ƽ�ʻ)
}RECORD_FLAG;
*/

#define ST_AUTO     	0x0001  // 
#define ST_REFUSE   	0x0002  // Bit for Refuse state.
#define ST_Passive 	0x0004  // Bit for Initiative state.
#define ST_Discontinued 	0x0008
//#define ST_FRAME  	    0x0010  // Bit to indicate frame is displayed.
//#define ST_DRAW     	0x4000  // Bit to indicate static text must be redrawn.
//#define ST_HIDE     	0x8000  // Bit to remove object from screen.


//reserve in eeprom
#define EESERIAL_NUMBER 0x0100   //

#define	useI2CRTCC	// for the I2C RTCC 

#define	RecordSizeMax	(256)

// Battery 
#define	BatteryMin 2100	// ��ؿ�ADֵ
#define	BatteryMax 2750	// �������ADֵ
#define	BatterySize	((BatteryMax-BatteryMin)/8)	// ��ظ�ADֵ

#define	USE_BTN_MSG_RELEASED
//#define	USE_adcX_adcy
//#define USE_GPS
#define Mdy 0xaa
#define Dmy 0xbb
#define EEPROM_VERSION  0x7FFE



//reserve in eeprom
#define EESERIAL_NUMBER 0x0100   //
//time zone
#define TIME_ZONE	0x0102
#define RX4_BUFFLEN 500	//500
#define TX4_BUFFLEN 10
#define U4R_TOUT 200

extern unsigned char rx4_buff[RX4_BUFFLEN],*rx4_ptr;
extern unsigned char tx4_buff[TX4_BUFFLEN],*tx4_ptr,*tx4_ptrend;
extern unsigned int uart4_rtimer,t4len; 
extern unsigned char satellite,satellite_cnt;
//extern unsigned char longitude[8],latitude[8];
extern unsigned char eeset[100];
extern DATETIMES gps_date,sys_date,old_date;
extern unsigned int sys_clk,USB_sys_clk;
extern unsigned int jhm98;
extern unsigned char I2C_ERROR,i2busy,usbcmd,usbcount;
extern unsigned int I2C_COUNTER,softMS;
extern unsigned char softMin,softSec,timeSet[7];

extern BOOL Unit_Select;
extern unsigned int ST_IRstatus;
extern BOOL alarmON;
#endif