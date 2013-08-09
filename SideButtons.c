/*****************************************************************************
 *
 * Side buttons driver
 * (S3(RD6), S4(RD13), S5(RA7) and S6(RD7) on Explorer 16 board)
 *
 *****************************************************************************
 * FileName:        SideButtons.c
 * Dependencies:    MainDemo.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
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
 * Anton Alkhimenok		07/20/07	...
 *****************************************************************************/

#include "MainDemo.h"
extern WORD g_rtc_counter;	//
extern WORD g_standby_time;	//

#define BTN_Pressed 0x11
#define BTN_Release 0x22


BOOL Power_ON_OFF(void)
{
/*
	unsigned char i,j;
	unsigned int	Power_Count;
	BTN_S6=1;
	delay();
	if(BTN_S6)
		{
			POWER_CONTROL = 0;
		}
	else
		{
			Power_Count = 0;
			delay();
			if(!BTN_S6)
			{
				for(i=0;i>255;i++)
				for(j=0;j>255;j++)
				{
					if(!BTN_S6)
					{
						Power_Count++;
						if(Power_Count>(255*200))
							return TRUE;// ¿ª¹Ø»ú
						else
						{
//							Power_Count = 0;
//							return FALSE;	//²»×ö´¦Àí
							continue;
						}
					}
					else
						return FALSE;	//²»×ö´¦Àí
				}
			}
			else
				return FALSE;	//²»×ö´¦Àí
					

		}
*/
}
/************************************************************************
* Function: void SideButtonsMsg(GOL_MSG* msg)
*                                                                       
* Overview: this function checks buttons state and creates GOL messages.
*           S3 and S4 are used for ARROW UP PRESSED and ARROW DOWN PRESSED
*           messages. S6 moves focus to the next control.
*           S5 creates CARRIGE RETURN PRESSED/RELEASED messages.
*                                                                       
* Input: pointer to the GOL message structure                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void SideButtonsMsg(GOL_MSG* msg){
// Previous states of the buttons
static char S3 = 1;
//static char S4 = 1;
//static char S5 = 1;
static char S6 = 1,S3_state=BTN_Release,S4_state=BTN_Release;
static int S6_count = 1,S3_count=1,S6_Press=0,S4_count=1;
char state;
OBJ_HEADER* obj;
OBJ_HEADER* pFocusedObj;


    
    msg->uiEvent  = EVENT_INVALID;

    state = BTN_S6;

	BTN_S6 = 1;
	DelayMs(10);

	// state = BTN_S5;
    if(S6 != state){
        // Get pointer to the focused object
        obj = GOLGetFocus();
        if(obj != NULL){
			//S6_count = 0;
            if(S6){
				    S6_count ++;
					S6_Press=1;
		            g_rtc_counter = 0;
                    msg->type    = TYPE_KEYBOARD;
                    msg->uiEvent = EVENT_KEYSCAN;           
                    msg->param1  = obj->ID;
                    msg->param2  = SCAN_CR_PRESSED;
            }else{
                    S6_count = 0;
                    msg->type    = TYPE_KEYBOARD;
                    msg->uiEvent = EVENT_KEYSCAN;           
                    msg->param1  = obj->ID;
                    msg->param2  = SCAN_CR_RELEASED;
					g_rtc_counter = 0;
            }
        }
        S6 = state;

    }

	if(BTN_S6 == 0)
		{
		S6_count ++;
		S6_Press=1;
		g_rtc_counter = 0;
       
	}
	else
		S6_count = 0;


	BTN_S6 = 1;
	
	if(S6_count >300)	// ³¤°´5Ãë¹Ø»ú
	{
		POWER_CONTROL = 0;//¿ª¹Ø»ú
		ErrorTrap("POWER OFF");
		S6_count = 0;
		return 1;
	}
	else
		{
		if((!BTN_S3)&&(S3_state == BTN_Release))
				{
				S3_count++;
				}
		else S3_count=0;
		
		if((S3_count>=10)&&(!BTN_S3))
			{
					Beep();
					pFocusedObj = GOLGetFocusNext();
					GOLSetFocus(pFocusedObj);
					S3_count=0;
					g_rtc_counter = 0;
					S3_state = BTN_Pressed;
					}
		if((BTN_S3)&&(S3_state == BTN_Pressed))
		{
            delayms(50);
			if(BTN_S3)
              S3_state = BTN_Release;

		 }
				
			
		if((!BTN_S4)&&(S4_state == BTN_Release))
				{
				S4_count++;
				}
		else S4_count=0;
		
		if((S4_count>=10)&&(!BTN_S4))
			{
					Beep();
					pFocusedObj = GOLGetFocusPrev();
					GOLSetFocus(pFocusedObj);
					S4_count=0;
					g_rtc_counter = 0;
					S4_state = BTN_Pressed;
					}
		if((BTN_S4)&&(S4_state == BTN_Pressed))
		{
            delayms(50);
			if(BTN_S4)
              S4_state = BTN_Release;

		 }
				
		}			

}
