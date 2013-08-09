/*****************************************************************************
 *
 *  Custom control.
 *  GOLDraw() and  GOLMsg() are modified to provide GOL drawing and messaging 
 *  for this control. See GOL.c file for details.
 *  To remove the code related to this control comment 
 *  #define USE_CUSTOM line in GraphicsConfig.h header file.
 *
 *****************************************************************************
 * FileName:        CustomControl.c
 * Dependencies:    None 
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, MPLAB C32
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
 * Anton Alkhimenok     06/18/07    Beta release
 *****************************************************************************/

#include "MainDemo.h"

#ifdef USE_CUSTOM

/*********************************************************************
* Function: CUSTOM* CcCreate(WORD ID, SHORT left, SHORT top, SHORT right, 
*                              SHORT bottom, WORD state, GOL_SCHEME *pScheme)
*
* Overview: creates the object
*
********************************************************************/
CUSTOM *CcCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom, 
			       WORD state, GOL_SCHEME *pScheme)
{
	CUSTOM *pCc = NULL;
	
	pCc = malloc(sizeof(CUSTOM));
	if (pCc == NULL)
		return pCc;

	pCc->ID      	= ID;
	pCc->pNxtObj 	= NULL;
	pCc->type    	= OBJ_CUSTOM;
	pCc->left    	= left;
	pCc->top     	= top;
	pCc->right   	= right;
	pCc->bottom  	= bottom;
	pCc->pos     	= (pCc->top+pCc->bottom)>>1; // position
	pCc->prevPos   	= pCc->bottom-GOL_EMBOSS_SIZE;  // previos position
	pCc->state   	= state;                // set state

	// Set the style scheme to be used
	if (pScheme == NULL)
		pCc->pGolScheme = _pDefaultGolScheme; 
	else 	
		pCc->pGolScheme = (GOL_SCHEME *)pScheme; 	

    GOLAddObject((OBJ_HEADER*) pCc);
	
	return pCc;
}

/*********************************************************************
* Function: WORD CcTranslateMsg(CUSTOM *pCc, GOL_MSG *pMsg)
*
* Overview: translates the GOL message to this control
*
********************************************************************/
WORD CcTranslateMsg(CUSTOM *pCc, GOL_MSG *pMsg)
{
    // Check if disabled first
	if ( GetState(pCc,CC_DISABLED) )
		return OBJ_MSG_INVALID;

#ifdef USE_TOUCHSCREEN
    if(pMsg->type == TYPE_TOUCHSCREEN){
    	// Check if it falls in the control area
	    if( (pCc->left+GOL_EMBOSS_SIZE < pMsg->param1) &&
   	        (pCc->right-GOL_EMBOSS_SIZE  > pMsg->param1) &&
            (pCc->top+GOL_EMBOSS_SIZE < pMsg->param2) &&
            (pCc->bottom-GOL_EMBOSS_SIZE > pMsg->param2) ){

            return CC_MSG_SELECTED;
        }
    }
#endif

	return OBJ_MSG_INVALID;	
}

/*********************************************************************
* Function: void CcMsgDefault(CUSTOM* pCc, GOL_MSG* pMsg)
*
* Overview: changes the state of the object by default
*
********************************************************************/
void CcMsgDefault(CUSTOM* pCc, GOL_MSG* pMsg){
  
    pCc->pos = pMsg->param2;
    SetState(pCc,CC_DRAW_BAR);

}

/*********************************************************************
* Function: WORD CcDraw(CUSTOM *pCc)
*
* Output: returns the status of the drawing
*		  0 - not complete
*         1 - done
*
* Overview: draws control
*
********************************************************************/
WORD CcDraw(CUSTOM *pCc)
{
typedef enum {
	REMOVE,
	BOX_DRAW,
	RUN_DRAW,
    BAR_DRAW,
	GRID_DRAW
} CC_DRAW_STATES;


static CC_DRAW_STATES state = REMOVE;
static SHORT counter;
static SHORT delta;

    switch(state){

        case REMOVE:
            if(GetState(pCc,CC_HIDE)){
                if(IsDeviceBusy())
                    return 0;
                SetColor(pCc->pGolScheme->CommonBkColor);
                Bar(pCc->left,pCc->top,pCc->right,pCc->bottom);
                return 1;
            }
            state = BOX_DRAW;

        case BOX_DRAW:

            if(GetState(pCc,CC_DRAW)){

                GOLPanelDraw(pCc->left,pCc->top,
                             pCc->right,pCc->bottom,0,
                             pCc->pGolScheme->Color0,
                             pCc->pGolScheme->EmbossDkColor,
                             pCc->pGolScheme->EmbossLtColor,
                             NULL, GOL_EMBOSS_SIZE);

                state = RUN_DRAW;

                case RUN_DRAW:

                    if(!GOLPanelDrawTsk())
                        return 0;
            }

            state = BAR_DRAW;

        case BAR_DRAW:

            if(IsDeviceBusy())
                return 0;

            if(pCc->prevPos > pCc->pos){
 
               SetColor(pCc->pGolScheme->Color1);
               Bar(pCc->left+GOL_EMBOSS_SIZE,
                   pCc->pos,pCc->right-GOL_EMBOSS_SIZE,pCc->prevPos);

            }else{

               SetColor(pCc->pGolScheme->Color0);
               Bar(pCc->left+GOL_EMBOSS_SIZE,
                   pCc->prevPos,pCc->right-GOL_EMBOSS_SIZE,pCc->pos);
            }

            SetColor(pCc->pGolScheme->TextColor0);
            counter = 1;
            delta = (pCc->bottom-pCc->top-(2*GOL_EMBOSS_SIZE))>>3;
            state = GRID_DRAW;

        case GRID_DRAW:
            while(counter<8){
                if(IsDeviceBusy())
                    return 0;
                Bar(pCc->left+GOL_EMBOSS_SIZE,
                     pCc->top+GOL_EMBOSS_SIZE+counter*delta,
                     pCc->right-GOL_EMBOSS_SIZE,
                     pCc->top+GOL_EMBOSS_SIZE+counter*delta);
                counter++;
            }          
            pCc->prevPos = pCc->pos;
            state = REMOVE;
            return 1;
    }
    return 1;
}

#endif // USE_CUSTOM
