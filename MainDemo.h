/*********************************************************************
 * Microchip Graphic Library Demo Application
 * The header file joins all header files used in the project.
 *********************************************************************
 * FileName:        MainDemo.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC24/PIC30/PIC32
 * Compiler:        C30/C32
 * Company:         Microchip Technology, Inc.
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
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok		05/29/2007
 * Sean Justice         02/07/2008  PIC32 support
 ********************************************************************/
#ifndef _MAINDEMO_H
#define _MAINDEMO_H
#include "Graphics.h"

typedef enum {

	CREATE_MAIN=0,
    DISPLAY_MAIN,
    CREATE_SUBJECT_NAME,
    DISPLAY_SUBJECT_NAME,
    CREATE_PLACE,
    DISPLAY_PLACE,
    CREATE_VEHICLENO,	// ³µÅÆºÅ
    DISPLAY_VEHICLENO,
    CREATE_BADGENO,		// ¼ÝÊ»Ö¤ºÅ
    DISPLAY_BADGENO,
    CREATE_OPERATORNAME,	//¾¯¹ÙºÅ
    DISPLAY_OPERATORNAME,

    CREATE_PREPROCESS,		// ²âÊÔ×¼±¸
    DISPLAY_PREPROCESS,
    CREATE_BLOW,			// Çë´µÆø
    DISPLAY_BLOW,
    CREATE_DEPARTMENT,
    DISPLAY_DEPARTMENT,
    
	

	CREATE_RECORD,
	DISPLAY_RECORD,	
	CREATE_JUMP,
	DISPLAY_JUMP,
	CREATE_SETTING,
	DISPLAY_SETTING,
	CREATE_EDITEEPROM,
	DISPLAY_EDITEEPROM,
	CREATE_EDITID,	
	DISPLAY_EDITID,	
	CREATE_EDITPRINTID,	
	DISPLAY_EDITPRINTID,
	CREATE_GPS,
	DISPLAY_GPS,

	CREATE_SETBACKLIGHT,
	DISPLAY_SETBACKLIGHT,


	CREATE_TO_PC,
	DISPLAY_TO_PC,

	CREATE_RESULT,
	DISPLAY_RESULT,

	CREATE_UNIT_SCREEN,
	DISPLAY_UNIT_SCREEN,
	DISPLAY_UNIT_PDMENU,
	SHOW_UNIT_PDMENU,

	CREATE_THICKNESS,
	DISPLAY_THICKNESS,
	CREATE_KSETTING,
	DISPLAY_KSETTING,
	CREATE_PRINTSETTING,
	DISPLAY_PRINTSETTING,	
	CREATE_SoundControl,
	DISPLAY_SoundControl,
	CREATE_DEMARCATE,
	DISPLAY_DEMARCATE,
	CREATE_DEMARCATE2,
	DISPLAY_DEMARCATE2,  	
    CREATE_EDITDEMARCATE1,
	DISPLAY_EDITDEMARCATE1,
	CREATE_EDITDEMARCATE2,
	DISPLAY_EDITDEMARCATE2,
	CREATE_EDITDEMARCATE3,
	DISPLAY_EDITDEMARCATE3,
	CREATE_EDITDEMARCATE4,
	DISPLAY_EDITDEMARCATE4,
	CREATE_EDITDEMARCATE5,
	DISPLAY_EDITDEMARCATE5,
	CREATE_EDITDEMARCATE6,
	DISPLAY_EDITDEMARCATE6,
    CREATE_INPUT,
	DISPLAY_INPUT,
    CREATE_Mark,
	DISPLAY_Mark,
	CREATE_Wait,
	DISPLAY_Wait,
	CREATE_Waittest,
	DISPLAY_Waittest,
	CREATE_Waitdem,
	DISPLAY_Waitdem,
	CREATE_Waitdem2,
	DISPLAY_Waitdem2,
	CREATE_Debug,
	DISPLAY_Debug,
	CREATE_Debug2,
	DISPLAY_Debug2,
	CREATE_DEMARCATEshow,
	DISPLAY_DEMARCATEshow,
	DISPLAY_passivity,
	CREATE_REFUSE,
	DISPLAY_REFUSE,
	CREATE_DISCONTINUED,
	DISPLAY_DISCONTINUED,	
	CREATE_BLOW_PRESS,
	DISPLAY_BLOW_PRESS,



    CREATE_PASSWORD,
    DISPLAY_PASSWORD,
    CREATE_ScreenPASSWORD,
    DISPLAY_ScreenPASSWORD,
    CREATE_MASTERPASSWORD,
    DISPLAY_MASTERPASSWORD,
    CREATE_POWEROFF,
    DISPLAY_POWEROFF,
    CREATE_ITEM,
    DISPLAY_ITEM,
    CREATE_ITEM0,
    DISPLAY_ITEM0,
    CREATE_ITEM1,
    DISPLAY_ITEM1,
    CREATE_ITEM2,
    DISPLAY_ITEM2,
    CREATE_ITEM3,
    DISPLAY_ITEM3,
    CREATE_ITEM4,
    DISPLAY_ITEM4,
    CREATE_ITEM5,
    DISPLAY_ITEM5,
    CREATE_ITEM6,
    DISPLAY_ITEM6,
    CREATE_ITEM7,
    DISPLAY_ITEM7,
    CREATE_ITEM8,
    DISPLAY_ITEM8,
    CREATE_ITEM9,
    DISPLAY_ITEM9,
    CREATE_AGENT,
    DISPLAY_AGENT,
    CREATE_Master,
    DISPLAY_Master,
    CREATE_Setpassword,
    DISPLAY_Setpassword,
    CREATE_InputSetpassword1,
    DISPLAY_InputSetpassword1,
    CREATE_InputSetpassword2,
    DISPLAY_InputSetpassword2,
    CREATE_OutcomeSetpassword,
    DISPLAY_OutcomeSetpassword,    
    CREATE_RESET,
    DISPLAY_RESET,
    CREATE_CYC,
    DISPLAY_CYC, 
   #ifdef Argentina
    CREATE_EDITMAINDISPLAY,
    DISPLAY_EDITMAINDISPLAY,
   #endif  
    CREATE_CALLOCK,
    DISPLAY_CALLOCK,
    CREATE_DELETE,
    DISPLAY_DELETE,
    CREATE_YESNO,
    DISPLAY_YESNO,

    
    
    
    // these states are for time and date settings 0xF3xx is used here as a 
    // state ID to check when date and time are to be updated or not.
    CREATE_DATETIME = 0xF300,		// creates the date and time setting menu		
    DISPLAY_DATETIME = 0xF301,		// displays the menu for the date and time setting
    DISPLAY_DATE_PDMENU = 0xF302,	// displays the pulldown menu to set one of the date items (month, day or year)
    SHOW_DATE_PDMENU = 0xF303,		// creates the pulldown menu to set one of the date items (month, day or year)
    HIDE_DATE_PDMENU = 0xF304		// hides the pulldown menu to set one of the date items (month, day or year)

} SCREEN_STATES;

////////////////////////////// INCLUDES //////////////////////////////
#ifdef __PIC32MX
#include <plib.h>
#else
#include <p24Fxxxx.h>
#endif

#include "GenericTypeDefs.h"
#include "EEPROM.h"
#include "SST25VF032B.h"
//#include "TouchScreen.h"
#include "Beep.h"
#include "SideButtons.h"
#include "rtcc.h"

#include "DefineXY.h"
#include "Printer.h"

#include "usb.h"
#include "usb_function_hid.h" 
#include "HardwareProfile.h"
#endif // _MAINDEMO_H
