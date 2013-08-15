/*****************************************************************************
 * Microchip Graphics Library Demo Application
 * This program shows how to use the Graphics Objects Layer.
 *****************************************************************************
 * FileName:        MainDemo.c
 * Dependencies:    MainDemo.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright ?2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS?WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok		30/05/07	...
 * Paolo Tamayo         09/20/07    Round buttons demo screen is added
 * Paolo Tamayo			10/05/07    Modified navigation, added Meter, Dial, 
 *									RTCC demos
 * Paolo Tamayo			01/21/08    Updated for v1.1
 * Sean Justice         02/07/08    PIC32 support 
 *****************************************************************************/

#include "MainDemo.h"
#include "MainDemoStrings.h"
/** INCLUDES *******************************************************/
#include "GenericTypeDefs.h"
//#include "Compiler.h"
/** CONFIGURATION **************************************************/
// Configuration bits
#ifdef __PIC32MX__
#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_18, FPLLIDIV = DIV_2, FWDTEN = OFF, FCKSM = CSECME, FPBDIV = DIV_8
#pragma config OSCIOFNC = ON, POSCMOD = XT, FSOSCEN = ON, FNOSC = PRIPLL
#pragma config CP = OFF, BWP = OFF, PWP = OFF
#else
#ifdef	USE_PIC24FJ256GB110
//_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & ICS_PGx1 & FWDTEN_OFF & FWPSA_PR128 & WDTPS_PS256)
//        _CONFIG2( 0xF7FF & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_NONE & FNOSC_FRC & PLLDIV_DIV2 & IOL1WAY_ON)
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1) 
_CONFIG2( 0xF7FF & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV2 & IOL1WAY_ON)// IOL1WAY_OFF
//_CONFIG2(0xF7FF & FNOSC_PRI & FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS);
//_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & ICS_PGx1 & FWDTEN_OFF);
#else
_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT) // Primary XT OSC with PLL
_CONFIG1(JTAGEN_OFF & FWDTEN_OFF)   // JTAG off, watchdog timer off
#endif
#endif
/** V A R I A B L E S ********************************************************/
#pragma udata

unsigned char Calibrate_Value[28];
unsigned int i_chk_paper;
BOOL stringPrinted;

//usb
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];
USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
BOOL blinkStatusValid = TRUE;

/** PRIVATE PROTOTYPES *********************************************/
void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
static void InitializeSystem(void);
void ProcessIO(void);
void UserInit(void);
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();
WORD_VAL ReadPOT(void);


/** P R I V A T E  P R O T O T Y P E S ***************************************/
void testing(void);
//static void InitializeSystem(void);
//void USBDeviceTasks(void);

void YourHighPriorityISRCode(void);
void YourLowPriorityISRCode(void);

/** VECTOR REMAPPING ***********************************************/

//void __attribute__((interrupt, shadow, no_auto_psv)) _T1Interrupt(void);
void  __attribute__((interrupt, shadow, no_auto_psv)) _T2Interrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _T3Interrupt(void);
//void __attribute__((interrupt, shadow, no_auto_psv)) _ADC1Interrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _T4Interrupt(void);
void __attribute__((interrupt, shadow, no_auto_psv)) _T5Interrupt(void);
//void __attribute__((interrupt, shadow, no_auto_psv)) _U4RXInterrupt(void);
//void __attribute__((interrupt, shadow, no_auto_psv)) _U4TXInterrupt(void);

void __attribute__ ((address(0x2800))) ISRTable(){
	asm("reset"); //reset instruction to prevent runaway code
	//asm("goto %0"::"i"(&_T1Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_T2Interrupt));  //T2Interrupt's address
//	asm("goto %0"::"i"(&_T3Interrupt));  //T2Interrupt's address
//	asm("goto %0"::"i"(&_ADC1Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_T4Interrupt));  //T2Interrupt's address
	asm("goto %0"::"i"(&_T5Interrupt));  //T2Interrupt's address
	//asm("goto %0"::"i"(&_U4RXInterrupt));  //T2Interrupt's address
	//asm("goto %0"::"i"(&_U4TXInterrupt));  //T2Interrupt's addre
}

#if defined(__18CXX)
	//On PIC18 devices, addresses 0x00, 0x08, and 0x18 are used for
	//the reset, high priority interrupt, and low priority interrupt
	//vectors.  However, the current Microchip USB bootloader 
	//examples are intended to occupy addresses 0x00-0x7FF or
	//0x00-0xFFF depending on which bootloader is used.  Therefore,
	//the bootloader code remaps these vectors to new locations
	//as indicated below.  This remapping is only necessary if you
	//wish to program the hex file generated from this project with
	//the USB bootloader.  If no bootloader is used, edit the
	//usb_config.h file and comment out the following defines:
	//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER
	//#define PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x1000
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018
	#elif defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x800
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x808
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x818
	#else	
		#define REMAPPED_RESET_VECTOR_ADDRESS			0x00
		#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
		#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
	#endif
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	extern void _startup (void);        // See c018i.c in your C18 compiler dir
	#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
	void _reset (void)
	{
	    _asm goto _startup _endasm
	}
	#endif
	#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
	void Remapped_High_ISR (void)
	{
	     _asm goto YourHighPriorityISRCode _endasm
	}
	#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
	void Remapped_Low_ISR (void)
	{
	     _asm goto YourLowPriorityISRCode _endasm
	}
	
	#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER)
	//Note: If this project is built while one of the bootloaders has
	//been defined, but then the output hex file is not programmed with
	//the bootloader, addresses 0x08 and 0x18 would end up programmed with 0xFFFF.
	//As a result, if an actual interrupt was enabled and occured, the PC would jump
	//to 0x08 (or 0x18) and would begin executing "0xFFFF" (unprogrammed space).  This
	//executes as nop instructions, but the PC would eventually reach the REMAPPED_RESET_VECTOR_ADDRESS
	//(0x1000 or 0x800, depending upon bootloader), and would execute the "goto _startup".  This
	//would effective reset the application.
	
	//To fix this situation, we should always deliberately place a 
	//"goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS" at address 0x08, and a
	//"goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS" at address 0x18.  When the output
	//hex file of this project is programmed with the bootloader, these sections do not
	//get bootloaded (as they overlap the bootloader space).  If the output hex file is not
	//programmed using the bootloader, then the below goto instructions do get programmed,
	//and the hex file still works like normal.  The below section is only required to fix this
	//scenario.
	#pragma code HIGH_INTERRUPT_VECTOR = 0x08
	void High_ISR (void)
	{
	     _asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#pragma code LOW_INTERRUPT_VECTOR = 0x18
	void Low_ISR (void)
	{
	     _asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
	}
	#endif	//end of "#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)||defined(PROGRAMMABLE_WITH_USB_LEGACY_CUSTOM_CLASS_BOOTLOADER)"

	#pragma code
	
	
	//These are your actual interrupt handling routines.
	#pragma interrupt YourHighPriorityISRCode
	void YourHighPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
		#if defined(USB_INTERRUPT)
	        USBDeviceTasks();
        #endif

	}	//This return will be a "retfie fast", since this is in a #pragma interrupt section 
	#pragma interruptlow YourLowPriorityISRCode
	void YourLowPriorityISRCode()
	{
		//Check which interrupt flag caused the interrupt.
		//Service the interrupt
		//Clear the interrupt flag
		//Etc.
	
	}	//This return will be a "retfie", since this is in a #pragma interruptlow section 

#elif defined(__C30__)
    #if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
        /*
         *	ISR JUMP TABLE
         *
         *	It is necessary to define jump table as a function because C30 will
         *	not store 24-bit wide values in program memory as variables.
         *
         *	This function should be stored at an address where the goto instructions 
         *	line up with the remapped vectors from the bootloader's linker script.
         *  
         *  For more information about how to remap the interrupt vectors,
         *  please refer to AN1157.  An example is provided below for the T2
         *  interrupt with a bootloader ending at address 0x1400
         */
//        void __attribute__ ((address(0x1404))) ISRTable(){
//        
//        	asm("reset"); //reset instruction to prevent runaway code
//        	asm("goto %0"::"i"(&_T2Interrupt));  //T2Interrupt's address
//        }
    #endif
#endif
/////////////////////////////////////////////////////////////////////////////
//                              OBJECT'S IDs
/////////////////////////////////////////////////////////////////////////////
#define ID_WINDOW1      10

#define ID_BUTTON1      11
#define ID_BUTTON2      12
#define ID_BUTTON3      13
#define ID_BUTTON4      14
#define ID_BUTTON5      15
#define ID_BUTTON6      16
#define ID_BUTTON7      17

#define ID_BUTTON8      25
#define ID_BUTTON9      26
#define ID_BUTTON10    27
#define ID_BUTTON11    28
#define ID_BUTTON12    29

#define ID_BUTTON_BACK  300
#define ID_BUTTON_NEXT  301
#define ID_BUTTON_HOME  302

#define ID_CHECKBOX1    20
#define ID_CHECKBOX2    21
#define ID_CHECKBOX3    22
#define ID_CHECKBOX4    23
#define ID_CHECKBOX5    24
#define ID_CHECKBOX6    25
#define ID_CHECKBOX7    26
#define ID_CHECKBOX8    27
#define ID_CHECKBOX9    28
#define ID_CHECKBOX10    0
#define ID_CHECKBOX11    1
#define ID_CHECKBOX12    2
#define ID_CHECKBOX13    3
#define ID_CHECKBOX14    4
#define ID_CHECKBOX15    5
#define ID_CHECKBOX16    6
#define ID_CHECKBOX17    7
#define ID_CHECKBOX18    8
#define ID_CHECKBOX19    9
#define ID_CHECKBOX20    10
#define ID_CHECKBOX21    11
#define ID_CHECKBOX22    12
#define ID_CHECKBOX23    13
#define ID_CHECKBOX24    14
#define ID_CHECKBOX25    15
#define ID_CHECKBOX26    16
#define ID_CHECKBOX27    17
#define ID_CHECKBOX28    18
#define ID_CHECKBOX29    19

#define ID_RADIOBUTTON1 30
#define ID_RADIOBUTTON2 31
#define ID_RADIOBUTTON3 32
#define ID_RADIOBUTTON4 33
#define ID_RADIOBUTTON5 34
#define ID_RADIOBUTTON6 35
#define ID_RADIOBUTTON7 36
#define ID_RADIOBUTTON8 37
#define ID_RADIOBUTTON9 38
#define ID_RADIOBUTTON10 39

#define ID_SLIDER1      40
#define ID_SLIDER2      41
#define ID_SLIDER3      42
#define ID_SLIDER4      44

#define ID_GROUPBOX1    50
#define ID_GROUPBOX2    51
#define ID_STATICTEXT1  52
#define ID_STATICTEXT2  53
#define ID_STATICTEXT3  54
#define ID_STATICTEXT4  55
#define ID_STATICTEXT5  56
#define ID_STATICTEXT6  57
#define ID_STATICTEXT7  58
#define ID_STATICTEXT8  59
#define ID_STATICTEXT9  60
#define ID_STATICTEXT10  61
#define ID_STATICTEXT11  62
#define ID_STATICTEXT12  63
#define ID_STATICTEXT13  64
#define ID_STATICTEXT14  65
#define ID_STATICTEXT15  66
#define ID_STATICTEXT16  67
#define ID_STATICTEXT17  68
#define ID_STATICTEXT18  69
#define ID_STATICTEXT19  70
#define ID_STATICTEXTID  74

#define ID_PICTURE1     71

#define ID_PROGRESSBAR1 72
#define ID_CUSTOM1      73

#define ID_LISTBOX1     80
#define ID_LISTBOX2     81
#define ID_LISTBOX3     82
#define ID_EDITBOX1     83
#define ID_LISTBOX4     84

#define ID_CALL         91
#define ID_STOPCALL		92
#define ID_HOLD			93
#define ID_BACKSPACE    94
#define ID_POUND		95
#define ID_ASTERISK		96
#define ID_KEYPADPOID   99
#define ID_KEYPAD       100	/* uses 100 to 110 for phone key pad demo */
#define ID_KEYCHINAPAD       110	/* uses 110 to 109 for China key pad demo */

#define ID_ROUNDDIAL    120
#define ID_METER1	    130
#define ID_METER2	    131
#define ID_BUTTONChina_P   140
#define ID_BUTTONChina_M   141
#define ID_BUTTONHR_P   150
#define ID_BUTTONHR_M   151
#define ID_BUTTONMN_P   152
#define ID_BUTTONMN_M   153
#define ID_BUTTONSC_P   154
#define ID_BUTTONSC_M   155

#define ID_BUTTON_DATE_UP 180
#define ID_BUTTON_DATE_DN 181

#define ID_BUTTON_UNIT_UP 182
#define ID_BUTTON_UNIT_DN 183

#define ID_BUTTON_UNIT    185

#define ID_BUTTON_MO    186
#define ID_BUTTON_DY    187
#define ID_BUTTON_YR    188
#define ID_BUTTON_RET   189

#define ID_EB_UNIT		199

#define ID_EB_MONTH		200
#define ID_EB_DAY		201
#define ID_EB_YEAR		202

#define ID_EB_HOUR      205
#define ID_EB_MINUTE    206
#define ID_EB_SECOND    207

#define ID_ABC		97
#define ID_ABCPAD       210	/* uses 210 to 235 for ABC key pad demo */

//batt type
#define BATT_TYPE_ALKALINE		0
#define BATT_TYPE_NIH			1
#define BATT_TYPE_LITHIUM		2


/////////////////////////////////////////////////////////////////////////////
//                            LOCAL PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
void StartScreen();								// draws intro screen
void CreatePage(XCHAR *pText); 					// creates the navigation buttons and window for each screen
void CreatePage0(XCHAR *pText); 




void CreateDateTime();							// creates date time demo screen
WORD MsgDateTime(WORD objMsg, OBJ_HEADER* pObj);//  processes messages for date and time demo screen
void UpdateRTCCDates(LISTBOX* pLb); 			// update the date edit boxes 
WORD MsgSetDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); // processes messages for setting dates
void ShowPullDownMenu();						// create objects for pull down menu and draw 
WORD RemovePullDownMenu();						// removes pop out pull down menu from screen
void updateDateTimeEb();						// update edit boxes with current date and time values 



void ErrorTrap(XCHAR* message);              	// outputs text message on screen and stop execution

void TickInit(void);                        	// starts tick counter  

void CreatePullDown(); 
WORD MsgPullDown(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); // processes screen pull down menu

void	CreateMain();
WORD MsgMain(WORD objMsg, OBJ_HEADER* pObj);
void Check_Screen();
void CreateEditTest(XCHAR *EditTestStr,WORD CharMax);
WORD MsgEditTest(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); 
WORD MsgEditMark(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgScreenPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgMasterPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateEditABC(XCHAR *EditTestStr,WORD CharMax);
WORD MsgEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateRecord();
WORD MsgRecord(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg); 
void	CreateSetting();
void CreateEditeeprom(XCHAR *EditTestStr,WORD CharMax);
WORD MsgEditeeprom(WORD objMsg, OBJ_HEADER * pObj, GOL_MSG * pMsg);
WORD MsgEditDemarcate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgEditID(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void  CreateBlowPlease();
WORD MsgBlowPlease(WORD objMsg, OBJ_HEADER* pObj);
void CreateDISCONTINUED();
WORD MsgDISCONTINUED(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgSetting(WORD objMsg, OBJ_HEADER* pObj);
void	ShowBattery(int value, char batt_type);
void	CreateSetBackLight();
void  CreateUnitPDmenu();
WORD MsgUnitPDment(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void ShowUnitPullDownMeny();
void CreateConnectToPC();
WORD MsgToPC(WORD objMsg, OBJ_HEADER* pObj);
WORD MsgSetBackLight(WORD objMsg, OBJ_HEADER* pObj);
void CreateResult(void);
WORD MsgResult(WORD objMsg, OBJ_HEADER* pObj);
void CreateThickness();
WORD MsgThickness(WORD objMsg, OBJ_HEADER* pObj); 
void CreateKSetting();
void MsgkSettingSel(OBJ_HEADER* pObj);
WORD MsgKSetting(WORD objMsg, OBJ_HEADER* pObj); 
void CreatePrintSetting();
WORD MsgPrintSetting(WORD objMsg, OBJ_HEADER* pObj); 

void CreateTestSetting();
WORD MsgTestSetting(WORD objMsg, OBJ_HEADER* pObj); 
void CreateDemarcate();
WORD MsgDemarcate(WORD objMsg, OBJ_HEADER* pObj);
void CreateDemarcate2();
WORD MsgDemarcate2(WORD objMsg, OBJ_HEADER* pObj);
void CreateMark();
WORD MsgMark(WORD objMsg, OBJ_HEADER* pObj);
void CreateWait();
WORD MsgWait(WORD objMsg, OBJ_HEADER* pObj);
void  CreatePowerOff();
WORD MsgPowerOFF(WORD objMsg, OBJ_HEADER* pObj);
void  CreateRefuse();
WORD MsgRefuse(WORD objMsg, OBJ_HEADER* pObj);
void  CreateBlowPress();
WORD MsgBlowPress(WORD objMsg, OBJ_HEADER* pObj);
void  CreateItem();
WORD MsgItem(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
WORD MsgItemEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateAgent(void);
WORD MsgAgent(WORD objMsg, OBJ_HEADER* pObj);
void CreateMaster(void);
WORD MsgMaster(WORD objMsg, OBJ_HEADER* pObj);
void CreateSetpassword(void);
WORD MsgSetpassword(WORD objMsg, OBJ_HEADER* pObj);
WORD MsgEditPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
void CreateOutcomeSetpassword(void);
WORD MsgOutcomeSetpassword(WORD objMsg, OBJ_HEADER* pObj);
void CreateYesNo(XCHAR* YesOrNo);
WORD MsgYesNo(WORD objMsg, OBJ_HEADER* pObj);
void CreateCalibrate();
WORD MsgCalibrate(WORD objMsg, OBJ_HEADER* pObj);
void CreateCalLock();
WORD MsgCalLock(WORD objMsg, OBJ_HEADER* pObj);
void SaveReadItem(BOOL SaveRead);
void CreateDebug();
void CreateDebug2();
void CreateDemarcateshow();
WORD MsgDemarcateshow(WORD objMsg, OBJ_HEADER* pObj);
#ifdef Argentina
WORD MsgEditInspectDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);
#endif
/////////////////////////////////////////////////////////////////////////////
//                            IMAGES USED
/////////////////////////////////////////////////////////////////////////////
/*
// internal flash image
extern const BITMAP_FLASH intro;
extern const BITMAP_FLASH mchpLogo;
extern const BITMAP_FLASH bulbon;
extern const BITMAP_FLASH bulboff;
extern const BITMAP_FLASH gradientButton;
extern const BITMAP_FLASH Engine1;
extern const BITMAP_FLASH Engine2;
extern const BITMAP_FLASH Engine3;
extern const BITMAP_FLASH Engine4;
extern const BITMAP_FLASH arrowUp;
extern const BITMAP_FLASH arrowDown;
extern const BITMAP_FLASH redphone;
extern const BITMAP_FLASH greenphone;
extern const BITMAP_FLASH mchpIcon0;
extern const BITMAP_FLASH mchpIcon;
*/
extern const BITMAP_FLASH WellLogo;
extern const BITMAP_FLASH L1;
extern const BITMAP_FLASH L2;
extern const BITMAP_FLASH L3;
extern const BITMAP_FLASH L4;
extern const BITMAP_FLASH arrowUp;
extern const BITMAP_FLASH arrowDown;
extern const BITMAP_FLASH HOME;
extern const BITMAP_FLASH LEFT;
extern const BITMAP_FLASH RIGHT;
extern const BITMAP_FLASH ag3;
extern const BITMAP_FLASH printerbutton;


//extern BITMAP_EXTERNAL E;
/////////////////////////////////////////////////////////////////////////////
//                             FONTS USED
/////////////////////////////////////////////////////////////////////////////
extern const FONT_FLASH GOLFontDefault; 	// default GOL font
extern const FONT_FLASH GOLMediumFont; 		// medium font
extern const FONT_FLASH GOLSmallFont; 		// small font
extern const FONT_FLASH BigArial;			// only for the TestResult Display
extern const FONT_FLASH GOLArial;
extern const FONT_FLASH Arial;
//#ifdef	USE_MULTIBYTECHAR
extern const FONT_FLASH CHINESE_LARGE; 			// Japanese large font
extern const FONT_FLASH CHINESE_SMALL; 			// Japanese small font

//FONT_FLASH *ptrLargeAsianFont = &CHINESE_LARGE;
const FONT_FLASH *ptrLargeAsianFont = &GOLMediumFont;//&GOLFontDefault;
const FONT_FLASH *ptrSmallAsianFont = &GOLMediumFont;//&CHINESE_SMALL;// &GOLFontDefault;
const FONT_FLASH *ptrBigArialFont = &BigArial;
const FONT_FLASH *ptrArialFont = &Arial;
//#endif


/////////////////////////////////////////////////////////////////////////////
//                            DEMO STATES      
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//                       GLOBAL VARIABLES FOR DEMO
/////////////////////////////////////////////////////////////////////////////
#ifdef	South_Africa_Version
SCREEN_STATES  screenState =  CREATE_MAIN;//;CREATE_PLACE//CREATE_BUTTONS; // current state of main demo state mashine 
#else
SCREEN_STATES  screenState = CREATE_BLOW;//; // current state of main demo state mashine 
#endif
GOL_SCHEME*    altScheme;					 // alternative style scheme
GOL_SCHEME*    alt2Scheme;					 // alternative 2 style scheme
GOL_SCHEME*    alt3Scheme;					 // alternative 3 style scheme
GOL_SCHEME*    alt4Scheme;					 // alternative 4 style scheme
GOL_SCHEME*    alt5Scheme;					 // alternative 5 style scheme
GOL_SCHEME*	   timeScheme;	
GOL_SCHEME*    navScheme;		 			 // style scheme for the navigation
GOL_SCHEME*    failScheme;
GOL_SCHEME*    redScheme;                    // alternative red style scheme
GOL_SCHEME*    greenScheme;                  // alternative green style scheme
GOL_SCHEME*    yellowScheme;                 // alternative yellow style scheme
GOL_SCHEME*    mainScheme;
GOL_SCHEME*	   timemainScheme;
GOL_SCHEME*    blackScheme;

OBJ_HEADER*	   pNavList;					 // pointer to navigation list

volatile DWORD tick = 0;                     // tick counter
volatile unsigned int offcount=0,PrintCount=0;
volatile BYTE PrintSec=0,Pressflag=0;
volatile BOOL ChoicePrint = FALSE,inputflag = TRUE;
;

char           animate;                      // switch to control animation for picture demo
OBJ_HEADER*	   pGenObj;						 // pointer to a general purpose object
PICTURE*       pPicture;                     // pointer to picture object for picture demo  

PROGRESSBAR*   pProgressBar;                 // pointer to progress bar object for progress bar demo

SLIDER*		   pSlider;						 // pointer to the slider controlling the animation speed 

// GLOBAL DEFINITIONS AND VARIABLES FOR SLIDER DEMO
#define CUR_BAR_SIZE    3                    // half size of center point for cursor
#define CUR_BRD_LEFT    67+CUR_BAR_SIZE      // cursor area left border
#define CUR_BRD_RIGHT   (GetMaxX() - CUR_BRD_LEFT - CUR_BAR_SIZE)-CUR_BAR_SIZE     // cursor area right border
#define CUR_BRD_TOP     52+CUR_BAR_SIZE      // cursor area top border
#define CUR_BRD_BOTTOM  (GetMaxY() - CUR_BRD_TOP - CUR_BAR_SIZE)-CUR_BAR_SIZE     // cursor area bottom border

SHORT x;                                     // cursor X position
SHORT y;                                     // cursor Y position
#ifndef	USE_SSD1289
// GLOBAL DEFINITIONS AND VARIABLES FOR ECG DEMO
// Array of ECG points to emulate ECG input
const SHORT ecgPoints[] ={ 
0x0068,0x0068,0x0068,0x0068,0x0068,0x0069,0x0068,0x0068,0x0068,0x0067,0x0067,0x0066,0x0066,0x0068,0x0068,0x0068,0x0067,0x0066,0x0066,0x0066,0x0066,0x0065,0x0065,0x0065,0x0065,0x0065,0x0065,0x0064,0x0063,0x0063,0x0063,0x0064,
0x0064,0x0065,0x0065,0x0064,0x0064,0x0064,0x0064,0x0065,0x0066,0x0066,0x0062,0x0061,0x0060,0x0059,0x0055,0x004a,0x0049,0x0047,0x0047,0x0048,0x004a,0x004f,0x0053,0x0056,0x005c,0x005d,0x0060,0x0061,0x0062,0x0064,0x0064,0x0064,
0x0065,0x0066,0x0067,0x0068,0x0068,0x0069,0x0070,0x0070,0x0055,0x0030,0x0004,0x0028,0x005d,0x006c,0x0069,0x0069,0x0069,0x0069,0x0068,0x0067,0x0065,0x0063,0x0061,0x005e,0x005e,0x0062,0x0064,0x0066,0x0067,0x0068,0x0068,0x006a
};
// Scanning window size
#define ECG_WINDOW_SIZE 8
// ECG data circular buffer size
#define ECG_BUFFER_SIZE 256
// ECG data circular buffer
SHORT  ecgBuffer[ECG_BUFFER_SIZE];

// GLOBAL DEFINITIONS AND VARIABLES FOR PLOTTING POTENTIOMETER DEMO
// Graph shift
#define POT_MOVE_DELTA  2
// Potentiometer circular buffer size
#define POT_BUFFER_SIZE 320
// Potentiometer data circular buffer
SHORT  potBuffer[POT_BUFFER_SIZE];

// Temporary buffer for graph demo screens
SHORT tempBuffer[10];
#endif
// Variables for the pulldown menus
SHORT pDwnLeft, pDwnTop, pDwnRight, pDwnBottom;

// Variables for date and time 
#ifdef	USE_SSD1289
SCREEN_STATES prevState = CREATE_MAIN;//CREATE_BUTTONS;	// used to mark state where time setting was called	
#else
SCREEN_STATES prevState = CREATE_BUTTONS;	// used to mark state where time setting was called	
#endif
OBJ_HEADER *pListSaved;			// temporary storage for object list
WORD 		DateItemID;			// ID of date item being modified (ID_EB_MONTH(DAY)(YEAR))

XCHAR 		dateTimeStr[26];		// string variable for date and time display
XCHAR 		DisplaydateTimeStr[26]={0,0,0,0x0020,0,0,0x003A,0,0,0x003A,0,0,0x000A,0,0,0,0x0020,0,0,0x002C,0x0020,0x0032,0x0030,0,0,0};		//spring add
XCHAR OperatorNameTempStr[19]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
XCHAR  date_str[13];
XCHAR WeekStr[4] = {0, 0, 0, 0};
CHAR		MAXCHAROld=0;
WORD		AlcTypeaddress=0X38;


static BOOL status_Press = TRUE;
BOOL BoolBatteryOFF = FALSE;
WORD g_backlight_time=0;    //¹Ø±³¹âÊ±¼ä,1ÃëÎªµ¥Î»
//WORD g_sleep_time=0;    //Ë¯ÃßÊ±¼ä,1ÃëÎªµ¥Î»
WORD g_standby_time=0;    //×Ô¶¯¹Ø»úÊ±¼ä,1ÃëÎªµ¥Î»
WORD g_rtc_counter=0;    //rtc ÏûÏ¢¼Æ?
WORD g_blow_time = 0;	// ´µÆøÊ±¼ä
static BYTE TestMode=0xaa;  //²âÊÔÀàÐÍaa=1,bb=2,cc=3
WORD Quickly_test=0;  //ÊÇ·ñ¿ì²âÄ£Ê½
BOOL Unit_Select = FALSE;	// Å¨¶Èµ¥Î»Ñ¡Ôñ,Ä¬ÈÏÎªmg/100ml
BOOL FirstIn = FALSE;
BOOL alarmON = FALSE;
static char YesNoNumber = 0;
extern XCHAR Test_Result[];
volatile XCHAR	PasswordBuffer[7];
WORD	Temp_adc,initialpress;
WORD    epPress;
WORD ItemID = 0;
extern WORD	Temp_adc;
unsigned char ST_IRstatus = 0;
extern  unsigned int  Tem_AD;
extern  unsigned int  AD_press;
extern  unsigned int  MarkDensity;
extern  unsigned int   nAdc;
extern unsigned int MarkAdc;
extern unsigned int MarkTem;
extern unsigned int WriteTable;
extern unsigned int ReadTable;
extern  unsigned int Kt;
BOOL alocholdetecet= FALSE;
extern XCHAR TestMiMaStr[3];
XCHAR DemarcateAD[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // ÅÐ¶¨
XCHAR DemarcateAD1[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // ÅÐ¶¨;
XCHAR DemarcateAD2[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD3[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD4[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // ÅÐ¶¨;
XCHAR DemarcateAD5[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // ÅÐ¶¨;
XCHAR DemarcateAD6[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD7[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD8[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD9[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD10[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};   // ÅÐ¶¨;
XCHAR DemarcateAD11[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD12[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};    // ÅÐ¶¨;
XCHAR DemarcateAD13[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR Marktem[5]	= {0,0,0,0x43,0x00};
XCHAR MarkAD2[7]	= {0x30,0x58,0,0,0,0,0x00};
XCHAR DemarcateAD14[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD15[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD16[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD17[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD18[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD19[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
XCHAR DemarcateAD20[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};     // ÅÐ¶¨;
//WORD MarkAD3[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};
//WORD MarkAD4[7]	= {0x0030,0x0058,0x0030,0x0030,0x0030,0x0030,0x0000};
unsigned int Adj_adc=0,Adj_tem=0,Demarcate_tem,Demarcate_adc, Mark_adc,Marktemporary;
XCHAR T1Str[3]={0x002d,0x0032,0x0000};
XCHAR T2Str[3]={0x0020,0x0035,0x0000};
XCHAR T3Str[3]={0x0031,0x0035,0x0000};
XCHAR T4Str[3]={0x0032,0x0032,0x0000};
XCHAR T5Str[3]={0x0032,0x0038,0x0000};
XCHAR T6Str[3]={0x0033,0x0035,0x0000};
XCHAR T7Str[3]={0x0034,0x0035,0x0000};
XCHAR T8Str[3]={0x0032,0x0035,0x0000};
XCHAR Mark1Str[3]={0x0032,0x0030,0x0000};
XCHAR Mark2Str[3]={0x0033,0x0030,0x0000};
XCHAR Mark3Str[3]={0x0035,0x0030,0x0000};
XCHAR Mark4Str[3]={0x0038,0x0030,0x0000};

volatile XCHAR	Flashbuff[RecordSizeMax] = {0, 0, 0, 0,0x00};	// ÍùFLASH ÖÐÐ´Êý¾ÝÊ±ºòÔÝÊ±±£´æµØ·½
volatile XCHAR	TempbuffStr[19] = {0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0};	// ÁÙÊ±±£´æµØÖ·

volatile XCHAR	consbuffStr[19] = {0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0};
DATETIMES sys_datetime;
WORD_VAL wordval;
volatile unsigned int serialnumber = 1,serialnumberRecord = 1,Marktimes;
USERTITLE user_title;
static unsigned char IDumberStr[9]={0,0,0,0,0,0,0,0,0};
//unsigned char Model_buff[11]={0,0,0,0,0,0,0,0,0,0,0},*Model_ptr,*Model_ptrend;
volatile unsigned char IDPrintModel[9]={0,0,0,0,0,0,0,0,0};
unsigned char timeSet[7];
static int S4_count = 1,initialpower = 0;
extern volatile unsigned int Sprinnum;

unsigned int sys_clk,MarkTemp=0;
unsigned int dif;
unsigned int jhm98;
unsigned char I2C_ERROR,i2busy,usbcmd,usbcount;
unsigned char I2C_ERROR,i2busy;
unsigned int I2C_COUNTER,softMS;
unsigned char softMin,softSec,timeSet[7];
DATETIMES sys_date; 
unsigned char satellite,satellite_cnt;
WORD Valnum; 
unsigned char eeset[100];
WORD BatteryValue;

unsigned int EETemp0,EETemp1;
unsigned char Markday,Markmonth,Markyear,Markdata,Currentyear,Currentmonth,Currentday;
unsigned int  TTTemp0,TTTemp1;
BOOL Demarcate=FALSE;


extern volatile XCHAR Printbuff[];
extern volatile BOOL SPrint,endprint;


BYTE Setpasswordposition;
BYTE Settingposition;
BYTE    BnumberStr[2]  = {0x30,0x00};
BYTE    UnumberStr[2]  = {0x30,0x00};
static XCHAR  PDread[6] = {0};
BOOL PDset=FALSE;
BYTE ADLIST;

OBJ_HEADER*      *pbtn;

 volatile BYTE Datetype;



 void delay_ms(unsigned int cctt)
{
	unsigned int save_delayt=6000;
	unsigned long aa;
	//aa = save_delayt*cctt;
	while(save_delayt!=0) 
	{
		save_delayt--;
	}
} 
#if	0
#define spi2_data_wait 180
#define SPIGet() SPI2BUF

void initSPI2(void)
{
	SPI2STAT = 0;
    SPI2CON1 = 0x001b;
    SPI2CON1bits.MSTEN = 1; 
    SPI2CON2 = 0;
    SPI2CON1bits.MODE16 = 0;
    SPI2CON1bits.CKE = 0;
    SPI2CON1bits.CKP = 1;
    SPI2CON1bits.SMP = 1;
    SPI2STATbits.SPIEN = 1;
	_TRISC13 = 0;//flash1 /cs control output
    Nop();
	_RC13 = 1;
	Nop();
	_TRISD11 = 0;//flash2 /cs control output
    Nop();
	_RD11 = 1;
	Nop();
	_TRISD12 = 0;//eeprom /cs control output
    Nop();
	//_RD12 = 1;
	Nop();
	_TRISG6 = 0;//SPI SCK2 output
    Nop();
	_TRISG8 = 0;//SPI SDO2 output
    Nop();
	_TRISG7 = 1;//SPI SDI2 input
}
//extern void delay_ms(unsigned int cctt);

void SPI2_WAIT(void)
{	
	//unsigned char k=0;
	unsigned int k=0;
	while (SPI2STATbits.SPITBF)
	{
		if (k < 180) k++; 
		//if (k < 720) k++;
		else 
		{
			initSPI2();
			return;         
		}
	}
}		
/*DATA FLASH read*/
void FLASH_RD(unsigned int page_address, unsigned int byte_address, unsigned int length, unsigned char *rdptr,unsigned char cs)
{
	unsigned char x;   
	unsigned int k = 0,i;
	ClrWdt();
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
#if 1
	SPI2_WAIT();    /*if spi2txb is full,wait*/
	SPI2BUF = 0xe8;                 /*transfer the opcode e8h*/
	SPI2_WAIT();
	SPI2BUF = (page_address >> 6) & 0xff;  /*one reserved bits(0) and PA12 - PA6*/
	SPI2_WAIT();
	SPI2BUF = ((page_address << 2) | (byte_address>>8))& 0xff;  /*PA5 - PA0,BA9,BA8*/ 
	SPI2_WAIT();
	SPI2BUF = byte_address & 0xff;  /*BA7 - BA0*/ 
	SPI2_WAIT();
	SPI2BUF = 0;                    /*32 donot care bits*/
	SPI2_WAIT();
	SPI2BUF = 0;
	SPI2_WAIT();
	SPI2BUF = 0;
	SPI2_WAIT();
	SPI2BUF = 0;
#endif

	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	k = SPI2BUF;
	SPI2STATbits.SPIROV = 0;        /*clear receive overflow flag bit*/
	while (length)                      /*the length of data expected*/
	{
#if 1
		SPI2_WAIT();
		SPI2BUF = 0;                    /*generate clock pulses*/
#endif

		k = 0;
		while (!SPI2STATbits.SPIRBF)    /*if spi2rxb is empty,wait*/
		{
			if (k < spi2_data_wait)     /*timeout value*/
			k++;                     /*wait for time out operation*/
			else 
			{
//				_SPI2ERR = 1;
				initSPI2();
				break;         /*Time out,return number of byte not to be read*/
			}
		}       
		*rdptr++ = SPI2BUF;             /*the received data to be recorded to this array*/
		//k = SPI2BUF;
		SPI2STATbits.SPIROV = 0;        /*clear receive overflow flag bit*/
		length--;                       /*reduce string length count by 1*/
	}
	Nop();
	Nop();
	Nop();
	Nop();
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
}

void flash_buffer(unsigned char opcode, unsigned int page_address,unsigned char cs)
{
	unsigned char i;
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
#if 1
	SPI2_WAIT();           /*if spi2txb is full,wait*/
	SPI2BUF = opcode;                      /*53h for buffer1,55h for buffer2*/
	SPI2_WAIT();
	SPI2BUF = (page_address >> 6) & 0xff;  /*one reserved bits(0) and PA12 - PA6*/
	SPI2_WAIT();
	SPI2BUF = (page_address << 2) & 0xff;  /*PA5 - PA0,00*/ 
	SPI2_WAIT();
	SPI2BUF = 0;                           /*8 donot care bits*/
	SPI2_WAIT();
#endif

	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
	delay_ms(2);
}

void write_bufferstart(unsigned char opcode, unsigned int buffer_address,unsigned char cs)
{
	unsigned char x;	
	ClrWdt();       
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
#if 1
	SPI2_WAIT();
	SPI2BUF = opcode;                          /*84h for buffer1,87h for buffer2*/
	SPI2_WAIT();
	SPI2BUF = 0;                               /*8 donot care bits*/
	SPI2_WAIT();
	SPI2BUF = (buffer_address >> 8) & 0xff;    /*7 donot care bits,BFA8*/ 
	SPI2_WAIT();
	SPI2BUF = buffer_address & 0xff;           /*BFA7 - BFA0*/ 
#endif

}

void write_buffer(unsigned char *ptra,unsigned int length)
{
	unsigned int i;
	for(i= 0;i<length;i++)
	{
#if 1
	SPI2_WAIT();
	SPI2BUF = *ptra++; 
#endif
#if 0
	SPIPut(*ptra++);
	SPIGet();
#endif             
	}
}

void write_bufferend(unsigned char cs)
{
	unsigned char i;
	SPI2_WAIT();
	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
}

void solid_data(unsigned char opcode, unsigned int page_address,unsigned char cs)
{
	unsigned char i;
	if(cs)	_RD11 = 0;//enable CS FLASH2
	else _RC13 = 0;//enable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
#if 1
	SPI2_WAIT();
	SPI2BUF = opcode;                      /*83h for buffer1,86h for buffer2*/
	SPI2_WAIT();
	SPI2BUF = (page_address >> 6) & 0xff;  /*one reserved bits(0) and PA12 - PA6*/
	SPI2_WAIT();
	SPI2BUF = (page_address << 2) & 0xff;  /*PA5 - PA0,0,0*/ 
	SPI2_WAIT();
	SPI2BUF = 0;                           /*8 donot care bits*/
	SPI2_WAIT();
#endif

	for (i = 0; i < 20; i++)
	{
		Nop();
	}
	if(cs)	_RD11 = 1;//disenable CS FLASH2
	else _RC13 = 1;//disenable CS FLASH1
	Nop();
	Nop();
	Nop();
	Nop();
	delay_ms(20);
}

/*DATA FLASH AUTO PAGE REWRITE*/
/*i = 0-127*/
void AUTO_REWR(unsigned char i)
{
	unsigned char j,x;
	for (j = 0; j < 64; j++)         /*main memory page address bits PA6 - PA0,each page of the main memory to be rewritten*/
	{
		_RC13 = 0;
		Nop();
		Nop();
		Nop();
		Nop();
		SPI2_WAIT();  /*if spi2txb is full,wait*/
		SPI2BUF = 0x59;               /*auto page rewrite code*/
		SPI2_WAIT();
		SPI2BUF = i;                  /*three reserved bits(000),and five address bits PA11 - PA7*/
		SPI2_WAIT();
		SPI2BUF = (j<<2);             /*seven address bits PA5 - PA0,and two donot care bit*/
		SPI2_WAIT();
		SPI2BUF = 0;                  /*eight donot care bits*/
		SPI2_WAIT();
		for (i = 0; i < 20; i++)
		{
			Nop();
		}
		_RG14 = 1;
		ClrWdt();
	}
}
/*
	write_bufferstart(0x84,0); //0x84 is buff1,0 is buff1 addr
	write_buffer(ptra,33);//ptra is buffer address,33 is write length
	write_bufferend();
	solid_data(0x83,yy);//0x83 is buff1,yy is page no.
	FLASH_RD(page_address,byte_address,length,rdptr);
*/


#endif


void	CopyChar(XCHAR *Dest, XCHAR *Str, unsigned int uCount)
{
	unsigned int i;
	if(uCount<=0)return 0;
	for(i = 0; i< uCount; i++)
	{
	Dest[i] = Str[i];
	}


}
#if	1


void SaveAdjust(void)//±£´æµ±Ç°±ê¶¨µÄÊ±¼äÈÕÆÚ
{
	EEPROMWriteByte(_time.yr, MarkYrDataAddress);//Êµ¼ÊÐ£×¼
	EEPROMWriteByte(_time.mth, MarkMthDataAddress);
	EEPROMWriteByte(_time.day, MarkDayDataAddress);
	EEPROMWriteWord(0, TestTimesDataAddress);
}

void InitialSETTING(void)
{
//EEPROMWriteByte(ID_RADIOBUTTONStar + 3, RButtonAddress);
//EEPROMWriteWord(600, BackLightTimeAddress);//Auto off
EEPROMWriteByte(ID_RADIOBUTTONStar + 35, BlowTimeAddress);
EEPROMWriteWord(10, BlowTimeDataAddress);//time
EEPROMWriteByte(ID_RADIOBUTTONStar + 41, BlowPressAddress);
EEPROMWriteWord(5, BlowPressDataAddress);//press
EEPROMWriteByte(7, ThicknessAddress);//Unit
EEPROMWriteByte(ID_RADIOBUTTONStar + 30, KSelectAddress);
EEPROMWriteWord(2200, KSelectDataAddress);//K

   EEPROMWriteByte(0x00, MarkYrDataAddress);//Êµ¼ÊÐ£×¼
 EEPROMWriteByte(0x01, MarkMthDataAddress);
 EEPROMWriteByte(0x01, MarkDayDataAddress);
 
EEPROMWriteWord(0, TestTimesDataAddress);


EEPROMWriteByte(0X00,ID_JIQIADDRESS);
EEPROMWriteByte(0X00,ID_JIQIADDRESS+1);
EEPROMWriteByte(0X00,ID_JIQIADDRESS+2);
EEPROMWriteByte(0X00,ID_JIQIADDRESS+3);

EEPROMWriteWord(800,0x6020);
EEPROMWriteWord(5200,0x6022);//±ê¶¨Öµ

       EEPROMWriteWord(0x0092,0x6000);//-2ÎÂ¶È
       EEPROMWriteWord(0x0d48,0x600e);//-2 AD

       EEPROMWriteWord(0x00ac,0x6002);//5 tem
       EEPROMWriteWord(0x1105,0x6010);//5 AD

       EEPROMWriteWord(0x00ca,0x6004);//15 tem
       EEPROMWriteWord(0x1367,0x6012);//15 AD

       EEPROMWriteWord(0x00df,0x6006);//22 tem
       EEPROMWriteWord(0x1498,0x6014);//22 AD

	   EEPROMWriteWord(0x00f2,0x6008);//28 tem
       EEPROMWriteWord(0x1498,0x6016);//28 AD

       EEPROMWriteWord(0x0107,0x600a);//35 tem
       EEPROMWriteWord(0x1546,0x6018);//35 AD

       EEPROMWriteWord(0x0126,0x600c);//45 tem
       EEPROMWriteWord(0x142e,0x601a);//45 AD

}

void DefaultSETTING(void)
{
XCHAR  Initialee[20]={0};
//EEPROMWriteByte(ID_RADIOBUTTONStar + 35, BlowTimeAddress);
//EEPROMWriteWord(10, BlowTimeDataAddress);//time
//EEPROMWriteByte(ID_RADIOBUTTONStar + 41, BlowPressAddress);
//EEPROMWriteWord(5, BlowPressDataAddress);//press
//EEPROMWriteByte(7, ThicknessAddress);//Unit
//EEPROMWriteByte(ID_RADIOBUTTONStar + 30, KSelectAddress);
//EEPROMWriteWord(2200, KSelectDataAddress);//K
EEPROMWriteByte(0x30, PrintSelectAddress);//under 0
EEPROMWriteByte(0x31, PrintSelectAddress+1);//beyond 0
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX7 );//auto print
#ifdef DOT
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX6 );//print  cal
#endif
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX3 );//auto print
//EEPROMWriteByte(ID_RADIOBUTTONStar + 3, RButtonAddress);
//EEPROMWriteWord(600, BackLightTimeAddress);//Auto off
//EEPROMWriteByte(ID_RADIOBUTTONStar + 21, AdjustminAddress);
//EEPROMWriteWord(360, AdjustMthDataAddress);//Cal days
//EEPROMWriteByte(ID_RADIOBUTTONStar + 24, AdjustAddress);
//EEPROMWriteWord(500, AdjustTimesDataAddress);//Test times
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX8 );//Lock
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX10 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX20 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX11 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX21 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX12 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX22 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX13 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX23 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX14 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX24 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX15 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX25 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX16 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX26 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX17 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX27 );//
EEPROMWriteByte(1, ID_CHECKBOXADDRESS + ID_CHECKBOX18 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX28 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX19 );//
EEPROMWriteByte(0, ID_CHECKBOXADDRESS + ID_CHECKBOX29 );//
EEPROMWriteArray(Item0Address, Initialee, 20);
EEPROMWriteArray(Item1Address, Initialee, 20);
EEPROMWriteArray(Item2Address, Initialee, 20);
EEPROMWriteArray(Item3Address, Initialee, 20);
EEPROMWriteArray(Item4Address, Initialee, 20);
EEPROMWriteArray(Item5Address, Initialee, 20);
EEPROMWriteArray(Item6Address, Initialee, 20);
EEPROMWriteArray(Item7Address, Initialee, 20);
EEPROMWriteArray(Item8Address, Initialee, 20);
EEPROMWriteArray(Item9Address, Initialee, 20);
EEPROMWriteArray(Item0Address, DriverName, 12);
EEPROMWriteArray(Item1Address, VehicleNoStr, 15);
EEPROMWriteArray(Item2Address, LicenceNumber, 14);
EEPROMWriteArray(Item3Address, PoliceName, 11);
EEPROMWriteArray(Item4Address, BadgeNoStr, 12);
EEPROMWriteArray(Item5Address, DepartmentStr, 10);
EEPROMWriteArray(Item6Address, ItemStr, 5);
EEPROMWriteArray(Item7Address, ItemStr, 5);
EEPROMWriteArray(Item8Address, Signature, 9);
EEPROMWriteArray(Item9Address, ItemStr, 5);
/*
EEPROMWriteWord(200, Beepsld1Address);
EEPROMWriteWord(400, Beepsld2Address);
BEEP_TIME = 200;
TIMER_BASE = 400;
*/
#ifdef POLVersion
EEPROMWriteByte(0x33, SettingPasswordAddress);
EEPROMWriteByte(0x68, SettingPasswordAddress+1);
EEPROMWriteByte(0x89, SettingPasswordAddress+2);
EEPROMWriteByte(0x33, AdvanceSettingPasswordAddress);
EEPROMWriteByte(0x61, AdvanceSettingPasswordAddress+1);
EEPROMWriteByte(0x12, AdvanceSettingPasswordAddress+2);
#else
EEPROMWriteByte(0x11, SettingPasswordAddress);
EEPROMWriteByte(0x91, SettingPasswordAddress+1);
EEPROMWriteByte(0x19, SettingPasswordAddress+2);

EEPROMWriteByte(0x11, AdvanceSettingPasswordAddress);
EEPROMWriteByte(0x21, AdvanceSettingPasswordAddress+1);
EEPROMWriteByte(0x12, AdvanceSettingPasswordAddress+2);
#ifdef Argentina
EEPROMWriteByte(0x12, DeleteSettingPasswordAddress);
EEPROMWriteByte(0x34, DeleteSettingPasswordAddress+1);
EEPROMWriteByte(0x56, DeleteSettingPasswordAddress+2);
#endif
#endif
EEPROMWriteByte(0x47, updateVision);
}

void Markjudgement(void)
{

XCHAR PeriodsText[] = "Calibration Needed!";//{'P','l','e','a','s','e',' ','D','e','m','a','r','c','a','t','e',0};
// Èç¹û³¬³ö±ê¶¨ÆÚÏÞ£¬ÔòÏÈÏÔÊ¾Çë±ê¶¨½çÃæ¡£
// µ±Ç°Ê±¼äÈÕÆÚ - ±ê¶¨Ê±Ê±¼äÈÕÆÚÎªÉè¶¨ÔÂ·Ý
// »òµ±Ç°¼ÇÂ¼ÌõÊý¼õÈ¥±ê¶¨Ê±ÌõÊý´óÓÚÉè¶¨ÌõÊýÔòÌáÊ¾±ê¶¨



    EETemp0 = EEPROMReadWord(AdjustMthDataAddress);
	
    Markdata = EEPROMReadByte(MarkDayDataAddress);
    Markday = (Markdata/16)*10+Markdata%16;	
	
    Markdata = EEPROMReadByte(MarkMthDataAddress);
    Markmonth = (Markdata/16)*10+Markdata%16;	
	
    Markdata = EEPROMReadByte(MarkYrDataAddress);
    Markyear = (Markdata/16)*10+Markdata%16;	

    Currentyear =  (_time.yr/16)*10+_time.yr%16;

    Currentmonth = (_time.mth/16)*10+_time.mth%16;	

    Currentday = 	(_time.day/16)*10+_time.day%16;	
	
Nop();
Nop();
	if(Markyear <Currentyear)
	{
		
		EETemp1 = ((Currentyear-Markyear-1)*12+Currentmonth-1+12-Markmonth)*30+Currentday+31-Markday;
		if(EETemp1>EETemp0)
			Demarcate = TRUE;
	}
	else if(Markyear ==Currentyear)
		{
		  
		if(Markmonth <Currentmonth)
                  EETemp1=(Currentmonth-Markmonth-1)*30+Currentday+31-Markday;
       		if(EETemp1>EETemp0)
			Demarcate = TRUE;

	}
	// Èç¹û³¬³ö±ê¶¨ÆÚÏÞ£¬ÔòÏÈÏÔÊ¾Çë±ê¶¨½çÃæ¡£
	TTTemp0=EEPROMReadWord(AdjustTimesDataAddress);
	
if(TTTemp0 != 0xabcd)
{
	TTTemp1=EEPROMReadWord(TestTimesDataAddress);
	if(TTTemp0<=TTTemp1 )
		Demarcate = TRUE;
}
		
	if(Demarcate)
	{
		//SaveAdjust();
		MoveTo(30,150);
		SetColor(BRIGHTRED);
		SetFont((void*)&FONTDEFAULT);// FONTMEDIUM
	    while(!OutText((XCHAR*)PeriodsText));
//	    while(TouchGetX() == -1)Beep();
	    
	    delay(500);
	}



}

XCHAR DisplayUnitStr[9];
void DisplayThickness(XCHAR* ThicknessTemp)
{
//	XCHAR ThicknessTemp;
	switch(Flashbuff[9])//EEPROMReadByte(ThicknessAddress))
	{
		case 0:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 7)
		ThicknessTemp[0] = 'm';//mg_100mlStr;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[1] = 'g';
		ThicknessTemp[2] = '/';
		ThicknessTemp[3] = '1';
		ThicknessTemp[4] = '0';
		ThicknessTemp[5] = '0';
		ThicknessTemp[6] = 'm';
		ThicknessTemp[7] = 'L';
		ThicknessTemp[8] = 0;
		break;
		case 1:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 8)
		ThicknessTemp[0] = 'm';//mg_lStr;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[1] = 'g';
		ThicknessTemp[2] = '/';
		ThicknessTemp[3] = 'L';
		ThicknessTemp[4] = 0;
		break;
		case 2:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 9)
		ThicknessTemp[0] = '%';//mg_100Str;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[1] = 0;
		break;
		case 3:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 10)
//		ThicknessTemp = g_100mlStr;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[0] = 'g';
		ThicknessTemp[1] = '/';
		ThicknessTemp[2] = '1';
		ThicknessTemp[3] = '0';
		ThicknessTemp[4] = '0';
		ThicknessTemp[5] = 'm';
		ThicknessTemp[6] = 'L';
		ThicknessTemp[7] = 0;
		break;
		case 4:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 11)
//		ThicknessTemp = mg_1000mlStr;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[0] = '%';//Ç§·ÖºÅ
		ThicknessTemp[1] = 'o';
		ThicknessTemp[2] = 0;
		break;
		case 5:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 12)
		ThicknessTemp[0] = 'u';//ug_100mlStr;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[1] = 'g';
		ThicknessTemp[2] = '/';
		ThicknessTemp[3] = '1';
		ThicknessTemp[4] = '0';
		ThicknessTemp[5] = '0';
		ThicknessTemp[6] = 'm';
		ThicknessTemp[7] = 'L';
		ThicknessTemp[8] = 0;
		break;
		case 6:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 13)
		ThicknessTemp[0] = 'm';//mg_mlStr;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[1] = 'g';
		ThicknessTemp[2] = '/';
		ThicknessTemp[3] = 'm';
		ThicknessTemp[4] = 'L';
		ThicknessTemp[5] = 0;
		break;
		case 7:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 14)
		ThicknessTemp[0] = 'g';//mg_mg100Str;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[1] = '/';
		ThicknessTemp[2] = 'L';
		ThicknessTemp[3] = 0;
		break;
		case 8:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 15)
//		ThicknessTemp = g_210lStr;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		ThicknessTemp[0] = 'g';
		ThicknessTemp[1] = '/';
		ThicknessTemp[2] = '2';
		ThicknessTemp[3] = '1';
		ThicknessTemp[4] = '0';
		ThicknessTemp[5] = 'L';
		ThicknessTemp[6] = 0;
		break;
/*		case 16:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 16)
		0;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;*/
		default:
			break;
	}
//	Flashbuff[9] = ThicknessTemp;// ±£´æ¶ÔÓ¦µÄµ¥Î»

}

void GetThickness(void)
{
	XCHAR ThicknessTemp;
	switch(EEPROMReadByte(ThicknessAddress))
	{
		case 7:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 7)
		ThicknessTemp = 0;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 8:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 8)
		ThicknessTemp = 1;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 9:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 9)
		ThicknessTemp = 2;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 10:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 10)
		ThicknessTemp = 3;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 11:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 11)
		ThicknessTemp = 4;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 12:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 12)
		ThicknessTemp = 5;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 13:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 13)
		ThicknessTemp = 6;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 14:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 14)
		ThicknessTemp = 7;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
		case 15:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 15)
		ThicknessTemp = 8;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;
/*		case 16:	//Êµ¼ÊÎª(ID_RADIOBUTTONStar + 16)
		0;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		break;*/
		default:
			break;
	}
	Flashbuff[9] = ThicknessTemp;// ±£´æ¶ÔÓ¦µÄµ¥Î»
	DisplayThickness(DisplayUnitStr);

}
DWORD recordaddr=0;
void writerecord(void)
{//return 0;
	unsigned char i,j,k,f=0;


	serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
if(serialnumber <= 2000)
  {

	wordval.Val = (serialnumber);  //hex_bcd2(serialnumber);	record serial number ,type is unsigned int
	Flashbuff[0] = 0x00;
	Flashbuff[1] = wordval.v[1];
	Flashbuff[2] = wordval.v[0];
	Flashbuff[3] = _time.yr;// sys_datetime.year;// dateTimeStr
	Flashbuff[4] = _time.mth;// sys_datetime.month;
	Flashbuff[5] = _time.day;// sys_datetime.day;
	Flashbuff[6] = _time.hr;// sys_datetime.hour;
	Flashbuff[7] = _time.min;// sys_datetime.minute;
	Flashbuff[8] = _time.sec;// sys_datetime.second;	
/*if(EEPROMReadByte(RButtonAddress+4) == (ID_RADIOBUTTONStar + 7))
	{
		Flashbuff[9] = 0;	// alcohol_unit;// Å¨¶Èµ¥Î»0:mg/100ml
		Unit_Select = FALSE;
	}
	else	
	{
		Flashbuff[9] = 1;	// 1: mg/L
		Unit_Select = TRUE;
	}*/
	GetThickness();
//	Flashbuff[9]  unit
//	Flashbuff[10] // result high
//	Flashbuff[11] result low

	Flashbuff[13] = Temperature();
	Flashbuff[14] = Kt;	
	Flashbuff[15] = 0;

#ifdef SA_VERSION
    if(TestMode == 0xcc)
    	{
		 Flashbuff[16] = Screen_mode;// ²âÊÔÄ£Ê½

		 Flashbuff[10] = ((alocholdetecet)?Screen_mode:0);
		 
    	}
	else	 
		Flashbuff[16] = ST_IRstatus;// ²âÊÔÄ£Ê½
#else
	Flashbuff[16] = ST_IRstatus;// ²âÊÔÄ£Ê½
#endif	
	Flashbuff[17]=EEPROMReadByte(MarkYrDataAddress);

       Flashbuff[18]=EEPROMReadByte(MarkMthDataAddress);

       Flashbuff[19]=EEPROMReadByte(MarkDayDataAddress);
	   	
	recordaddr =( serialnumber-1)*30;//

	//Flashbuff[0]=((DWORD_VAL) 660).v[2];
	//Flashbuff[1]=((DWORD_VAL) 660).v[1];
	//Flashbuff[2]=((DWORD_VAL) 660).v[0];

    SST25WriteArray(recordaddr, Flashbuff, 30);
	//SST25WriteWord(0x1111, 0x00700e);
   // SST25WriteWord(0x2222, 0x007000);
	serialnumberRecord = serialnumber;
	serialnumber++;
	EEPROMWriteWord(serialnumber,EESERIAL_NUMBER);

   }
else  ErrorTrap("Memory full!");
}
/******************************************************************************
 * Function:        BYTE readrecord(char *inbuffer, char *outbuffer)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        readrecord from flash,the record content lenth have240bytes
 *
 * Note:            
 *
 *****************************************************************************/
void readrecord(void)
{
    DWORD recordaddr=0;

    recordaddr =( serialnumberRecord-1)*30;

    SST25ReadArray(recordaddr, Flashbuff, 30);
   // Flashbuff[0]=SST25ReadWord(0x00700e);
    //Flashbuff[1]=SST25ReadWord(0x007000);


}//end read record
#endif


/////////////////////////////////////////////////////////////////////////////
//                                  MAIN
/////////////////////////////////////////////////////////////////////////////
int main(void){
//uch str2[]={"Wellcome to shen zhen well co.LTD"};
/******************************************/
//new  EEprom&Flash
// Assign SDI To Pin RP26(11)
RPINR22bits.SDI2R = 26;
// Assign SCK To Pin RP21(10)
RPOR10bits.RP21R = 11;
// Configure Output Functions (Table 9-2)
// Assign SDO To Pin RP19(12)
RPOR9bits.RP19R = 10;

/*
//uart  printer
// Assign U1RX To Pin RPI33
RPINR18bits.U1RXR = 33;
// Assign U1TX To Pin RP16
RPOR8bits.RP17R = 3;

//int2 RP29   ads7843inq
RPINR1bits.INT2R = 29;
*/
// Assign U1TX To Pin RP17/RF5
RPOR8bits.RP17R = 3;


// Lock Registers
asm volatile ( "MOV #OSCCON, w1 \n"
"MOV #0x46, w2 \n"
"MOV #0x57, w3 \n"
"MOV.b w2, [w1] \n"
"MOV.b w3, [w1] \n"
"BSET OSCCON, #6" );
/**********************************************/
    InitializeSystem();//use for USB
#if defined(USB_INTERRUPT)
        USBDeviceAttach();
#endif
  //SR=0x0080;    
unsigned long int i;
unsigned int j_print;
DelayMs(300);


GOL_MSG msg;        			// GOL message structure to interact with GOL
//PORTFbits.RF3 = 0;
/////////////////////////////////////////////////////////////////////////////
// ADC Explorer 16 Development Board Errata (work around 2)
// RB15 should be output
/////////////////////////////////////////////////////////////////////////////
//	LATDbits.LATD9 = 0;
//	TRISDbits.TRISD9 = 0;//changed by zyq
/////////////////////////////////////////////////////////////////////////////

#ifdef __PIC32MX__
    INTEnableSystemMultiVectoredInt();
    SYSTEMConfigPerformance(GetSystemClock());	
#endif

    EEPROMInit();   			// Initialize EEPROM
    SST25Init();
   	TickInit();     			// Start tick counter    
    GOLInit();      			// Initialize graphics library and crete default style scheme for GOL
    BeepInit();     			// Initialize beeper
     adcinit();
	initialUART1();
//    InitializeADS7843();
 //   InitializeINT2();

	inti_time5();

  // inti_time3();
	
	initI2();
	Nop();
	Nop();
	Nop();
	Nop();
	error_i2();

	RTCRead(&timeSet[0]);
	Nop();
	Nop();
	
	Nop();
	Nop();

    RTCCProcessEvents();
//            Temp_adc=Adc_Count(9,5);
//            Temp_adc+=150;
    //adcinit();
inti_time5();
//  initimer2();
inti_ads7822();
TRISFbits.TRISF1=0;
TRISFbits.TRISF0=0;
pumpinit();
TRISAbits.TRISA6=0;
PORTAbits.RA6 = 0;

delay(200);
//serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
PORTAbits.RA6 = 1;

/*
    // If S3 button on Explorer 16 board is pressed calibrate touch screen
    if(PORTDbits.RD6 == 0)
    	{
        TouchCalibration();
        TouchStoreCalibration();
    }
*/
    // If it's a new board (EEPROM_VERSION byte is not programed) calibrate touch screen

    if(GRAPHICS_LIBRARY_VERSION != EEPROMReadWord(EEPROM_VERSION))
{       
       InitialSETTING();
     //  TouchCalibration();
     //  TouchStoreCalibration();
       EEPROMWriteWord(GRAPHICS_LIBRARY_VERSION,EEPROM_VERSION);
       EEPROMWriteWord(1,EESERIAL_NUMBER);// Ð´Ò»×÷Îª¼ÇÂ¼µÄÌõÊý¿ªÊ¼
      // InitialSETTING();
    }

if(0x44 ==EEPROMReadByte(updateVision))
{
	EEPROMWriteByte(_time.yr, MarkYrDataAddress);//Êµ¼ÊÐ£×¼
	EEPROMWriteByte(_time.mth, MarkMthDataAddress);
	EEPROMWriteByte(_time.day, MarkDayDataAddress);
	EEPROMWriteWord(0, TestTimesDataAddress);
	EEPROMWriteByte(0x45, updateVision);
}

if(0x47!=EEPROMReadByte(updateVision))
    
	   DefaultSETTING();
	   

#ifdef South_Africa_Version

       Datetype = Dmy;
#else

	if(EEPROMReadByte(ID_CHECKBOXADDRESS + ID_CHECKBOX4))
		Datetype = Mdy;
	
	else
		Datetype = Dmy;
#endif	

    // Load touch screen calibration parameters from EEPROM
  //  TouchLoadCalibration();
   g_standby_time = EEPROMReadWord(BackLightTimeAddress);  
    serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
   Marktimes=EEPROMReadWord(TestTimesDataAddress);
    serialnumberRecord = serialnumber - 1;// ±£³Ö¼ÇÂ¼Ò»ÖÂ
    memset(&Flashbuff[0],0,256);
    POWER_CONTROL = 1;	//for the Power On
    
    Markjudgement();

    StartScreen();  // Show intro screen and wait for touch
    prepare_and_Charge();// PORTAbits.RA6 = 1;
    pump_reset=0;
    pump_on=1;
	delayms(2000);
	pump_reset=0;
    pump_on=0;
    if(serialnumberRecord >= 2000)
		ErrorTrap("Memory full!");
	// create the alternate schemes
	navScheme = GOLCreateScheme(); 		// alternative scheme for the navigate buttons
    altScheme = GOLCreateScheme(); 		// create alternative 1 style scheme
    alt2Scheme = GOLCreateScheme(); 	// create alternative 2 style scheme
    alt3Scheme = GOLCreateScheme(); 	// create alternative 3 style scheme
    alt4Scheme = GOLCreateScheme(); 	// create alternative 4 style scheme
    blackScheme = GOLCreateScheme();
    timeScheme = GOLCreateScheme();
    failScheme = GOLCreateScheme();
    redScheme = GOLCreateScheme();   	// create red style scheme
    greenScheme = GOLCreateScheme(); 	// create green style scheme
    yellowScheme = GOLCreateScheme(); 	// create yellow style scheme
    mainScheme = GOLCreateScheme(); 		// create alternative 1 style scheme
    timemainScheme = GOLCreateScheme();


	/* for Microtips display */
	altScheme->Color0 = RGB565CONVERT(0x4C, 0x7E, 0xFF);
    altScheme->Color1 = RGB565CONVERT(0xFF, 0xCB, 0x3C);
	altScheme->EmbossDkColor = RGB565CONVERT(0x1E, 0x00, 0xE5);
	altScheme->EmbossLtColor = RGB565CONVERT(0xA9, 0xDB, 0xEF);
    altScheme->ColorDisabled = RGB565CONVERT(0xD4, 0xE1, 0xF7);
    altScheme->TextColor1 = RGB565CONVERT(0x4C, 0x7E, 0xFF);
    altScheme->TextColor0 = RGB565CONVERT(0xFF, 0xCB, 0x3C);
    altScheme->TextColorDisabled = RGB565CONVERT(0xB8, 0xB9, 0xBC);




    alt2Scheme->TextColor1 = BRIGHTRED;
    alt2Scheme->TextColor0 = BRIGHTBLUE;
//    alt2Scheme->CommonBkColor = BLACK;

 	alt2Scheme->pFont = (void*)ptrSmallAsianFont;


//	mainScheme->TextColor1 = BLACK;//BRIGHTRED;
//	mainScheme->TextColor0 = BLACK;//WHITE;//BRIGHTRED;//BRIGHTBLUE;
//	mainScheme->CommonBkColor = BLACK;
	mainScheme->EmbossDkColor = RGB565CONVERT(30, 85, 105);
	mainScheme->EmbossLtColor = RGB565CONVERT(30, 85, 105);
	mainScheme->pFont = (void*)&GOLMediumFont;



    alt3Scheme->Color0 = LIGHTBLUE; 		
    alt3Scheme->Color1 = BRIGHTGREEN;
    alt3Scheme->TextColor0 = BLACK;
    alt3Scheme->TextColor1 = WHITE;
    alt3Scheme->pFont = (void*)&GOLMediumFont;


    alt4Scheme->Color0 = LIGHTBLUE; 		
    alt4Scheme->Color1 = BRIGHTGREEN;
    alt4Scheme->TextColor0 = BLACK;
    alt4Scheme->TextColor1 = WHITE;
    alt4Scheme->pFont = (void*)&FONTMEDIUM;	// GOLSmallFont;


    blackScheme->EmbossDkColor=RGB565CONVERT(0xEF, 0xFE, 0xFF);
   blackScheme->EmbossLtColor=RGB565CONVERT(0xEF, 0xFE, 0xFF);

	failScheme->Color0 = RGB565CONVERT(0xCC, 0x00, 0x00); 
    failScheme->Color1 = BRIGHTRED;
    failScheme->EmbossDkColor = RED4;
    failScheme->EmbossLtColor = FIREBRICK1;
    failScheme->TextColor0 = BRIGHTRED;//RGB565CONVERT(0xC8, 0xD5, 0x85); //ÏÔÊ¾×ÖÌåµÄÑÕÉ«
    failScheme->TextColor1 = BLACK;
    
    redScheme->Color0 = RGB565CONVERT(0xCC, 0x00, 0x00); 
    redScheme->Color1 = BRIGHTRED;
    redScheme->EmbossDkColor = RED4;
    redScheme->EmbossLtColor = FIREBRICK1;
    redScheme->TextColor0 = BRIGHTRED;//RGB565CONVERT(0xC8, 0xD5, 0x85); //ÏÔÊ¾×ÖÌåµÄÑÕÉ«
    redScheme->TextColor1 = BLACK;
    redScheme->pFont = (void*)&BigArial;

    greenScheme->Color0 = RGB565CONVERT(0x23, 0x9E, 0x0A); 
    greenScheme->Color1 = BRIGHTGREEN;
    greenScheme->EmbossDkColor = DARKGREEN;
    greenScheme->EmbossLtColor = PALEGREEN;
    greenScheme->TextColor0 = BRIGHTGREEN;//RGB565CONVERT(0xDF, 0xAC, 0x83); 
    greenScheme->TextColor1 = BLACK; 
    greenScheme->pFont = (void*)&BigArial;//ptrBigArialFont;//

    yellowScheme->Color0 = BRIGHTYELLOW; 
    yellowScheme->Color1 = YELLOW;
    yellowScheme->EmbossDkColor = RGB565CONVERT(0xFF, 0x94, 0x4C);
    yellowScheme->EmbossLtColor = RGB565CONVERT(0xFD, 0xFF, 0xB2);
    yellowScheme->TextColor0 = RGB565CONVERT(0xAF, 0x34, 0xF3);
    yellowScheme->TextColor1 = RED;

    timeScheme->Color0 = BLACK; 		
    timeScheme->Color1 = WHITE;
    timeScheme->TextColor0 = BRIGHTBLUE;
    timeScheme->TextColor1 = WHITE;
    timeScheme->EmbossDkColor = GRAY20; 		
    timeScheme->EmbossLtColor = GRAY80;
    timeScheme->pFont = (void*)&GOLSmallFont;    
    


    timemainScheme->TextColor0 = RGB565CONVERT(120, 196,66);//RGB565CONVERT(30, 95,80);//BRIGHTBLUE;
    timemainScheme->TextColor1 = BLACK;//WHITE;
    timemainScheme->CommonBkColor = RGB565CONVERT(5,70,60);//BLACK;
    timemainScheme->pFont = (void*)&GOLSmallFont;


    alt5Scheme->TextColor0 = BRIGHTBLUE;
    alt5Scheme->TextColor1 = WHITE;
    alt5Scheme->EmbossDkColor = GRAY20; 		
    alt5Scheme->EmbossLtColor = GRAY80;
    alt5Scheme->CommonBkColor = BLACK;
//    alt5Scheme->pFont = (void*)&GOLSmallFont;
	unsigned char tt;//use for USB

       //InitialSETTING();


/*
//mark v  %   °ÄÖÞ
       EEPROMWriteWord(0x0097,0x6000);//-2ÎÂ¶È
       EEPROMWriteWord(0x1900,0x600e);//-2 AD

       EEPROMWriteWord(0x00ab,0x6002);//5 tem
       EEPROMWriteWord(0x1950,0x6010);//5 AD

       EEPROMWriteWord(0x00ca,0x6004);//15 tem
       EEPROMWriteWord(0x1993,0x6012);//15 AD

       EEPROMWriteWord(0x00df,0x6006);//22 tem
       EEPROMWriteWord(0x19c6,0x6014);//22 AD
       
       EEPROMWriteWord(0x00f3,0x6008);//28 tem
       EEPROMWriteWord(0x19c6,0x6016);//28 AD

       EEPROMWriteWord(0x010a,0x600a);//35 tem
       EEPROMWriteWord(0x193a,0x6018);//35 AD

       EEPROMWriteWord(0x012a,0x600c);//45 tem
       EEPROMWriteWord(0x1800,0x601a);//45 AD
*/


//mark v  g/l  ²¨À¼
/*
       EEPROMWriteWord(0x0097,0x6000);//-2ÎÂ¶È
       EEPROMWriteWord(0x1b7e,0x600e);//-2 AD

       EEPROMWriteWord(0x00aa,0x6002);//5 tem
       EEPROMWriteWord(0x1c1d,0x6010);//5 AD

       EEPROMWriteWord(0x00ca,0x6004);//15 tem
       EEPROMWriteWord(0x1cdb,0x6012);//15 AD

       EEPROMWriteWord(0x00df,0x6006);//22 tem
       EEPROMWriteWord(0x1d00,0x6014);//22 AD
       
       EEPROMWriteWord(0x00f3,0x6008);//28 tem
       EEPROMWriteWord(0x1d00,0x6016);//28 AD

       EEPROMWriteWord(0x010a,0x600a);//35 tem
       EEPROMWriteWord(0x1cbb,0x6018);//35 AD

       EEPROMWriteWord(0x012a,0x600c);//45 tem
       EEPROMWriteWord(0x1b79,0x601a);//45 AD
*/
	//SST25ChipErase();
	
	//EEPROMWriteWord(30,EESERIAL_NUMBER);

    while(1){
		  
		// Application-specific tasks.
		// Application related code may be added here, or in the ProcessIO() function.
		ProcessIO();      
	if((USBDeviceState ==POWERED_STATE)||(USBDeviceState == DEFAULT_STATE)||(USBSuspendControl==1)||(USBDeviceState==ATTACHED_STATE))
       {
       if(GOLDraw()){             // Draw GOL objects
       
            // Drawing is done here, process messages
   //         TouchGetMsg(&msg);     // Get message from touch screen
    //        GOLMsg(&msg);          // Process message
            SideButtonsMsg(&msg);  // Get message from side buttons
            GOLMsg(&msg);          // Process message
        }
       if((FirstIn)&&(tt>250)) FirstIn = FALSE;
        	}
    else if(!FirstIn)
		{
			screenState = CREATE_TO_PC;
			GOLDraw();
			FirstIn = TRUE;
			tt=0;
		}
		tt++;

    } 

}
/********************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization
 *                  routine. All required USB initialization routines
 *                  are called from here.
 *
 *                  User application initialization routine should
 *                  also be called from here.                  
 *
 * Note:            None
 *******************************************************************/
 static void InitializeSystem(void)
{
 
    
    UserInit();

    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
}//end InitializeSystem

/******************************************************************************
 * Function:        void UserInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine should take care of all of the demo code
 *                  initialization that is required.
 *
 * Note:            
 *
 *****************************************************************************/
void UserInit(void)
{
    //initialize the variable holding the handle for the last
    // transmission
    USBOutHandle = 0;
    USBInHandle = 0;
}//end UserInit

/********************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user
 *                  routines. It is a mixture of both USB and
 *                  non-USB tasks.
 *
 * Note:            None
 *******************************************************************/
void ProcessIO(void)
{   
	static unsigned char packetcnt;
    unsigned char pz;
//////////////////////////////////////////
	unsigned long j;
	static unsigned long startrecordnmb;
	WORD page,x;
	static unsigned int length;
	static unsigned char buff[255];
	unsigned char cs=0;
	WORD_VAL wordval;
	DWORD addr;
//////////////////////////////////////////
    // User Application USB tasks
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return;

    if(!HIDRxHandleBusy(USBOutHandle))//Check if data was received from the host.
    {   
    
        switch(ReceivedDataBuffer[0])				//Look at the data the host sent, to see what kind of application specific command it sent.
        {
        case 0x01:
        	{
        	ToSendDataBuffer[0] = 0x01;
        	switch(ReceivedDataBuffer[1])				//Look at the data the host sent, to see what kind of application specific command it sent.
        	{
        	case 0x01:
        		ToSendDataBuffer[1] = 0x01;
        		//RD_MAX_RECORD 2bytes
			wordval.Val = serialnumber-1; 
			ToSendDataBuffer[2] = wordval.v[1];
			ToSendDataBuffer[3] = wordval.v[0];
			//DEVICE ID    3btyes
                	EEPROMReadArray(0x6a00, &(ToSendDataBuffer[4]), 4);
                	//CHECKITEM   10bytes
                	EEPROMReadArray(0x7e00, &(ToSendDataBuffer[8]), 10);//ITEM
                	//CALIBRATE DATE 3bytes
                	EEPROMReadArray(0x7f3d, &(ToSendDataBuffer[18]), 3);//CALIBRATE DATE
            		//ITEM1 16bytes
            		EEPROMReadArray(0x7040, &(ToSendDataBuffer[21]), 16);
            		break;
            	case 0x02:
            		ToSendDataBuffer[1] = 0x02;
            		// ITEM2-3-4   48bytes
            		EEPROMReadArray(0x7050, &(ToSendDataBuffer[2]), 48);
      			break;
      		case 0x03:
            		ToSendDataBuffer[1] = 0x03;
            		// ITEM5-6-7   48bytes
            		EEPROMReadArray(0x7080, &(ToSendDataBuffer[2]), 48);
      			break;	
      		case 0x04:
            		ToSendDataBuffer[1] = 0x04;
            		// ITEM8-9-10   48bytes
            		EEPROMReadArray(0x70b0, &(ToSendDataBuffer[2]), 48);
      			break;	
            	}
            	if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
                }
	case 0x02://RD_RECORD
		{
        	ToSendDataBuffer[0] = 0x02;
        	switch(ReceivedDataBuffer[1])				//Look at the data the host sent, to see what kind of application specific command it sent.
        	{
        	case 0x01:
        		ToSendDataBuffer[1] = 0x01;
        		wordval.v[1] = ReceivedDataBuffer[2];
			wordval.v[0] = ReceivedDataBuffer[3];
			x = wordval.Val;
			if(x < serialnumber) 
			{
	           addr = (x-1)*30;//

               SST25ReadArray(addr, buff, 30);
			   
			   memcpy(&ToSendDataBuffer[2],&buff[0],62);
			}
            		break;
            	case 0x02:
            		ToSendDataBuffer[1] = 0x02;
            		memcpy(&ToSendDataBuffer[2],&buff[62],62);
      			break;
      		case 0x03:
            		ToSendDataBuffer[1] = 0x03;
            		memcpy(&ToSendDataBuffer[2],&buff[124],62);
      			break;	
      		case 0x04:
            		ToSendDataBuffer[1] = 0x04;
            		memcpy(&ToSendDataBuffer[2],&buff[186],62);
      			break;
      		}
      		if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
        	}
        case 0x03://set device id
        	{
        	ToSendDataBuffer[1] = 0x03;	
        	//DEVICE ID    3btyes
                EEPROMWriteArray(0x6a00, &(ReceivedDataBuffer[2]), 4);
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }	
        	break;	
        	}
        case 0x04://clear record
        	{
        	ToSendDataBuffer[1] = 0x04;
        	EEPROMWriteWord(1,EESERIAL_NUMBER);// Ð´Ò»×÷Îª¼ÇÂ¼µÄÌõÊý¿ªÊ¼
		serialnumber = 1;//EEPROMReadWord(EESERIAL_NUMBER);
		serialnumberRecord = serialnumber - 1;// ±£³Ö¼ÇÂ¼Ò»ÖÂ
		if(!HIDTxHandleBusy(USBInHandle))
                {
                    USBInHandle = HIDTxPacket(HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
        	break;	
        	}
        }  
        //Re-arm the OUT endpoint for the next packet
        USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);
    }
    
}//end ProcessIO


/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{


}





/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{

}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.
	
	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
  *****************************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();

}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *****************************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/******************************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 *
 * Note:            None
 *****************************************************************************/
void USBCBInitEP(void)
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP,(BYTE*)&ReceivedDataBuffer,64);

}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function should only be called when:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            Interrupt vs. Polling
 *                  -Primary clock
 *                  -Secondary clock ***** MAKE NOTES ABOUT THIS *******
 *                   > Can switch to primary first by calling USBCBWakeFromSuspend()
 
 *                  The modifiable section in this routine should be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of 1-13 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at lest 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    USBResumeControl = 1;                // Start RESUME signaling
    
    delay_count = 1800U;                // Set RESUME line for 1-13 ms
    do
    {
        delay_count--;
    }while(delay_count);
    USBResumeControl = 0;
}


/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER:
            Nop();
            break;
        default:
            break;
    }      
    return TRUE; 
}
/** EOF main.c ***************************************************************/
/////////////////////////////////////////////////////////////////////////////
// Function: WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
// Input: objMsg - translated message for the object,
//        pObj - pointer to the object,
//        pMsg - pointer to the non-translated, raw GOL message 
// Output: if the function returns non-zero the message will be processed by default
// Overview: this function must be implemented by user. GOLMsg() function calls it each
//           time the valid message for the object received
/////////////////////////////////////////////////////////////////////////////
WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

    // beep if button is pressed
    if(objMsg == BTN_MSG_PRESSED){
        Beep();
    }else{
        if(GetObjType(pObj) == OBJ_RADIOBUTTON){
            Beep();
        }else{
            if(GetObjType(pObj) == OBJ_CHECKBOX)
                Beep();
        }
    }

	if ((screenState & 0xF300) != 0xF300) {
		// check for time setting press, process only when not setting time and date
		if (objMsg == ST_MSG_SELECTED) {

/*	//  µã»÷Ê±¼ä´¥·¢ÉèÖÃ
			if ((GetObjID(pObj) == ID_STATICTEXT1) || (GetObjID(pObj) == ID_STATICTEXT2)) {
				prevState = screenState - 1;			// save the current create state
	    	    screenState = CREATE_DATETIME;			// go to date and time setting screen
		    	return 1;	
		    }
		    */
			if ((GetObjID(pObj) == ID_STATICTEXT4)|(GetObjID(pObj) == ID_STATICTEXT5)|
				(GetObjID(pObj) == ID_STATICTEXT16)|(GetObjID(pObj) == ID_STATICTEXT17)|
				(GetObjID(pObj) == ID_STATICTEXT18)|(GetObjID(pObj) == ID_STATICTEXT19)|
				(GetObjID(pObj) == ID_STATICTEXT10)|(GetObjID(pObj) == ID_STATICTEXT11)|
				(GetObjID(pObj) == ID_STATICTEXT12)|(GetObjID(pObj) == ID_STATICTEXT13)|
				(GetObjID(pObj) == ID_STATICTEXT14)|(GetObjID(pObj) == ID_STATICTEXT15))
				{
				ItemID = GetObjID(pObj);
					prevState = screenState - 1;
					screenState = CREATE_PREPROCESS;
				}
	    } 
    }


// check if pull down menu is called
    if (GetObjID(pObj) == ID_WINDOW1) {
#ifndef	USE_SSD1289
	if ((objMsg == WND_MSG_TITLE) && (screenState != DISPLAY_PULLDOWN))
#endif
	{
	// check area of press
		    if ((pMsg->param1 <= 40) && (pMsg->param2 <= 40)) {
			    switch(screenState) {

					// pull down is disabled when setting date and time
			    	case CREATE_DATETIME: 						
    				case DISPLAY_DATETIME: 		
    				case DISPLAY_DATE_PDMENU: 
    				case SHOW_DATE_PDMENU:
    				case HIDE_DATE_PDMENU:
    					
    				case CREATE_UNIT_SCREEN:
    				case DISPLAY_UNIT_SCREEN:
    				case SHOW_UNIT_PDMENU:
    				case DISPLAY_UNIT_PDMENU:
    					return 0; 
    		    	default:	
		    			prevState = screenState;		// save the current create state
			    	 	break;
			   	}

			    
			    return 1;
			}
		}
	}
   
    // process messages for demo screens
    switch(screenState){


	

        // date and time settings display     
        case DISPLAY_DATETIME:
			g_standby_time = EEPROMReadWord(BackLightTimeAddress);
            return MsgDateTime(objMsg, pObj);
        case DISPLAY_DATE_PDMENU:
         	return MsgSetDate(objMsg, pObj, pMsg);  
    	case CREATE_DATETIME: 						
		case SHOW_DATE_PDMENU:
    	case HIDE_DATE_PDMENU:
    		return 0;

	
        case DISPLAY_MAIN:
            return MsgMain(objMsg, pObj);
        case DISPLAY_SUBJECT_NAME:
//        	return MsgSubjectName(objMsg, pObj, pMsg);
return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_PLACE:
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_VEHICLENO:
//		return MsgEditTest(objMsg, pObj, pMsg);
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_BADGENO:
//		return MsgEditTest(objMsg, pObj, pMsg);
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_OPERATORNAME:
		return MsgEditABC(objMsg, pObj, pMsg);

	case DISPLAY_DEPARTMENT:
		return MsgEditTest(objMsg, pObj, pMsg);

	case DISPLAY_PREPROCESS:
		return MsgEditTest(objMsg, pObj, pMsg);
		
	case DISPLAY_RECORD:
		return MsgRecord(objMsg, pObj, pMsg);

	case DISPLAY_BLOW:
		return	MsgBlowPlease(objMsg, pObj);
	case DISPLAY_DISCONTINUED:
		return     MsgDISCONTINUED(objMsg, pObj, pMsg);
	case DISPLAY_SETTING:
		return MsgSetting(objMsg, pObj);
		
	case DISPLAY_EDITEEPROM:
		return  MsgEditeeprom(objMsg,  pObj,  pMsg);

	case DISPLAY_EDITID:
		return  MsgEditID(objMsg,  pObj,  pMsg);	

	case DISPLAY_EDITPRINTID:
		return  MsgEditID(objMsg,  pObj,  pMsg);		
		
#ifdef	USE_BACKLIGHT
	case DISPLAY_SETBACKLIGHT:
		g_standby_time = EEPROMReadWord(BackLightTimeAddress);
		return MsgSetBackLight(objMsg, pObj);
#endif

	case DISPLAY_UNIT_PDMENU:
//         	return MsgSetDate(objMsg, pObj, pMsg); 
         	return MsgUnitPDment(objMsg, pObj, pMsg);

	case DISPLAY_TO_PC:
		return MsgToPC(objMsg, pObj);
	case DISPLAY_RESULT:
		return MsgResult(objMsg, pObj);

	case DISPLAY_THICKNESS:
		return	MsgThickness(objMsg, pObj);
	case DISPLAY_KSETTING:
		MsgkSettingSel(pObj);
		return	MsgKSetting(objMsg, pObj);
	case DISPLAY_PRINTSETTING:
		return	MsgPrintSetting(objMsg, pObj);
	case DISPLAY_DEMARCATE:
		return	MsgDemarcate(objMsg, pObj);
	case DISPLAY_DEMARCATE2:
		return	MsgDemarcate2(objMsg, pObj);
	case DISPLAY_INPUT:
		return  MsgEditMark(objMsg, pObj, pMsg);
	case DISPLAY_Wait:	
		return  MsgWait(objMsg, pObj);
    case DISPLAY_Mark:
		return	MsgMark(objMsg, pObj);

 
		case DISPLAY_PASSWORD:
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
            		screenState = CREATE_INPUT;//CREATE_SETTING;
            		PasswordBuffer[0] = 0;
            		}
		return MsgPassword(objMsg, pObj, pMsg);
		
		case DISPLAY_ScreenPASSWORD:

		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
            			dif=1;
				screenState = CREATE_DELETE;
				YesNoNumber = 2;// É¾³ý¼ÇÂ¼
            		PasswordBuffer[0] = 0;
            		}
		return MsgScreenPassword(objMsg, pObj, pMsg);

		case DISPLAY_MASTERPASSWORD:

			if((PasswordBuffer[0] == '2')&&(PasswordBuffer[1] == '5')&&(PasswordBuffer[2] == '8')\
            		&&(PasswordBuffer[3] == '7')&&(PasswordBuffer[4] == '5')&&(PasswordBuffer[5] == '3'))
			{
			//while( ADS7843_INT!=1);//wait button released
            		screenState = CREATE_AGENT;//CREATE_SETTING;
            		PasswordBuffer[0] = 0;
            		}

			
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
			//while( ADS7843_INT!=1);//wait button released
		
			//while( ADS7843_INT!=1);//wait button released
            		screenState = CREATE_Master;//CREATE_SETTING;
            		PasswordBuffer[0] = 0;
            		}
            		
		return MsgMasterPassword(objMsg, pObj, pMsg);
		
		case DISPLAY_POWEROFF:
			return MsgPowerOFF(objMsg, pObj);

		case DISPLAY_REFUSE:
			return MsgRefuse(objMsg, pObj);
		
		case DISPLAY_BLOW_PRESS:
			return MsgBlowPress(objMsg, pObj);

		case DISPLAY_ITEM:
#ifdef USE_ITEM
			return MsgItem(objMsg, pObj, pMsg);
//			return MsgEditABC(objMsg, pObj, pMsg);
#else
			return 1;
#endif
		case DISPLAY_ITEM0:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM1:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM2:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM3:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM4:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM5:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM6:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM7:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM8:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_ITEM9:
			return MsgItemEditABC(objMsg, pObj, pMsg);
		case DISPLAY_AGENT:
			return MsgAgent(objMsg, pObj);
		case DISPLAY_Master:
			return MsgMaster(objMsg, pObj);
		case DISPLAY_Setpassword:
			return MsgSetpassword(objMsg, pObj);
		case DISPLAY_InputSetpassword1:
			return MsgEditPassword(objMsg, pObj, pMsg);
		case DISPLAY_InputSetpassword2:
			return MsgEditPassword(objMsg, pObj, pMsg);
		case DISPLAY_OutcomeSetpassword:
			return MsgOutcomeSetpassword(objMsg, pObj);				
		case DISPLAY_RESET:
			return MsgYesNo(objMsg, pObj);
		case DISPLAY_DELETE:
			return MsgYesNo(objMsg, pObj);
		case DISPLAY_CYC:
			return MsgCalibrate(objMsg, pObj);	
#ifdef Argentina
        case DISPLAY_EDITMAINDISPLAY:
			return MsgEditInspectDate(objMsg, pObj,pMsg);

#endif			
		case DISPLAY_CALLOCK:
			return MsgCalLock(objMsg, pObj);		
		case DISPLAY_YESNO:
			return MsgYesNo(objMsg, pObj);
		

        default:
            // process message by default
            return 1;
    }
}
/////////////////////////////////////////////////////////////////////////////
// Function: WORD GOLDrawCallback()
// Output: if the function returns non-zero the draw control will be passed to GOL
// Overview: this function must be implemented by user. GOLDraw() function calls it each
//           time when GOL objects drawing is completed. User drawing should be done here.
//           GOL will not change color, line type and clipping region settings while 
//           this function returns zero.
/////////////////////////////////////////////////////////////////////////////
WORD GOLDrawCallback(){
static DWORD prevTick  = 0;  		// keeps previous value of tick
static DWORD prevTime  = 0;  		// keeps previous value of time tick
static DWORD prevBatteryTime  = 0;
static BYTE  direction = 1,autoprintflag; 		// direction switch for progress bar
static BYTE  arrowPos  = 0;  		// arrows pictures position for custom control demo
static BYTE  pBDelay   = 40;  		// progress bar delay variable
OBJ_HEADER 	*pObj;					// used to change text in Window
SLIDER     	*pSld;					// used when updating date and time
LISTBOX    	*pLb;					// used when updating date and time
WORD  		i,j;
static BYTE  directionZYQ = 1;
BYTE  printnum,pddata;
static XCHAR predateTime = 0;


	// update the time display
	if ((screenState & 0x0000F300) != 0x0000F300) {		// process only when NOT setting time and date
		if ((tick-prevTime) > 1000){
			
			RTCCProcessEvents();						// update the date and time string variabes 



		if(screenState == DISPLAY_MAIN)
		{	
			BatteryValue = Adc_Count(4,5);
			ShowBattery(BatteryValue,BATT_TYPE_LITHIUM);//add by Spring.Chen
			prevBatteryTime = tick;
		}

		g_rtc_counter++;


	        if((g_standby_time != 0 && g_rtc_counter >= g_standby_time)|BoolBatteryOFF ) //×Ô¶¯¹Ø»ú
	        {
	        	POWER_CONTROL = 0;

	        }

	       if(g_blow_time != 0 && g_rtc_counter == g_blow_time) //×Ô¶¯¹Ø»ú
	        {
	        	screenState = CREATE_REFUSE;
	        	g_blow_time = 0;

	        }


			i = 0;						
			while (i < 12) {
				dateTimeStr[i] = _time_str[i];
				dateTimeStr[i+13] = _date_str[i];
				i++;
			}
			dateTimeStr[12] = 0x0000;//0x000A; 	// (XCHAR)'\n';
			dateTimeStr[25] = 0x0000;	// (XCHAR)'\0';
			WeekStr[0] = dateTimeStr[0];
			WeekStr[1] = dateTimeStr[1];
			WeekStr[2] = dateTimeStr[2];
			for(i=0;i<13;i++)
         {

             date_str[i]=dateTimeStr[13+i];
            }
			if(screenState == DISPLAY_MAIN)
		{
			if(dateTimeStr[4]==0x0030&&dateTimeStr[5]==0x0030&&dateTimeStr[7]==0x0030&&dateTimeStr[8]==0x0030&&dateTimeStr[10]==0x0030&&dateTimeStr[11]==0x0030)
				{
				  for(i=30;i>0;i--)
                    {	
                        SetColor(RGB565CONVERT(i,4*i,4*i));
	                    Bar(3, i, 40, i+1);
                    }
			   	   SetFont((void*)&GOLFontDefault);
                   SetColor(RGB565CONVERT(120, 196,66));
                   OutTextXY(2,3,WeekStr);
               for(i=50;i>0;i--)
                 { 
                    SetColor(RGB565CONVERT(i,2*i,5*i/2));//SetColor(RGB565CONVERT(5,70,60));
	                Bar(0, (GetMaxY()-i), GetMaxX(), (GetMaxY()-i+1));
                 }
	               SetFont((void*)&GOLFontDefault);
                   SetColor(RGB565CONVERT(120, 196,66));
                   OutTextXY(75,280,date_str);

			}
		 if(dateTimeStr[7]==0x0030&&dateTimeStr[8]==0x0030&&dateTimeStr[10]==0x0030&&dateTimeStr[11]==0x0030)
				{
				for(i=30;i>0;i--)
            {	SetColor(RGB565CONVERT(i,4*i,4*i));
	            Bar(52, i, 78, i+1);
             }

			}
			
		if(dateTimeStr[10]==0x0030&&dateTimeStr[11]==0x0030)
				{	for(i=30;i>0;i--)
            {	SetColor(RGB565CONVERT(i,4*i,4*i));
	            Bar(80, i, 107, i+1);
             }
			}
		if(dateTimeStr[11]==0x0030)
			{	for(i=30;i>0;i--)
                   {	SetColor(RGB565CONVERT(i,4*i,4*i));
	                    Bar(110, i, 125, i+1);
                   }
			}
		if(predateTime != dateTimeStr[11])
				{
			for(i=30;i>0;i--)
            {	SetColor(RGB565CONVERT(i,4*i,4*i));//SetColor(RGB565CONVERT(i,4*i,4*i));
	            Bar(125, i, 135, i+1);
             }
	            predateTime = dateTimeStr[9];			
				}
		
               SetFont((void*)&GOLFontDefault);
               SetColor(RGB565CONVERT(120, 196,66));
               OutTextXY(52,3,&dateTimeStr[4]);
				}
			prevTime = tick;							// reset tick timer
		}
	} else {											// process only when setting time and date
		if (screenState != DISPLAY_DATE_PDMENU) {		// do not update when pull down menus are on
			if ((tick-prevTime) > 1000){
			    updateDateTimeEb();						// update edit boxes for date and time settings
				prevTime = tick;						// reset tick timer
			}
		}
	}
	
    switch(screenState){

      


        case CREATE_DATETIME:
            CreateDateTime(); 							// create date and time demo
            screenState = DISPLAY_DATETIME; 			// switch to next state
            return 1;       							// draw objects created
            
	    case SHOW_DATE_PDMENU:
	    	ShowPullDownMenu(); 
	    	screenState = DISPLAY_DATE_PDMENU;
	    	return 1;    
	    	
		case HIDE_DATE_PDMENU:
			if (RemovePullDownMenu()) 
		        screenState = DISPLAY_DATETIME; 		// switch to next state
			return 1;

        case DISPLAY_DATE_PDMENU:
        	// this moves the slider and editbox for the date setting to 
        	// move while the up or down arrow buttons are pressed
        	if((tick-prevTick)>100) {
        		pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX1);		
			    pSld = (SLIDER*)GOLFindObject(ID_SLIDER1);
				pObj = GOLFindObject(ID_BUTTON_DATE_UP);
				
            	if(GetState(pObj, BTN_PRESSED)) {
                	LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                	SetState(pLb, LB_DRAW_ITEMS);
                	SldSetPos(pSld,SldGetPos(pSld)+1);
                	SetState(pSld, SLD_DRAW_THUMB);
                }	
				pObj = GOLFindObject(ID_BUTTON_DATE_DN);
				
            	if(GetState(pObj, BTN_PRESSED)) {
	                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
    	            SetState(pLb, LB_DRAW_ITEMS);
        	        SldSetPos(pSld,SldGetPos(pSld)-1);
            	    SetState(pSld, SLD_DRAW_THUMB);
            	}
            	prevTick = tick; 
            }
            return 1;

       case DISPLAY_DATETIME:
//       	case DISPLAY_UNIT_SCREEN:

        	// Checks if the pull down menus are to be created or not
		    pObj = GOLFindObject(ID_BUTTON_MO);
		    if (GetState(pObj, BTN_PRESSED)) {
                screenState = SHOW_DATE_PDMENU;  	// change state 
		   		return 1;
		   	}
		    pObj = GOLFindObject(ID_BUTTON_YR);
		    if (GetState(pObj, BTN_PRESSED)) {
                screenState = SHOW_DATE_PDMENU;  	// change state 
		   		return 1;
		   	}
		    pObj = GOLFindObject(ID_BUTTON_DY);
		    if (GetState(pObj, BTN_PRESSED)) {
                screenState = SHOW_DATE_PDMENU;  	// change state 
		   		return 1;
		   	}
        	// this increments the values for the time settings
        	// while the + or - buttons are pressed
       		if((tick-prevTick)>200) {
	       		pObj = GOLFindObject(ID_BUTTONHR_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONHR_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
	            prevTick = tick; 
	        }
	        return 1;
 
     

	case CREATE_MAIN:	
        	CreateMain();
        	screenState = DISPLAY_MAIN; 
        	return 1;    
        	
        case DISPLAY_MAIN:
            return 1; 									// redraw objects if needed

	case CREATE_SUBJECT_NAME:	
        	CreateEditABC(SubjectNameStr,SubjectNameMax);//MaxChar
        	screenState = DISPLAY_SUBJECT_NAME; 
        	return 1;    
        	
        case DISPLAY_SUBJECT_NAME:
            return 1; 									// redraw objects if needed

        case CREATE_PLACE:	
        	CreateEditABC(PlaceStr,PlaceMax);//MaxChar
        	screenState = DISPLAY_PLACE; 
        	return 1;    
        	
        case DISPLAY_PLACE:
            return 1; 									// redraw objects if needed

	case CREATE_VEHICLENO:	
//        	CreateEditTest(VehicleNoStr,VehicleNoMax);//MaxChar
#ifndef USE_ITEM
        	CreateEditABC(VehicleNoStr,VehicleNoMax);//MaxChar
#else
        	CreateEditABC(Item0Str,ItemMax);
#endif
        	screenState = DISPLAY_VEHICLENO; 
        	return 1;    
        	
        case DISPLAY_VEHICLENO:
		//	pump_pwr=1;
			return 1; 									// redraw objects if needed

        case CREATE_BADGENO:	
        	CreateEditTest(BadgeNoStr,BadgeNoMax);//  1); //MaxChar
        	screenState = DISPLAY_BADGENO; 
        	return 1;    
        	
        case DISPLAY_BADGENO:
            return 1; 									// redraw objects if needed

        case CREATE_OPERATORNAME:	
        	CreateEditABC(OperatorNameStr,OperatorNameMax);//MaxChar
        	screenState = DISPLAY_OPERATORNAME; 
        	return 1;    
        	
        case DISPLAY_OPERATORNAME:
            return 1; 									// redraw objects if needed

        case CREATE_DEPARTMENT:	
        	CreateEditTest(DepartmentStr,DepartmentMax);//MaxChar
        	screenState = DISPLAY_DEPARTMENT; 
        	return 1;    
        	
        case DISPLAY_DEPARTMENT:
            return 1; 									// redraw objects if needed

        case CREATE_PREPROCESS:
#ifdef USE_ITEM
				if((ItemID == ID_STATICTEXT10))
		{	
//			CreateEditTest(Item0Str,ItemMax);
			CreateEditABC(Item0Str,ItemMax);
			ItemID = ID_STATICTEXT10;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT11))
		{
//			CreateEditTest(Item1Str,ItemMax);
			CreateEditABC(Item1Str,ItemMax);
			ItemID = ID_STATICTEXT11;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT12))
		{	
//			CreateEditTest(Item2Str,ItemMax);
			CreateEditABC(Item2Str,ItemMax);
			ItemID = ID_STATICTEXT12;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT13))
		{
//			CreateEditTest(Item3Str,ItemMax);
			CreateEditABC(Item3Str,ItemMax);
			ItemID = ID_STATICTEXT13;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT14))
		{	
//			CreateEditTest(Item4Str,ItemMax);
			CreateEditABC(Item4Str,ItemMax);
			ItemID = ID_STATICTEXT14;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT15))
		{
//			CreateEditTest(Item5Str,ItemMax);
			CreateEditABC(Item5Str,ItemMax);
			ItemID = ID_STATICTEXT15;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT16))
		{	
//			CreateEditTest(Item6Str,ItemMax);
			CreateEditABC(Item6Str,ItemMax);
			ItemID = ID_STATICTEXT16;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT17))
		{
//			CreateEditTest(Item7Str,ItemMax);
			CreateEditABC(Item7Str,ItemMax);
			ItemID = ID_STATICTEXT17;
			screenState = DISPLAY_ITEM;
    		}
		else if((ItemID == ID_STATICTEXT18))
		{	
//			CreateEditTest(Item8Str,ItemMax);
			CreateEditABC(Item8Str,ItemMax);
			ItemID = ID_STATICTEXT18;
			screenState = DISPLAY_ITEM;
		}
		else if((ItemID == ID_STATICTEXT19))
		{
//			CreateEditTest(Item9Str,ItemMax);
			CreateEditABC(Item9Str,ItemMax);
			ItemID = ID_STATICTEXT19;
			screenState = DISPLAY_ITEM;
    		}
#endif
        	return 1;    
        	
        case DISPLAY_PREPROCESS:
            return 1; 									// redraw objects if needed

	case CREATE_BLOW:
            CreateBlowPlease();
            PbSetPos(pProgressBar,100);
           Temp_adc=Adc_Count(9,5);
		   initialpress =Temp_adc;//+ EEPROMReadWord(BlowPressDataAddress)/2;//Temp_adc;
           Temp_adc+=EEPROMReadWord(BlowPressDataAddress);
           pBDelay = EEPROMReadWord(BlowTimeDataAddress);
		   if(TestMode==0xcc)
		   	{
		   	  Temp_adc=5+initialpress;
			  pBDelay =1;	
		   	}
           g_blow_time = DelayRefussTime;
           status_Press=FALSE;
            screenState = DISPLAY_BLOW;
        	return 1;
        	
        case DISPLAY_BLOW:

        	 if((tick-prevTick)>pBDelay){

        	 if(press_test(0xaa))
        	 	{
#ifdef QuickTestMode
		   if(TestMode==0xcc)
		   	{
		   	    direction = 1;    				// change direction
                directionZYQ = 0;
 				goto goto_test;
		   	}
#endif
                if(direction){
                    if(pProgressBar->pos == 0)
                    {
                        	direction = 1;    				// change direction
 //                       	PbSetPos(pProgressBar,100);
                        	directionZYQ = 0;
 				goto goto_test;
                    }
                    else
                    {
                        PbSetPos(pProgressBar,PbGetPos(pProgressBar)-1); // decrease
				if(pProgressBar->pos == 0)
				{
					directionZYQ = 0;
					direction = 1;
					epPress=Adc_Count(9,5);
					epPress=(epPress+initialpress)/2;//Adc_Count(9,5);
					//epPress=epPress-8;
				}
				status_Press=TRUE;
				g_blow_time = 0;//´µÆø¹ý³Ì½ûÖ¹¾Ü¾ø²âÊÔ½çÃæ³öÏÖ
				Beep();
                    }
                    
                }
                SetState(pProgressBar,PB_DRAW_BAR); 	// redraw bar only
                prevTick = tick;
        	 	}
        	 else if(status_Press&&(pProgressBar->pos!=0))
        	 	{
	        	 	PbSetPos(pProgressBar,100);
				SetState(pProgressBar,PB_DRAW_BAR); 	// redraw bar only
				status_Press=FALSE;
				if(TestMode==0xcc)
					screenState = CREATE_BLOW;
				else
        	 	    screenState = CREATE_DISCONTINUED;        	 		
				    return 1;
        	 	}
        	 directionZYQ = 1;
			 
            }

goto_test:		

	    if(!directionZYQ)//&&(!press_test(0xbb)))
		   {
            //while(press_test(0xbb));
            Beep();
            DelayMs(100);
		   if(TestMode==0xcc)
		   	{

           	   Manual_Analyze();//µ÷²âÊÔ¾Æ¾«Å¨¶È³ÌÐò
			   screenState = CREATE_RESULT; 
			   status_Press=TRUE;
		   }   
           else if(!press_test(0xbb))
            {
			   Manual_Analyze();//µ÷²âÊÔ¾Æ¾«Å¨¶È³ÌÐò
			   screenState = CREATE_RESULT; 
			   status_Press=TRUE;
            }
		  }
            return 1; 									// redraw objects if needed
            
       case CREATE_DISCONTINUED:
        	CreateDISCONTINUED();//
        	screenState = DISPLAY_DISCONTINUED; 
        	return 1;    
        	
        case DISPLAY_DISCONTINUED:
			g_blow_time = 0;//´µÆø¹ý³Ì½ûÖ¹¾Ü¾ø²âÊÔ½çÃæ³öÏÖ
            return 1; 
	case CREATE_RECORD:

        	CreateRecord();//MaxChar

        	screenState = DISPLAY_RECORD; 
        	return 1;    
        	
        case DISPLAY_RECORD:

            return 1; 									

	case CREATE_SETTING:	
        	CreateSetting();//MaxChar
        	screenState = DISPLAY_SETTING; 
        	return 1;    
        	
        case DISPLAY_SETTING:
            return 1; 									// redraw objects if needed

	case CREATE_EDITEEPROM:	
        	CreateEditeeprom(EEPROMEDIT,6);
        	screenState = DISPLAY_EDITEEPROM; 
        	return 1;    
        	
        case DISPLAY_EDITEEPROM:
            return 1; 									// redraw objects if needed

	case CREATE_EDITID:	
        	CreateEditTest(IDumberStr,8);
        	screenState = DISPLAY_EDITID; 
        	return 1;    
        	
        case DISPLAY_EDITID:
            return 1; 								

	case CREATE_EDITPRINTID:	
        	CreateEditTest(IDPrintModel,8);
        	screenState = DISPLAY_EDITPRINTID; 
        	return 1;    
        	
    case DISPLAY_EDITPRINTID:
            return 1;			
			
#ifdef	USE_BACKLIGHT
	case CREATE_SETBACKLIGHT:	
        	CreateSetBackLight();//MaxChar
//        	CreatePicture();
        	screenState = DISPLAY_SETBACKLIGHT; 
        	return 1;    
        	
        case DISPLAY_SETBACKLIGHT:
            return 1; 
#endif

	case CREATE_TO_PC:	
        	CreateConnectToPC( );//To the pc
        	screenState = DISPLAY_TO_PC; 
        	return 1;    
        	
        case DISPLAY_TO_PC:
            return 1;

       case CREATE_RESULT:
		    SPrint=TRUE;
		
        	g_backlight_time = 0;	//´µÆø³É¹¦»Ö¸´²»Ìø×ª
        	g_blow_time = 0;		// ½á¹û³öÀ´»Ö¸´Õý³£
        	CreateResult( );
        	screenState = DISPLAY_RESULT; 
			autoprintflag = EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX7);		
        	return 1;    
        	
        case DISPLAY_RESULT:

		    BTN_S3 = 1;
	        DelayMs(10);
	        if(BTN_S3 == 0)
		      {S4_count ++;}
	        else
		      {S4_count = 0;}
	        BTN_S3 = 1;
	        if(S4_count >80)	// ³¤°´5ÃëDebug
	          {
		       screenState=CREATE_Debug2;
		       S4_count = 0;
		       return 1;
	          }
	if(autoprintflag&&inputflag)
    {
			SPrint=TRUE;
			ChoicePrint = TRUE;
			Sprinnum = 0;
			PrintSec=0;
			PrintCount = 0;
			memset(&Flashbuff[0],0,30);
			readrecord();
			for(i=0;i<30;i++)
				Printbuff[i]=(unsigned char)Flashbuff[i];
			Inputdata();
			p_n = 0;
            Print_TestRecord();
			inputflag = FALSE;
			
   }			

			return 1;


	case CREATE_UNIT_SCREEN:	
        	CreateUnitPDmenu();//MaxChar
        	screenState = DISPLAY_UNIT_SCREEN; 
        	return 1;

        case DISPLAY_UNIT_SCREEN:
        	pObj = GOLFindObject(ID_BUTTON_UNIT);
		    if (GetState(pObj, BTN_PRESSED)) 
                	{
                		screenState = SHOW_UNIT_PDMENU;  	// change state 
		   		return 1;
		    	}
if((tick-prevTick)>200) {
	       		pObj = GOLFindObject(ID_BUTTONHR_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONHR_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONMN_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_P);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
				pObj = GOLFindObject(ID_BUTTONSC_M);
            	if(GetState(pObj, BTN_PRESSED)) {
	            	MsgDateTime(BTN_MSG_PRESSED, pObj);
	            }
	            prevTick = tick; 
	        }
		    return 1;
		   	

        	
        case SHOW_UNIT_PDMENU:
        	ShowUnitPullDownMeny();
        	screenState = DISPLAY_UNIT_PDMENU;  	// change state 
            return 1; 									// redraw objects if needed

        case DISPLAY_UNIT_PDMENU:
        	if((tick-prevTick)>100) {
        		pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX4);		
			    pSld = (SLIDER*)GOLFindObject(ID_SLIDER4);
				pObj = GOLFindObject(ID_BUTTON_DATE_UP);
				
            	if(GetState(pObj, BTN_PRESSED)) {
                	LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                	SetState(pLb, LB_DRAW_ITEMS);
                	SldSetPos(pSld,SldGetPos(pSld)+1);
                	SetState(pSld, SLD_DRAW_THUMB);
                }	
				pObj = GOLFindObject(ID_BUTTON_DATE_DN);
				
            	if(GetState(pObj, BTN_PRESSED)) {
	                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
    	            SetState(pLb, LB_DRAW_ITEMS);
        	        SldSetPos(pSld,SldGetPos(pSld)-1);
            	    SetState(pSld, SLD_DRAW_THUMB);
            	}
            	prevTick = tick; 
            }
            return 1;
	case CREATE_THICKNESS:
		CreateThickness();// Ó¢ÎÄÓÃ
//		CreateUnits();// ÖÐÎÄÓÃ
		screenState = DISPLAY_THICKNESS;
		return 1;    
	case DISPLAY_THICKNESS:
		return 1;
	case CREATE_KSETTING:
		CreateKSetting();
		screenState = DISPLAY_KSETTING;
		return 1;    
	case DISPLAY_KSETTING:
		return 1;
	case CREATE_PRINTSETTING:
//		TestMiMaStr[0] = EEPROMReadByte(PrintTimesAddress);
		CreatePrintSetting();
		screenState = DISPLAY_PRINTSETTING;
		return 1;    
	case DISPLAY_PRINTSETTING:
		return 1;

	case CREATE_DEMARCATE:
		Adj_tem = EEPROMReadWord(DemarcateAddress);
		Adj_adc = EEPROMReadWord(DemarcateAddress+14);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+2);
		Adj_adc = EEPROMReadWord(DemarcateAddress+16);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+4);
		Adj_adc = EEPROMReadWord(DemarcateAddress+18);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+6);
		Adj_adc = EEPROMReadWord(DemarcateAddress+20);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+8);
		Adj_adc = EEPROMReadWord(DemarcateAddress+22);
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);


        Adj_tem = EEPROMReadWord(DemarcateAddress+10);
		Adj_adc = EEPROMReadWord(DemarcateAddress+24);
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

		Adj_tem = EEPROMReadWord(DemarcateAddress+12);
		Adj_adc = EEPROMReadWord(DemarcateAddress+26);
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);
		CreateDemarcate();
		screenState = DISPLAY_DEMARCATE;
		return 1;    
	case DISPLAY_DEMARCATE:
		return 1;
	case CREATE_DEMARCATE2:
		Adj_tem = EEPROMReadWord(DemarcateAddress);
		Adj_adc = EEPROMReadWord(DemarcateAddress+14);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = EEPROMReadWord(DemarcateAddress+2);
		Adj_adc = EEPROMReadWord(DemarcateAddress+16);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+4);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+18);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+6);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+20);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+8);
		Adj_adc = EEPROMReadWord(DemarcateAddress+22);
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);


        Adj_tem = EEPROMReadWord(DemarcateAddress+10);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+24);
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

	    Adj_tem = EEPROMReadWord(DemarcateAddress+12);
		Adj_adc = EEPROMReadWord(DemarcateAddress+26);
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);

		CreateDemarcate2();
		screenState = DISPLAY_DEMARCATE2;
		return 1;
	case DISPLAY_DEMARCATE2:
		return 1;

	case CREATE_INPUT:
		CreateEditTest(INPUTStr,6);
		screenState = DISPLAY_INPUT;
		return 1;
	case DISPLAY_INPUT:
		return 1;
    case CREATE_Wait:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Wait;
		return 1;
	case DISPLAY_Wait:
		prepare_and_Charge();
        screenState =CREATE_Mark;
		 return 1;
    
	case CREATE_Waittest:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Waittest;
		return 1;
	case DISPLAY_Waittest:
		prepare_and_Charge();
             screenState =CREATE_BLOW;
			Nop();
        	       Nop();
			pump_on =0;
			Nop();
			Nop();
		return 1;

	case CREATE_Waitdem:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Waitdem;
		return 1;
	case DISPLAY_Waitdem:
		prepare_and_Charge();
        screenState =CREATE_DEMARCATE;
		return 1;	

	case CREATE_Waitdem2:
		CreateWait();
		//prepare_and_Charge();
		screenState = DISPLAY_Waitdem2;
		return 1;
	case DISPLAY_Waitdem2:
		prepare_and_Charge();
        screenState =CREATE_DEMARCATE2;
		return 1;	

    case CREATE_Mark:

        MarkAdc=EEPROMReadWord(0x6022);
		Hexshow(MarkAD2,MarkAdc);

        CreateMark();
		screenState = DISPLAY_Mark;
		return 1;   
	case DISPLAY_Mark:
         return 1;




	case CREATE_PASSWORD:
		CreateEditTest(PasswordStr,6);
		screenState = DISPLAY_PASSWORD;
		pddata=EEPROMReadByte(SettingPasswordAddress);
		PDread[0] = (pddata&0xF0)/16+0x30;
		PDread[1] = (pddata&0x0F)+0x30;
		pddata=EEPROMReadByte(SettingPasswordAddress+1);		
		PDread[2] = (pddata&0xF0)/16+0x30;
		PDread[3] = (pddata&0x0F)+0x30;
		pddata=EEPROMReadByte(SettingPasswordAddress+2);		
		PDread[4] = (pddata&0xF0)/16+0x30;
		PDread[5] = (pddata&0x0F)+0x30;		
		return 1;    
	case DISPLAY_PASSWORD:
		return 1;

   	case CREATE_ScreenPASSWORD:
		CreateEditTest(DeleteStr,6);
		screenState = DISPLAY_ScreenPASSWORD;
#ifdef Argentina		
		PDread[0] = (EEPROMReadByte(DeleteSettingPasswordAddress)&0xF0)/16+0x30;
		PDread[1] = (EEPROMReadByte(DeleteSettingPasswordAddress)&0x0F)+0x30;
		PDread[2] = (EEPROMReadByte(DeleteSettingPasswordAddress+1)&0xF0)/16+0x30;
		PDread[3] = (EEPROMReadByte(DeleteSettingPasswordAddress+1)&0x0F)+0x30;
		PDread[4] = (EEPROMReadByte(DeleteSettingPasswordAddress+2)&0xF0)/16+0x30;
		PDread[5] = (EEPROMReadByte(DeleteSettingPasswordAddress+2)&0x0F)+0x30;	
#endif		
		return 1;    
	case DISPLAY_ScreenPASSWORD:
		return 1;
		
	case CREATE_MASTERPASSWORD:
		CreateEditTest(PasswordStr,6);
		screenState = DISPLAY_MASTERPASSWORD;
		PDread[0] = (EEPROMReadByte(AdvanceSettingPasswordAddress)&0xF0)/16+0x30;
		PDread[1] = (EEPROMReadByte(AdvanceSettingPasswordAddress)&0x0F)+0x30;
		PDread[2] = (EEPROMReadByte(AdvanceSettingPasswordAddress+1)&0xF0)/16+0x30;
		PDread[3] = (EEPROMReadByte(AdvanceSettingPasswordAddress+1)&0x0F)+0x30;
		PDread[4] = (EEPROMReadByte(AdvanceSettingPasswordAddress+2)&0xF0)/16+0x30;
		PDread[5] = (EEPROMReadByte(AdvanceSettingPasswordAddress+2)&0x0F)+0x30;
		return 1;    
	case DISPLAY_MASTERPASSWORD:
		return 1;

	case CREATE_POWEROFF:
		CreatePowerOff();
		BoolBatteryOFF = TRUE;
		screenState = DISPLAY_POWEROFF;
		return 1;
	case DISPLAY_POWEROFF:
		return 1;

	case CREATE_REFUSE:
		CreateRefuse();
		screenState = DISPLAY_REFUSE;
		return 1;
	case DISPLAY_REFUSE:
		return 1;
		
	case CREATE_BLOW_PRESS:
		CreateBlowPress();
		screenState = DISPLAY_BLOW_PRESS;
		return 1;
	case DISPLAY_BLOW_PRESS:
		return 1;
   case CREATE_Debug:

        Adj_tem = Decade[0];
		Adj_adc = Decade[1];
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = Decade[2];
		Adj_adc = Decade[3];
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = Decade[4];
	    Adj_adc = Decade[5];
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);
	

        Adj_tem = Decade[6];
	    Adj_adc = Decade[7];
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

		Adj_tem = Decade[8];
	    Adj_adc = Decade[9];
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);

        Adj_tem = Decade[10];
	    Adj_adc = Decade[11];
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

        Adj_tem = Decade[12];
	    Adj_adc = Decade[13];
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);

		Adj_tem = Decade[14];
	    Adj_adc = Decade[15];
        Hexshow(DemarcateAD14, Adj_tem);
		Hexshow(DemarcateAD15, Adj_adc);

		Adj_tem = Decade[16];
	    Adj_adc = Decade[17];
        Hexshow(DemarcateAD16, Adj_tem);
		Hexshow(DemarcateAD17, Adj_adc);

		Adj_tem = Decade[18];
	    Adj_adc = Decade[19];
        Hexshow(DemarcateAD18, Adj_tem);
		Hexshow(DemarcateAD19, Adj_adc);


		Adj_tem = EEPROMReadWord(DebugADDRESS+11);
	    
        Hexshow(DemarcateAD20, Adj_tem);
		
	    while(!BTN_S3); 
   	     CreateDebug();
		 screenState = DISPLAY_Debug;
		 return 1;
   		 
   case DISPLAY_Debug:

		while(BTN_S3);         
         screenState = CREATE_Debug2;
         

		 return 1;
   case CREATE_Debug2:

        Adj_tem = EEPROMReadWord(DebugADDRESS+3);
		Adj_adc = EEPROMReadWord(DebugADDRESS+7);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = EEPROMReadWord(DebugADDRESS+9);
		Adj_adc = EEPROMReadWord(DebugADDRESS+5);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(DebugADDRESS+11);
	    Adj_adc = EEPROMReadWord(DebugADDRESS+13);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);
	

        Adj_tem = EEPROMReadWord(0x6020);
	    Adj_adc = EEPROMReadWord(0x6022);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(DebugADDRESS+15);
	  
        Hexshow(DemarcateAD8, Adj_tem);
	
		
	    while(!BTN_S3); 
   	     CreateDebug2();
		 screenState = DISPLAY_Debug2;
		 return 1;
   		 
   case DISPLAY_Debug2:

		while(BTN_S3);         
         screenState =CREATE_DEMARCATEshow;
         

		 return 1;

   case CREATE_DEMARCATEshow:	

        Adj_tem = EEPROMReadWord(DemarcateAddress);
		Adj_adc = EEPROMReadWord(DemarcateAddress+14);
        Hexshow(DemarcateAD, Adj_tem);
		Hexshow(DemarcateAD1, Adj_adc);

		Adj_tem = EEPROMReadWord(DemarcateAddress+2);
		Adj_adc = EEPROMReadWord(DemarcateAddress+16);
        Hexshow(DemarcateAD2, Adj_tem);
		Hexshow(DemarcateAD3, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+4);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+18);
        Hexshow(DemarcateAD4, Adj_tem);
		Hexshow(DemarcateAD5, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+6);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+20);
        Hexshow(DemarcateAD6, Adj_tem);
		Hexshow(DemarcateAD7, Adj_adc);

        Adj_tem = EEPROMReadWord(DemarcateAddress+8);
		Adj_adc = EEPROMReadWord(DemarcateAddress+22);
        Hexshow(DemarcateAD8, Adj_tem);
		Hexshow(DemarcateAD9, Adj_adc);


        Adj_tem = EEPROMReadWord(DemarcateAddress+10);
	    Adj_adc = EEPROMReadWord(DemarcateAddress+24);
        Hexshow(DemarcateAD10, Adj_tem);
		Hexshow(DemarcateAD11, Adj_adc);

	    Adj_tem = EEPROMReadWord(DemarcateAddress+12);
		Adj_adc = EEPROMReadWord(DemarcateAddress+26);
        Hexshow(DemarcateAD12, Adj_tem);
		Hexshow(DemarcateAD13, Adj_adc);

		CreateDemarcateshow();
		screenState = DISPLAY_DEMARCATEshow;
		return 1;
	case DISPLAY_DEMARCATEshow:
        while(BTN_S3);
		screenState =CREATE_MAIN;
		return 1;

	case CREATE_ITEM:
#ifdef USE_ITEM
		CreateItem();
		screenState = DISPLAY_ITEM;
#endif
		return 1;
	case DISPLAY_ITEM:
		return 1;

	case CREATE_ITEM0:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item0Str,ItemMax);
		screenState = DISPLAY_ITEM0;
#endif
		return 1;
	case DISPLAY_ITEM0:
		return 1;

	case CREATE_ITEM1:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item1Str,ItemMax);
		screenState = DISPLAY_ITEM1;
#endif
		return 1;
	case DISPLAY_ITEM1:
		return 1;

	case CREATE_ITEM2:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item2Str,ItemMax);
		screenState = DISPLAY_ITEM2;
#endif
		return 1;
	case DISPLAY_ITEM2:
		return 1;

	case CREATE_ITEM3:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item3Str,ItemMax);
		screenState = DISPLAY_ITEM3;
#endif
		return 1;
	case DISPLAY_ITEM3:
		return 1;
		
	case CREATE_ITEM4:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item4Str,ItemMax);
		screenState = DISPLAY_ITEM4;
#endif
		return 1;
	case DISPLAY_ITEM4:
		return 1;

	case CREATE_ITEM5:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item5Str,ItemMax);
		screenState = DISPLAY_ITEM5;
#endif
		return 1;
	case DISPLAY_ITEM5:
		return 1;

	case CREATE_ITEM6:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item6Str,ItemMax);
		screenState = DISPLAY_ITEM6;
#endif
		return 1;
	case DISPLAY_ITEM6:
		return 1;

	case CREATE_ITEM7:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item7Str,ItemMax);
		screenState = DISPLAY_ITEM7;
#endif
		return 1;
	case DISPLAY_ITEM7:
		return 1;

	case CREATE_ITEM8:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item8Str,ItemMax);
		screenState = DISPLAY_ITEM8;
#endif
		return 1;
	case DISPLAY_ITEM8:
		return 1;

	case CREATE_ITEM9:
#ifdef USE_ITEM
//		CreateItem();
		CreateEditABC(Item9Str,ItemMax);
		screenState = DISPLAY_ITEM9;
#endif
		return 1;
	case DISPLAY_ITEM9:
		return 1;

	case CREATE_AGENT:
		CreateAgent();	
		screenState = DISPLAY_AGENT;
		return 1;
	case DISPLAY_AGENT:		
		return 1;
   	case CREATE_Master:
		CreateMaster();
		screenState = DISPLAY_Master;
		return 1;
	case DISPLAY_Master:
		return 1;

   	case CREATE_Setpassword:
		CreateSetpassword();
		screenState = DISPLAY_Setpassword;
		return 1;
	case DISPLAY_Setpassword:
		return 1;	


   	case CREATE_InputSetpassword1:
		CreateEditTest(EnterPasswordStr, 6);
		screenState = DISPLAY_InputSetpassword1;
		return 1;
	case DISPLAY_InputSetpassword1:
		return 1;		

   	case CREATE_InputSetpassword2:
		CreateEditTest(Confirmpasswordstr, 6);
		screenState = DISPLAY_InputSetpassword2;
		return 1;
	case DISPLAY_InputSetpassword2:
		return 1;	

   	case CREATE_OutcomeSetpassword:
		CreateOutcomeSetpassword();
		screenState = DISPLAY_OutcomeSetpassword;
		return 1;
	case DISPLAY_OutcomeSetpassword:
		return 1;		
		
	case CREATE_RESET:
		CreateYesNo(ResetStr);
		screenState = DISPLAY_RESET;
		return 1;
	case DISPLAY_RESET:
		return 1;

	case CREATE_DELETE:
		CreateYesNo(DeleteStr);
		screenState = DISPLAY_DELETE;
		return 1;
	case DISPLAY_DELETE:
		return 1;

	case CREATE_CYC:
		CreateCalibrate();
		screenState = DISPLAY_CYC;
		return 1;
	case DISPLAY_CYC:
		return 1;
#ifdef Argentina
    case  CREATE_EDITMAINDISPLAY:

		CreateEditTest(EDITDATEstr, 6);
        screenState = DISPLAY_EDITMAINDISPLAY;
		
		return 1;


	case DISPLAY_EDITMAINDISPLAY:
		return 1;
#endif		
	case CREATE_CALLOCK:
			CreateCalLock();
			screenState = DISPLAY_CALLOCK;
			return 1;
	case DISPLAY_CALLOCK:
			return 1;	

	case CREATE_YESNO:
		CreateYesNo(Yes_NoStr);
		screenState = DISPLAY_YESNO;
		return 1;
	case DISPLAY_YESNO:
		return 1;



    }

    return 1;    										// release drawing control to GOL

}

// Shows intro screen and waits for touch
void StartScreen(void){
	
SHORT counter;
#ifdef	USE_MULTIBYTECHAR
XCHAR *text = TouchScreenStr;
XCHAR *Name = WatNameStr;
static const XCHAR Model[] = {'W','A','T','8','9','E','C','-','8',0};//"ALCOVISOR@";
static const XCHAR edition[] = {'E','n','g',' ','V','2','.','0','0',0};//{0x43,0x4e,0x20,0x56,0x31,0x2e,0x30,0x30,0x00};//"CN V1.00"; //edition
static const XCHAR PeriodsText[] = {'P','l','e','a','s','e',' ','D','e','m','a','r','c','a','t','e',0};
BOOL Demarcate=FALSE;
unsigned char EETemp0,EETemp1;
#else
XCHAR *text = TouchScreenStr;
XCHAR *Name = WatNameStr;
//static const XCHAR text[] = "Touch screen to continue";
#ifdef DOT
static const XCHAR Model[] = "Mark X plus";//"ALCOVISOR@";
#else
static const XCHAR Model[] = "Mark X2";//"ALCOVISOR@";
#endif

#ifdef Richard
static const XCHAR Alcovisor[] = "Richard Pauer-3P";
#else
#ifdef Argentina
static const XCHAR Alcovisor[] = "ALCOMETER";//{'A','L','C','O','M','E','T','E','R',0};
#else
static const XCHAR Alcovisor[] = "ALCOVISOR";
#endif
#endif

/*
#ifdef POLVersion
XCHAR edition[] = "POL 1.06"; //edition
#elif defined(South_Africa_Version)
#ifdef Argentina
XCHAR edition[] = "ARG 1.06"; //edition
#else
XCHAR edition[] = "SA 1.07"; //edition
#endif
#else
XCHAR edition[] = "EN 1.03"; //edition
#endif
*/
XCHAR edition[] = "LAT 1.06"; //edition

#ifdef USECABLE
 edition[7] ='c';
 edition[8] = NULL;
#endif
static const XCHAR PeriodsText[] = "Please Demarcate";//{'P','l','e','a','s','e',' ','D','e','m','a','r','c','a','t','e',0};
BOOL Demarcate=FALSE;
unsigned char EETemp0,EETemp1;
#endif
SHORT width, height;
WORD i, j, k;
WORD m;
WORD ytemp, xtemp, srRes = 0x0001; 

    SetColor(BLACK); //WHITE
    ClearDevice();      

#ifndef	USE_SSD1289

    PutImage(0,0,(void*)&mchpLogo,IMAGE_NORMAL);
    PutImage(2,60,(void*)&intro,IMAGE_NORMAL);//IMAGE_X2 ·Å´óÏÔÊ¾
//    PutImage(20,160,(void*)&WellLogo,IMAGE_NORMAL);

    for(counter=0;counter<GetMaxX()-32;counter++){  // move Microchip icon
        PutImage(counter,205,(void*)&mchpIcon0,IMAGE_NORMAL);
    }
#else

	for(counter=0;counter<GetMaxY()-180;counter++)
	{  // move Microchip icon
        	PutImage(5,counter,(void*)&WellLogo,IMAGE_NORMAL);

			if(counter>=10)
		{
			SetColor(BLACK)
			Bar(5, 0, 30, counter-5);
		}
    	}

//	PutImage(10,210,(void*)&jupiter,IMAGE_NORMAL);

#endif
// Èç¹û³¬³ö±ê¶¨ÆÚÏÞ£¬ÔòÏÈÏÔÊ¾Çë±ê¶¨½çÃæ¡£
// µ±Ç°Ê±¼äÈÕÆÚ - ±ê¶¨Ê±Ê±¼äÈÕÆÚÎªÉè¶¨ÔÂ·Ý
// »òµ±Ç°¼ÇÂ¼ÌõÊý¼õÈ¥±ê¶¨Ê±ÌõÊý´óÓÚÉè¶¨ÌõÊýÔòÌáÊ¾±ê¶¨
/*	if(EEPROMReadByte(DemarcateYrDataAddress) -_time.yr)
	{
		EETemp0 = EEPROMReadByte(AdjustMthDataAddress);
		EETemp1 = EEPROMReadByte(DemarcateMthDataAddress)+12 -_time.mth;
		if(EETemp1>EETemp0)
			Demarcate = TRUE;
	}
	else if((EEPROMReadByte(DemarcateMthDataAddress) -_time.mth)>EEPROMReadByte(AdjustMthDataAddress))
		Demarcate = TRUE;
	else if((EEPROMReadByte(DemarcateDayDataAddress) -_time.day)==(EEPROMReadByte(AdjustDayDataAddress)))
		if(_time.day>=(EEPROMReadWord(DemarcateDayDataAddress)))
		Demarcate = TRUE;
	// Èç¹û³¬³ö±ê¶¨ÆÚÏÞ£¬ÔòÏÈÏÔÊ¾Çë±ê¶¨½çÃæ¡£
	if(EEPROMReadWord(AdjustTimesDataAddress)>=(serialnumber - EEPROMReadWord(DemarcateTimesDataAddress)))
		Demarcate = TRUE;*/
	if(Demarcate)
	{
		SaveAdjust();
		MoveTo(40,200);
		SetColor(BRIGHTRED);
		SetFont((void*)&FONTDEFAULT);// FONTMEDIUM
	    while(!OutText((XCHAR*)PeriodsText));
	  //  while(TouchGetX() == -1)Beep();
	      DelayMs(1000);
	      delay(500);
	      Beep();
	}

    SetColor(BRIGHTBLUE);
    SetFont((void*)&FONTDEFAULT);// FONTMEDIUM

	MoveTo(145,290);
    while(!OutText((XCHAR*)edition));

#ifdef Richard
    MoveTo(60,140);
#else
    MoveTo(73,140);
#endif
    while(!OutText((XCHAR*)Alcovisor));


    MoveTo(80,200);
    while(!OutText((XCHAR*)Model));
    MoveTo(160,145);


    SetColor(BRIGHTRED);
    SetFont((void*)ptrLargeAsianFont);    



    SetColor(BRIGHTBLUE);
    MoveTo((GetMaxX()- GetTextWidth(Name,(void*)ptrLargeAsianFont))>>1,20);
    while(!OutText(Name));

   // while(TouchGetX() == -1);
    DelayMs(1000);

    Beep();

#ifndef	USE_SSD1289
	// random fade effect using a Linear Feedback Shift Register (LFSR)
    SetColor(WHITE);
    for (i = 1800; i > 0 ; i--) {
		// for a 16 bit LFSR variable the taps are at bits: 1, 2, 4, and 15
   		srRes = (srRes >> 1) ^ (-(int)(srRes & 1) & 0x8006);  
    	xtemp = (srRes & 0x00FF)%40;
    	ytemp = (srRes >> 8)%30;
    
    	// by replicating the white (1x1) bars into 8 areas fading is faster
		for (j = 0; j < 8; j++) {
    		for (k = 0; k < 8; k++)
	    		PutPixel(xtemp+(j*40), ytemp+(k*30)); 
	    }		
	}
#endif
}

void CreatePage(XCHAR *pText) {
OBJ_HEADER* obj;
SHORT i;
	//pump_pwr=1;// ±Ã¿ªÊ¼³äµç

    WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              (void*)&WellLogo,               	// icon
              pText,	   				// set text 
              navScheme);               // default GOL scheme 
if(screenState!=CREATE_RECORD&&screenState!=CREATE_BLOW&&screenState!=CREATE_RESULT)
	{
	BtnCreate(ID_BUTTON_BACK,         	// button ID 
              0,GetMaxY()-40,					   	// left, top corner	
              40,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&LEFT,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);          	   	// use navigation scheme
              
    BtnCreate(ID_BUTTON_NEXT,   	    // button ID 
              GetMaxX()-40,GetMaxY()-40,
              GetMaxX(),GetMaxY(),0,   	// dimension (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&RIGHT,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);   
}
if(screenState==CREATE_RECORD)
		 	BtnCreate(ID_BUTTON_HOME,
		      60,GetMaxY()-41,					   	// left, top corner	
                   100,GetMaxY()-2,0, 		   	// right, bottom corner (with radius = 0)
                   BTN_DRAW,               	// will be dislayed after creation
                   (void*)&HOME,					   	// no bitmap	
                  NULL,//, HomeStr			// LEFT arrow as text
                  blackScheme);	
else if(screenState==CREATE_RESULT)
{
		BtnCreate(ID_BUTTON_HOME,         	// button ID 
              0,GetMaxY()-40,					   	// left, top corner	
              40,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&HOME,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);          	   	// use navigation scheme

    obj=(OBJ_HEADER*)BtnCreate(ID_BUTTON_NEXT,   	    // button ID 
              GetMaxX()-40,GetMaxY()-40,
              GetMaxX(),GetMaxY(),0,   	// dimension (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&ag3,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);  

}
  else    
	BtnCreate(ID_BUTTON_HOME,
		      100,GetMaxY()-40,					   	// left, top corner	
              140,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&HOME,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);
  
	RTCCProcessEvents();				// update the date and time strings
	i = 0;						
	while (i < 12) {
		dateTimeStr[i] 	  = (XCHAR)_time_str[i];
		dateTimeStr[i+13] = (XCHAR)_date_str[i];
		i++;
	}
	dateTimeStr[12] = (XCHAR)'\n';
	dateTimeStr[25] = (XCHAR)'\0';   
if(screenState==CREATE_RESULT)
	GOLSetFocus(obj);
              
}
void CreatePage0(XCHAR *pText) {
OBJ_HEADER* obj;
SHORT i;

    WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              (void*)&WellLogo,               	// icon
              pText,	   				// set text 
              navScheme);               // default GOL scheme 
        	   	// use navigation scheme
        
	RTCCProcessEvents();				// update the date and time strings
	i = 0;						
	while (i < 12) {
		dateTimeStr[i] 	  = (XCHAR)_time_str[i];
		dateTimeStr[i+13] = (XCHAR)_date_str[i];
		i++;
	}
	dateTimeStr[12] = 0;//(XCHAR)'\n';
	dateTimeStr[25] = (XCHAR)'\0';

           
              
}

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
//			START OF DATE AND TIME SETTING
/*********************************************************************************/

#define EB1STARTX		2	//ÔÂ
#define EB2STARTX		75	//ÈÕ
#define EB3STARTX		130	//Äê
#define EBSTARTY		55
#define EBSTARTY1		110
#define EB4STARTX		10	//Ê±
#define EB5STARTX		85	//·Ö
#define EB6STARTX		160	//Ãë
#define EBSTARTY2		170

#define SELECTBTNWIDTH	22


// For the units add by Spring.Chen
const XCHAR TenUnits[] = { 'J','a','n',0x000A,
							'F','e','b',0x000A,
							'M','a','r',0x000A,
							'J','u','n',0x000A,
							'J','u','l',0x000A,
							'A','u','g',0x000A,
							'S','e','p',0x000A,
							'O','c','t',0x000A,
							'N','o','v',0x000A,
							'D','e','c',0x0000
//						'g','/','L',0x0000


						};






// Items list
// Months Items list
const XCHAR MonthItems[] = {'J','a','n',0x000A,
							'F','e','b',0x000A,
							'M','a','r',0x000A,
							'A','p','r',0x000A,
							'M','a','y',0x000A,
							'J','u','n',0x000A,
							'J','u','l',0x000A,
							'A','u','g',0x000A,
							'S','e','p',0x000A,
							'O','c','t',0x000A,
							'N','o','v',0x000A,
							'D','e','c',0x0000}; 
                                 
// Days Items list
const XCHAR DayItems[] = {'0','1',0x000A, '0','2',0x000A, '0','3',0x000A, '0','4',0x000A, '0','5',0x000A, 
						  '0','6',0x000A, '0','7',0x000A, '0','8',0x000A, '0','9',0x000A, '1','0',0x000A,
						  '1','1',0x000A, '1','2',0x000A, '1','3',0x000A, '1','4',0x000A, '1','5',0x000A,
						  '1','6',0x000A, '1','7',0x000A, '1','8',0x000A, '1','9',0x000A, '2','0',0x000A,
						  '2','1',0x000A, '2','2',0x000A, '2','3',0x000A, '2','4',0x000A, '2','5',0x000A,
						  '2','6',0x000A, '2','7',0x000A, '2','8',0x000A, '2','9',0x000A, '3','0',0x000A, '3','1',0x0000};

// Year Items list
const XCHAR YearItems[] = {'0','0',0x000A, '0','1',0x000A, '0','2',0x000A, '0','3',0x000A, '0','4',0x000A, 
						   '0','5',0x000A, '0','6',0x000A, '0','7',0x000A, '0','8',0x000A, '0','9',0x000A, 
						   '1','0',0x000A, '1','1',0x000A, '1','2',0x000A, '1','3',0x000A, '1','4',0x000A, 
						   '1','5',0x000A, '1','6',0x000A, '1','7',0x000A, '1','8',0x000A, '1','9',0x000A, '2','0',0x0000};

XCHAR *DTSetText    = SetDateTimeStr; 
XCHAR DTPlusSym[] 	= {'+',0};
XCHAR DTMinusSym[] 	= {'-',0};
XCHAR *DTDoneText 	= ExitStr; 
XCHAR *DTDownArrow 	= DownArrowStr; 
XCHAR *DTUpArrow 	= UpArrowStr; 


// Creates date and time screen
void CreateDateTime()
	{
	
		WORD textWidth, textHeight, tempStrWidth;
		XCHAR tempStr[] = {'M',0};
	
		// free memory for the objects in the previous linked list and start new list
		GOLFree();	 
		RTCCProcessEvents();				// update the global time and date strings
		
		/* ************************************* */
		/* Create Month, Day and Year Edit Boxes */ 
		/* ************************************* */
		tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);
		textHeight	 = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1);
	

		WndCreate(ID_WINDOW1,				// ID
				  0,0,GetMaxX(),GetMaxY(),	// dimension
				  WND_DRAW, 				// will be dislayed after creation
				  (void*)&WellLogo, 				// icon
				  DTSetText,				// set text 
				  navScheme);						// use default scheme 
	
	if(Datetype == Mdy)
	{
		/* *************************** */
		// create month components
		/* *************************** */
		// months has three characters, thus we multiply by three
		textWidth = (tempStrWidth*3) +(GOL_EMBOSS_SIZE<<1);
	
		EbCreate(ID_EB_MONTH,				// ID
				  EB1STARTX,
				  EBSTARTY,
				  EB1STARTX+textWidth-10, 
				  EBSTARTY+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  3,						// max characters is 3
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTON_MO, 			// button ID 
				  EB1STARTX+textWidth+1-10,
				  EBSTARTY,
				  EB1STARTX+textWidth+SELECTBTNWIDTH+1-10,
				  EBSTARTY+textHeight,				
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  (void*)&arrowDown,						// no bitmap	
				  NULL, 				// DOWN arrow as text
				  altScheme);				// use alternate scheme
				  
		/* *************************** */
		// create day components
		/* *************************** */
		// day has two characters, thus we multiply by two
		textWidth = (tempStrWidth*2) +(GOL_EMBOSS_SIZE<<1);
	
		EbCreate(ID_EB_DAY, 				// ID
				  EB2STARTX,
				  EBSTARTY,
				  EB2STARTX+textWidth,
				  EBSTARTY+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  2,						// max characters is 3
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTON_DY, 			// button ID 
				  EB2STARTX+textWidth+1,
				  EBSTARTY,
				  EB2STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY+textHeight,				
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  (void*)&arrowDown,						// no bitmap	
				  NULL, 			 // DOWN arrow as text
				  altScheme);				// use alternate scheme
	}
	else 
	{
	
		/* *************************** */
		// create day components
		/* *************************** */
		// day has two characters, thus we multiply by two
		textWidth = (tempStrWidth*2) +(GOL_EMBOSS_SIZE<<1);
	
		EbCreate(ID_EB_DAY, 				// ID
				  EB1STARTX,
				  EBSTARTY,
				  EB1STARTX+textWidth,
				  EBSTARTY+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  2,						// max characters is 3
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTON_DY, 			// button ID 
				  EB1STARTX+textWidth+1,
				  EBSTARTY,
				  EB1STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY+textHeight,				
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  (void*)&arrowDown,						// no bitmap	
				  NULL, 			 // DOWN arrow as text
				  altScheme);				// use alternate scheme
				  
		/* *************************** */
		// create month components
		/* *************************** */
		// months has three characters, thus we multiply by three
		textWidth = (tempStrWidth*3) +(GOL_EMBOSS_SIZE<<1);
	
		EbCreate(ID_EB_MONTH,				// ID
				  EB2STARTX-7,
				  EBSTARTY,
				  EB2STARTX+textWidth-17, 
				  EBSTARTY+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  3,						// max characters is 3
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTON_MO, 			// button ID 
				  EB2STARTX+textWidth+1-17,
				  EBSTARTY,
				  EB2STARTX+textWidth+SELECTBTNWIDTH+1-17,
				  EBSTARTY+textHeight,				
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  (void*)&arrowDown,						// no bitmap	
				  NULL, 				// DOWN arrow as text
				  altScheme);				// use alternate scheme
				  
	
	
	
	}

		/* *************************** */
		// create year components
		/* *************************** */
		// year has four characters, thus we multiply by four
		textWidth = (tempStrWidth*4) +(GOL_EMBOSS_SIZE<<1);

		EbCreate(ID_EB_YEAR,				// ID
				  EB3STARTX+10,
				  EBSTARTY,
				  EB3STARTX+textWidth,
				  EBSTARTY+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  4,						// max characters is 4
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTON_YR, 			// button ID 
				  EB3STARTX+textWidth+1,
				  EBSTARTY,
				  EB3STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY+textHeight,				
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  (void*)&arrowDown,						// no bitmap	
				  NULL,//DTDownArrow,				// DOWN arrow as text
				  altScheme);				// use alternate scheme
				  

		/* ****************************************** */
		/* Create Hour, Minute and Seconds Edit Boxes */ 
		/* ****************************************** */
	
		RTCCProcessEvents();				// update the time and date
	
		textHeight = (GetTextHeight(altScheme->pFont)<<1);
		tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);
	
		/* *************************** */
		// create hour components
		/* *************************** */
		// items here have 2 characters each so we use 2 as multiplier
		textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);
	
		EbCreate(ID_EB_HOUR,				// ID
				  EB4STARTX,
				  EBSTARTY2,
				  EB4STARTX+textWidth, 
				  EBSTARTY2+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  2,						// max characters is 2
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTONHR_P,			// button ID 
				  EB4STARTX+textWidth+1,
				  EBSTARTY2,
				  EB4STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY2+(textHeight>>1),			
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  NULL, 					// no bitmap	
				  DTPlusSym,				// plus symbol as text
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTONHR_M,			// button ID 
				  EB4STARTX+textWidth+1,
				  EBSTARTY2+(textHeight>>1)+1,
				  EB4STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY2+textHeight, 			
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  NULL, 					// no bitmap	
				  DTMinusSym,				// minus symbols as text
				  altScheme);				// use alternate scheme
	
		/* *************************** */
		// create minute components
		/* *************************** */
	
		EbCreate(ID_EB_MINUTE,				// ID
				  EB5STARTX,
				  EBSTARTY2,
				  EB5STARTX+textWidth, 
				  EBSTARTY2+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  2,						// max characters is 2
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTONMN_P,			// button ID 
				  EB5STARTX+textWidth+1,
				  EBSTARTY2,
				  EB5STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY2+(textHeight>>1),			
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  NULL, 					// no bitmap	
				  DTPlusSym,				// plus symbol as text
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTONMN_M,			// button ID 
				  EB5STARTX+textWidth+1,
				  EBSTARTY2+(textHeight>>1)+1,
				  EB5STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY2+textHeight, 			
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  NULL, 					// no bitmap	
				  DTMinusSym,				// minus symbols as text
				  altScheme);				// use alternate scheme
	
		/* *************************** */
		// create seconds components
		/* *************************** */
	
		EbCreate(ID_EB_SECOND,				// ID
				  EB6STARTX,
				  EBSTARTY2,
				  EB6STARTX+textWidth, 
				  EBSTARTY2+textHeight,
				  EB_DRAW,					// will be dislayed after creation
				  NULL,
				  2,						// max characters is 2
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTONSC_P,			// button ID 
				  EB6STARTX+textWidth+1,
				  EBSTARTY2,
				  EB6STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY2+(textHeight>>1),			
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  NULL, 					// no bitmap	
				  DTPlusSym,				// plus symbol as text
				  altScheme);				// use alternate scheme
	
		BtnCreate(ID_BUTTONSC_M,			// button ID 
				  EB6STARTX+textWidth+1,
				  EBSTARTY2+(textHeight>>1)+1,
				  EB6STARTX+textWidth+SELECTBTNWIDTH+1,
				  EBSTARTY2+textHeight, 			
				  0,						// draw rectangular button	
				  BTN_DRAW, 				// will be dislayed after creation
				  NULL, 					// no bitmap	
				  DTMinusSym,				// minus symbols as text
				  altScheme);				// use alternate scheme
	
		 //-----------------------------------------------------
		 // Done Button
		
		textHeight = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1) + 5;
		
		BtnCreate(	  ID_BUTTON4,			// button ID 
					  EB6STARTX-20,
					  EBSTARTY2+2*textHeight,
					  EB6STARTX+textWidth+SELECTBTNWIDTH,
					  EBSTARTY2+3*textHeight,			
					  8,					// draw rectangular button	
					  BTN_DRAW, 			// will be dislayed after creation
					  NULL, 				// no bitmap	
					  DTDoneText,			// set text
					  altScheme);			// use alternate scheme
#ifndef South_Africa_Version
        CbCreate(ID_CHECKBOX4,				// ID 
		  positionax+3,280,positionmaxx-2,310,			// dimension
		  CB_DRAW,					// will be dislayed and checked after creation
		  TimeType,			// "Text Bottom"
		  alt5Scheme);					// alternative GOL scheme					  
#endif					  
		updateDateTimeEb(); 				  
	}


void updateDateTimeEb() {
	OBJ_HEADER* obj;
	SHORT 		i;

	RTCCProcessEvents();				// update the time and date
	
	i = 0;	
	while (i < 12) {
		dateTimeStr[i] = _time_str[i];
		dateTimeStr[i+13] = _date_str[i];
		i++;
	}
	dateTimeStr[12] = 0x000A; 	// (XCHAR)'\n';
	dateTimeStr[25] = 0x0000;	// (XCHAR)'\0';


	// update date edit boxes

//#ifdef USE_MDY
	if(Datetype == Mdy)
		{
	obj = GOLFindObject(ID_EB_MONTH);
	EbSetText((EDITBOX*)obj, &dateTimeStr[13]);
	SetState(obj, EB_DRAW);

	obj = GOLFindObject(ID_EB_DAY);
	EbSetText((EDITBOX*)obj, &dateTimeStr[17]);
	SetState(obj, EB_DRAW);
		}
else
{
	obj = GOLFindObject(ID_EB_MONTH);
	EbSetText((EDITBOX*)obj, &dateTimeStr[16]);
	SetState(obj, EB_DRAW);

	obj = GOLFindObject(ID_EB_DAY);
	EbSetText((EDITBOX*)obj, &dateTimeStr[13]);
	SetState(obj, EB_DRAW);
}
	obj = GOLFindObject(ID_EB_YEAR);
	EbSetText((EDITBOX*)obj, &dateTimeStr[21]);
	SetState(obj, EB_DRAW);

	// update time edit boxes
	obj = GOLFindObject(ID_EB_HOUR);
	EbSetText((EDITBOX*)obj, &dateTimeStr[4]);
	SetState(obj, EB_DRAW);
	
	obj = GOLFindObject(ID_EB_MINUTE);
	EbSetText((EDITBOX*)obj, &dateTimeStr[7]);
	SetState(obj, EB_DRAW);

	obj = GOLFindObject(ID_EB_SECOND);
	EbSetText((EDITBOX*)obj, &dateTimeStr[10]);
	SetState(obj, EB_DRAW);
}

/* this will show the pull down menus */
void ShowPullDownMenu() {
	
	WORD 		textWidth=0, textHeight, tempStrWidth;
	LISTBOX*    pLb;
	XCHAR*		pItems = NULL;
	XCHAR 		tempString[] = {'M',0};

	pListSaved = GOLGetList();
	GOLNewList();

	textHeight = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE << 1);
	
	// Note: pDwnBottom, pDwnTop, pDwnLeft and pDwnRight are globals that will
	// 		 be used to redraw the area where the pulldown menu covered.
	pDwnBottom = 230;
	pDwnTop    = EBSTARTY;

	tempStrWidth = GetTextWidth(tempString,altScheme->pFont);

if(Datetype == Mdy)
			{	
	switch (DateItemID) {

//#ifdef USE_MDY

		case ID_EB_MONTH:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH-10;
			pItems = (XCHAR*)MonthItems;
			break;
		case ID_EB_DAY:
			textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB2STARTX;
			pDwnRight  = EB2STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)DayItems;
			break;

		case ID_EB_YEAR:
			textWidth  = (tempStrWidth*4) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB3STARTX;
			pDwnRight  = EB3STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)YearItems;
			break;

		case	ID_EB_UNIT:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)TenUnits;
	}
}
else
{
		switch (DateItemID) {


		case ID_EB_MONTH:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB2STARTX-10;
			pDwnRight  = EB2STARTX+textWidth+SELECTBTNWIDTH-10;
			pItems = (XCHAR*)MonthItems;
			break;
		case ID_EB_DAY:
			textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)DayItems;
			break;

		case ID_EB_YEAR:
			textWidth  = (tempStrWidth*4) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB3STARTX;
			pDwnRight  = EB3STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)YearItems;
			break;

		case	ID_EB_UNIT:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)TenUnits;
	}

}
	pLb = (LISTBOX *)LbCreate(
			   ID_LISTBOX1,         	// List Box ID
		       pDwnLeft,				// left
		       pDwnTop+textHeight,		// top
		       pDwnRight-SELECTBTNWIDTH,// right
		       pDwnBottom, 				// bottom dimension
		       LB_DRAW|LB_SINGLE_SEL|LB_DRAW_FOCUS, 	// draw after creation, single selection
		       pItems,        			// items list 
		       altScheme);
		
	SldCreate(ID_SLIDER1,              	// Slider ID
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+(textHeight<<1),  		   	
		      pDwnRight,
		      pDwnBottom-textHeight,
		      SLD_DRAW|SLD_SCROLLBAR|
		      SLD_VERTICAL,   			// vertical, draw after creation
		      LbGetCount(pLb),       	// range
		      5,                       	// page 
		      LbGetCount(pLb)-1,       	// pos
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_UP,       	// up button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+textHeight,
		      pDwnRight,
		      pDwnTop+(textHeight<<1),0, 		   	
		      BTN_DRAW,                 // draw after creation
		      (void*)&arrowUp,//NULL,                    	// no bitmap
		      NULL,//DTUpArrow,              	// text
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_DN,        // down button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnBottom-textHeight,
		      pDwnRight,
		      pDwnBottom,0,  		   	
		      BTN_DRAW,                 // draw after creation
		      (void*)&arrowDown,//NULL,                    	// no bitmap
		      NULL,//DTDownArrow,              // text
		      altScheme);
		      

}  

WORD RemovePullDownMenu() {

   OBJ_HEADER* pObjTemp;

	// check if objects has been hidden
	pObjTemp = GOLGetList();
	while(pObjTemp) {							// parse the active list
		if (GetState(pObjTemp, HIDE))			// an object is still not hidden 
			return 0;							// go back without changing state
		pObjTemp = pObjTemp->pNxtObj;			// go to next object
	}

	// all objects are hidden we can now remove the list	
	GOLFree();   								// remove the list of pull down menu
    GOLSetList(pListSaved);						// set active list back to saved list

    // redraw objects that were overlapped by pulldown menu
    GOLRedrawRec(pDwnLeft, pDwnTop, pDwnRight, pDwnBottom);		
    
	// must reset the pressed button, this code is more compact than searching  
	// which one of the three is pressed. 
	pObjTemp = GOLFindObject(ID_BUTTON_MO);
	ClrState(pObjTemp, BTN_PRESSED); 
	pObjTemp = GOLFindObject(ID_BUTTON_YR);
	ClrState(pObjTemp, BTN_PRESSED); 
    pObjTemp = GOLFindObject(ID_BUTTON_DY);
	ClrState(pObjTemp, BTN_PRESSED); 
	
	updateDateTimeEb();							// Update the edit boxes to reflect the new dates
	return 1;									// go back normally 	
	
}	

/* this selects the new month or day or year from the list box*/
WORD MsgSetDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg) {
	
LISTBOX*   	pLb;
SLIDER*  	pSld;
OBJ_HEADER* pObjHide;
LISTITEM*	pItemSel;

    pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX1);		
    pSld = (SLIDER*)GOLFindObject(ID_SLIDER1);

    switch(GetObjID(pObj)){

        case ID_LISTBOX1:
        	if (pMsg->uiEvent == EVENT_MOVE) {
	        	pMsg->uiEvent = EVENT_PRESS;			// change event for listbox
	            // Process message by default
	            LbMsgDefault(objMsg, (LISTBOX*)pObj, pMsg);
	            // Set new list box position
	            SldSetPos(pSld,LbGetCount(pLb)-LbGetFocusedItem(pLb)-1);
	            SetState(pSld, SLD_DRAW_THUMB);
	        	pMsg->uiEvent = EVENT_MOVE;				// restore event for listbox
	        }
	       	else if ((pMsg->uiEvent == EVENT_PRESS)||(pMsg->uiEvent == EVENT_KEYSCAN)){//(pMsg->uiEvent == EVENT_PRESS) {
		       	// call the message default processing of the list box to select the item
		       	LbMsgDefault(objMsg, (LISTBOX *)pObj, pMsg);
				pObjHide = GOLGetList();  				// hide all the objects in the current list
		        while (pObjHide) {						// set all objects with HIDE state
	            	SetState(pObjHide, HIDE);
	            	pObjHide = pObjHide->pNxtObj;
	            }
				pItemSel = LbGetSel(pLb,NULL);			// get the selected item
				if (pItemSel != NULL) 					// process only when an item was selected
					UpdateRTCCDates(pLb);				// update the RTCC values 
    
			    screenState = HIDE_DATE_PDMENU;			// go to hide state
		    }
	       	else if ((pMsg->uiEvent == EVENT_RELEASE)||(pMsg->uiEvent == EVENT_KEYSCAN)) {
		        
		        pObjHide = GOLGetList();  				// hide all the objects in the current list
		        while (pObjHide) {						// set all objects with HIDE state
	            	SetState(pObjHide, HIDE);
	            	pObjHide = pObjHide->pNxtObj;
	            }
				pItemSel = LbGetSel(pLb,NULL);			// get the selected item
				if (pItemSel != NULL) 					// process only when an item was selected
					UpdateRTCCDates(pLb);				// update the RTCC values 
    
			    screenState = HIDE_DATE_PDMENU;			// go to hide state
	        }

	        // The message was processed
	        return 0;

        case ID_SLIDER1:
            // Process message by default
            SldMsgDefault(objMsg, (SLIDER*)pObj, pMsg);
            // Set new list box position
            if(LbGetFocusedItem(pLb) != LbGetCount(pLb)-SldGetPos(pSld)){
                LbSetFocusedItem(pLb,LbGetCount(pLb)-SldGetPos(pSld));
                SetState(pLb, LB_DRAW_ITEMS);
            }
            // The message was processed
            return 0;

        case ID_BUTTON_DATE_UP:					// slider button up was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)+1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;

        case ID_BUTTON_DATE_DN:					// slider button down was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)-1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;
    }    
    return 1;    	
}	
			

// Processes messages for date and time screen
WORD MsgDateTime(WORD objMsg, OBJ_HEADER* pObj) {
	
	EDITBOX *pEbObj;
	char   hour = 0, minute = 0, second = 0;
	WORD   btnID; 
	WORD   ebID = 0, i;
	//static XCHAR  tempString[3];

    switch(btnID = GetObjID(pObj)) {
	    
        case ID_BUTTON4:							// exit setting of date and time
            if(objMsg == BTN_MSG_RELEASED){
				/*
				RTCCSetBinMonth(1);
				RTCCSetBinYear(11);
				RTCCSetBinDay(25);
				RTCCCalculateWeekDay();
				mRTCCSet();
				*/
                screenState = CREATE_SETTING; 			// goto last state screen
                return 1;
            }
			break;

		//////////////////////////////////////////////////////////////////////////
		// 		Date update: this will create the pulldown menu
		//////////////////////////////////////////////////////////////////////////

        case ID_BUTTON_MO:							// month is selected show the pull down menu
            if(objMsg == BTN_MSG_PRESSED){
	            DateItemID = ID_EB_MONTH;
                return 1;   
            }
			break;

        case ID_BUTTON_DY:							// month is selected show the pull down menu
            if(objMsg == BTN_MSG_PRESSED){
	            DateItemID = ID_EB_DAY;
                return 1;
            }
			break;

        case ID_BUTTON_YR:							// month is selected show the pull down menu
            if(objMsg == BTN_MSG_PRESSED){
	            DateItemID = ID_EB_YEAR;
                return 1;
            }
			break;

		//////////////////////////////////////////////////////////////////////////
		// 		Time update through the '+' and '-' buttons
		//////////////////////////////////////////////////////////////////////////
			
		case ID_BUTTONHR_P:  						// increment hour value
		    if(objMsg == BTN_MSG_PRESSED){
				hour = +1;
				ebID = ID_EB_HOUR;
				i = 4;								
			}
			break;
        case ID_BUTTONHR_M:							// decrement hour value
			if(objMsg == BTN_MSG_PRESSED){
				hour = -1;
				ebID = ID_EB_HOUR;
				i = 4;
			}
			break;
        case ID_BUTTONMN_P:  						// increment minute value
        	if(objMsg == BTN_MSG_PRESSED){
				minute = +1;
				ebID = ID_EB_MINUTE;
				i = 7;
			}
			break;
        case ID_BUTTONMN_M:							// decrement minute value
        	if(objMsg == BTN_MSG_PRESSED){
				minute = -1;
				ebID = ID_EB_MINUTE;
				i = 7;
			}
			break;
        case ID_BUTTONSC_P:  						// increment seconds value
        	if(objMsg == BTN_MSG_PRESSED){
				second = +1;
				ebID = ID_EB_SECOND;
				i = 10;
			}
			break;				
        case ID_BUTTONSC_M:							// decrement seconds value
        	if(objMsg == BTN_MSG_PRESSED){
				second = -1;
				ebID = ID_EB_SECOND;
				i = 10;
			} 
            break; 
            
        default:
        	ebID = 0;
            break; 
    }

	if (ebID) {										// check if time was modified
		mRTCCOff();
    	RTCCSetBinHour(mRTCCGetBinHour() + hour);	// update RTCC registers
    	RTCCSetBinMin(mRTCCGetBinMin() + minute);
    	RTCCSetBinSec(mRTCCGetBinSec() + second);
		mRTCCSet();									// copy the new values to the RTCC registers

		// update the edit box that was changed. This is done to immediately update
		// the values on the edit box. The normal update by updateDateTimeEb() is done
		// for every second. This may be too long to wait when the increment or decrement 
		// was successfully performed.
		RTCCProcessEvents();						// update string values
		updateDateTimeEb();
    }	
		
    return 1;
}

//void UpdateRTCCDates(XCHAR *pText, LISTBOX* pLb) {
void UpdateRTCCDates(LISTBOX* pLb) {
	static char value;
	char ch;
	XCHAR *pItemText;

	// get the selected item
	pItemText = LbGetSel(pLb,pLb->pItemList)->pText; 	
	switch (DateItemID) {

		default :
		case ID_EB_MONTH:
			value = LbGetFocusedItem(pLb)+1;	// get the selected month item +1 is needed since 
												// RTCC stores months from index 1 as january
			RTCCSetBinMonth(value);				// set the month value in RTCC module
   	        RTCCCalculateWeekDay();				// calculate the weekday from this new value
			break;

        case ID_EB_YEAR:
		case ID_EB_DAY:
			ch = *pItemText++;					// get the first character
			value = (ch-48) * 10;				// get the value of the first character
			ch = *pItemText;					// get the next character
			value = value + (ch-48);			// add the value of the second character to the first one
			if (DateItemID == ID_EB_DAY)				
				RTCCSetBinDay(value);			// set the day value in the RTCC module
			else {
				RTCCSetBinYear(value);			// set the day value in the RTCC module
			}
            RTCCCalculateWeekDay();				// calculate the weekday from this new value
            break;
	}
	mRTCCSet();									// copy the new values to the RTCC registers

}	



//create main
void CreateMain(){
//CheangeModel();
OBJ_HEADER* obj;
unsigned char	i,j;
#define MAINSTARTX 	30
#define MAINSTARTY 	50
#define MAINSIZEX  	15
#define MAINSIZEY  	32
#define MAINCHARSIZE 52
	initialpower=0;
//pump_pwr=0;
timemainScheme->pFont = (void*)&GOLFontDefault;
    GOLFree();   // free memory for the objects in the previous linked list and start new list
SetColor(RGB565CONVERT(30, 85,105));
    ClearDevice();
for(i=30;i>0;i--)
{	SetColor(RGB565CONVERT(i,4*i,4*i));
	Bar(0, i, GetMaxX(), i+1);
}

    SetColor(RGB565CONVERT(0,0,0));
	Bar(0,0, GetMaxX(),1);

 #ifdef South_Africa_Version
    obj = (OBJ_HEADER*)BtnCreate(ID_BUTTON1, 				// button ID 
              25,60,
              93,128,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              (void*)&L1,                    	// no bitmap
              NULL,//ButtonStr, 				// "Button",     	// text
              mainScheme);              	// use alternate scheme
    BtnCreate(ID_BUTTON2, 				// button ID 
              150,60,
              218,128,         	// dimension
              0,
              BTN_DRAW, 				// will be dislayed after creation 
              (void*)&L2,          // use bitmap
              NULL,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	            // alternative GOL scheme 
#else
			  

    BtnCreate(ID_BUTTON2, 				// button ID 
              90,60,
              160,128,         	// dimension
              0,
              BTN_DRAW, 				// will be dislayed after creation 
              (void*)&L2,          // use bitmap
              NULL,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	            // alternative GOL scheme 

#endif
    BtnCreate(ID_BUTTON3,             	// button ID 
              25,160,
              93,228,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              (void*)&L3,                    	// no bitmap
              NULL,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme


    BtnCreate(ID_BUTTON4,             	// button ID 
              150,160,
              218,228,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              (void*)&L4,                    	// no bitmap
              NULL,//SettingStr,//OnStr, 					// "ON",		// text
              mainScheme);             // use alternate scheme 
              /*
	StCreate(ID_STATICTEXT1,           	// ID For the Time display
             10 ,
             3,							// dimensions
             95,
             30,//+(GetTextHeight((void*)&BigArial)<<1),	
             ST_DRAW|ST_FRAME|
             ST_CENTER_ALIGN,     		// will be dislayed, has frame and center aligned
             dateTimeStr, 				// text is from time value
             timemainScheme); 

             
	StCreate(ID_STATICTEXT2,           	// ID For the Time display
             50,
             270,							// dimensions
             200,
             310,//+(GetTextHeight((void*)&BigArial)<<1),	
             ST_DRAW|ST_CENTER_ALIGN,     		// will be dislayed, has frame and center aligned
             date_str, 				// text is from time value
             timemainScheme); 
   */          
             
for(i=50;i>0;i--)
{	SetColor(RGB565CONVERT(i,2*i,5*i/2));//SetColor(RGB565CONVERT(5,70,60));
	Bar(0, (GetMaxY()-i), GetMaxX(), (GetMaxY()-i+1));
}
	SetFont((void*)&GOLFontDefault);
    SetColor(RGB565CONVERT(120, 196,66));
    OutTextXY(2,3,WeekStr);
	
    SetFont((void*)&GOLFontDefault);
    SetColor(RGB565CONVERT(120, 196,66));
    OutTextXY(52,3,&dateTimeStr[4]);
	
	SetFont((void*)&GOLFontDefault);
    SetColor(RGB565CONVERT(120, 196,66));
	
	date_str[3]	= 0x20;
	date_str[4]	= (_time.mth >> 4) + '0';
	date_str[5]	= (_time.mth & 0xF) + '0';
    OutTextXY(75,280,date_str);

}

WORD MsgMain(WORD objMsg, OBJ_HEADER* pObj){
	//	OBJ_HEADER* pOtherRbtn;
unsigned int MsgAdd;
	unsigned char ik;
    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default


        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
				
		if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
						screenState = CREATE_CALLOCK;
		else
			{	
			       for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;
			       TestMode=0xaa;
			       g_backlight_time = 3*60;// 3·ÖÖÓ²»´µÆø£¬×Ô¶¯Ìø³öÀ´
			       /*
                          if(EEPROMReadByte(ID_CHECKBOXADDRESS + ID_CHECKBOX3))
                             {
		                 SaveReadItem(FALSE);
            	                 for(MsgAdd =0;MsgAdd<10;MsgAdd++)//ItemMax
            	                     {
            		                if(EEPROMReadByte(ID_CHECKBOXADDRESS +MsgAdd))
            		                 break;
            	                    }
            	                 if(MsgAdd<10)//ItemMax
                                    screenState = 2*MsgAdd+CREATE_ITEM0;//CREATE_VEHICLENO;//CREATE_SUBJECT_NAME; 		// goto ECG demo screen
                               else
              	                     screenState = CREATE_Waittest;
                               }
                            else 
                            */
                                   screenState = CREATE_Waittest;

                   }
            	}
            return 1; 							// process by default
   #ifdef South_Africa_Version
        case ID_BUTTON1:
        	if(objMsg == BTN_MSG_PRESSED){ 		// change text and scheme

		if(Demarcate&&EEPROMReadByte(ID_CHECKBOXADDRESS+ID_CHECKBOX8))
					   screenState = CREATE_CALLOCK;
		 else {

			     TestMode=0xcc;
			     screenState = CREATE_Waittest; 		// goto ECG demo screen
			     status_Press=FALSE;
			     unsigned char ik;
	                   for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;//clear  buffer --add by xiao 090811
			       Flashbuff[49] = 0;
			      Flashbuff[68] = 0;
			     Flashbuff[87] = 0;
			    Flashbuff[106] = 0;
			    Flashbuff[125] = 0;
			   Flashbuff[144] = 0;
			   Flashbuff[163] = 0;
			  Flashbuff[182] = 0;
			  Flashbuff[201] = 0;
			  Flashbuff[220] = 0;
                      Quickly_test=1;//¿ì²âÄ£Ê½   
			g_backlight_time = 3*60;// 3·ÖÖÓ²»´µÆø£¬×Ô¶¯Ìø³öÀ´
		 	}	
            }
            return 1;  							// process by default
#endif
        case ID_BUTTON3:
            if(objMsg == BTN_MSG_RELEASED){
				    serialnumber = EEPROMReadWord(EESERIAL_NUMBER);
                               serialnumberRecord = serialnumber - 1;// ±£³Ö¼ÇÂ¼Ò»Ö
            	if(serialnumberRecord>0)				//±£Ö¤²»Ð´¼ÇÂ¼Ê±ºò²»ÏÔÊ¾
                screenState = CREATE_RECORD; 		// goto ECG demo screen
            }
            return 1; 							// process by default
            
        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			{

	            screenState = CREATE_SETTING;//CREATE_PASSWORD;//; 		// goto SETTING demo screen
	        }
		    return 1; 							// Do not process by default


        default:
            return 1; 							// process by default
    }
}
// create main end


void Check_Screen()//µ÷ÊÔ´¥ÃþÆÁ
{
/*
	TouchCalibration();
        TouchStoreCalibration();
        EEPROMWriteWord(GRAPHICS_LIBRARY_VERSION,EEPROM_VERSION);
    

    // Load touch screen calibration parameters from EEPROM
//    TouchLoadCalibration();
    screenState = CREATE_MAIN;
    */
}

EDITBOX* pEbzyq;


void CreateEditTest(XCHAR *EditTestStr,WORD CharMax)
{

	static XCHAR pTempXchar[][2] = 	{	{'0',0},
										{'1',0},
										{'2',0},
										{'3',0},
										{'4',0},
										{'5',0},
										{'6',0},
										{'7',0},
										{'8',0},
										{'9',0},
										{0x2E,0},
										{'#',0}
									};

    GOLFree();  // free memory for the objects in the previous linked list and start new list
//pump_pwr = 1;
	CreatePage(EditTestStr);//EditBoxStr
	
#define KEYSTARTX 	15
#define KEYSTARTY 	38
#define KEYSIZEX	70
#define KEYSIZEY  	45
#define MAXCHARSIZE CharMax
MAXCHAROld=CharMax;
//if(CharMax != 1)
	EbCreate(ID_EDITBOX1,              	// ID
              4,KEYSTARTY+1,GetMaxX()-4,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              CharMax,//MAXCHARSIZE,
              altScheme);               // default GOL scheme


    BtnCreate(ID_KEYPAD+1,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[1],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+2,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[2],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+3,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[3],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+4,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[4],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+5,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[5],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+6,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[6],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+7,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[7],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+8,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[8],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+9,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[9],    		// text
              altScheme);              	// alternative GOL scheme 




    BtnCreate(ID_BACKSPACE,            	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              UpArrowStr, 			// Left Arrow 
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+0,             	// ID 
              KEYSTARTX+1*KEYSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[0],    		// text
              altScheme);              	// alternative GOL scheme 			  

    BtnCreate(ID_KEYPADPOID,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[10],    		// text
              altScheme); 
              

}

// Processes messages for the edit box demo screen
WORD MsgEditTest(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

	        pEbzyq = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEbzyq,'0'+id-ID_KEYPAD);
	        		SetState(pEbzyq, EB_DRAW);
	        		TestMiMaStr[0] = '0'+id-ID_KEYPAD;
	    }
        return 1;        
    }

    switch(id){

        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = prevState ;//++;//= CREATE_MAIN; 	// goto meter screen
//                EEPROMWriteByte((XCHAR)pEb->pBuffer, 0x7F10);	//add by Spring.Chen
            }
            if(AlcTypeaddress+16>0xF300)
            	ErrorTrap("No Memory");
            else
            	AlcTypeaddress+=16;//ErrorTrap("Memory Enough");
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}
WORD MsgEditPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not
static XCHAR	DIYPD[7];
BYTE i=0,pdtemp;

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){

		if(screenState==DISPLAY_InputSetpassword1)
					
		{
                     for(;i<=6;i++)
                     	{
                        DIYPD[i]=PasswordBuffer[i];
						PasswordBuffer[i] = 0;
			 }
					
                      screenState=CREATE_InputSetpassword2;
		}
				
             else if(screenState==DISPLAY_InputSetpassword2)
               	{
                           if(DIYPD[0]==PasswordBuffer[0]&&DIYPD[1]==PasswordBuffer[1]&&DIYPD[2]==PasswordBuffer[2]&&DIYPD[3]==PasswordBuffer[3]&&DIYPD[4]==PasswordBuffer[4]&&DIYPD[5]==PasswordBuffer[5])        
           	         {
		               PDset=TRUE;
                       PasswordBuffer[0] = 0;
			        if(Setpasswordposition==0xaa)
			   	   {
			               pdtemp=(DIYPD[0]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[1]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, SettingPasswordAddress);

			               pdtemp=(DIYPD[2]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[3]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, SettingPasswordAddress+1);

			               pdtemp=(DIYPD[4]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[5]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, SettingPasswordAddress+2);	

						   EEPROMWriteByte(0xaa, PasswordsettedAddress); 
			   	 }
				   else if(Setpasswordposition==0xbb)
			   	{

#ifdef Argentina
						   pdtemp=(DIYPD[0]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[1]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, DeleteSettingPasswordAddress);

			               pdtemp=(DIYPD[2]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[3]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, DeleteSettingPasswordAddress+1);

			               pdtemp=(DIYPD[4]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[5]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, DeleteSettingPasswordAddress+2);

						   EEPROMWriteByte(0xaa, PasswordsettedAddress);

#endif
			       } 					
			      else if(Setpasswordposition==0xcc)
			   	{

			               pdtemp=(DIYPD[0]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[1]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, AdvanceSettingPasswordAddress);

			               pdtemp=(DIYPD[2]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[3]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, AdvanceSettingPasswordAddress+1);

			               pdtemp=(DIYPD[4]-0x30)&0x0F;
			               pdtemp=(pdtemp<<4)|((DIYPD[5]-0x30)&0x0F);
			               EEPROMWriteByte(pdtemp, AdvanceSettingPasswordAddress+2);

						   EEPROMWriteByte(0xaa, PasswordsettedAddress);


			       }
			   
           	}
           else  PDset=FALSE;
				   screenState=CREATE_OutcomeSetpassword;
         }
  }

             return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
			
                    if(screenState == DISPLAY_InputSetpassword1)
			     screenState = CREATE_Setpassword;
		     else if(screenState == DISPLAY_InputSetpassword2)
			     screenState = CREATE_InputSetpassword1;
            }
            return 1; // process by default
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}
WORD MsgEditMark(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned int  j=1,i=0,point=0;
static char status = 0;			// status to check if calling, holding or not
//TempbuffStr[]={0};
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+10){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
/*
          if(id  == ID_KEYPAD+10)
		  	EbAddChar2(pEb,0x2e);
          else*/
			EbAddChar2(pEb,'0'+id-ID_KEYPAD);
	        SetState(pEb, EB_DRAW);
	      /*	  pEbzyq = (EDITBOX*)GOLFindObject(ID_STATICTEXT4);
	        		EbAddChar(pEbzyq,'0'+id-ID_KEYPAD);
	        		SetState(pEbzyq, EB_DRAW);*/
	        		TestMiMaStr[0] = '0'+id-ID_KEYPAD;
                  //k=pEb->charMax;

				//  consbuffStr[]=TempbuffStr[];

				  
	    }
        return 1;        
    }

    if(id  == ID_KEYPADPOID)
    {
    	if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
        
		  	EbAddChar2(pEb,0x2e);
		  	SetState(pEb, EB_DRAW);
    		}
    	return 1;
    }
}


	
	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				 MarkTemp=0;
				 TempbuffStr[Valnum]=0;
                 for(i=0;i<19;i++){
                     consbuffStr[i]=TempbuffStr[i];
					 TempbuffStr[i] = 0x0000;
   	                   }
				 /*  if(point<=2){
					 for(i=0;i<19;i++){
                     consbuffStr[i]=0x0000;					
   	                   }
                	}*/
				if((consbuffStr[0]!=0)&&(consbuffStr[0]!=0x2E)){	

                   for(i=0;i<=Valnum-1;i++){
					
					if(consbuffStr[i]==0x2E)
						point=i;
                	}

                 if(point==0){

					for(i=0,j=1;i<=(Valnum-1);i++)
						{
					      j*=10;
                	     MarkTemp+=(consbuffStr[Valnum-1-i]-48)*j;

					}
                 	}
				else {
					if(point==(Valnum-1)){
			  	   consbuffStr[Valnum-1]=0x0000;
				   for(i=0,j=1;i<=(Valnum-2);i++)
                	  {
					     j*=10;
                	     MarkTemp+=(consbuffStr[Valnum-2-i]-48)*j;
                	   }
              	 }
                  else  
                	{
				MarkTemp+=(consbuffStr[Valnum-1]-48);              
			    for(i=0,j=1;i<=(Valnum-3);i++)
                	{
					     j*=10;
                	     MarkTemp+=(consbuffStr[Valnum-3-i]-48)*j;
                	}
                	}
            	}
			EEPROMWriteWord(MarkTemp,0x6020);
		}
                
				Marktemporary=Temperature();
				EEPROMWriteWord(Marktemporary,0x6024);
			//	MarkTem= (unsigned char)Marktemporary;                   				

				Marktem[2]=(Marktemporary%10+'0');
                
				Marktem[1]=(Marktemporary/10+'0');

				if(Kt=1)
					Marktem[0]=0x0020;
				else
					Marktem[0]=0x002D;
				
				screenState = CREATE_Wait;//CREATE_Mark;// goto meter screen
//                EEPROMWriteByte((XCHAR)pEb->pBuffer, 0x7F10);	//add by Spring.Chen
            }
            if(AlcTypeaddress+16>0xF300)
            	ErrorTrap("No Memory");
            else
            	AlcTypeaddress+=16;//ErrorTrap("Memory Enough");
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}
WORD MsgPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
            		screenState = CREATE_INPUT;
            		PasswordBuffer[0] = 0;
            		}
            }
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; // process by default
	    case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}
WORD MsgScreenPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            	//if((PasswordBuffer[0] == '1')&&(PasswordBuffer[1] == '2')&&(PasswordBuffer[2] == '3')
            		//&&(PasswordBuffer[3] == '4')&&(PasswordBuffer[4] == '5')&&(PasswordBuffer[5] == '6'))
            		//{
            		//PasswordBuffer[0] = 0;
            		//Check_Screen();
            		
            		//}
            }
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){

					screenState = CREATE_SETTING;

            }
            return 1; // process by default
	    case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}

WORD MsgMasterPassword(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {
/*	        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	        EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        SetState(pEb, EB_DRAW);*/
//	        pEb = (EDITBOX*)GOLFindObject(ID_STATICTEXT4);
		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
//	        		EbAddChar(pEb,0x2A);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
//	        		TestMiMaStr[0] = '0'+id-ID_KEYPAD;
//	        		EEPROMWriteByte(TestMiMaStr[0] , PrintTimesAddress);
	    }
        return 1;        
    }

    switch(id){

        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;

        case ID_BUTTON_NEXT:
	
            if(objMsg == BTN_MSG_RELEASED){
            	if((PasswordBuffer[0] == '2')&&(PasswordBuffer[1] == '5')&&(PasswordBuffer[2] == '8')
            		&&(PasswordBuffer[3] == '7')&&(PasswordBuffer[4] == '5')&&(PasswordBuffer[5] == '3'))
            		{
            		screenState = CREATE_AGENT;
            		PasswordBuffer[0] = 0;
            		}
            }
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
    
		   if(objMsg == BTN_MSG_RELEASED){
		if((PasswordBuffer[0] == PDread[0])&&(PasswordBuffer[1] == PDread[1])&&(PasswordBuffer[2] == PDread[2])\
            		&&(PasswordBuffer[3] == PDread[3])&&(PasswordBuffer[4] == PDread[4])&&(PasswordBuffer[5] == PDread[5]))
            		{
            		screenState = CREATE_Master;
            		PasswordBuffer[0] = 0;
            		}
            }
            else
            	{
			pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                EbSetText(pEb, WrongPasswordStr);	
                SetState(pEb, EB_DRAW);
            	}
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 	// goto list box screen
            }
            return 1; // process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}

#ifdef Argentina
WORD MsgEditInspectDate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not
static XCHAR	DIYPE[7];
BYTE i=0,pdtemp;

    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    if(id >= ID_KEYPAD)
    if(id  < ID_KEYPAD+10){
	    if (!status) {

		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	        		SetState(pEb, EB_DRAW);
	    }
        return 1;        
    }

    switch(id){
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;


        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
	
			for(;i<=6;i++)
			   {
			   DIYPE[i]=TempbuffStr[i];
			   TempbuffStr[i] = 0;
	}


			               pdtemp=DIYPE[0]-0x30;
			               pdtemp = ((pdtemp<<4)&0xf0)|((DIYPE[1]-0x30)&0x0f);//pdtemp=pdtemp*10+DIYPE[1]-0x30;
			               EEPROMWriteByte(pdtemp, MarkDayDataAddress);

			               pdtemp=DIYPE[2]-0x30;
			               pdtemp = ((pdtemp<<4)&0xf0)|((DIYPE[3]-0x30)&0x0f);//pdtemp=pdtemp*10+DIYPE[3]-0x30;
			               EEPROMWriteByte(pdtemp, MarkMthDataAddress);

			               pdtemp=DIYPE[4]-0x30;
			               pdtemp = ((pdtemp<<4)&0xf0)|((DIYPE[5]-0x30)&0x0f);//pdtemp=pdtemp*10+DIYPE[5]-0x30;
			               EEPROMWriteByte(pdtemp, MarkYrDataAddress);	

					 EEPROMWriteByte(0xaa, inspectdateSettedAddress); 
		   
         	screenState = CREATE_Master;	//  
         }


             return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
			
			screenState = CREATE_Master;	//

            }
            return 1; // process by default
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1; // process by default
    }


}

#endif
// Creates the edit box demo screen
static XCHAR pTempYchar1[][2] = 	{	{0x00F7, 0x0000},
										{0x00C2, 0x0000},
										{0x0128, 0x0000},
										{0x00FB, 0x0000},
										{0x0122, 0x0000},
										{0x00F5, 0x0000},
										{0x00E7, 0x0000},
										{0x0121, 0x0000}
									};
static XCHAR pTempYchina2[][2] = 	{	{0x0138, 0x0000},
										{0x00ED, 0x0000},
										{0x00AB, 0x0000},
										{0x0098, 0x0000},
										{0x00EE, 0x0000},
										{0x00DC, 0x0000},
										{0x008B, 0x0000},
										{0x0125, 0x0000}
									};
static XCHAR pTempYchina3[][2] = 	{	{0x0137, 0x0000},
										{0x0111, 0x0000},
										{0x012F, 0x0000},
										{0x00BB, 0x0000},
										{0x0133, 0x0000},
										{0x00FA, 0x0000},
										{0x0110, 0x0000},
										{0x0131, 0x0000}
									};
static XCHAR pTempYchina4[][2] = 	{	{0x00A8, 0x0000},
										{0x00FD, 0x0000},
										{0x00F6, 0x0000},
										{0x00D8, 0x0000},
										{0x00F3, 0x0000},
										{0x0120, 0x0000},
										{0x0104, 0x0000},
										{0x008A, 0x0000}
									};
static XCHAR pTempYchina5[][2] = 	{	{0x0112, 0x0000},
										{0x00F1, 0x0000},
										{0x00C7, 0x0000},		//¹ã
										{0x0099, 0x0000},		//¾ü
										//{0x00DB, 0x0000},		//¹ð
										{0x008B, 0x0000},		//¾©
										{0x00EE, 0x0000},		//½ò
										{0x00ED, 0x0000},		//»¦
										{0x00F3, 0x0000}		//Óå
								};
static XCHAR pTempXchina[][2] = 	{	{0x00F7,0x0000},
										{0x00C2, 0x0000},
										{0x0128, 0x0000},
										{0x00FB, 0x0000},
										{0x0122, 0x0000},
										{0x00F5, 0x0000},
										{0x00E7, 0x0000},
										{0x0121, 0x0000}
									};
#define	ChinaCharMax 8
void CopyArrays(char TChinaPage)
{
	unsigned int i = 0,j=0;
	void *bChianChar;
	
	switch(TChinaPage)
	{
		case 1:
			for(i= 0; i < ChinaCharMax; i++)
		for(j = 0; j < 2;j++)
			pTempXchina[i][j] = pTempYchar1[i][j];
			break;
		case 2:
			for(i= 0; i < ChinaCharMax; i++)
		for(j = 0; j < 2;j++)
			pTempXchina[i][j] = pTempYchina2[i][j];
			break;
		case 3:
			for(i= 0; i < ChinaCharMax; i++)
		for(j = 0; j < 2;j++)
			pTempXchina[i][j] = pTempYchina3[i][j];
			break;
		case 4:
			for(i= 0; i < ChinaCharMax; i++)
		for(j = 0; j < 2;j++)
			pTempXchina[i][j] = pTempYchina4[i][j];
			break;
		case 5:
			for(i= 0; i < ChinaCharMax; i++)
		for(j = 0; j < 2;j++)
			pTempXchina[i][j] = pTempYchina5[i][j];
			break;
		defalut:
			break;

	}

}

void CreateEditABC(XCHAR *EditTestStr,WORD CharMax)
{
	unsigned char temp;
	static XCHAR pTempXchar[][2] = 	{	{'0',0},
										{'1',0},
										{'2',0},
										{'3',0},
										{'4',0},
										{'5',0},
										{'6',0},
										{'7',0},
										{'8',0},
										{'9',0},
										{'*',0},
										{'#',0}
									};
	

	static XCHAR pTempABC[][2] = 	{		{'A',0},
										{'B',0},
										{'C',0},
										{'D',0},
										{'E',0},
										{'F',0},
										{'G',0},
										{'H',0},
										{'I',0},
										{'J',0},
										{'K',0},
										{'L',0},
										{'M',0},
										{'N',0},
										{'O',0},
										{'P',0},
										{'Q',0},
										{'R',0},
										{'S',0},
										{'T',0},
										{'U',0},
										{'V',0},
										{'W',0},
										{'X',0},
										{'Y',0},
										{'Z',0}
									};
	for(temp = 0;temp<19;temp++)
              TempbuffStr[0] = 0;

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(EditTestStr);//EditBoxStr
	
#ifndef USE_PORTRAIT
#define KEYSTARTX 	50
#define KEYABCSTARTX 	4
#else
#define KEYABCSTARTX 	4
#endif

#define KEYSTARTY 	43
#define KEYABCSIZEX 23
#define KEYSIZEY  	39
#define MAXCHARSIZE CharMax
MAXCHAROld=CharMax;
	EDITBOX* pEb;
	unsigned char Operatorcs=0;
	unsigned int Operatorpage,Operatoraddr;
	XCHAR OperatorNameTemp[RecordSizeMax];
	unsigned int LockAdd,ItemStarAdd;

if(CharMax == 32)
	EbCreate(ID_EDITBOX1,              	// ID
              KEYABCSTARTX,KEYSTARTY+1,GetMaxX()-KEYABCSTARTX,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              CharMax,//MAXCHARSIZE,
              alt3Scheme);               // default GOL scheme
else
EbCreate(ID_EDITBOX1,              	// ID
              KEYABCSTARTX,KEYSTARTY+1,GetMaxX()-KEYABCSTARTX,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              CharMax,//MAXCHARSIZE,
              altScheme);               // default GOL scheme

       if(screenState>=CREATE_ITEM0)
       	if(screenState<DISPLAY_ITEM9)
       {
       	Operatorpage = (serialnumberRecord-1)/2;
		Operatoraddr = ((serialnumberRecord-1)%2)*256;
		if(serialnumberRecord>0)
		{
			switch(screenState)// ItemState
	            	{
	            	case CREATE_ITEM0:
	            		LockAdd =ID_CHECKBOX20;
	            		ItemStarAdd = Item0DataAddress;
	            		break;
	            	case CREATE_ITEM1:
	            		LockAdd =ID_CHECKBOX21;
	            		ItemStarAdd = Item1DataAddress;
	            		break;
	            	case CREATE_ITEM2:
	            		LockAdd =ID_CHECKBOX22;
	            		ItemStarAdd = Item2DataAddress;
	            		break;
	            	case CREATE_ITEM3:
	            		LockAdd =ID_CHECKBOX23;
	            		ItemStarAdd = Item3DataAddress;
	            		break;
	            	case CREATE_ITEM4:
	            		LockAdd =ID_CHECKBOX24;
	            		ItemStarAdd = Item4DataAddress;
	            		break;
	            	case CREATE_ITEM5:
	            		LockAdd =ID_CHECKBOX25;
	            		ItemStarAdd = Item5DataAddress;
	            		break;
	            	case CREATE_ITEM6:
	            		LockAdd =ID_CHECKBOX26;
	            		ItemStarAdd = Item6DataAddress;
	            		break;
	            	case CREATE_ITEM7:
	            		LockAdd =ID_CHECKBOX27;
	            		ItemStarAdd = Item7DataAddress;
	            		break;
	            	case CREATE_ITEM8:
	            		LockAdd =ID_CHECKBOX28;
	            		ItemStarAdd = Item8DataAddress;
	            		break;
	            	case CREATE_ITEM9:
	            		LockAdd =ID_CHECKBOX29;
	            		ItemStarAdd = Item9DataAddress;
	            		break;
	            	default:
	            		break;
	            	}
			/*
	            	if(EEPROMReadByte(ID_CHECKBOXADDRESS +LockAdd))
	            	{
				FLASH_RD(Operatorpage,Operatoraddr,RecordSizeMax,&OperatorNameTemp[0],Operatorcs);
				CopyChar(&OperatorNameTempStr[0],&OperatorNameTemp[ItemStarAdd],18);
				CopyChar(TempbuffStr,OperatorNameTempStr,ItemMax);//¿½±´µ±ÌìÐÅÏ¢µ½TempbuffStr
		       	pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
		             EbSetText(pEb, OperatorNameTempStr);
		             SetState(pEb, EB_DRAW);
	            	}
	            	*/
		}
	}
       
    BtnCreate(ID_ABCPAD+16,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,		
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[16],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+22,             	// ID 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[22],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+4,             	// ID 
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[4],    		// text
              altScheme);              	// alternative GOL scheme 

	BtnCreate(ID_ABCPAD+17,             	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,		
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[17],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+19,             	// ID 
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[19],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+24,             	// ID 
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[24],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_ABCPAD+20,             	// ID 
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[20],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+8,             	// ID 
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[8],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+14,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[14],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+15,             	// ID 
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[15],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+0,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[0],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+18,             	// ID 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[18],    		// text
              altScheme);              	// alternative GOL scheme 
              
    BtnCreate(ID_ABCPAD+3,             	// ID 
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[3],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+5,             	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[5],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_ABCPAD+6,             	// ID 
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[6],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+7,             	// ID 
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[7],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+9,             	// ID 
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[9],    		// text
              altScheme);              	// alternative GOL scheme   

	BtnCreate(ID_ABCPAD+10,             	// ID 
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[10],    		// text
              altScheme);              	// alternative GOL scheme 
#if	0              
    BtnCreate(ID_ABCPAD+11,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[11],    		// text
              altScheme);              	// alternative GOL scheme 
#else
	BtnCreate(ID_ABCPAD+11,  
		KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[11],    		// text
              altScheme);
#endif
    BtnCreate(ID_ABCPAD+25,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[25],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+23,             	// ID 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[23],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+2,             	// ID 
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[2],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_ABCPAD+21,             	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[21],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_ABCPAD+1,             	// ID 
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[1],    		// text
              altScheme);              	// alternative GOL scheme 

	BtnCreate(ID_ABCPAD+13,             	// ID 
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[13],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_ABCPAD+12,             	// ID 
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempABC[12],    		// text
              altScheme);              	// alternative GOL scheme 
#if	0
    BtnCreate(ID_ASTERISK,             	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[10],    		// text
              altScheme);              	// alternative GOL scheme   
#endif
    BtnCreate(ID_KEYPAD+1,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[1],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+2,             	// ID 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[2],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+3,             	// ID 
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[3],    		// text
              altScheme);              	// alternative GOL scheme 

	BtnCreate(ID_KEYPAD+4,             	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[4],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+5,             	// ID 
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[5],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+6,             	// ID 
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[6],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_KEYPAD+7,             	// ID 
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[7],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+8,             	// ID 
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[8],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+9,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[9],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+0,             	// ID 
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[0],    		// text
              altScheme);              	// alternative GOL scheme 
if(0)//screenState != CREATE_OPERATORNAME)
{
    BtnCreate(ID_BUTTONChina_M,//ID_KEYCHINAPAD+1,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,		
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              DTDownArrow,//pTempXchina[1],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYCHINAPAD+0,             	// ID 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[0],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYCHINAPAD+1,             	// ID 
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[1],    		// text
              altScheme);              	// alternative GOL scheme 

	BtnCreate(ID_KEYCHINAPAD+2,             	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,		
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[2],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYCHINAPAD+3,             	// ID 
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[3],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYCHINAPAD+4,             	// ID 
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[4],    		// text
              altScheme);              	// alternative GOL scheme 


    BtnCreate(ID_KEYCHINAPAD+5,             	// ID 
              KEYABCSTARTX+6*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[5],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYCHINAPAD+6,             	// ID 
              KEYABCSTARTX+7*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[6],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYCHINAPAD+7,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchina[7],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_BUTTONChina_P,//ID_KEYCHINAPAD+0,             	// ID 
              KEYABCSTARTX+9*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              DTUpArrow,//pTempXchina[0],    		// text
              altScheme);              	// alternative GOL scheme 
}

#if	0
    BtnCreate(ID_POUND,             	// ID 
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+5*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[11],    		// text
              altScheme);              	// alternative GOL scheme   
#endif
	BtnCreate(ID_ABC,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              abcStr,    		// text
              altScheme);              	// alternative GOL scheme   
              

#ifdef USE_PORTRAIT

    BtnCreate(ID_BACKSPACE,            	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              UpArrowStr, 			// Left Arrow 
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_CALL,                	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+10*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                     // (void*)&greenphone,              // use green phone bitmap
              NULL,                     // text
              altScheme);              	// alternative GOL scheme 
#if	0
    BtnCreate(ID_STOPCALL,            	// ID 
              KEYABCSTARTX+2*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                     // (void*)&redphone,                // use redphone bitmap
              NULL,                     // text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_HOLD,            		// ID 
              KEYABCSTARTX+1*KEYABCSIZEX,
              KEYSTARTY+6*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+7*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              HoldStr, 					// "Hld"
              altScheme);              	// alternative GOL scheme 
#endif
#else
	
    BtnCreate(ID_BACKSPACE,            	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              LeftArrowStr, 			// Left Arrow 
              altScheme);              	// alternative GOL scheme 
#if	0
    BtnCreate(ID_CALL,                	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              (void*)&greenphone,              // use green phone bitmap
              NULL,                     // text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_STOPCALL,            	// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              (void*)&redphone,                // use redphone bitmap
              NULL,                     // text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_HOLD,            		// ID 
              KEYABCSTARTX+3*KEYABCSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYABCSTARTX+4*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              HoldStr, 					// "Hld"
              altScheme);              	// alternative GOL scheme 
#endif
#endif
              

}

// Processes messages for the edit box demo screen
WORD MsgEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
int ii = 0;
static char status = 0;			// status to check if calling, holding or not
static BOOL abcStatus = 0;		// status to check if Large ABC or not
static char ChinaPage = 1;
    id = GetObjID(pObj);

    // If number key is pressed
#ifdef	USE_BTN_MSG_RELEASED
    if(objMsg == BTN_MSG_RELEASED)// BTN_MSG_PRESSED 
#ifdef	USE_adcX_adcy
	if((adcX == -1)||(adcY == -1))
#endif
#else
	if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED 
#endif
    {
  //***********************************************//
	    //if(adcX==-1)
	    //{
         //  TouchGetX();
         //  TouchGetY();
         // break;		    
		//}
 //***********************************************//
	
    	if(id >= ID_KEYPAD)
    		if(id  < ID_KEYPAD+10)
    		{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		if(pEb->length>=18)
	        		{
	        			pEb->length--;
	        		EbAddChar(pEb,'X');
	        		}
	        		else
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
//	        		memcpy(&Flashbuff[32],(pTempXchina[id-ID_KEYCHINAPAD-1][2]),2);
	        		SetState(pEb, EB_DRAW);
	    		}
        		return 1;        
    		}
    	if(id >= ID_KEYCHINAPAD)
    		if(id  < ID_KEYCHINAPAD+10)
    		{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,(pTempXchina[id-ID_KEYCHINAPAD-1][2]));
	        		SetState(pEb, EB_DRAW);
	    		}
        		return 1;        
    		}

	    if(id >= ID_ABCPAD)
	    if(id  < ID_ABCPAD+26)
	    	{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		if(!abcStatus)
	        		{
	        			EbAddChar(pEb,'a'+id-ID_ABCPAD);
	        		}
	        		else
	        		{
	        			EbAddChar(pEb,'A'+id-ID_ABCPAD);
	        		}
	        			
	        		SetState(pEb, EB_DRAW);
	    		} 
        		return 1;        
    		}
	}

    

    switch(id){

        case ID_CALL:
            if(objMsg == BTN_MSG_PRESSED){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
//                EbSetText(pEb, CallingStr);
                EbAddChar(pEb,(XCHAR)' ');	// changed by Spring.Chen
                SetState(pEb, EB_DRAW);
                status = 0;// 1; Changed by Spring.chen
            }
            return 1;
#if	0
        case ID_STOPCALL:
            if(objMsg == BTN_MSG_PRESSED){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                temp = 0x0000;
                EbSetText(pEb, &temp); 	
                SetState(pEb, EB_DRAW);
                status = 0;
            }
            return 1;
#endif
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
#if	0
        case ID_HOLD:
            if(objMsg == BTN_MSG_PRESSED){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	            if (status == 1) {
	                EbSetText(pEb, HoldingStr);	
	                status = 2;
    	        } else if (status == 2) {
	                EbSetText(pEb, CallingStr);	
	                status = 1;
	            } 
   	            SetState(pEb, EB_DRAW);
            }
            return 1;

        case ID_ASTERISK:
	    	if (!status) {
	            if(objMsg == BTN_MSG_PRESSED){
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbAddChar(pEb,(XCHAR)'*');
	                SetState(pEb, EB_DRAW);
	            }
	        } 
            return 1;

        case ID_POUND:
	    	if (!status) {
	            if(objMsg == BTN_MSG_PRESSED){
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbAddChar(pEb,(XCHAR)'#');
	                SetState(pEb, EB_DRAW);
	            }
	        }
            return 1;
#endif

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            	abcStatus = 0;
            	if(screenState == DISPLAY_VEHICLENO)
            		CopyChar(&Flashbuff[49],TempbuffStr,18);
            	else if(screenState == DISPLAY_BADGENO)
            		CopyChar(&Flashbuff[68],TempbuffStr,18);
            	else if(screenState == DISPLAY_OPERATORNAME)
            		CopyChar(&Flashbuff[86],TempbuffStr,18);
              if(screenState != DISPLAY_OPERATORNAME)  screenState++;// = CREATE_RECORD; 	// goto meter screen
              else screenState = CREATE_BLOW;
              for(ii = 0;ii<19;ii++)
              TempbuffStr[ii] = 0;
            }
            if(AlcTypeaddress+16>0xF300)
            	ErrorTrap("No Memory");
            else
            	AlcTypeaddress+=16;//ErrorTrap("Memory Enough");
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default

        case ID_ABC:
        	if(objMsg == BTN_MSG_RELEASED){
        		if(!abcStatus)
        		{
                		BtnSetText((BUTTON*)pObj, ABCStr);
        		}
        		else
        		{
        			BtnSetText((BUTTON*)pObj, abcStr);
        		}
        		abcStatus = !abcStatus;
            }
            return 1; // process by default

        case ID_BUTTONChina_P:
        	if(objMsg == BTN_MSG_RELEASED)
        	{
#if	1
			abcStatus = 0;
        	if(ChinaPage < ChinaPageMax)
        	{	ChinaPage++;
        	CopyArrays(ChinaPage);
        	screenState = screenState--; 	//
//        	SetState(pObj, BTN_DRAW_FOCUS|EB_DRAW);
        	// ÖÐÎÄ°´¼üÖ¸ÏòÏÂÒ»Ò³
        	}
#endif
        	}
        	return 1; // process by default
        case ID_BUTTONChina_M:
        	// ÖÐÎÄ°´¼üÖ¸ÏòÉÏÒ»Ò³
        	if(objMsg == BTN_MSG_RELEASED)
        	{
 #if	1
 			abcStatus = 0;
        		if(ChinaPage > 1)
        		{
        			ChinaPage --;
//        			return 0; // process by default
        		screenState = screenState--; 	//
        		CopyArrays(ChinaPage);
			}
#endif
        	}
        	return 1; // process by default

        default:
            return 1; // process by default
    }
}
	XCHAR serialnumberStr[] = {0x0030,0x0030,0x0030,0x0030,0x0030,0x0000};// ×î´óÊÇserialnumber == 65535;

#ifdef South_Africa_Version
	void CopyMonth(XCHAR FlashMth)
	{

	switch (FlashMth) {
							default:
							case 0x0001: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '1'; break; 
							case 0x0002: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '2'; break; 
							case 0x0003: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '3'; break; 
							case 0x0004: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '4'; break; 
							case 0x0005: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '5'; break; 
							case 0x0006: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '6'; break; 
							case 0x0007: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '7'; break; 
							case 0x0008: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '8'; break; 
							case 0x0009: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '0'; DisplaydateTimeStr[18] = '9'; break; 
							case 0x0010: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '1'; DisplaydateTimeStr[18] = '0'; break; 
							case 0x0011: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '1'; DisplaydateTimeStr[18] = '1'; break; 
							case 0x0012: DisplaydateTimeStr[16] = ' '; DisplaydateTimeStr[17] = '1'; DisplaydateTimeStr[18] = '2'; break; 
						}
	
	}
	void CopyTime(void)
	{

	
		DisplaydateTimeStr[0] = 0x0020;//Flashbuff[];
		DisplaydateTimeStr[1] = 0x0020;//Flashbuff[];
		DisplaydateTimeStr[2] = 0x0020;//Flashbuff[];// ÐÇÆÚ
		DisplaydateTimeStr[4] = ((Flashbuff[6]>>4)&0x000F)|0x0030;
		DisplaydateTimeStr[5] = (Flashbuff[6]&0x000F)|0x0030;// Ê±
		DisplaydateTimeStr[7] = (Flashbuff[7]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[8] = (Flashbuff[7]&0x000F)|0x0030;// ·Ö
		DisplaydateTimeStr[10] = ((Flashbuff[8]>>4)&0x000F)|0x0030;
		DisplaydateTimeStr[11] = (Flashbuff[8]&0x000F)|0x0030;// Ãë
		CopyMonth(Flashbuff[4]);
		DisplaydateTimeStr[13] = (Flashbuff[5]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[14] = (Flashbuff[5]&0x000F)|0x0030;// ÈÕ
		DisplaydateTimeStr[15] =  '/';
		DisplaydateTimeStr[19] =  '/';
		DisplaydateTimeStr[23] = (Flashbuff[3]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[24] = (Flashbuff[3]&0x000F)|0x0030;// Äê
	}
	
#else
	void CopyMonth(XCHAR FlashMth)
	{
		if(Datetype == Mdy)
			{
	
	switch (FlashMth) {
					default:
					case 0x0001: DisplaydateTimeStr[13] = 'J'; DisplaydateTimeStr[14] = 'a'; DisplaydateTimeStr[15] = 'n'; break; 
					case 0x0002: DisplaydateTimeStr[13] = 'F'; DisplaydateTimeStr[14] = 'e'; DisplaydateTimeStr[15] = 'b'; break; 
					case 0x0003: DisplaydateTimeStr[13] = 'M'; DisplaydateTimeStr[14] = 'a'; DisplaydateTimeStr[15] = 'r'; break; 
					case 0x0004: DisplaydateTimeStr[13] = 'A'; DisplaydateTimeStr[14] = 'p'; DisplaydateTimeStr[15] = 'r'; break; 
					case 0x0005: DisplaydateTimeStr[13] = 'M'; DisplaydateTimeStr[14] = 'a'; DisplaydateTimeStr[15] = 'y'; break; 
					case 0x0006: DisplaydateTimeStr[13] = 'J'; DisplaydateTimeStr[14] = 'u'; DisplaydateTimeStr[15] = 'n'; break; 
					case 0x0007: DisplaydateTimeStr[13] = 'J'; DisplaydateTimeStr[14] = 'u'; DisplaydateTimeStr[15] = 'l'; break; 
					case 0x0008: DisplaydateTimeStr[13] = 'A'; DisplaydateTimeStr[14] = 'u'; DisplaydateTimeStr[15] = 'g'; break; 
					case 0x0009: DisplaydateTimeStr[13] = 'S'; DisplaydateTimeStr[14] = 'e'; DisplaydateTimeStr[15] = 'p'; break; 
					case 0x0010: DisplaydateTimeStr[13] = 'O'; DisplaydateTimeStr[14] = 'c'; DisplaydateTimeStr[15] = 't'; break; 
					case 0x0011: DisplaydateTimeStr[13] = 'N'; DisplaydateTimeStr[14] = 'o'; DisplaydateTimeStr[15] = 'v'; break; 
					case 0x0012: DisplaydateTimeStr[13] = 'D'; DisplaydateTimeStr[14] = 'e'; DisplaydateTimeStr[15] = 'c'; break; 
				}
			}
		else 
			{
			switch (FlashMth) {
							default:
							case 0x0001: DisplaydateTimeStr[16] = 'J'; DisplaydateTimeStr[17] = 'a'; DisplaydateTimeStr[18] = 'n'; break; 
							case 0x0002: DisplaydateTimeStr[16] = 'F'; DisplaydateTimeStr[17] = 'e'; DisplaydateTimeStr[18] = 'b'; break; 
							case 0x0003: DisplaydateTimeStr[16] = 'M'; DisplaydateTimeStr[17] = 'a'; DisplaydateTimeStr[18] = 'r'; break; 
							case 0x0004: DisplaydateTimeStr[16] = 'A'; DisplaydateTimeStr[17] = 'p'; DisplaydateTimeStr[18] = 'r'; break; 
							case 0x0005: DisplaydateTimeStr[16] = 'M'; DisplaydateTimeStr[17] = 'a'; DisplaydateTimeStr[18] = 'y'; break; 
							case 0x0006: DisplaydateTimeStr[16] = 'J'; DisplaydateTimeStr[17] = 'u'; DisplaydateTimeStr[18] = 'n'; break; 
							case 0x0007: DisplaydateTimeStr[16] = 'J'; DisplaydateTimeStr[17] = 'u'; DisplaydateTimeStr[18] = 'l'; break; 
							case 0x0008: DisplaydateTimeStr[16] = 'A'; DisplaydateTimeStr[17] = 'u'; DisplaydateTimeStr[18] = 'g'; break; 
							case 0x0009: DisplaydateTimeStr[16] = 'S'; DisplaydateTimeStr[17] = 'e'; DisplaydateTimeStr[18] = 'p'; break; 
							case 0x0010: DisplaydateTimeStr[16] = 'O'; DisplaydateTimeStr[17] = 'c'; DisplaydateTimeStr[18] = 't'; break; 
							case 0x0011: DisplaydateTimeStr[16] = 'N'; DisplaydateTimeStr[17] = 'o'; DisplaydateTimeStr[18] = 'v'; break; 
							case 0x0012: DisplaydateTimeStr[16] = 'D'; DisplaydateTimeStr[17] = 'e'; DisplaydateTimeStr[18] = 'c'; break; 
						}
	
		}
	}
	void CopyTime(void)
	{
	if(Datetype == Mdy)
		{
		DisplaydateTimeStr[0] = 0x0020;//Flashbuff[];
		DisplaydateTimeStr[1] = 0x0020;//Flashbuff[];
		DisplaydateTimeStr[2] = 0x0020;//Flashbuff[];// ÐÇÆÚ
		DisplaydateTimeStr[4] = ((Flashbuff[6]>>4)&0x000F)|0x0030;
		DisplaydateTimeStr[5] = (Flashbuff[6]&0x000F)|0x0030;// Ê±
		DisplaydateTimeStr[7] = (Flashbuff[7]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[8] = (Flashbuff[7]&0x000F)|0x0030;// ·Ö
		DisplaydateTimeStr[10] = ((Flashbuff[8]>>4)&0x000F)|0x0030;
		DisplaydateTimeStr[11] = (Flashbuff[8]&0x000F)|0x0030;// Ãë
		CopyMonth(Flashbuff[4]);
		DisplaydateTimeStr[17] = (Flashbuff[5]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[18] = (Flashbuff[5]&0x000F)|0x0030;// ÈÕ
		DisplaydateTimeStr[23] = (Flashbuff[3]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[24] = (Flashbuff[3]&0x000F)|0x0030;// Äê
	}
	else
	{
	
	DisplaydateTimeStr[0] = 0x0020;//Flashbuff[];
		DisplaydateTimeStr[1] = 0x0020;//Flashbuff[];
		DisplaydateTimeStr[2] = 0x0020;//Flashbuff[];// ÐÇÆÚ
		DisplaydateTimeStr[4] = ((Flashbuff[6]>>4)&0x000F)|0x0030;
		DisplaydateTimeStr[5] = (Flashbuff[6]&0x000F)|0x0030;// Ê±
		DisplaydateTimeStr[7] = (Flashbuff[7]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[8] = (Flashbuff[7]&0x000F)|0x0030;// ·Ö
		DisplaydateTimeStr[10] = ((Flashbuff[8]>>4)&0x000F)|0x0030;
		DisplaydateTimeStr[11] = (Flashbuff[8]&0x000F)|0x0030;// Ãë
		CopyMonth(Flashbuff[4]);
		DisplaydateTimeStr[13] = (Flashbuff[5]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[14] = (Flashbuff[5]&0x000F)|0x0030;// ÈÕ
		DisplaydateTimeStr[15] =  0x0020;
		DisplaydateTimeStr[23] = (Flashbuff[3]>>4&0x000F)|0x0030;
		DisplaydateTimeStr[24] = (Flashbuff[3]&0x000F)|0x0030;// Äê
	
	}
	}
#endif
void DisplayTestResult(void)
{
	XCHAR temp,temp1;
	temp = Flashbuff[10];
	temp1 = Flashbuff[11];
	switch(Flashbuff[9])
	{
		case 0:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= temp&0x000F|0X0030;
			Test_Result[2]= temp1>>4&0x000F|0X0030;
			Test_Result[3]= 0x002E;
			Test_Result[4]= temp1&0x000F|0X0030;
		break;
		case 1:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
		break;
		case 2:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 3:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 4:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 5:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= temp&0x000F|0X0030;
			Test_Result[2]= temp1>>4&0x000F|0X0030;
			Test_Result[3]= 0x002E;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 6:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 7:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 8:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
			Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		case 9:
			Test_Result[0]= temp>>4&0x000F|0X0030;
			Test_Result[1]= 0x002E;
			Test_Result[2]= temp&0x000F|0X0030;
			Test_Result[3]= temp1>>4&0x000F|0X0030;
		 	Test_Result[4]= temp1&0x000F|0X0030;
			//Flashbuff[15]= 0;
		break;
		default:
			break;

	}

}


void CreateRecord()
{
	XCHAR temp,temp1;
	readrecord();
	SaveReadItem(FALSE);
	OBJ_HEADER* obj;
//serialnumber = 65535;
		serialnumberStr[0] = (serialnumberRecord /10000) +'0';// ÍòÎ»
		serialnumberStr[1] = (serialnumberRecord /1000 %10) +'0';// Ç§Î»
		serialnumberStr[2] = (serialnumberRecord /100 %10) +'0';// °ÙÎ»
		serialnumberStr[3] = (serialnumberRecord /10 %100%10) +'0';// Ê®Î»
		serialnumberStr[4] = (serialnumberRecord %10) +'0';// ¸öÎ»
		ST_IRstatus = Flashbuff[16];
		temp = Flashbuff[10];
		temp1 = Flashbuff[11];
		DisplayTestResult();//ÏÔÊ¾²âÊÔ½á¹û
		CopyTime();
	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(RecordStr);	// CreatePage("Buttons");

StCreate(ID_STATICTEXT3,           		// ID 
               positionax,positionay+0*positionmaxy,
              positionax+3*positionmaxx,positionay+1*positionmaxy,      		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              NumberStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+0*positionmaxy,
              7*(positionax+positionmaxx),positionay+1*positionmaxy,            		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              serialnumberStr,//NULL,//GetdataStr, 				// "TEST", 	// text
              mainScheme);        //   alt2Scheme        // use alternate scheme
              
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+1*positionmaxy+30,
              positionax+3*positionmaxx,positionay+2*positionmaxy+30,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              DateStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
     StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+1*positionmaxy+30,
              7*(positionax+positionmaxx),positionay+2*positionmaxy+30,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              &DisplaydateTimeStr[13],//&Flashbuff[86],//GetdataStr, 				// "TEST", 	// text
              mainScheme);    //    alt2Scheme           // use alternate scheme          
              
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+2*positionmaxy+60,
              positionax+3*positionmaxx,positionay+3*positionmaxy+60,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TimeStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme	
       StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+2*positionmaxy+60,
              7*(positionax+positionmaxx)-10,positionay+3*positionmaxy+60,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              DisplaydateTimeStr,//&Flashbuff[86],//GetdataStr, 				// "TEST", 	// text
              mainScheme);     // alt2Scheme             // use alternate scheme
                            
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,positionay+3*positionmaxy+90,
              positionax+4*positionmaxx,positionay+4*positionmaxy+90,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              InitiativeStr, 				// "TEST", 	// text
              mainScheme);  
#ifdef SA_VERSION
if(Screen_mode&ST_IRstatus)
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy+90,
              7*(positionax+positionmaxx),positionay+4*positionmaxy+90,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              ScreenmodeStr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);    //   alt2Scheme            // use alternate scheme
else
#endif	
{
if(ST_IRstatus&(ST_REFUSE))
	
StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy+90,
              7*(positionax+positionmaxx),positionay+4*positionmaxy+90,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              RefuseStr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);    //   alt2Scheme            // use alternate scheme
  else if(ST_IRstatus&ST_Passive)
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy+90,
              7*(positionax+positionmaxx),positionay+4*positionmaxy+90,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              ManualStr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);

  else if(ST_IRstatus&ST_Discontinued)
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy+90,
              7*(positionax+positionmaxx),positionay+4*positionmaxy+90,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              Discontinuedstr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);

  else //if(ST_IRstatus&0x0002)
  	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+3*positionmaxy+90,
              7*(positionax+positionmaxx),positionay+4*positionmaxy+90,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              AutoStr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);
}            


	StCreate(ID_STATICTEXT3,           		// ID ¾Æ¾«º¬Á¿
              positionax,positionay+4*positionmaxy+120,
              positionax+3*positionmaxx,positionay+5*positionmaxy+120,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TestResultStr, 				// "TEST", 	// text
              mainScheme);                   // use alternate scheme
#ifdef SA_VERSION
if((Screen_mode & ST_IRstatus)&&(Flashbuff[10] & Screen_mode))
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+4*positionmaxy+120,
              7*(positionax+positionmaxx),positionay+5*positionmaxy+120,          		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              AlocholDetectedstr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);    //   
else if((Screen_mode & ST_IRstatus)&&(!Flashbuff[10]))
		StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+4*positionmaxy+120,
              7*(positionax+positionmaxx),positionay+5*positionmaxy+120,          		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              NoAlocholstr,//&GetdataStr[96],//GetdataStr, 				// "TEST", 	// text
              mainScheme);
else
#endif
{
	if(ST_IRstatus&(ST_REFUSE))
	{
		StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+4*positionmaxy+120,
              7*(positionax+positionmaxx),positionay+5*positionmaxy+120,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		RefuseStr,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);           
	}

    else if(ST_IRstatus&ST_Discontinued)	  
	{
		StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+4*positionmaxy+120,
              7*(positionax+positionmaxx),positionay+5*positionmaxy+120,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		Discontinuedstr,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);          
	}		
	else
	{StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx,positionay+4*positionmaxy+120,
              7*(positionax+positionmaxx),positionay+5*positionmaxy+120,         		
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		Test_Result,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);
	
	DisplayThickness(DisplayUnitStr);
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax+5*positionmaxx,positionay+4*positionmaxy+120,
              7*(positionax+positionmaxx),positionay+5*positionmaxy+120,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		 DisplayUnitStr,//GetdataStr,// SubjectNameStr, 				// "TEST", 	// text
              mainScheme);
	}
}
	obj = (OBJ_HEADER*)BtnCreate(ID_BUTTONChina_M,//ID_KEYCHINAPAD+1,             	// ID 
              KEYABCSTARTX+0*KEYABCSIZEX,		
              KEYSTARTY+5*KEYSIZEY+40,
              KEYABCSTARTX+2*KEYABCSIZEX-6,
              KEYSTARTY+6*KEYSIZEY+40,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              (void*)&LEFT,                    	// no bitmap
              NULL,//pTempXchina[1],    		// text
              blackScheme);
	BtnCreate(ID_BUTTONChina_P,//ID_KEYCHINAPAD+0,             	// ID 
              KEYABCSTARTX+8*KEYABCSIZEX,
              KEYSTARTY+5*KEYSIZEY+40,
              KEYABCSTARTX+10*KEYABCSIZEX-6,
              KEYSTARTY+6*KEYSIZEY+40,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              (void*)&RIGHT,                    	// no bitmap
              NULL,//pTempXchina[0],    		// text
              blackScheme);              	// alternative GOL scheme 
              
	pbtn = (OBJ_HEADER*)BtnCreate(ID_BUTTON8,             	// button ID 
              123, KEYSTARTY+5*KEYSIZEY+40,
              163,KEYSTARTY+6*KEYSIZEY+40,        	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              (void*)&printerbutton,//(void*)&PicGPS,                    	// no bitmap
              NULL, //NULL,//OffStr, 					// "OFF",      	// text
              blackScheme);   //redScheme         	// use alternate scheme 	
     GOLSetFocus(obj);			  
	ST_IRstatus = 0;// »Ö¸´Õý³£²âÊÔÄ£Ê½
 
}

WORD MsgRecord(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp,i;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);


    switch(id){
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				

				ChoicePrint = FALSE;

				SPrint=FALSE;
               	screenState = CREATE_MAIN ;//++;prevState//= ; 	// goto meter screen
            }
            return 1; // process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				

				ChoicePrint = FALSE;

				SPrint=FALSE;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default
       case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
				

				ChoicePrint = FALSE;

				SPrint=FALSE;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){

			SPrint=TRUE;
			ChoicePrint = TRUE;
			Sprinnum = 0;
			PrintSec=0;
			PrintCount = 0;
			//Pressflag = 0xaa;
			memset(&Flashbuff[0],0,30);
			//memset(&tx1_buff[0],0,250);
			//memset(&rx1_buff[0],0,250);
			readrecord();
			for(i=0;i<30;i++)
				Printbuff[i]=(unsigned char)Flashbuff[i];
			Inputdata();
			p_n = 0;

           // StartPrint();
            Print_TestRecord();

            }
            return 1; // process by default
        case ID_BUTTONChina_P:
            if(objMsg == BTN_MSG_RELEASED){
                if(serialnumberRecord<serialnumber-1)
            	{
            		serialnumberRecord ++;//;= serialnumber
            		screenState = CREATE_RECORD;//
            	}
            	else
            	{
            		serialnumberRecord = serialnumber-1;
            		screenState = CREATE_MAIN; 	// goto list box screen
            	}
            }
            return 1; // process by default
            case ID_BUTTONChina_M:
            if(objMsg == BTN_MSG_RELEASED){
                if(serialnumberRecord>1)
            	{
            		serialnumberRecord --;//;= serialnumber
            		screenState = CREATE_RECORD;//
            	}
            	else
            	{
            		serialnumberRecord = serialnumber-1;
            		screenState = CREATE_MAIN; 	// goto list box screen
            	}
            }
            return 1; // process by default
            
        default:
            return 1; // process by default
    }


}

void CreateEditeeprom(XCHAR *EditTestStr,WORD CharMax)
{

	static XCHAR pTempXchar[][2] = 	{	{'0',0},
										{'1',0},
										{'2',0},
										{'3',0},
										{'4',0},
										{'5',0},
										{'6',0},
										{'7',0},
										{'8',0},
										{'9',0},
										{'A',0},
										{'B',0},
										{'C',0},
										{'D',0},
										{'E',0},
										{'F',0}
									};
#define KEYSTARTX 	10
#define KEYSTARTY 	38
#define KEYSIZEX	55
#define KEYSIZEY  	45
#define MAXCHARSIZE CharMax
MAXCHAROld=CharMax;	
OBJ_HEADER* obj;
SHORT i;
//char *pText="EEPROM EDIT";
    GOLFree();  // free memory for the objects in the previous linked list and start new list

 WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              (void*)&WellLogo,               	// icon
              EditTestStr,	   				// set text 
              navScheme);               // default GOL scheme 
BtnCreate(ID_BUTTON_BACK,         	// button ID 
              0,GetMaxY()-40,					   	// left, top corner	
              40,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&LEFT,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);          	   	// use navigation scheme
              
BtnCreate(ID_BUTTON_NEXT,   	    // button ID 
              GetMaxX()-40,GetMaxY()-40,
              GetMaxX(),GetMaxY(),0,   	// dimension (with radius = 0)
              BTN_DRAW,               	// will be dislayed and disabled after creation
              (void*)&RIGHT,					   	// no bitmap
              NULL,//, NextStr			// RIGHT arrow as text
              blackScheme);   


BtnCreate(ID_BACKSPACE,
		100,GetMaxY()-40,					   	// left, top corner	
              140,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              UpArrowStr,			// LEFT arrow as text
              altScheme);

  
	RTCCProcessEvents();				// update the date and time strings
	i = 0;						
	while (i < 12) {
		dateTimeStr[i] 	  = (XCHAR)_time_str[i];
		dateTimeStr[i+13] = (XCHAR)_date_str[i];
		i++;
	}
	dateTimeStr[12] = (XCHAR)'\n';
	dateTimeStr[25] = (XCHAR)'\0';
	

 EbCreate(ID_EDITBOX1,              	// ID
              4,KEYSTARTY+1,GetMaxX()-4,KEYSTARTY+1*KEYSIZEY-GOL_EMBOSS_SIZE,  // dimension
              EB_DRAW|EB_CARET|EB_FOCUSED, // will be dislayed after creation
              NULL,
              MAXCHARSIZE,
              altScheme);               // default GOL scheme

    BtnCreate(ID_KEYPAD+7,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[7],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+8,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[8],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+9,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[9],    		// text
              altScheme);              	// alternative GOL scheme 
              
     BtnCreate(ID_KEYPAD+15,             	// F
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+1*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[15],    		// text
              altScheme);              	// alternative GOL scheme            

    BtnCreate(ID_KEYPAD+4,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[4],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+5,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[5],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+6,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[6],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+14,             	// ID 
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+2*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[14],    		// text
              altScheme);              	// alternative GOL scheme 		

    BtnCreate(ID_KEYPAD+1,             	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[1],    		// text
              altScheme);              	// alternative GOL scheme   

    BtnCreate(ID_KEYPAD+2,             	// ID 
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[2],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+3,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[3],    		// text
              altScheme);              	// alternative GOL scheme 

    BtnCreate(ID_KEYPAD+13,             	// ID 
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+3*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[13],    		// text
              altScheme);              	// alternative GOL scheme 			  


    BtnCreate(ID_KEYPAD+0,            	// ID 
              KEYSTARTX+0*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+1*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[0],			// Left Arrow 
              altScheme);              	// alternative GOL scheme 

BtnCreate(ID_KEYPAD+10,             	// ID 
              KEYSTARTX+1*KEYSIZEX,		
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[10],    		// text
              altScheme);              	// alternative GOL scheme 
              
BtnCreate(ID_KEYPAD+11,             	// ID 
              KEYSTARTX+2*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[11],    		// text
              altScheme); 
			  
BtnCreate(ID_KEYPAD+12,             	// ID 
              KEYSTARTX+3*KEYSIZEX,
              KEYSTARTY+4*KEYSIZEY,
              KEYSTARTX+4*KEYSIZEX,
              KEYSTARTY+5*KEYSIZEY,    	// dimension
              0,
              BTN_DRAW,                	// will be dislayed after creation
              NULL,                    	// no bitmap
              pTempXchar[12],    		// text
              altScheme); 
              

}
WORD MsgEditeeprom(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned int  j=1,i=0,point=0;
static char status = 0;			// status to check if calling, holding or not
//XCHAR  EEPROMbuffStr[6]={0};
unsigned int  eepromaddress=0;
BYTE  eepromvalue=0;
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = '0'+id-ID_KEYPAD;

				  

        return 1;        
    }
}	
    if(id >= ID_KEYPAD+10)
    if(id  <= ID_KEYPAD+15){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'A'+id-ID_KEYPAD-10);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = 'A'+id-ID_KEYPAD-10;

				  

        return 1;        
    }
}	
    	}	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                 
                       eepromaddress=(TempbuffStr[0]-'0')&0x000F;
			eepromaddress=eepromaddress<<4;
			if(TempbuffStr[1]<='9')
			 eepromaddress=eepromaddress|((TempbuffStr[1]-'0')&0x000F);
			else 
			eepromaddress=eepromaddress|((TempbuffStr[1]-'A'+10)&0x000F);
			eepromaddress=eepromaddress<<4;
			
			if(TempbuffStr[2]<='9')
			 eepromaddress=eepromaddress|((TempbuffStr[2]-'0')&0x000F);
			else 
			eepromaddress=eepromaddress|((TempbuffStr[2]-'A'+10)&0x000F);
			eepromaddress=eepromaddress<<4;
			if(TempbuffStr[3]<='9')
			 eepromaddress=eepromaddress|((TempbuffStr[3]-'0')&0x000F);
			else 
			eepromaddress=eepromaddress|((TempbuffStr[3]-'A'+10)&0x000F);
		      	if(TempbuffStr[4]<='9')
			 eepromvalue=eepromvalue|((TempbuffStr[4]-'0')&0x000F);
			else 
			eepromvalue=eepromvalue|((TempbuffStr[4]-'A'+10)&0x000F);
			eepromvalue=eepromvalue<<4;
			if(TempbuffStr[5]<='9')
			 eepromvalue=eepromvalue|((TempbuffStr[5]-'0')&0x0F);
			else 
			eepromvalue=eepromvalue|((TempbuffStr[5]-'A'+10)&0x0F);
		
                        for(i=0;i<6;i++){
			TempbuffStr[i] = 0;
   	                   }	
				 
			EEPROMWriteByte(eepromvalue, eepromaddress);
			 screenState = CREATE_SETTING; 	// goto list box screen            
            }
        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 	// goto list box screen
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}

WORD MsgEditID(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned char  j=0,i=0;//,Tempbuffchar[9]={0,0,0,0,0,0,0,0,0};

BYTE  IDvalue[4]={0};
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9){

        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = '0'+id-ID_KEYPAD;

				  

        return 1;        
}	
    if(id >= ID_KEYPAD+10)
    if(id  <= ID_KEYPAD+15){

        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'A'+id-ID_KEYPAD-10);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = 'A'+id-ID_KEYPAD-10;

				  

        return 1;        

}	
    	}	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
            
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	        
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				
    // for(i=0;i<8;i++)
		//Tempbuffchar[i] = (unsigned char)TempbuffStr[i];
	 
	if((TempbuffStr[0]!=0)&&(TempbuffStr[7]!=0))
		{
           j = 0;
		for(i=0;i<4;i++)
			{  
                IDvalue[i]=(TempbuffStr[j]-'0')&0x0F;
			  
			    IDvalue[i]=IDvalue[i]<<4;
			

			    IDvalue[i]=IDvalue[i]|((TempbuffStr[1+j]-'0')&0x0F);


			  j=j+2;

			}

		
            for(i=0;i<9;i++)
			    TempbuffStr[i] = 0;
	
		if(screenState==DISPLAY_EDITID)
			{
			EEPROMWriteByte(IDvalue[0], ID_JIQIADDRESS);
			EEPROMWriteByte(IDvalue[1], ID_JIQIADDRESS+1);
			EEPROMWriteByte(IDvalue[2], ID_JIQIADDRESS+2);
			EEPROMWriteByte(IDvalue[3], ID_JIQIADDRESS+3);

		}

			 screenState = CREATE_AGENT; 	// goto list box screen            
            }
        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT; 	// goto list box screen
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }

}
}
WORD MsgEditDemarcate(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

{

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
unsigned int  j=1,i=0,point=0;
static char status = 0;			// status to check if calling, holding or not
unsigned int  INPUTtem=0,INPUTAD=0;
    id = GetObjID(pObj);

    // If number key is pressed
    if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED
    {
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'0'+id-ID_KEYPAD);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = '0'+id-ID_KEYPAD;

				  

        return 1;        
    }
}	
    if(id >= ID_KEYPAD+10)
    if(id  <= ID_KEYPAD+15){
	    if (!status) {
        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    

			EbAddChar3(pEb,'A'+id-ID_KEYPAD-10);
	               SetState(pEb, EB_DRAW);
	        	TestMiMaStr[0] = 'A'+id-ID_KEYPAD-10;

				  

        return 1;        
    }
}	
    	}	
    switch(id){


        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
         if((TempbuffStr[0]<='0')&&(TempbuffStr[1]<='0')&&(TempbuffStr[2]<='0')&&(TempbuffStr[3]<='0'))
         	{
			if(ADLIST>=1&&ADLIST<=3)
                screenState = CREATE_DEMARCATE; 	// goto list box screen
                else  screenState = CREATE_DEMARCATE2;
         	}
		 else {
          if(TempbuffStr[0]<='9')
			INPUTtem=(TempbuffStr[0]-'0')&0x000F;
	      else 
	  	    INPUTtem=(TempbuffStr[0]-'A'+10)&0x000F;
	        INPUTtem=INPUTtem<<4;
	  
			if(TempbuffStr[1]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[1]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[1]-'A'+10)&0x000F);
			INPUTtem=INPUTtem<<4;
			
			if(TempbuffStr[2]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[2]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[2]-'A'+10)&0x000F);
			INPUTtem=INPUTtem<<4;
			if(TempbuffStr[3]<='9')
			 INPUTtem=INPUTtem|((TempbuffStr[3]-'0')&0x000F);
			else 
			INPUTtem=INPUTtem|((TempbuffStr[3]-'A'+10)&0x000F);
			
      if(TempbuffStr[4]<='9')
			INPUTAD=(TempbuffStr[4]-'0')&0x000F;
	  else 
	  	    INPUTAD=(TempbuffStr[4]-'A'+10)&0x000F;
			INPUTAD=INPUTAD<<4;
			if(TempbuffStr[5]<='9')
			 INPUTAD=INPUTAD|((TempbuffStr[5]-'0')&0x000F);
			else 
			INPUTAD=INPUTAD|((TempbuffStr[5]-'A'+10)&0x000F);
			INPUTAD=INPUTAD<<4;
			
			if(TempbuffStr[6]<='9')
			 INPUTAD=INPUTAD|((TempbuffStr[6]-'0')&0x000F);
			else 
			INPUTAD=INPUTAD|((TempbuffStr[6]-'A'+10)&0x000F);
			INPUTAD=INPUTAD<<4;
			if(TempbuffStr[7]<='9')
			 INPUTAD=INPUTAD|((TempbuffStr[7]-'0')&0x000F);
			else 
			INPUTAD=INPUTAD|((TempbuffStr[7]-'A'+10)&0x000F);
		
           for(i=0;i<6;i++){
			TempbuffStr[i] = 0;
   	                   }	
		if(ADLIST==1){
		
			EEPROMWriteWord(INPUTtem, 0x6000);
			EEPROMWriteWord(INPUTAD, 0x600e);
		screenState = CREATE_DEMARCATE; 	// goto list box screen  
			}
		else if(ADLIST==2)
			{
			EEPROMWriteWord(INPUTtem, 0x6002);
			EEPROMWriteWord(INPUTAD, 0x6010);
			screenState = CREATE_DEMARCATE; 	// goto list box screen  
			}
		else if(ADLIST==3)
			{
			EEPROMWriteWord(INPUTtem, 0x6004);
			EEPROMWriteWord(INPUTAD, 0x6012);
			screenState = CREATE_DEMARCATE; 	// goto list box screen  
			}
			else if(ADLIST==4)
				{
			EEPROMWriteWord(INPUTtem, 0x6014);
			EEPROMWriteWord(INPUTAD, 0x6016);
			screenState = CREATE_DEMARCATE2; 	// goto list box screen  
				}
			else if(ADLIST==5)
				{
			EEPROMWriteWord(INPUTtem, 0x600a);
			EEPROMWriteWord(INPUTAD, 0x6018);
			screenState = CREATE_DEMARCATE2; 
				}
			
			else if(ADLIST==6)
				{
			EEPROMWriteWord(INPUTtem, 0x600c);
			EEPROMWriteWord(INPUTAD, 0x601a);
			screenState = CREATE_DEMARCATE2; 
			} 
			return 1;
            }
            	}
        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				 if(ADLIST>=1&&ADLIST<=3)
                screenState = CREATE_DEMARCATE; 	// goto list box screen
                else  screenState = CREATE_DEMARCATE2;
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}

void  CreateBlowPlease()
{

#ifdef DOT

	serialnumberStr[0] = ((serialnumberRecord+1)/10000) +'0';// ÍòÎ»
	serialnumberStr[1] = ((serialnumberRecord+1)/1000 %10) +'0';// Ç§Î»
	serialnumberStr[2] = ((serialnumberRecord+1)/100 %10) +'0';// °ÙÎ»
	serialnumberStr[3] = ((serialnumberRecord+1)/10 %100%10) +'0';// Ê®Î»
	serialnumberStr[4] = ((serialnumberRecord+1)%10) +'0';// ¸öÎ»

#endif
   OBJ_HEADER* obj;
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(BlowStr);	// CreatePage("Blow Please");
	//pump_pwr=1;// ±Ã³äµç
	pProgressBar = PbCreate(ID_PROGRESSBAR1,// ID
              40,85,(GetMaxX()-40),135,          		// dimension
              PB_DRAW,                 		// will be dislayed after creation
              25,                      		// position
              50,                      		// range
              NULL);         				// use default scheme
	
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              BlowStr, 				// "GPS", 	// text
              altScheme);    // alt2Scheme              // use alternate scheme
     obj = (OBJ_HEADER*)BtnCreate(ID_BUTTON1,             	// button ID 
               40,185,(GetMaxX()-40),235,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              ManualStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme

#ifdef DOT
   StCreate(ID_STATICTEXT3,           		// ID 
               positionax+13+20,positionay+0*positionmaxy,
              positionax+3*positionmaxx+20+20,positionay+1*positionmaxy+10,      		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              NumberStr, 				// "TEST", 	// text
              navScheme);                   // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax+3*positionmaxx+25+25,positionay+0*positionmaxy,
              7*(positionax+positionmaxx)+20+20,positionay+1*positionmaxy+10,            		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              serialnumberStr,//NULL,//GetdataStr, 				// "TEST", 	// text
              navScheme);        //   alt2Scheme        // use alternate scheme	
#endif 
     GOLSetFocus(obj);			  
}

WORD MsgBlowPlease(WORD objMsg, OBJ_HEADER* pObj){


        switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            	g_blow_time = 0;		// ½á¹û³öÀ´»Ö¸´Õý³£//²»´µÆøÌø×ª
            	Quickly_test=0;
				//pump_pwr=0;
                screenState = CREATE_MAIN; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
            	g_blow_time = 0;		// ½á¹û³öÀ´»Ö¸´Õý³£//²»´µÆøÌø×ª
            	Quickly_test=0;
				//pump_pwr=0;
                screenState = CREATE_MAIN;  		// goto round buttons screen
            }
            return 1; 
       case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
		        g_blow_time = 0;		// ½á¹û³öÀ´»Ö¸´Õý³£//²»´µÆøÌø×ª
		        Quickly_test=0;
				//pump_pwr=0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case  ID_BUTTON1:

		 if(objMsg == BTN_MSG_RELEASED){


			    Manual_Analyze();
			if(TestMode==0xaa||TestMode==0xbb)
			{
			   Marktimes++;
			   EEPROMWriteWord(Marktimes, TestTimesDataAddress);
			}			
			ST_IRstatus = ST_Passive;
                screenState = CREATE_RESULT; 	// goto radio buttons screen
                //pump_pwr=0;
            }
            return 1; 

         default:
   //      	pump_pwr=1;// ±Ã³äµç
                 return 1; 
        }
}		

void CreateDISCONTINUED()
{

	OBJ_HEADER* obj;
	

       //pump_pwr=0;
	
	GOLFree();   // free memory for the objects in the previous linked list and start new list

      WndCreate(ID_WINDOW1,				// ID
				  0,0,GetMaxX(),GetMaxY(),	// dimension
				  WND_DRAW, 				// will be dislayed after creation
				  (void*)&WellLogo, 				// icon
				  BlowDiscontinuedstr,					// set text 
				  navScheme);				// default GOL scheme 
	
	BtnCreate(ID_BUTTON_HOME,
		      100,GetMaxY()-40,					   	// left, top corner	
              140,GetMaxY(),0, 		   	// right, bottom corner (with radius = 0)
              BTN_DRAW,               	// will be dislayed after creation
              (void*)&HOME,					   	// no bitmap	
              NULL,//, HomeStr			// LEFT arrow as text
              blackScheme);

	
	//CreatePage(BlowDiscontinuedstr);	// CreatePage("Blow Please");
	
	 obj = (OBJ_HEADER*)BtnCreate(ID_BUTTON1,             	// button ID 
               40,185,(GetMaxX()-40),235,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              TestagainStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
        BtnCreate(ID_BUTTON2,             	// button ID 
               40,105,(GetMaxX()-40),155,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              Discontinuedstr,//InterruptedStr,//RefuseStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
              
		GOLSetFocus(obj); 


}

WORD MsgDISCONTINUED(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not

    id = GetObjID(pObj);


    switch(id){

            
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
			ST_IRstatus = 0;
			screenState = CREATE_BLOW;     	// goto radio buttons screen
			g_backlight_time = 3*60;// 3·ÖÖÓ²»´µÆø£¬×Ô¶¯Ìø³öÀ´
            }
            return 1; // process by default
        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
		ST_IRstatus=ST_Discontinued;
                screenState = CREATE_RESULT; 
            }
            return 1; // process by default

        default:
            return 1; // process by default
    }


}

void Battery()
{
	SetColor(WHITE);
				Line(206,8,206,11);	//|
				Line(206,8,208,8);		//--
				Line(206,11,208,11);		//_


				Line(208,5,208,15);		//|
				Line(208,5,233,5);		//_
				Line(208,15,233,15);		//_
				Line(233,5,233,15);		//|
}
void ShowBatteryEmpty()
{
	SetColor(BLACK);//WHITE
	Bar(208,5,233,15);
				
}
void	ShowBattery(int value, char batt_type)
{
	static int cur_value=0;    //µ±Ç°µçÁ¿Öµ
	static int BatteryOFF = 0,cur_value_prev=0;// µ±Ç°µç³Ø¾¯¸æ´ÎÊý
	static int change=0,Temp_value=0;
	BYTE i=0;

    if(value == -1)
    {
        cur_value++;
        if(cur_value > 7) cur_value = 0;
    }
    else
    {
        if(batt_type == BATT_TYPE_ALKALINE)    //¼îÐÔµç³Ø
        {
            if(value < 62) cur_value = 0;    //batt empty
            else if(value > 116 ) cur_value = 8;    //batt full
            else cur_value = (value - 62) / 7;      // 116 - 62
        }
        else if(batt_type == BATT_TYPE_NIH)    //ÄøÇâµç³Ø
        {
            if(value < 69) cur_value = 0;    //batt empty
            else if(value > 116 ) cur_value = 8;    //batt full
            else cur_value = (value - 69) / 6;
        }
        else if(batt_type == BATT_TYPE_LITHIUM)    //ï®µç³Ø
        {
            if(value < BatteryMin) cur_value = 0;    //batt empty
            else if(value > BatteryMax)cur_value = 8;
            else cur_value = (value - BatteryMin) / BatterySize;    //
        }
        else ErrorTrap("FALSE");
    }

 	if(Temp_value!=cur_value)
 	  {
       Temp_value=cur_value;
	   change=0;
	  }

	else 
           change++;
	
  if(change==3||(initialpower==0))
  	{
	switch(cur_value)
		{
			
			case	8:
				//ShowBatteryEmpty();
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(208, 5+i, 233, 5+i+1);
                 }
				BatteryOFF =0;
                change=0;
				initialpower++;				
				break;
			case	7:
				ShowBatteryEmpty();
				Bar(208,5,233,15);
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(211, 5+i, 233, 5+i+1);
                 }
				BatteryOFF =0;
                change=0;
				initialpower++;				
				break;
			case	6:
				ShowBatteryEmpty();
				Bar(208,5,233,15);
				/*
				SetColor(BRIGHTGREEN);
				Bar(209,10,230,20);
				*/
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(214, 5+i, 233, 5+i+1);
                 }
				BatteryOFF = 0;
                change=0;
				initialpower++;				
				break;
			case	5:
				ShowBatteryEmpty();
				Bar(208,5,233,15);
				/*
				SetColor(BRIGHTGREEN);
				Bar(213,10,230,20);
				*/
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(217, 5+i, 233, 5+i+1);
                 }				
				BatteryOFF =0;
                change=0;
				initialpower++;				
				break;
			case	4:
				ShowBatteryEmpty();
				Bar(208,5,233,15);
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(220, 5+i, 233, 5+i+1);
                 }
                change=0;
				initialpower++;				
				break;
			case	3:
				ShowBatteryEmpty();
				Bar(208,5,233,15);
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(224, 5+i, 233, 5+i+1);
                 }
				BatteryOFF = 0;
                change=0;
				initialpower++;				
				break;
			case	2:
				ShowBatteryEmpty();
				Bar(208,5,233,15);
				for(i=0;i<10;i++)
                 {	SetColor(RGB565CONVERT(0,35*i-35*i*i/10+128,0));
	                Bar(228, 5+i, 233, 5+i+1);
                 }
                change=0;
				initialpower++;				
				break;
			case	1:
				ShowBatteryEmpty();
				Bar(208,5,233,15);
				SetColor(BRIGHTRED);
				Bar(231,5,233,15);
				BatteryOFF =0;
                change=0;
				initialpower++;				
				break;
			case	0:
				ShowBatteryEmpty();
				BatteryOFF++;
                change=0;
				initialpower++;				
				if(BatteryOFF > 5)
					screenState = CREATE_POWEROFF;
			defalt:
				break;

		}
		SetColor(BLACK);
	Battery();
  	}
		cur_value_prev=cur_value;

}



void	CreateSetBackLight()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(BacklightStr);	// CreatePage("Blow Please");



		RbCreate(ID_RADIOBUTTONStar + 0,          	// ID 
              1,(GetMaxY() - 180),50,(GetMaxY() - 150),           // dimension
              RB_DRAW|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              OneMinStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 1,          // ID 
              100,(GetMaxY() - 180),150,(GetMaxY() - 150),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              TwoMinStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 2,          // ID 
              2,(GetMaxY() - 140),80,(GetMaxY() - 110),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineMinStr, 			   // "Right"
              alt4Scheme);              // use alternate scheme

     RbCreate(ID_RADIOBUTTONStar + 3,          // ID 
              100,(GetMaxY() - 140),150,(GetMaxY() - 110),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              TenMinStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme


}


WORD MsgSetBackLight(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_SETTING;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;



        default:
            return 1; 							// process by default
    }
}


void  CreateUnitPDmenu()
{
	WORD textWidth, textHeight, tempStrWidth;
	XCHAR tempStr[] = {'M',0};

    // free memory for the objects in the previous linked list and start new list
	GOLFree();   
	RTCCProcessEvents();				// update the global time and date strings
	
	/* ************************************* */
	/* Create Month, Day and Year Edit Boxes */ 
	/* ************************************* */
	tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);
	textHeight   = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1);

#ifndef	USE_SSD1289
	WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              (void*)&mchpIcon,               	// icon
              DTSetText,				// set text 
              navScheme);                   	// use default scheme 
#else
	WndCreate(ID_WINDOW1,       		// ID
              0,0,GetMaxX(),GetMaxY(), 	// dimension
              WND_DRAW,                	// will be dislayed after creation
              (void*)&WellLogo,               	// icon
              DTSetText,				// set text 
              navScheme);                   	// use default scheme 
#endif
              
	/* *************************** */
	// create month components
	/* *************************** */
	// months has three characters, thus we multiply by three
	textWidth = (tempStrWidth*3) +(GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_MONTH,              	// ID
              EB1STARTX,
              EBSTARTY,
              EB1STARTX+textWidth, 
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              3,						// max characters is 3
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_MO,         	// button ID 
              EB1STARTX+textWidth+1,
              EBSTARTY,
              EB1STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTDownArrow,             	// DOWN arrow as text
              altScheme);               // use alternate scheme
 #if	0             
	/* *************************** */
	// create day components
	/* *************************** */
	// day has two characters, thus we multiply by two
	textWidth = (tempStrWidth*2) +(GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_DAY,              	// ID
              EB2STARTX,
              EBSTARTY,
              EB2STARTX+textWidth,
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 3
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_DY,         	// button ID 
              EB2STARTX+textWidth+1,
              EBSTARTY,
              EB2STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTDownArrow,              // DOWN arrow as text
              altScheme);               // use alternate scheme

	/* *************************** */
	// create year components
	/* *************************** */
	// year has four characters, thus we multiply by four
	textWidth = (tempStrWidth*4) +(GOL_EMBOSS_SIZE<<1);
#ifndef	USE_SSD1289
	EbCreate(ID_EB_YEAR,              	// ID
              EB3STARTX,
              EBSTARTY,
              EB3STARTX+textWidth,
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              4,						// max characters is 4
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_YR,         	// button ID 
              EB3STARTX+textWidth+1,
              EBSTARTY,
              EB3STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTDownArrow,             	// DOWN arrow as text
              altScheme);               // use alternate scheme
#else
	EbCreate(ID_EB_YEAR,              	// ID
              EB3STARTX,
              EBSTARTY,
              EB3STARTX+textWidth,
              EBSTARTY+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              4,						// max characters is 4
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTON_YR,         	// button ID 
              EB3STARTX+textWidth+1,
              EBSTARTY,
              EB3STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTDownArrow,             	// DOWN arrow as text
              altScheme);               // use alternate scheme
              
#endif
	/* ****************************************** */
	/* Create Hour, Minute and Seconds Edit Boxes */ 
	/* ****************************************** */

	RTCCProcessEvents();				// update the time and date

	textHeight = (GetTextHeight(altScheme->pFont)<<1);
	tempStrWidth = GetTextWidth(tempStr,altScheme->pFont);

	/* *************************** */
	// create hour components
	/* *************************** */
	// items here have 2 characters each so we use 2 as multiplier
	textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);

	EbCreate(ID_EB_HOUR,              	// ID
              EB4STARTX,
              EBSTARTY2,
              EB4STARTX+textWidth, 
              EBSTARTY2+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_P,         	// button ID 
              EB4STARTX+textWidth+1,
              EBSTARTY2,
              EB4STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+(textHeight>>1),  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_M,         	// button ID 
              EB4STARTX+textWidth+1,
              EBSTARTY2+(textHeight>>1)+1,
              EB4STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

	/* *************************** */
	// create minute components
	/* *************************** */

	EbCreate(ID_EB_MINUTE,              // ID
              EB5STARTX,
              EBSTARTY2,
              EB5STARTX+textWidth, 
              EBSTARTY2+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_P,         	// button ID 
              EB5STARTX+textWidth+1,
              EBSTARTY2,
              EB5STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+(textHeight>>1),  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_M,         	// button ID 
              EB5STARTX+textWidth+1,
              EBSTARTY2+(textHeight>>1)+1,
              EB5STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

	/* *************************** */
	// create seconds components
	/* *************************** */

	EbCreate(ID_EB_SECOND,              // ID
              EB6STARTX,
              EBSTARTY2,
              EB6STARTX+textWidth, 
              EBSTARTY2+textHeight,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              2,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONSC_P,         	// button ID 
              EB6STARTX+textWidth+1,
              EBSTARTY2,
              EB6STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+(textHeight>>1),  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONSC_M,         	// button ID 
              EB6STARTX+textWidth+1,
              EBSTARTY2+(textHeight>>1)+1,
              EB6STARTX+textWidth+SELECTBTNWIDTH+1,
              EBSTARTY2+textHeight,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

	 //-----------------------------------------------------
	 // Done Button
 	
 	textHeight = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE<<1) + 5;
 	
    BtnCreate(    ID_BUTTON4,         	// button ID 
                  EB6STARTX,
                  EBSTARTY2+2*textHeight,
                  EB6STARTX+textWidth+SELECTBTNWIDTH,
                  EBSTARTY2+3*textHeight,  		   	
                  8,					// draw rectangular button	
                  BTN_DRAW,             // will be dislayed after creation
                  NULL,					// no bitmap	
                  DTDoneText,           // set text
              	  altScheme);           // use alternate scheme
#endif
	updateDateTimeEb();
}

WORD MsgUnitPDment(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
{
	
LISTBOX*   	pLb;
SLIDER*  	pSld;
OBJ_HEADER* pObjHide;
LISTITEM*	pItemSel;

    pLb = (LISTBOX*)GOLFindObject(ID_LISTBOX4);		
    pSld = (SLIDER*)GOLFindObject(ID_SLIDER4);

    switch(GetObjID(pObj)){

        case ID_LISTBOX4:
        	if (pMsg->uiEvent == EVENT_MOVE) {
	        	pMsg->uiEvent = EVENT_PRESS;			// change event for listbox
	            // Process message by default
	            LbMsgDefault(objMsg, (LISTBOX*)pObj, pMsg);
	            // Set new list box position
	            SldSetPos(pSld,LbGetCount(pLb)-LbGetFocusedItem(pLb)-1);
	            SetState(pSld, SLD_DRAW_THUMB);
	        	pMsg->uiEvent = EVENT_MOVE;				// restore event for listbox
	        }
	       	else if (pMsg->uiEvent == EVENT_PRESS) {
		       	// call the message default processing of the list box to select the item
		       	LbMsgDefault(objMsg, (LISTBOX *)pObj, pMsg);
		    }
	       	else if (pMsg->uiEvent == EVENT_RELEASE) {
		        
		        pObjHide = GOLGetList();  				// hide all the objects in the current list
		        while (pObjHide) {						// set all objects with HIDE state
	            	SetState(pObjHide, HIDE);
	            	pObjHide = pObjHide->pNxtObj;
	            }
				pItemSel = LbGetSel(pLb,NULL);			// get the selected item
				if (pItemSel != NULL) 					// process only when an item was selected
					UpdateRTCCDates(pLb);				// update the RTCC values 
    
//			    screenState = HIDE_DATE_PDMENU;			// go to hide state
	        }

	        // The message was processed
	        return 0;

        case ID_SLIDER4:
            // Process message by default
            SldMsgDefault(objMsg, (SLIDER*)pObj, pMsg);
            // Set new list box position
            if(LbGetFocusedItem(pLb) != LbGetCount(pLb)-SldGetPos(pSld)){
                LbSetFocusedItem(pLb,LbGetCount(pLb)-SldGetPos(pSld));
                SetState(pLb, LB_DRAW_ITEMS);
            }
            // The message was processed
            return 0;

        case ID_BUTTON_UNIT_UP:					// slider button up was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)-1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)+1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;

        case ID_BUTTON_UNIT_DN:					// slider button down was pressed
            if(objMsg == BTN_MSG_RELEASED){
                LbSetFocusedItem(pLb,LbGetFocusedItem(pLb)+1);
                SetState(pLb, LB_DRAW_ITEMS);
                SldSetPos(pSld,SldGetPos(pSld)-1);
                SetState(pSld, SLD_DRAW_THUMB);
            }
            return 1;
    }    
    return 1;
    
}

void ShowUnitPullDownMeny()
{
	WORD 		textWidth=0, textHeight, tempStrWidth;
	LISTBOX*    pLb;
	XCHAR*		pItems = NULL;
	XCHAR 		tempString[] = {'M',0};

	pListSaved = GOLGetList();
	GOLNewList();

	textHeight = GetTextHeight(altScheme->pFont) + (GOL_EMBOSS_SIZE << 1);
	
	// Note: pDwnBottom, pDwnTop, pDwnLeft and pDwnRight are globals that will
	// 		 be used to redraw the area where the pulldown menu covered.
	pDwnBottom = 230;
	pDwnTop    = EBSTARTY;

	tempStrWidth = GetTextWidth(tempString,altScheme->pFont);
	switch (DateItemID) {
		case ID_EB_MONTH:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)MonthItems;
			break;
		case ID_EB_DAY:
			textWidth  = (tempStrWidth*2) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB2STARTX;
			pDwnRight  = EB2STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)DayItems;
			break;
		case ID_EB_YEAR:
			textWidth  = (tempStrWidth*4) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB3STARTX;
			pDwnRight  = EB3STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)YearItems;
			break;

		case	ID_EB_UNIT:
			textWidth  = (tempStrWidth*3) + (GOL_EMBOSS_SIZE<<1);
			pDwnLeft   = EB1STARTX;
			pDwnRight  = EB1STARTX+textWidth+SELECTBTNWIDTH;
			pItems = (XCHAR*)TenUnits;
	}

	pLb = (LISTBOX *)LbCreate(
			   ID_LISTBOX1,         	// List Box ID
		       pDwnLeft,				// left
		       pDwnTop+textHeight,		// top
		       pDwnRight-SELECTBTNWIDTH,// right
		       pDwnBottom, 				// bottom dimension
		       LB_DRAW|LB_SINGLE_SEL, 	// draw after creation, single selection
		       pItems,        			// items list 
		       altScheme);
		
	SldCreate(ID_SLIDER1,              	// Slider ID
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+(textHeight<<1),  		   	
		      pDwnRight,
		      pDwnBottom-textHeight,
		      SLD_DRAW|SLD_SCROLLBAR|
		      SLD_VERTICAL,   			// vertical, draw after creation
		      LbGetCount(pLb),       	// range
		      5,                       	// page 
		      LbGetCount(pLb)-1,       	// pos
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_UP,       	// up button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnTop+textHeight,
		      pDwnRight,
		      pDwnTop+(textHeight<<1),0, 		   	
		      BTN_DRAW,                 // draw after creation
		      NULL,                    	// no bitmap
		      DTUpArrow,              	// text
		      altScheme);
		
	BtnCreate(ID_BUTTON_DATE_DN,        // down button ID 
		      pDwnRight-SELECTBTNWIDTH+1,
		      pDwnBottom-textHeight,
		      pDwnRight,
		      pDwnBottom,0,  		   	
		      BTN_DRAW,                 // draw after creation
		      NULL,                    	// no bitmap
		      DTDownArrow,              // text
		      altScheme);

}

// creates Connect to the pc demo screen
void CreateConnectToPC(){
LISTBOX*    pLb;

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(ConnectToPCStr); 			// CreatePage("List box");

		StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-10,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-20,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY)-20,          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              ConnectToPCStr, 				// "GPS", 
              altScheme);    // alt2Scheme              // use alternate scheme


}

WORD MsgToPC(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
	default:
            return 1; 							// process by default
    	}
}

void CreateResult(void)
{

	XCHAR* UnitPoit;
	GetThickness();
#ifndef SA_VERSION
	if(TestMode!=0xcc)
#endif		
	writerecord();
	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(TestResultStr);	// CreatePage("Buttons");

	//pump_pwr=0;

	if(TestMode!=0xcc)
		{
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax*10,positionay+2*positionmaxy,
              positionax*10+4*positionmaxx,positionay+6*positionmaxy,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              TestResultStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	
	if((ST_IRstatus == ST_REFUSE))
	{
		StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW|ST_CENTER_ALIGN,		// display text
		              RefuseStr,//GetdataStr, 				// "TEST", 	// text
		              failScheme);
	}
	else if(ST_IRstatus == ST_Discontinued)
		{
		StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW|ST_CENTER_ALIGN,		// display text
		              Discontinuedstr,//GetdataStr, 				// "TEST", 	// text
		              failScheme);
	        }
	else
	{
	
		redScheme->pFont=(void*)&BigArial;
				
		greenScheme->pFont=(void*)&BigArial;
		
	if((Test_Result[0]>'0') |(Test_Result[1]>'1'))//|(Test_Result[2]>='2'))// ³¬±ê
		{
	
			StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
		              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
		              Test_Result,//GetdataStr, 				// "TEST", 	// text
		              redScheme);   //   alt2Scheme             // use alternate scheme
		}
	else
		{

           StCreate(ID_STATICTEXT3,           		// ID 
                     positionax+0*positionmaxx,positionay+5*positionmaxy,
                     5*(positionax+positionmaxx),positionay+9*positionmaxy,         		// dimension
              		 ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              		 Test_Result,//GetdataStr, 				// "TEST", 	// text
              		 greenScheme);   //   alt2Scheme             // use alternate scheme
	   }


	       StCreate(ID_STATICTEXT3,           		// ID 
                     positionax+4*positionmaxx+10,positionay+5*positionmaxy+10,
                     7*(positionax+positionmaxx),positionay+7*positionmaxy+10,         		// dimension
                     ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
                     DisplayUnitStr,//GetdataStr, 				// "TEST", 	// text
                     alt5Scheme);   //   alt2Scheme             // use alternate scheme

	}

	pbtn = (OBJ_HEADER*)BtnCreate(ID_BUTTON8,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)+3,GetMaxY()-40,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX-6,GetMaxY(),         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              (void*)&printerbutton,//(void*)&PicGPS,                    	// no bitmap
              NULL,
              blackScheme);
		}
 #ifdef South_Africa_Version	
	else {

		redScheme->pFont=(void*)&FONTDEFAULT;
		greenScheme->pFont=(void*)&FONTDEFAULT;

		 if(alocholdetecet)

           			StCreate(ID_STATICTEXT3,           		// ID 
		              positionax+0*positionmaxx+30,positionay+5*positionmaxy,
		              5*(positionax+positionmaxx)+30,positionay+9*positionmaxy,         		// dimension
		              ST_DRAW|ST_CENTER_ALIGN,		// display text
		              AlocholDetectedstr,//GetdataStr, 				// "TEST", 	// text
		              redScheme);   //   alt2Scheme             // use alternate scheme

		else  			  

                   StCreate(ID_STATICTEXT3,           		// ID 
                     positionax+0*positionmaxx+30,positionay+5*positionmaxy,
                     5*(positionax+positionmaxx)+30,positionay+9*positionmaxy,         		// dimension
              		 ST_DRAW|ST_CENTER_ALIGN,		// display text
              		 NoAlocholstr,//GetdataStr, 				// "TEST", 	// text
              		 greenScheme);   //   alt2Scheme             // use alternate scheme


	}
#endif		
		
	ST_IRstatus = 0;		// ²âÊÔÄ£Ê½»Ö¸´Õý³£
}


WORD MsgResult(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;
unsigned char ik,i;
unsigned int MsgAdd;
    switch(GetObjID(pObj)){

       case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                alarmON = FALSE;
				
                inputflag = TRUE;
				ChoicePrint = FALSE;

				SPrint=FALSE;

				redScheme->pFont=(void*)&BigArial;
				
		        greenScheme->pFont=(void*)&BigArial;
				
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){

				alarmON = FALSE;
				
                             inputflag = TRUE;
							 
				ChoicePrint = FALSE;

				SPrint=FALSE;
				
				redScheme->pFont=(void*)&BigArial;
				
		             greenScheme->pFont=(void*)&BigArial;
					
	
		
				 
			      screenState = CREATE_Waittest; 		// 
			      
			      status_Press=FALSE;	
				  
	                      for(ik=0;ik<200;ik++) Flashbuff[49+ik] = 0;//
	                      
			         g_backlight_time = 3*60;// 3·ÖÖÓ²»´µÆø£¬×Ô¶¯Ìø³öÀ´
								
              
            }
            return 1; 
			
       case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED)
            {
			SPrint=TRUE;
			ChoicePrint = TRUE;
			Sprinnum = 0;
			PrintSec=0;
			PrintCount = 0;
			//Pressflag = 0xaa;
			memset(&Flashbuff[0],0,30);
			//memset(&tx1_buff[0],0,250);
			//memset(&rx1_buff[0],0,250);
			readrecord();
			for(i=0;i<30;i++)
				Printbuff[i]=(unsigned char)Flashbuff[i];
			Inputdata();
			p_n = 0;

           // StartPrint();
            Print_TestRecord();


            	}
            return 1; // process by default
            
	default:
            return 1; 							// process by default
    	}
}

void CreateUnits(){

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(UnitsStr); 	// CreatePage("Static text");

	RbCreate(ID_RADIOBUTTONStar + 7,          	// ID 
              positionax,(GetMaxY() - 270),positionmaxx,(GetMaxY() - 240),            // dimension
              RB_DRAW|RB_GROUP,//|	\
             	//RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              mg_100mlStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 8,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 270),positionax+6*positionmaxx,(GetMaxY() - 240),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_lStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme
/*
    RbCreate(ID_RADIOBUTTONStar + 9,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 240),positionax+2*positionmaxx,(GetMaxY() - 210),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_100mlStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme

     RbCreate(ID_RADIOBUTTONStar + 10,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 240),positionax+6*positionmaxx,(GetMaxY() - 210),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              g_100mlStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 11,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 210),positionax+2*positionmaxx,(GetMaxY() - 180),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              g_210lStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 12,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 210),positionax+6*positionmaxx,(GetMaxY() - 180),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              ug_100mlStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 13,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 180),positionax+2*positionmaxx,(GetMaxY() - 150),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              mg_mlStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 14,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 180),positionax+6*positionmaxx,(GetMaxY() - 150),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_mg100Str, 			   // "Center"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 15,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 150),positionax+2*positionmaxx,(GetMaxY() - 120),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_lStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme
*/
}

void CreateThickness(){

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(ThicknessStr); 	// CreatePage("Static text");

	RbCreate(ID_RADIOBUTTONStar + 7,          	// ID 
              positionax,(GetMaxY() - 270),positionmaxx,(GetMaxY() - 240),            // dimension
              RB_DRAW|RB_GROUP,//|	\
             	//RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              mg_100mlStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 8,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 270),positionax+6*positionmaxx,(GetMaxY() - 240),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_lStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 9,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 240),positionax+2*positionmaxx,(GetMaxY() - 210),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_100Str, 			   // "Right"
              alt4Scheme);              // use alternate scheme

     RbCreate(ID_RADIOBUTTONStar + 10,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 240),positionax+6*positionmaxx,(GetMaxY() - 210),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              g_100mlStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 11,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 210),positionax+2*positionmaxx,(GetMaxY() - 180),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_1000mlStr, 			   // "Center"
              alt4Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 12,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 210),positionax+6*positionmaxx,(GetMaxY() - 180),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              ug_100mlStr, 			   // "Right"
              alt4Scheme);              // use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 13,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 180),positionax+2*positionmaxx,(GetMaxY() - 150),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              mg_mlStr, 					// "Left"
              alt4Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 14,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 180),positionax+6*positionmaxx,(GetMaxY() - 150),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              mg_mg100Str, 			   // "Center"
              alt4Scheme);              // use alternate scheme
/*
    RbCreate(ID_RADIOBUTTONStar + 15,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 150),positionax+2*positionmaxx,(GetMaxY() - 120),       // dimension
              RB_DRAW,//|RB_HIDE,                 // will be dislayed after creation
              g_210lStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme
*/

	RbCreate(ID_RADIOBUTTONStar + 26,          	// ID 
              1,(GetMaxY() - 140),50,(GetMaxY() - 110),           // dimension
              RB_DRAW|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              KselectStr,//LeftStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme
/*    RbCreate(ID_RADIOBUTTONStar + 27,          	// ID 
              98,(GetMaxY() - 270),150,(GetMaxY() - 240),           // dimension
              RB_DRAW,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2050StrA, 					// "Left"
              alt5Scheme);              	// use alternate scheme
*/
    RbCreate(ID_RADIOBUTTONStar + 28,          // ID 
              positionax+3*positionmaxx,(GetMaxY() - 140),
              positionax+5*positionmaxx,(GetMaxY() - 110),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2100StrA, 			   // "Center"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 29,          // ID 
              positionax+3*positionmaxx,(GetMaxY() - 110),
              positionax+5*positionmaxx,(GetMaxY() - 80),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2150StrA, 			   // "Center"
              alt5Scheme);              // use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 30,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 110),
              positionax+3*positionmaxx,(GetMaxY() - 80),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2200StrA, 					// "Left"
              alt5Scheme);              	// use alternate scheme

/*	RbCreate(ID_RADIOBUTTONStar + 31,          	// ID 
              positionax+3*positionmaxx,(GetMaxY() - 210),
              positionax+5*positionmaxx,(GetMaxY() - 180),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2250StrA, 					// "Left"
              alt5Scheme);              	// use alternate scheme
*/
    RbCreate(ID_RADIOBUTTONStar + 32,          // ID 
             1,(GetMaxY() - 80),50,(GetMaxY() - 50),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2300StrA, 			   // "Right"
              alt5Scheme);
              
}

WORD MsgThickness(WORD objMsg, OBJ_HEADER* pObj){
GROUPBOX   *pGb;
STATICTEXT *pSt;
/*
	uch i;
	i = g_standby_time/60;
	switch(i)
		{
			case 0:
				default:
				GetObjID(pObj) = ID_RADIOBUTTON2;
				break;
			case 1:
				GetObjID(pObj) = ID_RADIOBUTTON1;
				break;
			case 2:
				GetObjID(pObj) = ID_RADIOBUTTON3;
				break;
		}
		*/	

    switch(GetObjID(pObj)){
        case ID_RADIOBUTTON1: // change aligment to left
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb,GB_RIGHT_ALIGN|GB_CENTER_ALIGN);  	// clear right and center alignment states
            SetState(pGb, GB_DRAW);                        	// set redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN|ST_CENTER_ALIGN);  	// clear right and center alignment states    
            SetState(pSt, ST_DRAW);                        	// set redraw state
            return 1; 										// process by default

        case ID_RADIOBUTTON2:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_RIGHT_ALIGN);  				// clear right alignment state
            SetState(pGb, GB_CENTER_ALIGN|GB_DRAW);  		// set center alignment and redraw states
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN);  					// clear right aligment state
            SetState(pSt, ST_DRAW|ST_CENTER_ALIGN);  		// set center alignment and redraw states
            return 1; 										// process by default

        case ID_RADIOBUTTON3:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_CENTER_ALIGN);  				// clear center alignment state
            SetState(pGb, GB_RIGHT_ALIGN|GB_DRAW);  		// set right alignment and redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_CENTER_ALIGN);  				// clear center aligment state
            SetState(pSt,ST_RIGHT_ALIGN|ST_DRAW); 			// set right alignment and redraw states
            return 1; // process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               	if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT;				// goto slider screen
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1; 										// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        default:
            return 1;
    }
}

void CreateKSetting(){

    GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(KSettingStr); 	// CreatePage("Static text");

    RbCreate(ID_RADIOBUTTONStar + 26,          	// ID 
              1,(GetMaxY() - 270),50,(GetMaxY() - 240),           // dimension
              RB_DRAW|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              KselectStr,//LeftStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme
    RbCreate(ID_RADIOBUTTONStar + 27,          	// ID 
              98,(GetMaxY() - 270),150,(GetMaxY() - 240),           // dimension
              RB_DRAW,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2050StrA, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 28,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 240),
              positionax+2*positionmaxx,(GetMaxY() - 210),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2100StrA, 			   // "Center"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 29,          // ID 
              positionax+3*positionmaxx,(GetMaxY() - 240),
              positionax+5*positionmaxx,(GetMaxY() - 210),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2150StrA, 			   // "Center"
              alt5Scheme);              // use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 30,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 210),
              positionax+3*positionmaxx,(GetMaxY() - 180),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2200StrA, 					// "Left"
              alt5Scheme);              	// use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 31,          	// ID 
              positionax+3*positionmaxx,(GetMaxY() - 210),
              positionax+5*positionmaxx,(GetMaxY() - 180),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              K2250StrA, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 32,          // ID 
             1,(GetMaxY() - 180),50,(GetMaxY() - 150),           // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2300StrA, 			   // "Right"
              alt5Scheme);              // use alternate scheme
              
    RbCreate(ID_RADIOBUTTONStar + 33,          // ID 
              positionax+3*positionmaxx,(GetMaxY() - 180),
              positionax+5*positionmaxx,(GetMaxY() - 150),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2350StrA, 			   // "Center"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 34,          // ID 
              2,(GetMaxY() - 140),80,(GetMaxY() - 110),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              K2400StrA, 			   // "Right"
              alt5Scheme);              // use alternate scheme
              
}
void MsgkSettingSel(OBJ_HEADER* pObj)
{/*
	RADIOBUTTON *pRb;
	return 0;
        	if(EEPROMReadByte(0x7F18))
        		{
        		pRb = (RADIOBUTTON*) GOLFindObject(ID_RADIOBUTTON3); // get pointer heavy load setting
	                RbSetCheck(pRb, ID_RADIOBUTTON3);		// set radio button for light to be checked
	                SetState(pRb, RB_DRAW_CHECK);
        		}*/
}

void MsgKSettingSave(OBJ_HEADER* pObj)
{/*
RADIOBUTTON *pRb;
pRb = (RADIOBUTTON*) GOLFindObject(ID_RADIOBUTTON3); // get pointer heavy load setting
if ( GetState(pRb,RB_DISABLED) )
		EEPROMWriteByte(0,0x7F18);*/

}


WORD MsgKSetting(WORD objMsg, OBJ_HEADER* pObj){
GROUPBOX   *pGb;
STATICTEXT *pSt;
/*
	uch i;
	i = g_standby_time/60;
	switch(i)
		{
			case 0:
				default:
				GetObjID(pObj) = ID_RADIOBUTTON2;
				break;
			case 1:
				GetObjID(pObj) = ID_RADIOBUTTON1;
				break;
			case 2:
				GetObjID(pObj) = ID_RADIOBUTTON3;
				break;
		}
		*/	

    switch(GetObjID(pObj)){
        case ID_RADIOBUTTON1: // change aligment to left
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb,GB_RIGHT_ALIGN|GB_CENTER_ALIGN);  	// clear right and center alignment states
            SetState(pGb, GB_DRAW);                        	// set redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN|ST_CENTER_ALIGN);  	// clear right and center alignment states    
            SetState(pSt, ST_DRAW);                        	// set redraw state
            return 1; 										// process by default

        case ID_RADIOBUTTON2:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_RIGHT_ALIGN);  				// clear right alignment state
            SetState(pGb, GB_CENTER_ALIGN|GB_DRAW);  		// set center alignment and redraw states
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN);  					// clear right aligment state
            SetState(pSt, ST_DRAW|ST_CENTER_ALIGN);  		// set center alignment and redraw states
            return 1; 										// process by default

        case ID_RADIOBUTTON3:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_CENTER_ALIGN);  				// clear center alignment state
            SetState(pGb, GB_RIGHT_ALIGN|GB_DRAW);  		// set right alignment and redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_CENTER_ALIGN);  				// clear center aligment state
            SetState(pSt,ST_RIGHT_ALIGN|ST_DRAW); 			// set right alignment and redraw states
            return 1; // process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT;				// goto slider screen
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1; 										// process by default
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        default:
            return 1;
    }
}

void GetStatus(CHECKBOX* pCb)
{
	BOOL pCbstate = FALSE;
	WORD Address;
	Address = (0x7F00 + pCb->ID - ID_CHECKBOX1 + 1);//µÃµ½²»Í¬¸´Ñ¡¿ðµÄµØÖ·
	if(pCb->ID == ID_CHECKBOX1)
		Address--;
	if(EEPROMReadByte(Address))//Èç¹û·µ»ØÖµ·ÇÁã£¬Ôò±íÊ¾Ñ¡ÖÐ
		{
			SetState(pCb, CB_CHECKED|CB_DRAW_CHECK); // Set checked and redraw
			pCbstate = TRUE;
		}
	else
		{
			ClrState(pCb, CB_CHECKED);        // Reset check   
            		SetState(pCb, CB_DRAW_CHECK);     // Redraw
            		pCbstate = FALSE;
		}
	EEPROMWriteByte(pCbstate, Address);// ±£´æ×´Ì¬
		
}

void CreatePrintSetting(){

    OBJ_HEADER* obj;
	
    GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(PrintSettingStr); 	// CreatePage("Checkboxes");


/*
  StCreate(ID_STATICTEXT3,           		// ID 
              10,90,
              100,125,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              ZeroStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	
          
    EbCreate(ID_EB_HOUR,              	// ID
              130,
              85,
              160, 
              115,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              1,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_P,         	// button ID 
              165,
              77,
              185,
              97 ,		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONHR_M,         	// button ID 
              165,
              104,
              185,
              124,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,      			// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme

              */
       
  StCreate(ID_STATICTEXT3,           		// ID 
              10,110,
              100,145,         		// dimension
              ST_DRAW,//|ST_RIGHT_ALIGN,//ST_CENTER_ALIGN,		// display text
              BeyondZeroStr, 				// "TEST", 	// text
              alt5Scheme);                   // use alternate scheme	

	EbCreate(ID_EB_MINUTE,              // ID
              130,
              105,
              160, 
              135,
              EB_DRAW, 					// will be dislayed after creation
              NULL,
              1,						// max characters is 2
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_P,         	// button ID 
              165,
              97,
              185,
              117,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTPlusSym,              	// plus symbol as text
              altScheme);               // use alternate scheme

    BtnCreate(ID_BUTTONMN_M,         	// button ID 
              165,
              124,
              185,
              144,  		   	
              0,					   	// draw rectangular button	
              BTN_DRAW,               	// will be dislayed after creation
              NULL,					   	// no bitmap	
              DTMinusSym,               // minus symbols as text
              altScheme);               // use alternate scheme          


	CbCreate(ID_CHECKBOX7,             	// ID 
             positionax,180,positionmaxx,215,// positionax,230,positionmaxx,265,         	// dimension
              CB_DRAW,      			// will be dislayed and checked after creation
              AutoPrintStr, 			// "Text Bottom"
              alt5Scheme);              	// alternative GOL scheme 

	CbCreate(ID_CHECKBOX6,             	// ID 
             150,180,235,215,// positionax,230,positionmaxx,265,         	// dimension
              CB_DRAW,      			// will be dislayed and checked after creation
              DemarcateStr, 			// "Text Bottom"
              alt5Scheme);              	// alternative GOL scheme 

BnumberStr[0]= EEPROMReadByte(PrintSelectAddress);
UnumberStr[0]= EEPROMReadByte(PrintSelectAddress+1);
if(BnumberStr[0]==0xff||BnumberStr[0]==0)
	BnumberStr[0]=0x30;
EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
if(UnumberStr[0]==0xff||UnumberStr[0]==0)
	UnumberStr[0]=0x30;
EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);

/*
	obj = GOLFindObject(ID_EB_HOUR);
	EbSetText((EDITBOX*)obj, BnumberStr);
	SetState(obj, EB_DRAW);
*/	
	obj = GOLFindObject(ID_EB_MINUTE);
	EbSetText((EDITBOX*)obj, UnumberStr);
	SetState(obj, EB_DRAW);

}

WORD MsgPrintSetting(WORD objMsg, OBJ_HEADER* pObj){

OBJ_HEADER* obj;
switch(GetObjID(pObj)){


       case ID_BUTTONHR_P:  						// increment hour value
		    if(objMsg == BTN_MSG_PRESSED){
				BnumberStr[0]++;
				if(BnumberStr[0]>=0x35)
					BnumberStr[0]=0x35;
			//	EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
											
			}
			break;
        case ID_BUTTONHR_M:							// decrement hour value
			if(objMsg == BTN_MSG_PRESSED){
				BnumberStr[0]--;
				if(BnumberStr[0]<=0x2F)
					BnumberStr[0]=0x30;
			//	EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
			}
			break;
        case ID_BUTTONMN_P:  						// increment minute value
        	if(objMsg == BTN_MSG_PRESSED){
				UnumberStr[0] ++;
				if(UnumberStr[0]>=0x35)
					UnumberStr[0]=0x35;
			//	EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
			}
			break;
        case ID_BUTTONMN_M:							// decrement minute value
        	if(objMsg == BTN_MSG_PRESSED){
				UnumberStr[0] --;
				if(UnumberStr[0]<=0x2F)
					UnumberStr[0]=0x30;
			//	EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
			}
			break;

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){

				if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				
                if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
            }
            return 1; 										// process by default

		case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// 
            }
            return 1;

        default:
            return 1;
 

}
/*
BnumberStr[0]= EEPROMReadByte(PrintSelectAddress);
UnumberStr[0]= EEPROMReadByte(PrintSelectAddress+1);
if(BnumberStr[0]==0xff||BnumberStr[0]==0)
	BnumberStr[0]=0x30;
EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
if(UnumberStr[0]==0xff||UnumberStr[0]==0)
	UnumberStr[0]=0x30;
EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
*/

	obj = GOLFindObject(ID_EB_HOUR);
	EbSetText((EDITBOX*)obj, BnumberStr);
	SetState(obj, EB_DRAW);
	
	obj = GOLFindObject(ID_EB_MINUTE);
	EbSetText((EDITBOX*)obj, UnumberStr);
	SetState(obj, EB_DRAW);
	
	EEPROMWriteByte(BnumberStr[0], PrintSelectAddress);
	EEPROMWriteByte(UnumberStr[0], PrintSelectAddress+1);
}




void CreateTestSetting(){

}
WORD MsgTestSetting(WORD objMsg, OBJ_HEADER* pObj){

}

void CreateCalibrate()
{	// Ð£×¼

 GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(CalibrationPeriodStr); 	// CreatePage("Checkboxes");


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,50,
              3*positionmaxx,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              TestMthStr, 				// "TEST", 	// text
              altScheme);        //  mainScheme         // use alternate scheme

      StCreate(ID_STATICTEXT3,           		// ID 
              positionax+5*positionmaxx,50,
              positionax+7*positionmaxx,80,         		// dimension
              ST_DRAW,//|ST_CENTER_ALIGN,		// display text
              TestNumberStr, 				// "TEST", 	// text
              altScheme);         //  mainScheme        // use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 18,          	// ID 
              positionax,(GetMaxY() - 240),positionmaxx,(GetMaxY() - 210),           // dimension
              RB_DRAW|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              ThreeMthStr, 					// "Left"
              altScheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 19,          // ID 
              positionax,(GetMaxY() - 200),positionmaxx,(GetMaxY() - 170),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              SixMthStr, 			   // "Center"
              altScheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 20,          // ID 
              2,(GetMaxY() - 160),80,(GetMaxY() - 130),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              NineMthStr, 			   // "Right"
              altScheme);              // use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 21,          	// ID 
              positionax,(GetMaxY() - 120),positionmaxx,(GetMaxY() - 90),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              ElevenMthStr, 					// "Left"
              altScheme);              	// use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 22,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 240),positionax+6*positionmaxx,(GetMaxY() - 210),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              OneNumStr, 			   // "Center"
              altScheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 23,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 200),positionax+6*positionmaxx,(GetMaxY() - 170),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeNumStr, 			   // "Center"
              altScheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 24,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 160),positionax+6*positionmaxx,(GetMaxY() - 130),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineNumStr, 			   // "Right"
              altScheme);              // use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 25,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 120),positionax+6*positionmaxx,(GetMaxY() - 90),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              SevenNumStr, 					// "Left"
              altScheme);              

 	RbCreate(Unlimited_RA,          	
              120,(GetMaxY() - 80),237,(GetMaxY() - 50),           // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              UnlimitedStr, 					// "Left"
              altScheme); 
	
	CbCreate(ID_CHECKBOX8,             	// ID 
              positionax+3,235,positionmaxx-2,260,         	// dimension
              CB_DRAW,      			// will be dislayed and checked after creation
              LockStr, 			// "Text Bottom"
              alt5Scheme);              	// alternative GOL scheme              

}

WORD MsgCalibrate(WORD objMsg, OBJ_HEADER* pObj){

switch(GetObjID(pObj)){

        case ID_CHECKBOX1:
        	if (objMsg == CB_MSG_CHECKED) {
				SetState(pGenObj, 
						 BTN_TEXTLEFT| 
						 BTN_DRAW);        			// set align left and redraw button	
			} else {
				ClrState(pGenObj, BTN_TEXTLEFT);	// clear all text alignment	
				SetState(pGenObj, BTN_DRAW);       	// set align left and redraw button	
			}
            return 1; 								// process by default

        case ID_CHECKBOX2:
        	if (objMsg == CB_MSG_CHECKED) {
				SetState(pGenObj, 
						 BTN_TEXTBOTTOM| 
						 BTN_DRAW);        			// set align bottom and redraw button	
			} else {
				ClrState(pGenObj, BTN_TEXTBOTTOM);	// clear all text alignment	
				SetState(pGenObj, BTN_DRAW);       	// set align left and redraw button	
			}
            return 1; 								// process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_Master; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_Master;  		// goto round buttons screen
            }
            return 1; 								// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
            
        default:
            return 1; 								// process by default
    }
}

CreateCalLock(){

GOLFree();

CreatePage(AdjustStr); 	// CreatePage("Checkboxes");

redScheme->pFont=(void*)&GOLFontDefault;

StCreate(ID_STATICTEXT3,           		// ID 
         20,100,//positionax=2
         200,200,  // positionmaxx=32      		// dimension
         ST_DRAW|ST_CENTER_ALIGN,		// display text
         Calibrationneed, 				// "TEST", 	// text
         redScheme);        //  mainScheme         // use alternate scheme

}
WORD MsgCalLock(WORD objMsg, OBJ_HEADER* pObj){

 
switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				redScheme->pFont = (void*)&BigArial;
                screenState = CREATE_MAIN; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				redScheme->pFont = (void*)&BigArial;
                screenState = CREATE_MAIN;  		// goto round buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
				redScheme->pFont = (void*)&BigArial;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;


}

}
void CreateDemarcate(){	// ±ê¶¨

 GOLFree();   // free memory for the objects in the previous linked list and start new list

//9
/*       EEPROMWriteWord(0x008f,0x6000);//-2ÎÂ¶È
       EEPROMWriteWord(0x0cdd,0x600e);//-2 AD

       EEPROMWriteWord(0x00ad,0x6002);//5 tem
       EEPROMWriteWord(0x0d77,0x6010);//5 AD

       EEPROMWriteWord(0x00cb,0x6004);//15 tem
       EEPROMWriteWord(0x0fb9,0x6012);//15 AD

       EEPROMWriteWord(0x10d9,0x6014);//22 AD
       EEPROMWriteWord(0x10d9,0x6016);//28 AD

       EEPROMWriteWord(0x0105,0x600a);//35 tem
       EEPROMWriteWord(0x11ba,0x6018);//35 AD

       EEPROMWriteWord(0x0124,0x600c);//45 tem
       EEPROMWriteWord(0x10a3,0x601a);//45 AD

       EEPROMWriteWord(0x0320,0x6020);//Mark  cons
       EEPROMWriteWord(0x1104,0x6022);//mark AD 
         */
	CreatePage(AdjustStr); 	// CreatePage("Checkboxes");
//	pump_pwr=1;// ±Ã¿ªÊ¼³äµç

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,70,//positionax=2
              2*positionmaxx,100,  // positionmaxx=32      		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),70,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,140,
              2*positionmaxx,170,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),140,
              4*positionmaxx,170,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,215,
              2*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),215,
              4*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
/*
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,158,
              2*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),158,
              4*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,189,
              2*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),189,
              4*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD9, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,220,
              2*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD10, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),220,
              4*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD11, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,250,
              2*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD12, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),250,
              4*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD13, 				// "TEST", 	// text
              alt5Scheme);        //  mainScheme         // use alternate scheme
*/

	BtnCreate(ID_BUTTON1,             	// button ID 
              160,60,
              204,100,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//,(void*)&Picscreen,                    	// no bitmap
              T1Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

    BtnCreate(ID_BUTTON2,             	// button ID 
              160,130,
              204,170,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T2Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

     BtnCreate(ID_BUTTON3,             	// button ID 
              160,205,
              204,245,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T3Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

/*
     BtnCreate(ID_BUTTON4,             	// button ID 
              5*(positionax+positionmaxx)-15,positionay+3*positionsizey+positionax,
              6*(positionax+positionmaxx)-15,positionay+4*positionsizey,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T4Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme

     BtnCreate(ID_BUTTON5,             	// button ID 
              5*(positionax+positionmaxx)-15,positionay+4*positionsizey+positionax,
              6*(positionax+positionmaxx)-15,positionay+5*positionsizey,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T5Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme
    */
/*	BtnCreate(ID_BUTTON6,             	// button ID 
              5*(positionax+positionmaxx),positionay+5*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+6*positionsizey,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T6Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme

    BtnCreate(ID_BUTTON7,             	// button ID 
              5*(positionax+positionmaxx),positionay+6*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+7*positionsizey+positionax,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T7Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme
    BtnCreate(ID_BUTTON8,             	// button ID 
              5*(positionax+positionmaxx)+21,positionay+3*positionsizey+positionax,
              6*(positionax+positionmaxx)+21,positionay+5*positionsizey,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T8Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme          


*/

}
void CreateDemarcate2(){	// ±ê¶¨

 GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(AdjustStr); 	// CreatePage("Checkboxes");
//	pump_pwr=1;// ±Ã¿ªÊ¼³äµç

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,90,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,90,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD9, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,145,
              2*positionmaxx,175,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD10, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),145,
              4*positionmaxx,175,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD11, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,215,
              2*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD12, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),215,
              4*positionmaxx,245,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD13, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	
	BtnCreate(ID_BUTTON6,             	// button ID 
              160,135,
              204,175,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T6Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme

    BtnCreate(ID_BUTTON7,             	// button ID 
              160,205,
              204,245,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T7Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme
    BtnCreate(ID_BUTTON8,             	// button ID 
              160,60,
              204,110,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              T8Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              alt5Scheme);        // yellowScheme  	// use alternate scheme          




}

WORD MsgDemarcate(WORD objMsg, OBJ_HEADER* pObj){


int i;

switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_DEMARCATE2; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT;  		// goto round buttons screen
            }
            return 1; 								// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
	case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){


			   Adj_tem=Adc_Count(7,5);
			   Adj_adc=caiyang();
               
               //DISICNT = 0x0000; 

               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                delay_ms(10);
                EEPROMWriteWord(Adj_tem,0x6000);//-5 ÎÂ¶È
                EEPROMWriteWord(Adj_adc,0x600e);//-5  AD

                Hexshow(DemarcateAD,Adj_tem);
				Hexshow(DemarcateAD1,Adj_adc);
	
                screenState = CREATE_Waitdem;
              //  prepare_and_Charge();
                //	}
            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
                prepare_and_Charge();


				Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();

				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6002);// 5 tem
                EEPROMWriteWord(Adj_adc,0x6010);//5  AD

				Hexshow(DemarcateAD2,Adj_tem);
				Hexshow(DemarcateAD3,Adj_adc);


                screenState = CREATE_Waitdem;

                
            	}
            return 1; 							// process by default
            
        case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){ 		// change text and scheme
                prepare_and_Charge();

				Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();

				delay_ms(10);
                //if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6004);//15  tem
                EEPROMWriteWord(Adj_adc,0x6012);//15  AD

				Hexshow(DemarcateAD4,Adj_tem);
				Hexshow(DemarcateAD5,Adj_adc);


                screenState = CREATE_Waitdem; 


			//	 prepare_and_Charge();
    
           // }
        		}
            return 1;  							// process by default
/*
        case ID_BUTTON4:
		if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
		{       
		        prepare_and_Charge();

                Adj_tem=Adc_Count(7,5);
				//Adj_adc=caiyang();

				delay_ms(10);
                //if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6006);
                //EEPROMWriteWord(Adj_adc,0x6014);
                //nAdc=Adj_adc;

				Hexshow(DemarcateAD6,Adj_tem);
				//Hexshow(DemarcateAD3,Adj_adc);

               screenState = CREATE_DEMARCATE;

            }
            return 1;

        case ID_BUTTON5:
        	if(objMsg == BTN_MSG_RELEASED)// changed by Spring.chen
        	{   
        	    prepare_and_Charge();

                Adj_tem=Adc_Count(7,5);
				//Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6008);
                //EEPROMWriteWord(Adj_adc,0x6016);
                //nAdc=Adj_adc;

				Hexshow(DemarcateAD8,Adj_tem);
				//Hexshow(DemarcateAD3,Adj_adc);

               screenState = CREATE_DEMARCATE;
			 //  prepare_and_Charge();
	        //}
        		}
		    return 1; 							// Do not process by default

        case ID_BUTTON6:
            if(objMsg == BTN_MSG_RELEASED){


                prepare_and_Charge();


                Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();


				delay_ms(10);
                //if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x600a);
                EEPROMWriteWord(Adj_adc,0x6018);

                Hexshow(DemarcateAD10,Adj_tem);
				Hexshow(DemarcateAD11,Adj_adc);


               screenState = CREATE_DEMARCATE;
			  // prepare_and_Charge();
           // }
            	}
            return 1; 							// process by default

	case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){

                prepare_and_Charge();

                Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x600c);
                EEPROMWriteWord(Adj_adc,0x601a);

				Hexshow(DemarcateAD12,Adj_tem);
				Hexshow(DemarcateAD13,Adj_adc);


               screenState = CREATE_DEMARCATE;
			  // prepare_and_Charge();
            //}
            	}
            return 1;
   case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){

                prepare_and_Charge();

                //Adj_tem=ADC(7);
				Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_adc,0x6016);
                EEPROMWriteWord(Adj_adc,0x6014);



                Hexshow(DemarcateAD7,Adj_tem);
				Hexshow(DemarcateAD9,Adj_adc);

               screenState = CREATE_DEMARCATE;
			  // prepare_and_Charge();
            //}
            	}
            return 1;			
*/
        default:
            return 1; 								// process by default
    }
}
WORD MsgDemarcate2(WORD objMsg, OBJ_HEADER* pObj){


int i;

switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_AGENT; 	// goto radio buttons screen
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_DEMARCATE;  		// goto round buttons screen
            }
            return 1; 								// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
	 case ID_BUTTON4:
		if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
		{       
		        prepare_and_Charge();

                Adj_tem=0x00df;//Adc_Count(7,5);
				//Adj_adc=caiyang();

				delay_ms(10);
                //if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6006);
                //EEPROMWriteWord(Adj_adc,0x6014);
                //nAdc=Adj_adc;

				Hexshow(DemarcateAD6,Adj_tem);
				//Hexshow(DemarcateAD3,Adj_adc);

               screenState = CREATE_DEMARCATE2;

            }
            return 1;

        case ID_BUTTON5:
        	if(objMsg == BTN_MSG_RELEASED)// changed by Spring.chen
        	{   
        	    prepare_and_Charge();

                Adj_tem=0x00F3;//Adc_Count(7,5);
				//Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x6008);
                //EEPROMWriteWord(Adj_adc,0x6016);
                //nAdc=Adj_adc;

				Hexshow(DemarcateAD8,Adj_tem);
				//Hexshow(DemarcateAD3,Adj_adc);

               screenState = CREATE_DEMARCATE2;
			 //  prepare_and_Charge();
	        //}
        		}
		    return 1; 							// Do not process by default


        case ID_BUTTON6:
            if(objMsg == BTN_MSG_RELEASED){


                prepare_and_Charge();


                Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();


				delay_ms(10);
                //if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x600a);//35  tem
                EEPROMWriteWord(Adj_adc,0x6018);//35   AD

                Hexshow(DemarcateAD10,Adj_tem);
				Hexshow(DemarcateAD11,Adj_adc);

				
               screenState = CREATE_DEMARCATE2;
			  // prepare_and_Charge();
           // }
            	}
            return 1; 							// process by default

	case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){

                prepare_and_Charge();

                Adj_tem=Adc_Count(7,5);
				Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_tem,0x600c);//45  tem
                EEPROMWriteWord(Adj_adc,0x601a);//45   AD

				Hexshow(DemarcateAD12,Adj_tem);
				Hexshow(DemarcateAD13,Adj_adc);


               screenState = CREATE_DEMARCATE2;
			  // prepare_and_Charge();
            //}
            	}
            return 1;
   case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){

                prepare_and_Charge();

                //Adj_tem=ADC(7);
				Adj_adc=caiyang();


				delay_ms(10);
               // if((Adj_adc>=0x0010)&&(Adj_adc < 0x0fff)){
                EEPROMWriteWord(Adj_adc,0x6016);
                EEPROMWriteWord(Adj_adc,0x6014);
                EEPROMWriteWord(0x00df,0x6006);
                EEPROMWriteWord(0x00f3,0x6008);


                Hexshow(DemarcateAD7,Adj_tem);
				Hexshow(DemarcateAD9,Adj_adc);

               screenState = CREATE_DEMARCATE2;
			  // prepare_and_Charge();
            //}
            	}
            return 1;			

        default:
            return 1; 								// process by default
    }
}

void CreateMark(){	// ±ê¶¨

 //pump_pwr=1;
 GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(DemarcateStr); 	// CreatePage("Checkboxes");
//	pump_pwr=1;// ±Ã³äµç

   // prepare_and_Charge();

	StCreate(ID_STATICTEXT3,            
              positionax-2,60,
              3*positionmaxx,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              MarkconsStr, 				// "TEST", 	// text
              mainScheme);        //alt5Scheme  mainScheme         // use alternate scheme
	StCreate(ID_STATICTEXT3,           		// ID 
              3*positionmaxx,60,
              5*positionmaxx-10,80,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              consbuffStr, 				// "TEST", 	// text
              mainScheme);
	StCreate(ID_STATICTEXT3,           		// ID 
              5*positionmaxx,60,
              8*positionmaxx-3,80,         		// dimension
              ST_DRAW,		// display text
              mg_100mlStr, 				// "TEST", 	// text
              mainScheme);	//alt5Scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax-2,100,
              3*positionmaxx,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              MarktemStr, 				// "TEST", 	// text
              mainScheme); // alt5Scheme          // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              3*positionmaxx,100,
              5*positionmaxx-10,120,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Marktem,
              mainScheme);    // "TEST", 	// text
 
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax-2,140,
              3*positionmaxx-17,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              MarkvaluStr, 				// "TEST", 	// text
              mainScheme);        // alt5Scheme mainScheme         // use alternate scheme
              
             
	StCreate(ID_STATICTEXT3,           		// ID 
              3*positionmaxx,140,
              6*positionmaxx-10,160,         		// dimension
              ST_DRAW,		// display text
              MarkAD2, 				// "TEST", 	// text
              mainScheme);


	BtnCreate(ID_BUTTON1,             	// button ID 
              2*(positionax+positionmaxx),200,
              5*(positionax+positionmaxx),240,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              NULL,//,(void*)&Picscreen,                    	// no bitmap
              DemarcateStr,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme
 /*             
    BtnCreate(ID_BUTTON2,             	// button ID 
              5*(positionax+positionmaxx),positionay+2*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+3*positionsizey,           	// dimension
              0,					   	// set radius 
              BTN_DRAW|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Mark2Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme
    BtnCreate(ID_BUTTON3,             	// button ID 
              5*(positionax+positionmaxx),positionay+4*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+5*positionsizey,          	// dimension
              0,					   	// set radius 
              BTN_DRAW|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Mark3Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme  
    BtnCreate(ID_BUTTON4,             	// button ID 
              5*(positionax+positionmaxx),positionay+6*positionsizey+positionax,
              6*(positionax+positionmaxx),positionay+7*positionsizey+positionax,          	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,//(void*)&Picscreen,                    	// no bitmap
              Mark4Str,//,ScreenStr//LowStr, 					// "LO",       	// text
              mainScheme);        // yellowScheme  	// use alternate scheme           

*/
}



	
WORD MsgMark(WORD objMsg, OBJ_HEADER* pObj){



	
switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				// goto slider screen
                else if(Settingposition==0xbb)
					screenState = CREATE_Master;
				//pump_pwr=0;
            }
            return 1; 								// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_INPUT;  		// goto round buttons screen
                //pump_pwr=0;
			}
            return 1; 								// process by default

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
//            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
                //pump_pwr=0;
            }
            return 1;






	case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
        
            MarkAdc=caiyang();

            //MarkTem=Adc_Count(7,5);

            Marktemporary=MarkAdc;
            EEPROMWriteWord(MarkAdc,0x6022);
            Hexshow(MarkAD2,MarkAdc);
	     SaveAdjust();		
             screenState = CREATE_Wait;//CREATE_Mark;
              
            }

           return 1; 
	}

}			

void  CreateWait()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(WaitStr);	//DeleteStr CreatePage("Blow Please");
    StCreate(ID_STATICTEXT3,           		// ID 
              40,140,
              200,180,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              WaitStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme
          	// use alternate scheme

}
WORD  MsgWait(WORD objMsg, OBJ_HEADER* pObj)
{

  
 
prepare_and_Charge();


 //screenState =CREATE_Mark;
  
}
void  CreatePowerOff()
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(PowerOffStr);	// CreatePage("Blow Please");
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX-8,2*MAINSTARTY+30,
              MAINSTARTX+4*MAINCHARSIZE-8,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+30,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              PowerOffStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme
/*	
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX,2*MAINSTARTY,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              LongitudeStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),2*MAINSTARTY,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              sLongitude, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme
              
	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              LatitudeStr, 				// "TEST", 	// text
              alt5Scheme); 

	StCreate(ID_STATICTEXT3,           		// ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+3*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              sLatitude, 				// "GPS", 	// text
              alt5Scheme);    // alt2Scheme              // use alternate scheme
*/
}

WORD MsgPowerOFF(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
 //              screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
  //              screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

        default:
            return 1; 							// process by default
    }
}

void  CreateRefuse()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage(RefuseStr);	// CreatePage("Blow Please");
	//pump_pwr=0;
	BtnCreate(ID_BUTTON1,             	// button ID 
               40,185,(GetMaxX()-40),235,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              TestagainStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
        BtnCreate(ID_BUTTON2,             	// button ID 
               40,105,(GetMaxX()-40),155,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              RefuseStr,//LowStr, 					// "LO",       	// text
              altScheme);           	// use alternate scheme
}

WORD MsgRefuse(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
            
	case  ID_BUTTON1:
		 if(objMsg == BTN_MSG_RELEASED){
		 	ST_IRstatus = 0;
			screenState = CREATE_BLOW;     	// goto radio buttons screen
			g_backlight_time = 3*60;// 3·ÖÖÓ²»´µÆø£¬×Ô¶¯Ìø³öÀ´
            }
            return 1;
        case  ID_BUTTON2:
		 if(objMsg == BTN_MSG_RELEASED){
//                Manual_Analyze();
                ST_IRstatus = ST_REFUSE;
                screenState = CREATE_RESULT; 	// goto radio buttons screen
            }
            return 1;
        default:
            return 1; 							// process by default
    }
}

void  CreateBlowPress()
{
	GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(BlowTimePressStr); 	// CreatePage("Static text");

	StCreate(ID_STATICTEXT3,          	// ID 
              positionax,(GetMaxY() - 270),3*positionmaxx,(GetMaxY() - 240),            // dimension
              ST_DRAW|ST_CENTER_ALIGN, 				// will be dislayed and checked after creation
                                       	// first button in the group
              BlowTimeStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

 	RbCreate(ID_RADIOBUTTONStar + 35,          	// ID 
              positionax,(GetMaxY() - 240),2*positionmaxx,(GetMaxY() - 210),            // dimension
              RB_DRAW|RB_GROUP,//|	\
             	//RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              TwoFineSecStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme


    RbCreate(ID_RADIOBUTTONStar + 36,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 210),positionax+2*positionmaxx,(GetMaxY() - 180),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeSecStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 37,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 180),positionax+2*positionmaxx,(GetMaxY() - 150),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeFineSecStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme

	RbCreate(ID_RADIOBUTTONStar + 38,          	// ID 
              positionax+0*positionmaxx,(GetMaxY() - 150),positionax+2*positionmaxx,(GetMaxY() - 120),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              FourSecStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 39,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 120),positionax+2*positionmaxx,(GetMaxY() - 90),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              FourFineSecStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme
 
    RbCreate(ID_RADIOBUTTONStar + 40,          // ID 
              positionax+0*positionmaxx,(GetMaxY() - 90),positionax+2*positionmaxx,(GetMaxY() - 60),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineSecStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme


	// for the Press setting
   StCreate(ID_STATICTEXT3,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 270),positionax+7*positionmaxx,(GetMaxY() - 240),         // dimension
              ST_DRAW|ST_CENTER_ALIGN,           // will be dislayed after creation
              BlowPressStr, 			   // "Center"
              alt5Scheme); 

    RbCreate(ID_RADIOBUTTONStar + 41,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 240),positionax+6*positionmaxx,(GetMaxY() - 210),         // dimension
              RB_DRAW,                 // will be dislayed after creation
              OneStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme

     RbCreate(ID_RADIOBUTTONStar + 42,          	// ID 
              positionax+4*positionmaxx,(GetMaxY() - 210),positionax+6*positionmaxx,(GetMaxY() - 180),             // dimension
              RB_DRAW,//|RB_GROUP,//|	\
              //RB_CHECKED, 				// will be dislayed and checked after creation
                                       	// first button in the group
              TwoStr, 					// "Left"
              alt5Scheme);              	// use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 43,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 180),positionax+6*positionmaxx,(GetMaxY() - 150),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              ThreeStr, 			   // "Right"
              alt5Scheme);              // use alternate scheme

    RbCreate(ID_RADIOBUTTONStar + 44,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 150),positionax+6*positionmaxx,(GetMaxY() - 120),        // dimension
              RB_DRAW,                 // will be dislayed after creation
              FourStr, 			   // "Center"
              alt5Scheme);              // use alternate scheme
   RbCreate(ID_RADIOBUTTONStar + 45,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 120),positionax+6*positionmaxx,(GetMaxY() - 90),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              FineStr, 			   // "Right"
              alt5Scheme);

    RbCreate(ID_RADIOBUTTONStar + 46,          // ID 
              positionax+4*positionmaxx,(GetMaxY() - 90),positionax+6*positionmaxx,(GetMaxY() - 60),       // dimension
              RB_DRAW,                 // will be dislayed after creation
              SixStr, 			   // "Right"
              alt5Scheme);

}
WORD MsgBlowPress(WORD objMsg, OBJ_HEADER* pObj)
{
	GROUPBOX   *pGb;
	STATICTEXT *pSt;

    switch(GetObjID(pObj)){
        case ID_RADIOBUTTON1: // change aligment to left
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb,GB_RIGHT_ALIGN|GB_CENTER_ALIGN);  	// clear right and center alignment states
            SetState(pGb, GB_DRAW);                        	// set redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN|ST_CENTER_ALIGN);  	// clear right and center alignment states    
            SetState(pSt, ST_DRAW);                        	// set redraw state
            return 1; 										// process by default

        case ID_RADIOBUTTON2:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_RIGHT_ALIGN);  				// clear right alignment state
            SetState(pGb, GB_CENTER_ALIGN|GB_DRAW);  		// set center alignment and redraw states
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_RIGHT_ALIGN);  					// clear right aligment state
            SetState(pSt, ST_DRAW|ST_CENTER_ALIGN);  		// set center alignment and redraw states
            return 1; 										// process by default

        case ID_RADIOBUTTON3:
            pGb = (GROUPBOX*) GOLFindObject(ID_GROUPBOX1); 	// get pointer to group box
            ClrState(pGb, GB_CENTER_ALIGN);  				// clear center alignment state
            SetState(pGb, GB_RIGHT_ALIGN|GB_DRAW);  		// set right alignment and redraw state
            pSt = (STATICTEXT*) GOLFindObject(ID_STATICTEXT3); // get pointer to static text
            ClrState(pSt,ST_CENTER_ALIGN);  				// clear center aligment state
            SetState(pSt,ST_RIGHT_ALIGN|ST_DRAW); 			// set right alignment and redraw states
            return 1; // process by default

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT;				// goto slider screen
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1; 										// process by default
	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
        default:
            return 1;
    }

}
void  CreateDebug()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(DebugStr);	//DeleteStr CreatePage("Blow Please");
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,60,
              192,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,90,
              192,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,125,
              2*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),125,
              4*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,125,
              192,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,158,
              2*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD9, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),158,
              4*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD10, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,158,
              192,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD11, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

    StCreate(ID_STATICTEXT3,           		// ID 
              positionax,189,
              2*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD12, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),189,
              4*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD13, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,189,
              192,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD14, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,220,
              2*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD15, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),220,
              4*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD16, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,220,
              192,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD17, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

StCreate(ID_STATICTEXT3,           		// ID 
              positionax,250,
              2*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD18, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
   StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),250,
              4*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD19, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate schemee
   StCreate(ID_STATICTEXT3,           		// ID 
              132,250,
              192,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD20, 				// "TEST", 	// text
              mainScheme);  

}
void  CreateDebug2()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(DebugStr);	//DeleteStr CreatePage("Blow Please");
    StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,60,
              192,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,90,
              192,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,125,
              2*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

    StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),125,
              4*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
    StCreate(ID_STATICTEXT3,           		// ID 
              132,125,
              192,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


}
void  CreateDemarcateshow()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list
	CreatePage0(DemarcateshowStr);	//DeleteStr CreatePage("Blow Please");
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,60,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),60,
              4*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD1, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme


	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,90,
              2*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD2, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),90,
              4*positionmaxx,130,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD3, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,125,
              2*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD4, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),125,
              4*positionmaxx,160,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD5, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,158,
              2*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD6, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),158,
              4*positionmaxx,190,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD7, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,189,
              2*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD8, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),189,
              4*positionmaxx,220,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD9, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,220,
              2*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD10, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),220,
              4*positionmaxx,250,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD11, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme

	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,250,
              2*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD12, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate scheme
        StCreate(ID_STATICTEXT3,           		// ID 
              2*(positionax+positionmaxx),250,
              4*positionmaxx,280,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              DemarcateAD13, 				// "TEST", 	// text
              mainScheme);        //  mainScheme         // use alternate schemee

}

	static char Itemchar = 0;

void SaveReadItem(BOOL SaveRead)
{
	DWORD ItemAddress;
	BYTE* Itempoint;
	unsigned char i;
	for(i= 0;i<16;i++)// ×î´óÖ»¸ø16¸ö×Ö·ûItemMax
	{
		if(SaveRead)
	       {
//	            	EEPROMWriteByte(Itempoint, ItemAddress);
//	            	Itempoint++;
		}
		else
		{
//			Itempoint=EEPROMReadByte(ItemAddress);
//			Itempoint++;
			Item0Str[i]=EEPROMReadByte(Item0Address+i);
			Item1Str[i]=EEPROMReadByte(Item1Address+i);
			Item2Str[i]=EEPROMReadByte(Item2Address+i);
			Item3Str[i]=EEPROMReadByte(Item3Address+i);
			Item4Str[i]=EEPROMReadByte(Item4Address+i);
			Item5Str[i]=EEPROMReadByte(Item5Address+i);
			Item6Str[i]=EEPROMReadByte(Item6Address+i);
			Item7Str[i]=EEPROMReadByte(Item7Address+i);
			Item8Str[i]=EEPROMReadByte(Item8Address+i);
			Item9Str[i]=EEPROMReadByte(Item9Address+i);
		}
	}

}

void SaveItemByte(unsigned char ItemcharTemp,unsigned int ID_PAD_TEMP)
{
	WORD ItemAddress,Itemcounter;
	if(ItemcharTemp>(ItemMax-1)) return;
	switch(ItemID)
	 {
	        	case ID_STATICTEXT10:
	        		Item0Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item0Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item0Address;
	        		break;
	        	case ID_STATICTEXT11:
	        		Item1Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item1Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item1Address;
	        		break;
	        	case ID_STATICTEXT12:
	        		Item2Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item2Str[ItemcharTemp+1] = 0;
				ItemAddress = Item2Address;
	        		break;
	        	case ID_STATICTEXT13:
	        		Item3Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item3Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item3Address;
	        		break;
	        	case ID_STATICTEXT14:
	        		Item4Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item4Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item4Address;
	        		break;
	        	case ID_STATICTEXT15:
	        		Item5Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item5Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item5Address;
	        		break;
	        	case ID_STATICTEXT16:
	        		Item6Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item6Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item6Address;
	        		break;
	        	case ID_STATICTEXT17:
	        		Item7Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item7Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item7Address;
	        		break;
	        	case ID_STATICTEXT18:
	        		Item8Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item8Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item8Address;
	        		break;
	        	case ID_STATICTEXT19:
	        		Item9Str[ItemcharTemp] = ID_PAD_TEMP;
	        		Item9Str[ItemcharTemp+1] = 0;
	        		ItemAddress = Item9Address;
	        		break;

	        	default:
	        		break;

	        }

	        
		        EEPROMWriteByte((BYTE)(ID_PAD_TEMP), ItemAddress+ItemcharTemp);

			for(Itemcounter=ItemcharTemp+1;Itemcounter<ItemMax;Itemcounter++)
		        EEPROMWriteByte((BYTE)(0), ItemAddress+Itemcounter);//½áÊø·ûºÅºóÃæµÄÈ«²¿ÇåÁã
	        
	    

}

void  CreateItem()// ´´½¨ÏîÄ¿add by Spring.Chen
{

	unsigned char i;
	GOLFree();  // free memory for the objects in the previous linked list and start new list

	CreatePage(ItemStr);
	SaveReadItem(FALSE);
    	CbCreate(ID_CHECKBOX10,             	// ID 
              positionax,(positionay+0*positionItem),
              2*positionItem,(positionay+1*positionItem),          	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//Item0Str, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX20,             	// ID 
              100*positionax,(positionay-2+0*positionItem),
              11*positionItem-8,(positionay+1*positionItem),          	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//Item0Str, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT10,           		// ID 
              positionax+2*positionItem,(positionay+0*positionItem),
              9*positionItem,(positionay+1*positionItem),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item0Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX11,             	// ID 
              positionax,(positionay+1*positionItem),
              2*positionItem,(positionay+2*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX21,             	// ID 
              100*positionax,(positionay+1*positionItem-2),
              11*positionItem-8,(positionay+2*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT11,           		// ID 
              positionax+2*positionItem,(positionay+1*positionItem),
              9*positionItem,(positionay+2*positionItem),          		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item1Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX12,             	// ID 
              positionax,(positionay+2*positionItem),
              2*positionItem,(positionay+3*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX22,             	// ID 
              100*positionax,(positionay+2*positionItem-2),
              11*positionItem-8,(positionay+3*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT12,           		// ID 
              positionax+2*positionItem,(positionay+2*positionItem),
              9*positionItem,(positionay+3*positionItem),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item2Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX13,             	// ID 
              positionax,(positionay+3*positionItem),
              2*positionItem,(positionay+4*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX23,             	// ID 
              100*positionax,(positionay+3*positionItem-2),
              11*positionItem-8,(positionay+4*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT13,           		// ID 
              positionax+2*positionItem,(positionay+3*positionItem),
              9*positionItem,(positionay+4*positionItem),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item3Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX14,             	// ID 
              positionax,(positionay+4*positionItem),
              2*positionItem,(positionay+5*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX24,             	// ID 
              100*positionax,(positionay+4*positionItem-2),
              11*positionItem-8,(positionay+5*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT14,           		// ID 
              positionax+2*positionItem,(positionay+4*positionItem),
              9*positionItem,(positionay+5*positionItem),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item4Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX15,             	// ID 
              positionax,(positionay+5*positionItem),
              2*positionItem,(positionay+6*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX25,             	// ID 
              100*positionax,(positionay+5*positionItem-2),
              11*positionItem-8,(positionay+6*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT15,           		// ID 
              positionax+2*positionItem,(positionay+5*positionItem),
              9*positionItem,(positionay+6*positionItem),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item5Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX16,             	// ID 
              positionax,(positionay+6*positionItem),
              2*positionItem,(positionay+7*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX26,             	// ID 
              100*positionax,(positionay+6*positionItem-2),
              11*positionItem-8,(positionay+7*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT16,           		// ID 
              positionax+2*positionItem,(positionay+6*positionItem),
              9*positionItem,(positionay+7*positionItem),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item6Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX17,             	// ID 
              positionax,(positionay+7*positionItem),
              2*positionItem,(positionay+8*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX27,             	// ID 
              100*positionax,(positionay+7*positionItem-2),
              11*positionItem-8,(positionay+8*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT17,           		// ID 
              positionax+2*positionItem,(positionay+7*positionItem),
              9*positionItem,(positionay+8*positionItem),         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item7Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX18,             	// ID 
              positionax,(positionay+8*positionItem),
              2*positionItem,(positionay+9*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX28,             	// ID 
              100*positionax,(positionay+8*positionItem-2),
              11*positionItem-8,(positionay+9*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT18,           		// ID 
              positionax+2*positionItem,(positionay+8*positionItem),
              9*positionItem,(positionay+9*positionItem),        		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item8Str, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX19,             	// ID 
              positionax,(positionay+9*positionItem),
              2*positionItem,(positionay+10*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
       CbCreate(ID_CHECKBOX29,             	// ID 
              100*positionax,(positionay+9*positionItem-2),
              11*positionItem-8,(positionay+10*positionItem),         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              NULL,//TextLeftStr, 				// "Text Left"
              altScheme);
	StCreate(ID_STATICTEXT19,           		// ID 
              positionax+2*positionItem,(positionay+9*positionItem),
              9*positionItem,(positionay+10*positionItem),       		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              Item9Str, 				// "TEST", 	// text
              alt5Scheme);
/*
	CbCreate(ID_CHECKBOX1,             	// ID 
              100,110,265,145,         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,70,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              PrintStr, 				// "TEST", 	// text
              alt5Scheme);

	CbCreate(ID_CHECKBOX1,             	// ID 
              100,110,265,145,         	// dimension
              CB_DRAW,                 	// will be dislayed after creation
              TextLeftStr, 				// "Text Left"
              altScheme);              	// alternative GOL scheme 
	StCreate(ID_STATICTEXT3,           		// ID 
              positionax,70,
              2*positionmaxx,100,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              PrintStr, 				// "TEST", 	// text
              alt5Scheme);*/


}
WORD MsgItem(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
{

	EDITBOX* pEb;
	SHORT    id;
	XCHAR    temp;
	static char status = 0;			// status to check if calling, holding or not
	static BOOL abcStatus = 0;		// status to check if Large ABC or not
	BYTE* Itempoit;
	WORD ItemAddress;
	unsigned int ID_PAD_TEMP;

    id = GetObjID(pObj);

    // If number key is pressed
//#ifdef	USE_BTN_MSG_RELEASED
    if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED
	//if((adcX == -1)||(adcY == -1))
//#else
	//if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED 
//#endif
    {
    	
    if(id >= ID_KEYPAD)
    if(id  <= ID_KEYPAD+9)
    {
	    if (!status)
	    {
	        pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	        EbAddChar(pEb,'0'+id-ID_KEYPAD);
//	        ID_PAD_TEMP='0'+id-ID_KEYPAD;
	        if(pEb->length - 1 < pEb->charMax)
//		if(Itemchar<ItemMax)
	        SaveItemByte(Itemchar,('0'+id-ID_KEYPAD));
	        Itemchar++;
	        SetState(pEb, EB_DRAW);
	        return 1;
	    }
    	}
	if(id >= ID_ABCPAD)
		    if(id  < ID_ABCPAD+26)
		    	{
		    		if (!status) 
		    		{
		        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
		        		if(!abcStatus)
		        		{
		        			EbAddChar(pEb,'a'+id-ID_ABCPAD);
		        			ID_PAD_TEMP='a'+id-ID_ABCPAD;
//		        			if(pEb->length <= pEb->charMax)
//	        				SaveItemByte(Itemchar,('a'+id-ID_ABCPAD));
		        		}
		        		else
		        		{
		        			EbAddChar(pEb,'A'+id-ID_ABCPAD);
		        			ID_PAD_TEMP='A'+id-ID_ABCPAD;
//		        			if(pEb->length <= pEb->charMax)
//	        				SaveItemByte(Itemchar,('A'+id-ID_ABCPAD));
		        			
		        		}
		        		if(pEb->length - 1 < pEb->charMax)
//		        		if(Itemchar<ItemMax)
		        		SaveItemByte(Itemchar,ID_PAD_TEMP);
		        		Itemchar++;
		        		SetState(pEb, EB_DRAW);
		        		
		    		}
		    		return 1;
	    		}

    	}

    switch(id)
    {
            case ID_CALL:
            if(objMsg == BTN_MSG_PRESSED){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
//                EbSetText(pEb, CallingStr);
                EbAddChar(pEb,(XCHAR)' ');	// changed by Spring.Chen
                ID_PAD_TEMP=' ';
                SaveItemByte(Itemchar,ID_PAD_TEMP);
		   Itemchar++;
                SetState(pEb, EB_DRAW);
                status = 0;// 1; Changed by Spring.chen
            }
            return 1;
    	 case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	                Itemchar--;
	                if(Itemchar<=0)
	                	Itemchar = 0;
	                SaveItemByte(Itemchar,(0));// È¥µôÒ»¸ö×Ö·û
	            } 
            }
            return 1;
            
	case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            	Itemchar = 0;
            	abcStatus = 0;
//            	SaveReadItem(FLASH);
                screenState = CREATE_ITEM;//CREATE_SETTING;				// goto slider screen
            }
            return 1; 										// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
            	Itemchar = 0;
            	abcStatus = 0;
                if(dif)
					screenState = CREATE_Master;
				else
                    screenState = CREATE_AGENT; 			// goto radio buttons screen
            }
            return 1; 										// process by default

	case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;
            
            case ID_ABC:
        	if(objMsg == BTN_MSG_RELEASED){
        		if(!abcStatus)
        		{
                		BtnSetText((BUTTON*)pObj, ABCStr);
        		}
        		else
        		{
        			BtnSetText((BUTTON*)pObj, abcStr);
        		}
        		abcStatus = !abcStatus;
            }
            return 1; // process by default

        default:
            return 1;
    }

}

// Processes messages for the edit box demo screen
WORD MsgItemEditABC(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg){

EDITBOX* pEb;
SHORT    id;
XCHAR    temp;
static char status = 0;			// status to check if calling, holding or not
static BOOL abcStatus = 0;		// status to check if Large ABC or not
static char ChinaPage = 1;
unsigned int ItemAdd;
    id = GetObjID(pObj);

    // If number key is pressed
#ifdef	USE_BTN_MSG_RELEASED
    if(objMsg == BTN_MSG_RELEASED)// BTN_MSG_PRESSED 
#ifdef	USE_adcX_adcy
	if((adcX == -1)||(adcY == -1))
#endif
#else
	if(objMsg == BTN_MSG_PRESSED)//BTN_MSG_RELEASED 
#endif
    {
    	if(id >= ID_KEYPAD)
    		if(id  < ID_KEYPAD+10)
    		{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,'0'+id-ID_KEYPAD);
//	        		memcpy(&Flashbuff[32],(pTempXchina[id-ID_KEYCHINAPAD-1][2]),2);
	        		SetState(pEb, EB_DRAW);
	    		}
        		return 1;        
    		}
    	if(id >= ID_KEYCHINAPAD)
    		if(id  < ID_KEYCHINAPAD+10)
    		{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		EbAddChar(pEb,(pTempXchina[id-ID_KEYCHINAPAD-1][2]));
	        		SetState(pEb, EB_DRAW);
	    		}
        		return 1;        
    		}

	    if(id >= ID_ABCPAD)
	    if(id  < ID_ABCPAD+26)
	    	{
	    		if (!status) 
	    		{
	        		pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);
	        		if(!abcStatus)
	        		{
	        			EbAddChar(pEb,'a'+id-ID_ABCPAD);
	        		}
	        		else
	        		{
	        			EbAddChar(pEb,'A'+id-ID_ABCPAD);
	        		}
	        			
	        		SetState(pEb, EB_DRAW);
	    		} 
        		return 1;        
    		}
	}

    switch(id){

        case ID_CALL:
            if(objMsg == BTN_MSG_PRESSED){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
//                EbSetText(pEb, CallingStr);
                EbAddChar(pEb,(XCHAR)' ');	// changed by Spring.Chen
                SetState(pEb, EB_DRAW);
                status = 0;// 1; Changed by Spring.chen
            }
            return 1;
#if	0
        case ID_STOPCALL:
            if(objMsg == BTN_MSG_PRESSED){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
                temp = 0x0000;
                EbSetText(pEb, &temp); 	
                SetState(pEb, EB_DRAW);
                status = 0;
            }
            return 1;
#endif
        case ID_BACKSPACE:
            if(objMsg == BTN_MSG_PRESSED){
                if (!status) {
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbDeleteChar(pEb);
	                SetState(pEb, EB_DRAW);
	            } 
            }
            return 1;
#if	0
        case ID_HOLD:
            if(objMsg == BTN_MSG_PRESSED){
                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	            if (status == 1) {
	                EbSetText(pEb, HoldingStr);	
	                status = 2;
    	        } else if (status == 2) {
	                EbSetText(pEb, CallingStr);	
	                status = 1;
	            } 
   	            SetState(pEb, EB_DRAW);
            }
            return 1;

        case ID_ASTERISK:
	    	if (!status) {
	            if(objMsg == BTN_MSG_PRESSED){
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbAddChar(pEb,(XCHAR)'*');
	                SetState(pEb, EB_DRAW);
	            }
	        } 
            return 1;

        case ID_POUND:
	    	if (!status) {
	            if(objMsg == BTN_MSG_PRESSED){
	                pEb = (EDITBOX*)GOLFindObject(ID_EDITBOX1);    
	                EbAddChar(pEb,(XCHAR)'#');
	                SetState(pEb, EB_DRAW);
	            }
	        }
            return 1;
#endif

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
            	abcStatus = 0;
           		unsigned char ik;
            	switch(screenState)// ItemState
            	{
            	case DISPLAY_ITEM0:
					for(ik=0;ik<19;ik++)Flashbuff[49+ik] = 0; 
            		CopyChar(&Flashbuff[49],TempbuffStr,ItemMax);
            		ItemAdd =0+1;
            		break;
            	case DISPLAY_ITEM1:
					for(ik=0;ik<19;ik++)Flashbuff[68+ik] = 0;  
            		CopyChar(&Flashbuff[68],TempbuffStr,ItemMax);
            		ItemAdd =1+1;
            		break;
            	case DISPLAY_ITEM2:
					for(ik=0;ik<19;ik++)Flashbuff[87+ik] = 0; 
            		CopyChar(&Flashbuff[87],TempbuffStr,ItemMax);
            		ItemAdd =2+1;
            		break;
            	case DISPLAY_ITEM3:
					for(ik=0;ik<19;ik++)Flashbuff[106+ik] = 0; 
            		CopyChar(&Flashbuff[106],TempbuffStr,ItemMax);
            		ItemAdd =3+1;
            		break;
            	case DISPLAY_ITEM4:
					for(ik=0;ik<19;ik++)Flashbuff[125+ik] = 0; 
            		CopyChar(&Flashbuff[125],TempbuffStr,ItemMax);
            		ItemAdd =4+1;
            		break;
            	case DISPLAY_ITEM5:
					for(ik=0;ik<19;ik++)Flashbuff[144+ik] = 0; 
            		CopyChar(&Flashbuff[144],TempbuffStr,ItemMax);
            		ItemAdd =5+1;
            		break;
            	case DISPLAY_ITEM6:
					for(ik=0;ik<19;ik++)Flashbuff[163+ik] = 0; 
            		CopyChar(&Flashbuff[163],TempbuffStr,ItemMax);
            		ItemAdd =6+1;
            		break;
            	case DISPLAY_ITEM7:
					for(ik=0;ik<19;ik++)Flashbuff[182+ik] = 0; 
            		CopyChar(&Flashbuff[182],TempbuffStr,ItemMax);
            		ItemAdd =7+1;
            		break;
            	case DISPLAY_ITEM8:
					for(ik=0;ik<19;ik++)Flashbuff[201+ik] = 0; 
            		CopyChar(&Flashbuff[201],TempbuffStr,ItemMax);
            		ItemAdd =8+1;
            		break;
            	case DISPLAY_ITEM9:
					for(ik=0;ik<19;ik++)Flashbuff[220+ik] = 0; 
            		CopyChar(&Flashbuff[220],TempbuffStr,ItemMax);
            		ItemAdd =9+1;
            		break;
            	default:
            		break;
            	}
            	SaveReadItem(FLASH);
            	for(temp = 0;temp<19;temp++)
              TempbuffStr[temp] = 0;
              for(;ItemAdd<10;ItemAdd++)//ItemMax ItemAdd =0
            	{// ¶ÁÐ´ÏÂÒ»¸öµØÖ·
            		if(EEPROMReadByte(ID_CHECKBOXADDRESS +ItemAdd))
            		break;
            	}
            	if(ItemAdd<10)//ItemMax
                screenState = 2*ItemAdd+CREATE_ITEM0;//CREATE_VEHICLENO;//CREATE_SUBJECT_NAME; 		// goto ECG demo screen
		else
              {
          		screenState = CREATE_Waittest;//screenState = CREATE_MAIN;   12.10 by gxy
          		ItemAdd =0;
              }
		//for(temp = 0;temp<19;temp++)
            //  TempbuffStr[0] = 0;
            }
            return 1; // process by default

         case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
            	abcStatus = 0;
            	switch(screenState)// ItemState
            	{
            	case DISPLAY_ITEM0:
 //           		CopyChar(&Flashbuff[49],TempbuffStr,ItemMax);
            		ItemAdd =0;
            		break;
            	case DISPLAY_ITEM1:
//            		CopyChar(&Flashbuff[68],TempbuffStr,ItemMax);
            		ItemAdd =1;
            		break;
            	case DISPLAY_ITEM2:
//            		CopyChar(&Flashbuff[87],TempbuffStr,ItemMax);
            		ItemAdd =2;
            		break;
            	case DISPLAY_ITEM3:
//            		CopyChar(&Flashbuff[106],TempbuffStr,ItemMax);
            		ItemAdd =3;
            		break;
            	case DISPLAY_ITEM4:
//            		CopyChar(&Flashbuff[125],TempbuffStr,ItemMax);
            		ItemAdd =4;
            		break;
            	case DISPLAY_ITEM5:
//            		CopyChar(&Flashbuff[144],TempbuffStr,ItemMax);
            		ItemAdd =5;
            		break;
            	case DISPLAY_ITEM6:
 //           		CopyChar(&Flashbuff[163],TempbuffStr,ItemMax);
            		ItemAdd =6;
            		break;
            	case DISPLAY_ITEM7:
//            		CopyChar(&Flashbuff[182],TempbuffStr,ItemMax);
            		ItemAdd =7;
            		break;
            	case DISPLAY_ITEM8:
//            		CopyChar(&Flashbuff[201],TempbuffStr,ItemMax);
            		ItemAdd =8;
            		break;
            	case DISPLAY_ITEM9:
//            		CopyChar(&Flashbuff[220],TempbuffStr,ItemMax);
            		ItemAdd =9;
            		break;
            	default:
            		break;
            	}
             for(;ItemAdd>0;ItemAdd--)//ItemMax ItemAdd =0
            	{// ¶ÁÐ´ÉÏÒ»¸öµØÖ·
            		if(EEPROMReadByte(ID_CHECKBOXADDRESS +ItemAdd-1))
            		{
            			screenState = screenState - 3;
            			break;
            		}
            		else screenState = screenState - 2;
            	}
            	if(ItemAdd==0)//ItemMax
              {
          		screenState = CREATE_Waittest;//screenState = CREATE_MAIN;   12.10 by gxy
          		ItemAdd =0;
              }
            }
            return 1; 

        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
            	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1; // process by default

        case ID_ABC:
        	if(objMsg == BTN_MSG_RELEASED){
        		if(!abcStatus)
        		{
                		BtnSetText((BUTTON*)pObj, ABCStr);
        		}
        		else
        		{
        			BtnSetText((BUTTON*)pObj, abcStr);
        		}
        		abcStatus = !abcStatus;
            }
            return 1; // process by default
#if	0
        case ID_BUTTONChina_P:
        	if(objMsg == BTN_MSG_RELEASED)
        	{
#if	1
			abcStatus = 0;
        	if(ChinaPage < ChinaPageMax)
        	{	ChinaPage++;
        	CopyArrays(ChinaPage);
        	screenState = screenState--; 	//
//        	SetState(pObj, BTN_DRAW_FOCUS|EB_DRAW);
        	// ÖÐÎÄ°´¼üÖ¸ÏòÏÂÒ»Ò³
        	}
#endif
        	}
        	return 1; // process by default
        case ID_BUTTONChina_M:
        	// ÖÐÎÄ°´¼üÖ¸ÏòÉÏÒ»Ò³
        	if(objMsg == BTN_MSG_RELEASED)
        	{
 #if	1
 			abcStatus = 0;
        		if(ChinaPage > 1)
        		{
        			ChinaPage --;
//        			return 0; // process by default
        		screenState = screenState--; 	//
        		CopyArrays(ChinaPage);
			}
#endif
        	}
        	return 1; // process by default
#endif
        default:
            return 1; // process by default
    }
}

void CreateAgent(void)
{
unsigned char j=0,i=0;

for(i=0;i<4;i++)
{

IDumberStr[j] = EEPROMReadByte(ID_JIQIADDRESS+i)/16+'0';
Nop();

IDumberStr[j+1] = EEPROMReadByte(ID_JIQIADDRESS+i)%16+'0';
Nop();

j=j+2;

}

//delayms(200);
	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(EditIDstr);	// CreatePage("Setting");
	
    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+10,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE+10,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              NULL,//(void*)&Pictest,                    	// no bitmap
              EditIDstr,//ButtonStr, 				// "Button",     	// text
              alt4Scheme);              	// use alternate scheme

			  


	StCreate(ID_STATICTEXT3,				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+10,
              220,MAINSTARTY+1*MAINCHARSIZE+10,         	// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              IDumberStr,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	//alt4Scheme            // alternative GOL scheme 



/*
    BtnCreate(ID_BUTTON3,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+10,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE+10,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              ThicknessStr, //PrintStr,//LowStr, 					// "LO",       	// text
              alt4Scheme);           	// use alternate scheme
*/
  BtnCreate(ID_BUTTON2,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+2*MAINSIZEY,
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              PrintID,//MemoryStr,//OnStr, 					// "ON",		// text
              alt4Scheme);             // use alternate scheme 

	StCreate(ID_STATICTEXTID,				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+2*MAINSIZEY,
              220,MAINSTARTY+2*MAINCHARSIZE+2*MAINSIZEY,         	// dimension
              ST_DRAW|ST_CENTER_ALIGN,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              IDPrintModel,//HomeStr, 					// "HOME", 	    // text
              mainScheme);	//alt4Scheme            // alternative GOL scheme 
/*
    BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+2*MAINSIZEY+10,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+2*MAINSIZEY+10,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              NULL,//(void*)&PicGPS,                    	// no bitmap
              DeleteStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	

    BtnCreate(ID_BUTTON6, 				// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+2*MAINSIZEY+10,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+2*MAINSIZEY+10,          	// dimension
              0,
              BTN_DRAW,//BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text              
              NULL,//(void*)&bulboff,          // use bitmap
              ItemStr,//KSettingStr,//TimeStr,//NULL,//OffBulbStr, 				// text
              alt4Scheme);	            // alternative GOL scheme 

	BtnCreate(ID_BUTTON7,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              DemarcateStr,//NULL,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 


	BtnCreate(ID_BUTTON8,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              CalibrationPeriodStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 
   BtnCreate(ID_BUTTON9,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              AdjustStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 
*/
   BtnCreate(ID_BUTTON3,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              AdjustStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 
}


WORD MsgAgent(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_SETTING;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_EDITID; 		// goto ECG demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_EDITPRINTID; 		// goto ECG demo screen

            }
            return 1; 							// process by default

        case ID_BUTTON3:
            if(objMsg == BTN_MSG_RELEASED){
			
              screenState = CREATE_DEMARCATE; 		// goto anyother model demo screen
            }
            return 1;			
/*
	case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){
				dif=0;
			screenState = CREATE_THICKNESS;
            }
            else {

            }
            return 1;  							// process by default

        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			{
 		// goto SETTING demo screen
			//Check_Screen();
			dif=0;
			screenState = CREATE_ScreenPASSWORD;
	        }
		    return 1; 							// Do not process by default

        case ID_BUTTON5:
        
			if(objMsg == BTN_MSG_PRESSED) {
				dif=0;
				screenState = CREATE_DELETE;
				YesNoNumber = 2;// É¾³ý¼ÇÂ¼
			}
		    return 0; 							// Do not process by default

        case ID_BUTTON6:
            if(objMsg == BTN_MSG_RELEASED){
				dif=0;
			screenState = CREATE_ITEM;//CREATE_INPUT; 
            }
            return 1; 							// process by default

	    case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){
		       screenState = CREATE_PASSWORD;			// goto Demarcate demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_CYC;//CREATE_KSETTING;//CREATE_DEMARCATE; 		// goto Download demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON9:
            if(objMsg == BTN_MSG_RELEASED){
              screenState = CREATE_DEMARCATE; 		// goto anyother model demo screen
            }
            return 1;
*/
        default:
            return 1; 							// process by default
    }
}



void	CreateSetting()
{
	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(SettingStr);	// CreatePage("Setting");

/*
    BtnCreate(ID_BUTTON1, 				// button ID 
              25,60,
              95,130,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              BacklightStr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 



    BtnCreate(ID_BUTTON2,             	// button ID 
              145,60,
              215,130,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              PrintSettingStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	

    BtnCreate(ID_BUTTON3,             	// button ID 
              25,190,
              95,260,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              DemarcateStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	
          
    BtnCreate(ID_BUTTON4,             	// button ID 
              145,190,
              215,260,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              Advancestr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 

*/
	  BtnCreate(ID_BUTTON1, 				// button ID 
				  10,MAINSTARTY+15,
				  10+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE+15,				// dimension
				  0,						// set radius 
				  BTN_DRAW, 				// draw a beveled button
				  NULL,//(void*)&Pictest,						// no bitmap
				  TimeStr,//ButtonStr,				// "Button",		// text
				  alt4Scheme);					// use alternate scheme


		BtnCreate(ID_BUTTON2,				// button ID 
				  MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)-8,MAINSTARTY+15,
				  MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX+8,MAINSTARTY+1*MAINCHARSIZE+15,			// dimension
				  0,
				  BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE,				// will be dislayed after creation 
				  NULL,//(void*)&Picrecord, 		 // use bitmap
				  BacklightStr,//HomeStr,					// "HOME",		// text
				  alt4Scheme);	//alt4Scheme			// alternative GOL scheme 

		BtnCreate(ID_BUTTON3,				// button ID 
				  230-MAINCHARSIZE,MAINSTARTY+15,
				  230,MAINSTARTY+1*MAINCHARSIZE+15,				// dimension
				  0,						// set radius 
				  BTN_DRAW,//|BTN_DISABLED,//|BTN_TOGGLE,		// draw a vertical capsule button 
											// that has a toggle behavior
				  NULL, 					// no bitmap
				  PrintSettingStr, //PrintStr,//LowStr,					// "LO",		// text
				  alt4Scheme);	// use alternate scheme
#ifdef Argentina

          BtnCreate(ID_BUTTON4,				// button ID 
				  10,MAINSTARTY+1*MAINCHARSIZE+2*MAINSIZEY,
				  10+1*MAINCHARSIZE,MAINSTARTY+2*MAINCHARSIZE+2*MAINSIZEY,				// dimension
				  0,						// set radius 
				  BTN_DRAW,//|BTN_DISABLED, 				// draw a vertical capsule button
				  NULL,//(void*)&Picsetting,						// no bitmap
				  DeleteStr,//DemarcateStr,//MemoryStr,//OnStr,					// "ON",		// text
				  alt4Scheme);			  
#else
	      BtnCreate(ID_BUTTON4,				// button ID 
				  10,MAINSTARTY+1*MAINCHARSIZE+2*MAINSIZEY,
				  10+1*MAINCHARSIZE,MAINSTARTY+2*MAINCHARSIZE+2*MAINSIZEY,				// dimension
				  0,						// set radius 
				  BTN_DRAW,//|BTN_DISABLED, 				// draw a vertical capsule button
				  NULL,//(void*)&Picsetting,						// no bitmap
				  DemarcateStr,//MemoryStr,//OnStr,					// "ON",		// text
				  alt4Scheme);			   //
#endif
		BtnCreate(ID_BUTTON5,				// button ID 
				  MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX)-8,MAINSTARTY+1*MAINCHARSIZE+2*MAINSIZEY,
				  MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX+8,MAINSTARTY+2*MAINCHARSIZE+2*MAINSIZEY, 			// dimension
				  0,						// set radius 
				  BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,		// draw a vertical capsule button
				  NULL,//(void*)&PicGPS,						// no bitmap
				  Advancestr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",		// text
				  alt4Scheme);				// use alternate scheme 	



}

WORD MsgSetting(WORD objMsg, OBJ_HEADER* pObj){


    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_MAIN;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	    case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;


        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_DATETIME;//CREATE_SETBACKLIGHT; 		// goto ECG demo screen
            }
            return 1; 							// process by default

	   case ID_BUTTON2:
        	if(objMsg == BTN_MSG_RELEASED){

			screenState = CREATE_SETBACKLIGHT;
            }

            return 1;  							// process by default
   

	   case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){
            Settingposition=0xaa;
			screenState = CREATE_PRINTSETTING;
            }

            return 1;  							// process by default

        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)
				{
#ifdef Argentina
			   Settingposition=0xaa;
			   screenState = CREATE_ScreenPASSWORD;

#else
               Settingposition=0xaa;
			   screenState = CREATE_PASSWORD;
#endif
				}
		    return 1; 							// Do not process by default

        case ID_BUTTON5:
            if(objMsg == BTN_MSG_RELEASED){

                screenState = CREATE_MASTERPASSWORD;
            }
            return 1; 							// process by default


        default:
            return 1; 							// process by default
    }
}
#ifdef Argentina

void CreateMaster(void)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(MasterStr);	// CreatePage("Setting");
    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              NULL,//(void*)&Pictest,                    	// no bitmap
              BlowSettingStr,//ButtonStr, 				// "Button",     	// text
              alt4Scheme);              	// use alternate scheme

    BtnCreate(ID_BUTTON2, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              ResetStr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 

    BtnCreate(ID_BUTTON3,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              ThicknessStr, //PrintStr,//LowStr, 					// "LO",       	// text
              alt4Scheme);           	// use alternate scheme

BtnCreate(ID_BUTTON4,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              EDITCALDATEdstr,//PrintStr,//MemoryStr,//OnStr, 					// "ON",		// text
              alt4Scheme);             // use alternate scheme 

    BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              NULL,//(void*)&PicGPS,                    	// no bitmap
              CalPerStr,//DemarcateStr,//CalPerStr,//DeleteStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	

    BtnCreate(ID_BUTTON6, 				// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,          	// dimension
              0,
              BTN_DRAW,//BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text              
              NULL,//(void*)&bulboff,          // use bitmap
              DemarcateStr,//ItemStr,//KSettingStr,//TimeStr,//NULL,//OffBulbStr, 				// text
              alt4Scheme);	            // alternative GOL scheme 
 /*             
	BtnCreate(ID_BUTTON7,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              EDITCALDATEdstr,//NULL,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme


	BtnCreate(ID_BUTTON8,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              PasswordStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  
*/
			  
	BtnCreate(ID_BUTTON9,             	// button ID 
             MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
             MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              PasswordStr,//CalPerStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 


}
WORD MsgMaster(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_SETTING;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				dif=1;
                screenState = CREATE_BLOW_PRESS; 		// goto ECG demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
				dif=1;
                screenState = CREATE_RESET; 		// goto ECG demo screen
                YesNoNumber = 1;//Default setting
            }
            return 1; 							// process by default

	case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){
				dif=1;
			screenState = CREATE_THICKNESS;
            }
            else {

            }
            return 1;  							// process by default

        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			{
		       screenState = CREATE_EDITMAINDISPLAY;			// 

	        }
		    return 1; 							// Do not process by default

        case ID_BUTTON5:
        
			if(objMsg == BTN_MSG_PRESSED) {
                  screenState = CREATE_CYC;//
			}
		    return 1; 							// Do not process by default

        case ID_BUTTON6:
            if(objMsg == BTN_MSG_RELEASED){
				Settingposition=0xbb;
				
		       screenState = CREATE_PASSWORD;			// 
            }
            return 1; 							// process by default

/*
#ifdef Argentina
	    case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){

				
		       screenState = CREATE_EDITMAINDISPLAY;			// 
            }
            return 1; 							// 
#else
	    case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){

				Settingposition=0xbb;
				
		       screenState = CREATE_PASSWORD;			// 
            }
            return 1; 							// 
#endif

        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){
				
                screenState = CREATE_Setpassword;//CREATE_KSETTING;//CREATE_DEMARCATE; 		// goto Download demo screen
            }
            return 1; 							// process by default
*/
        case ID_BUTTON9:
            if(objMsg == BTN_MSG_RELEASED){
				
                   screenState = CREATE_Setpassword;
		       }
            return 1; 							// process by default


        default:
            return 1; 							// process by default
    }
}

#else

void CreateMaster(void)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(MasterStr);	// CreatePage("Setting");
    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              NULL,//(void*)&Pictest,                    	// no bitmap
              BlowSettingStr,//ButtonStr, 				// "Button",     	// text
              alt4Scheme);              	// use alternate scheme

    BtnCreate(ID_BUTTON2, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              ResetStr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 

    BtnCreate(ID_BUTTON3,             	// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_TOGGLE,   	// draw a vertical capsule button 
              						   	// that has a toggle behavior
              NULL,                    	// no bitmap
              ThicknessStr, //PrintStr,//LowStr, 					// "LO",       	// text
              alt4Scheme);           	// use alternate scheme

BtnCreate(ID_BUTTON4,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              PrintStr,//MemoryStr,//OnStr, 					// "ON",		// text
              alt4Scheme);             // use alternate scheme 

    BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              NULL,//(void*)&PicGPS,                    	// no bitmap
              DeleteStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              alt4Scheme);            	// use alternate scheme 	
/*
    BtnCreate(ID_BUTTON6, 				// button ID 
              MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY,
              MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY,          	// dimension
              0,
              BTN_DRAW,//BTN_DRAW|BTN_TEXTLEFT 	// will be dislayed after creation with text              
              NULL,//(void*)&bulboff,          // use bitmap
              ItemStr,//KSettingStr,//TimeStr,//NULL,//OffBulbStr, 				// text
              alt4Scheme);	            // alternative GOL scheme 
*/

	BtnCreate(ID_BUTTON7,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+1*MAINCHARSIZE+0*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              DemarcateStr,//NULL,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 

	BtnCreate(ID_BUTTON8,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              PasswordStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 
	BtnCreate(ID_BUTTON9,             	// button ID 
             MAINSTARTX+2*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+2*(MAINCHARSIZE+MAINSIZEY),
             MAINSTARTX+3*MAINCHARSIZE+2*MAINSIZEX,MAINSTARTY+3*MAINCHARSIZE+2*MAINSIZEY,           	// dimension
              0,					   	// set radius 
              BTN_DRAW, 			   	// draw a vertical capsule button
              NULL,//(void*)&Picsetting,                    	// no bitmap
              CalPerStr,//KSettingStr,//DemarWendStr,//CheckScreenStr,//OnStr, 					// "ON",		// text
              alt4Scheme);  //  greenScheme         // use alternate scheme 


}
WORD MsgMaster(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_SETTING;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_SETTING; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
 //           	abcStatus = 0;
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				dif=1;
                screenState = CREATE_BLOW_PRESS; 		// goto ECG demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
				dif=1;
                screenState = CREATE_RESET; 		// goto ECG demo screen
                YesNoNumber = 1;//Default setting
            }
            return 1; 							// process by default

	case ID_BUTTON3:
        	if(objMsg == BTN_MSG_RELEASED){
				dif=1;
			screenState = CREATE_THICKNESS;
            }
            else {

            }
            return 1;  							// process by default

        case ID_BUTTON4:

			if(objMsg == BTN_MSG_RELEASED)//BTN_MSG_PRESSED)
			{
            Settingposition=0xbb;
			screenState = CREATE_PRINTSETTING;
	        }
		    return 1; 							// Do not process by default

        case ID_BUTTON5:
        
			if(objMsg == BTN_MSG_PRESSED) {
				dif=1;
				screenState = CREATE_DELETE;
				YesNoNumber = 2;// É¾³ý¼ÇÂ¼
			}
		    return 0; 							// Do not process by default

        case ID_BUTTON6:
            if(objMsg == BTN_MSG_RELEASED){
				dif=1;
			screenState = CREATE_ITEM;//CREATE_INPUT; 
            }
            return 1; 							// process by default


	    case ID_BUTTON7:
            if(objMsg == BTN_MSG_RELEASED){

				Settingposition=0xbb;
				
		       screenState = CREATE_PASSWORD;			// 
            }
            return 1; 							// 


        case ID_BUTTON8:
            if(objMsg == BTN_MSG_RELEASED){
				
                screenState = CREATE_Setpassword;//CREATE_KSETTING;//CREATE_DEMARCATE; 		// goto Download demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON9:
            if(objMsg == BTN_MSG_RELEASED){
				
                screenState = CREATE_CYC;//CREATE_KSETTING;//CREATE_DEMARCATE; 		// goto Download demo screen
            }
            return 1; 							// process by default


        default:
            return 1; 							// process by default
    }
}

#endif
void CreateSetpassword(void)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(Setpasswordstr);	// CreatePage("Setting");

    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-10,MAINSTARTY,
              MAINSTARTX+3*MAINCHARSIZE+3*MAINSIZEX-10,MAINSTARTY+1*MAINCHARSIZE,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              DemarcateStr,//HomeStr, 					// "HOME", 	    // text
              alt4Scheme);	//alt4Scheme            // alternative GOL scheme 


#ifdef Argentina
    BtnCreate(ID_BUTTON2,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-10,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY-10,
              MAINSTARTX+3*MAINCHARSIZE+3*MAINSIZEX-10,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY-10,         
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              DeleteStr,//MasterStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					      
              alt4Scheme);             
#endif

    BtnCreate(ID_BUTTON3,             	// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX)-10,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+70,
              MAINSTARTX+3*MAINCHARSIZE+3*MAINSIZEX-10,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY+70,         
              0,					   	// 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	
              NULL,//(void*)&PicGPS,                    	
              MasterStr,//DeleteStr,//DemarWendStr,//ThicknessStr, //NULL,//OffStr, 					      
              alt4Scheme);
}

WORD MsgSetpassword(WORD objMsg, OBJ_HEADER* pObj){

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
               screenState = CREATE_Master;// goto check box demo screen
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_Master; 		// goto ECG demo screen
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

		case ID_BUTTON1:
            if(objMsg == BTN_MSG_RELEASED){
				Setpasswordposition=0xaa;
                screenState = CREATE_InputSetpassword1; 	// goto list box screen
            }
            return 1;

		case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
				Setpasswordposition=0xbb;
                screenState = CREATE_InputSetpassword1; 	// goto list box screen
            }
            return 1;
			
		case ID_BUTTON3:
            if(objMsg == BTN_MSG_RELEASED){
				Setpasswordposition=0xcc;
                screenState = CREATE_InputSetpassword1; 	// goto list box screen
            }
            return 1;

    	}

}

void CreateOutcomeSetpassword(void)
{

	GOLFree();   // 
	CreatePage(PasswordSettingStr);	// 

	
	
  if(PDset)
  	{

	// alt5Scheme->TextColor0 = BRIGHTGREEN;
	// alt5Scheme->Color0 = BRIGHTGREEN;
	// alt5Scheme->Color1 = BRIGHTGREEN;
		
	 StCreate(ID_STATICTEXT3,           		// ID 
              5,2*MAINSTARTY,
              235,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+60,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              SettingsuccessStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme

  	}
   else  
   	{

	// alt5Scheme->TextColor0 = BRIGHTRED;
	 
     StCreate(ID_STATICTEXT3,           		// ID 
              5,2*MAINSTARTY,
              235,MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+60,         		// dimension
              ST_DRAW|ST_CENTER_ALIGN,		// display text
              NotmatchStr, 				// "TEST", 	// text
              alt5Scheme);   //mainScheme   alt2Scheme             // use alternate scheme


   }


}

WORD MsgOutcomeSetpassword(WORD objMsg, OBJ_HEADER* pObj){


    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				alt5Scheme->TextColor0 = BRIGHTBLUE;
               screenState = CREATE_Setpassword;// 
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				alt5Scheme->TextColor0 = BRIGHTBLUE;
                screenState = CREATE_MAIN; 		// 
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
				alt5Scheme->TextColor0 = BRIGHTBLUE;
                screenState = CREATE_MAIN; 	// 
            }
            return 1;


    	}

}
void CreateYesNo(XCHAR* YesOrNo)
{

	GOLFree();   // free memory for the objects in the previous linked list and start new list

	CreatePage(YesOrNo);	// CreatePage("Setting");
/*
    BtnCreate(ID_BUTTON1, 				// button ID 
              MAINSTARTX+0*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY,
              MAINSTARTX+1*MAINCHARSIZE,MAINSTARTY+1*MAINCHARSIZE,           	// dimension
              10,					   	// set radius 
              BTN_DRAW,  			   	// draw a beveled button
              NULL,//(void*)&Pictest,                    	// no bitmap
              ResetStr,//ButtonStr, 				// "Button",     	// text
              alt4Scheme);              	// use alternate scheme
*/
    BtnCreate(ID_BUTTON2, 				// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+20,
              MAINSTARTX+2*MAINCHARSIZE+1*MAINSIZEX,MAINSTARTY+1*MAINCHARSIZE+20,         	// dimension
              0,
              BTN_DRAW,//|BTN_DISABLED,//BTN_HIDE, 				// will be dislayed after creation 
              NULL,//(void*)&Picrecord,          // use bitmap
              YesStr,//HomeStr, 					// "HOME", 	    // text
              altScheme);	//alt4Scheme            // alternative GOL scheme 

	BtnCreate(ID_BUTTON5,             	// button ID 
              MAINSTARTX+1*(MAINCHARSIZE+MAINSIZEX),MAINSTARTY+1*MAINCHARSIZE+MAINSIZEY+30,
              MAINSTARTX+2*MAINCHARSIZE+MAINSIZEX,MAINSTARTY+2*MAINCHARSIZE+MAINSIZEY+30,         	// dimension
              0,					   	// set radius 
              BTN_DRAW,//|BTN_DISABLED,//|BTN_PRESSED,  	// draw a vertical capsule button
              							// that is initially pressed
              NULL,//(void*)&PicGPS,                    	// no bitmap
              NoStr,//ThicknessStr, //NULL,//OffStr, 					// "OFF",      	// text
              altScheme);            	// use alternate scheme 	
}


WORD MsgYesNo(WORD objMsg, OBJ_HEADER* pObj){
	OBJ_HEADER* pOtherRbtn;

    switch(GetObjID(pObj)){

        case ID_BUTTON_NEXT:
            if(objMsg == BTN_MSG_RELEASED){
				if(YesNoNumber == 2)
                screenState = CREATE_SETTING;				
                else if(YesNoNumber == 1)
				screenState = CREATE_Master;
            }
            return 1; 							// process by default

        case ID_BUTTON_BACK:
            if(objMsg == BTN_MSG_RELEASED){
				if(YesNoNumber == 2)
                screenState = CREATE_SETTING;				
                else if(YesNoNumber == 1)
				screenState = CREATE_Master;				
            }
            return 1; 							// process by default
        case ID_BUTTON_HOME:
            if(objMsg == BTN_MSG_RELEASED){
                screenState = CREATE_MAIN; 	// goto list box screen
            }
            return 1;

        case ID_BUTTON2:
            if(objMsg == BTN_MSG_RELEASED){
            	if(YesNoNumber == 1)
            		{//Reset System
                      DefaultSETTING();
					  screenState = CREATE_YESNO;

            		}
            	else if(YesNoNumber == 2)
            	{// Clear  all Record
            		EEPROMWriteWord(1,EESERIAL_NUMBER);// Ð´Ò»×÷Îª¼ÇÂ¼µÄÌõÊý¿ªÊ¼
			        serialnumber = 1;//EEPROMReadWord(EESERIAL_NUMBER);
			        serialnumberRecord = serialnumber - 1;// ±£³Ö¼ÇÂ¼Ò»ÖÂ
			        SST25ChipErase();
					screenState = CREATE_YESNO;
            	}
				/*
				if(Settingposition==0xaa)
                screenState = CREATE_SETTING;				
                else if(Settingposition==0xbb)
				screenState = CREATE_Master;
				*/
            }
            return 1; 							// process by default

        case ID_BUTTON5:
			if(objMsg == BTN_MSG_PRESSED) {
				if(YesNoNumber == 2)
                screenState = CREATE_SETTING;				
                else if(YesNoNumber == 1)
				screenState = CREATE_Master;
			}
		    return 1; 							// Do not process by default

        default:
            return 1; 							// process by default
    }
}



// Output text message on screen and stop execution
void ErrorTrap(XCHAR* message){
    SetColor(BLACK);
    ClearDevice();
    SetFont((void*)&FONTDEFAULT);
    SetColor(WHITE);    
    OutTextXY((GetMaxX()-GetTextWidth((XCHAR*)message,(void*)&GOLFontDefault))>>1,GetMaxY()>>1,message);
//    while(1);
    DelayMs(1000);
while(POWER_CONTROL==0);
}

/*********************************************************************
* Function: Timer4 ISR
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: increments tick counter. Tick is approx. 1 ms.
*
* Note: none
*
********************************************************************/
/*#ifdef __PIC32MX__
#define __T4_ISR    __ISR(_TIMER_4_VECTOR, ipl1)
#else
#define __T4_ISR    __attribute__((interrupt, shadow, auto_psv))
#endif

void  __T4_ISR _T4Interrupt(void)
{
    tick++;    
    // Clear flag
#ifdef __PIC32MX__
    mT4ClearIntFlag();
#else
    IFS1bits.T4IF = 0;
#endif
}
*/
/*********************************************************************
* Function: void TickInit(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: sets tick timer
*
* Note: none
*
********************************************************************/
void TickInit(void){
#define TICK_PERIOD    16000
    // Initialize Timer4
    TMR4 = 0;
//    T3CONbits.TCKPS = 1;         //Set prescale to 1:8
    PR4 = TICK_PERIOD;
    IFS1bits.T4IF = 0;              //Clear flag
    IEC1bits.T4IE = 1;              //Enable interrupt
    T4CONbits.TON = 1;              //Run timer 
    //IPC6bits.T4IP = 4;
}
void __attribute__((interrupt, shadow, no_auto_psv)) _T4Interrupt(void)
{
//if(ChoicePrint == FALSE)
	tick++; 

    // Clear flag
    IFS1bits.T4IF = 0;


}
