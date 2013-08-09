#include "MainDemo.h"
 
unsigned char textbuffer[41]={0x00};

unsigned char TestRecord[64]={0x00};

#ifdef USECABLE
#define RX1_BUFFLEN 50
#define TX1_BUFFLEN 550
#else
#define RX1_BUFFLEN 255
#define TX1_BUFFLEN 255
#endif

volatile unsigned char PrintDataBuff[600];

//static unsigned int Datanum;
volatile unsigned char tx1_buff[TX1_BUFFLEN],*tx1_ptr,*tx1_ptrend;

volatile unsigned int uart1_rtimer,t1len;

volatile unsigned char rx1_buff[RX1_BUFFLEN],*rx1_ptr;


volatile BOOL SPrint = FALSE,endprint = FALSE;

static uchar pn=0;

extern XCHAR 	dateTimeStr[26];

volatile unsigned char Printbuff[256];
extern volatile BOOL ChoicePrint;

 extern volatile BYTE Datetype;
volatile unsigned int Sprinnum=0,Datanum=0;
unsigned char p_n=0;

extern volatile unsigned int serialnumberRecord;


/*---------------------------------------------------------------------
  Function Name: BCD_to_ASCII
  Description:   
  Inputs:        None
  Returns:       
-----------------------------------------------------------------------*/
void BCD_to_ASCII(const uchar Table[],uchar n,uchar Offset,uchar m)
{
    uchar tem_hign,tem_low;
    uchar iii;
    uchar *p_TestRecord = TestRecord;
    uchar *p_tem=Table;

    for(iii = 0; iii < n; iii++)
    {
        tem_low = *(p_TestRecord+Offset+iii+m);

        tem_hign = *(p_TestRecord+Offset+iii+m);

        tem_low &= 0x0f;

        tem_low += 0x30;

        tem_hign &= 0xf0;

        tem_hign>>= 4;

        tem_hign += 0x30; 
    
        *(p_tem+iii*2) = tem_hign;

        *(p_tem+(iii*2+1)) = tem_low;        
    }
       
}

void ASCII_to_BCD(const uint Table,uchar n,uchar Offset,uchar m)
{
    uchar tem_hign,tem_low;
    uchar iii;
    uchar *p_TestRecord = TestRecord;
    uint *p_tem=Table;

    for(iii = 0; iii < n; iii++)
    {
        tem_low = *(p_TestRecord+Offset+iii+m);

        tem_hign = *(p_TestRecord+Offset+iii+m);

        tem_low &= 0x0f;

        //tem_low += 0x30;

        tem_hign &= 0xf0;

        tem_hign>>= 4;

        //tem_hign += 0x30; 
    
        *(p_tem+iii*2) = (uint)tem_hign;

        *(p_tem+(iii*2+1)) = (uint)tem_low;        
    }
       
}

void ASCII_to_BCD_Eng(uchar *p_tem,uchar n,uchar Offset,uchar m)
{
    //uchar tem_hign,tem_low;
    //uchar tem;
    uchar iii;
    uchar *p_TestRecord = TestRecord;

    //for(iii = 0; iii < n; iii++)
    //{
    /*
        tem_low = *(p_TestRecord+Offset+iii+m);

        tem_hign = *(p_TestRecord+Offset+iii+m);

        tem_low &= 0x0f;

        tem_low += 0x30;

        tem_hign &= 0xf0;

        tem_hign>>= 4;

        tem_hign += 0x30; 
    
        *(p_tem+iii*2) = (uint)tem_hign;

        *(p_tem+(iii*2+1)) = (uint)tem_low; 
        */
        //*(p_tem+0)=0;
         *(p_tem+0) = (serialnumberRecord /10000) +'0';// 万位
         *(p_tem+1) = (serialnumberRecord /1000 %10) +'0';// 千位
		*(p_tem+2) = (serialnumberRecord /100 %10) +'0';// 百位
		*(p_tem+3) = (serialnumberRecord /10 %100%10) +'0';// 十位
		*(p_tem+4) = (serialnumberRecord %10) +'0';// 个位
        
        //*(p_tem+iii) = *(p_TestRecord+Offset+iii+m);
        //*(p_tem+iii) = tem;
        
    //}
       
}
/*---------------------------------------------------------------------
  Function Name:   Byte_to_PRN_TextBuffer
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Byte_to_PRN_TextBuffer(uchar a,int b)
{
 PrintDataBuff[Datanum] = a;

 Datanum++;
}

/*
void initialBlueTooth(void)
{

 _TRISE8 = 1;

 _TRISE9 = 1;

 //_TRISG0 = 0;

 _TRISC1 = 1;

 _TRISC2 = 0;

 PORTCbits.RC2=1;


}
*/
void initialUART1(void)
{

     U1MODE = 0xa000;
	 U1STA  = 0x8510;               
     U1BRG=103;

	  _U1RXIF = 0;             //clear the receive interrupt flag
        _U1TXIF = 0;             //clear the transmit interrupt flag
         _U1RXIF = 0;             //clear the transmit interrupt flag
        _U1RXIP = 5;             //set the receive priority level 5
        _U1TXIP = 5;             //set the transmit priority level 5
 
        _U1RXIE = 1;             //enable the receive interrupt

        _U1TXIE = 0;             //disable the transmit interrupt
        
	// _TRISB6 = 1;//u1tx input
	// AD1PCFGbits.PCFG6 = 1;//
	// AD1PCFGbits.PCFG5 = 1;//
     Nop();
	 _TRISF5 = 0;//u1rx output
     Nop();
    tx1_ptr = &tx1_buff[0];
	rx1_ptr = &rx1_buff[0];

	//initialBlueTooth();
     
}

//uart1 send interrupt 
void __attribute__ ((__interrupt__)) _U1TXInterrupt(void)
{
	DISICNT = 0x3FFF; /* disable interrupts */
	
	_U1TXIF = 0;
	
	if((t1len != 0) && (tx1_ptr < &tx1_buff[TX1_BUFFLEN])) 
	{
		
		U1TXREG = *tx1_ptr++;
		
		t1len--;
		
	}
	else _U1TXIE = 0;
	DISICNT = 0x0000; /* enable interrupts */
}
/**********************************************************************/
//uart1 receive interrupt 
void __attribute__ ((__interrupt__)) _U1RXInterrupt(void)
{
	DISICNT = 0x3FFF; /* disable interrupts */
	unsigned char i;
	_U1RXIF = 0;
	while(U1STAbits.URXDA == 1) 
	{
		i = U1RXREG;
		if(0x10==i)
			SPrint = TRUE;
		if(0xaa==i)
			endprint = TRUE;
		if(rx1_ptr < &rx1_buff[RX1_BUFFLEN])
		{
			*rx1_ptr++ = i;
			*rx1_ptr = 0;
		}
	}
	DISICNT = 0x0000; /* enable interrupts */
}

void UART1transtart(void)
{
	
	t1len = tx1_ptrend - &tx1_buff[0];

    tx1_ptr = &tx1_buff[0];
    
    _U1TXIE = 1;
    
    t1len--;
    
    U1TXREG = *tx1_ptr++;
    
}

void initPrint(void)
{
/*
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x1b;
   
    tx1_buff[1]  = 0x40;  
       
    tx1_ptrend   += 2; 

	UART1transtart();
	*/
	
}

void StartPrint(void)
{
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x1b;
   
    tx1_buff[1]  = 0xaa;



/*
	if(Flashbuff[10]==0&&Flashbuff[11]==0)
          tx1_buff[2]=0x01;//(EEPROMReadByte(PrintSelectAddress)-0x30);测试值为0  只打一份
	
	else 
	*/
    tx1_buff[2]=(EEPROMReadByte(PrintSelectAddress+1)-0x30);


   // tx1_buff[2]  = 0x01; 
     
       
    tx1_ptrend   += 3; 

	UART1transtart();
}

/*---------------------------------------------------------------------
  Function Name: Off_Printer
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Off_Printer(void)
{
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

   
    tx1_buff[0]  = 0x1b;
   
    tx1_buff[1]  = 0xbb;

	tx1_buff[2]  = 0xbb;

	tx1_ptrend   += 3; 

	UART1transtart();

}
/*---------------------------------------------------------------------
  Function Name: Search_Table
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
uchar Search_Table(uchar Table)
{
    unsigned char Table_ru_display[]=   //显示屏所用的省份简称的汉字代码
    { 
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 
    0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 
    0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D,
    0x9E, 0x9F, 0x7F, 0x7C, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 
    0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
    0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0x7D
    };

    unsigned char Table_ru_printer[]= //打印机所用的省份简称的汉字代码
    {
     0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
     0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,
     0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,
     0x9e,0x9f,0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,
     0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,
     0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,
     0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3
    };

    char tem_2;

    char loop_E;



    for(loop_E = 0; loop_E < 68; loop_E++)
    {
        if(Table_ru_display[loop_E] == Table)
           {
            tem_2 = Table_ru_printer[loop_E];
            break;
           }
    }

    return tem_2;


}

uchar Search_zero(uchar Table[])
{
    uchar pzero;
    uchar loop_E;


    for(loop_E = 0; loop_E < 20; loop_E++)
    {
        if(Table[loop_E] == 0)
           {
            pzero = loop_E;
            break;
           }
    }

    return pzero;

}
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_WAT89EC_8
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Alcovisor_Mercury(void)
{
#ifdef DOT
 unsigned char Table_Text_Alcovisor_Mercury[]= //"               Alcovisor-Jupiter"
 {
    0x20,0x20,0x20,0x20,0x20,
	'M','a','r','k',' ','V',' ','p','l','u','s',0x0d
 };


    CopyChar(&PrintDataBuff[Datanum],Table_Text_Alcovisor_Mercury,17);//30-57

	Datanum = Datanum+17;
#else
 unsigned char Table_Text_Alcovisor_Mercury[]= //"               Alcovisor-Jupiter"
 {
    0x20,0x20,0x20,
	'A','l','c','o','v','i','s','o','r','-','M','a','r','k',' ','V',' ','p','l','u','s',0x0d
 };


    CopyChar(&PrintDataBuff[Datanum],Table_Text_Alcovisor_Mercury,28);//30-57

	Datanum = Datanum+28;

#endif
}


/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_YIQIHAO
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_YIQIHAO()
{
  #ifdef POLVersion
  unsigned char Table_Text_YIQIHAO_Eng[]="Nr.fabryczny:";  //Serial Number:
  #else
  unsigned char Table_Text_YIQIHAO_Eng[]="Serial No:";  //Serial Number:
  #endif
  unsigned char Table_YIQIHAO[4] = {0x00};
  
  uchar *p_TestRecord = TestRecord;
  
  uchar Table_TestRecord[9]={0x00};
  
  *(p_TestRecord+n_Offset_JILUHAO+0) = EEPROMReadByte(ID_JIQIADDRESS);
  
  *(p_TestRecord+n_Offset_JILUHAO+1) = EEPROMReadByte(ID_JIQIADDRESS+1);
  
  
  *(p_TestRecord+n_Offset_JILUHAO+2) = EEPROMReadByte(ID_JIQIADDRESS+2);
  
  *(p_TestRecord+n_Offset_JILUHAO+3) = EEPROMReadByte(ID_JIQIADDRESS+3);
  
  BCD_to_ASCII(Table_TestRecord,4,n_Offset_JILUHAO,0);
  
  Table_TestRecord[8]= 0x0D;
  
  #ifdef POLVersion
  
  CopyChar(&PrintDataBuff[Datanum],Table_Text_YIQIHAO_Eng,13);
  
  Datanum = Datanum+13;
  #else
  CopyChar(&PrintDataBuff[Datanum],Table_Text_YIQIHAO_Eng,10);
  
  Datanum = Datanum+10;
  #endif	  
  //CopyChar(tx1_buff,Table_Text_YIQIHAO_Eng,5);
  CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,9);//63-71
  
  Datanum = Datanum+9;



}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_JILUHAO
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_JILUHAO()
{
#ifdef DOT
    unsigned char Table_Text_JILUHAO_Ru[]=  "Test No:";//Record No.：

#elif  defined(POLVersion)

     unsigned char Table_Text_JILUHAO_Ru[]=  "Number pomiaru:";//Record No.：

#else

    unsigned char Table_Text_JILUHAO_Ru[]=  "Record No. :";//Record No.：
    
#endif

    uchar *p_TestRecord = TestRecord;


    unsigned char Table_TestRecord[6]={0x00};

    *(p_TestRecord+n_Offset_JILUHAO+0) = Printbuff[0];

    *(p_TestRecord+n_Offset_JILUHAO+1) = Printbuff[1];

    *(p_TestRecord+n_Offset_JILUHAO+2) = Printbuff[2];

    ASCII_to_BCD_Eng(Table_TestRecord,3,n_Offset_JILUHAO,0);
    Table_TestRecord[5]= 0x0d;
 	
#ifdef DOT
    CopyChar(&PrintDataBuff[Datanum],Table_Text_JILUHAO_Ru,8);//30-57
	Datanum = Datanum+8;

#elif  defined(POLVersion)
    CopyChar(&PrintDataBuff[Datanum],Table_Text_JILUHAO_Ru,15);//30-57
	Datanum = Datanum+15;
#else	
    CopyChar(&PrintDataBuff[Datanum],Table_Text_JILUHAO_Ru,12);//30-57
	Datanum = Datanum+12;
#endif	
	CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,6);

	Datanum = Datanum+6;

 
}



void Print_Table_Text_RIQI()
{
#ifdef POLVersion
    unsigned char Table_Text_RIQI_EN[]= "Data:";  //Date?
#else

    unsigned char Table_Text_RIQI_EN[]= "Date:";  //Date：
#endif

	unsigned char MonthtempH,MonthtempL;
	
    uchar *p_TestRecord = TestRecord;

    *(p_TestRecord+n_Offset_RIQI+0) = Printbuff[3];

    *(p_TestRecord+n_Offset_RIQI+1) = Printbuff[4];

    *(p_TestRecord+n_Offset_RIQI+2) = Printbuff[5];

    unsigned char Table_TestRecord[3]={0x00};

if(Datetype == Mdy)
{

    CopyChar(&PrintDataBuff[Datanum],Table_Text_RIQI_EN,5);
	Datanum = Datanum+5;

	MonthtempH = (Printbuff[4]>>4)&0x0f;
	MonthtempL =  Printbuff[4]&0x0f; 
    Byte_to_PRN_TextBuffer(MonthtempH+'0',11);
	Byte_to_PRN_TextBuffer(MonthtempL+'0',12);

	
	PrintDataBuff[Datanum] = '/';
	Datanum++;	

	
    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,2);
    CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);
	Datanum = Datanum+2;


}
else{


    CopyChar(&PrintDataBuff[Datanum],Table_Text_RIQI_EN,5);
	Datanum = Datanum+5;


    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,2);

    CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum+2;

	PrintDataBuff[Datanum] = '/';

	Datanum++;	
	
	MonthtempH = (Printbuff[4]>>4)&0x0f;
	MonthtempL =  Printbuff[4]&0x0f; 

    Byte_to_PRN_TextBuffer(MonthtempH+'0',11);

	Byte_to_PRN_TextBuffer(MonthtempL+'0',12);	
}
	
    PrintDataBuff[Datanum] = '/';//Byte_to_PRN_TextBuffer(',',14);

	Datanum++;

    PrintDataBuff[Datanum] = '2';//Byte_to_PRN_TextBuffer('2',15);

	Datanum++;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',16);

	Datanum++;

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,0);

    CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum+2;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
	
 
}

 
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_SHIJIAN
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_SHIJIAN()
{
#ifdef POLVersion
    unsigned char Table_Text_SHIJIAN_EN[]= "Godzina: "; //Time:

#else
    unsigned char Table_Text_SHIJIAN_EN[]= "Time:"; //Time:

#endif
	
	
    uchar *p_TestRecord = TestRecord;

    *(p_TestRecord+n_Offset_SHIJIAN+0) = Printbuff[6];

    *(p_TestRecord+n_Offset_SHIJIAN+1) = Printbuff[7];

	*(p_TestRecord+n_Offset_SHIJIAN+2) = Printbuff[8];


    uchar Table_TestRecord[3]={0x0000};

#ifdef POLVersion
	CopyChar(&PrintDataBuff[Datanum],Table_Text_SHIJIAN_EN,9);

	Datanum = Datanum + 9;
#else

	CopyChar(&PrintDataBuff[Datanum],Table_Text_SHIJIAN_EN,5);

	Datanum = Datanum + 5;
#endif
    BCD_to_ASCII(Table_TestRecord,1,n_Offset_SHIJIAN,0);

	CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum + 2;

    PrintDataBuff[Datanum] = ':';//Byte_to_PRN_TextBuffer(':',10);

	Datanum++;

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_SHIJIAN,1);


	CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum + 2;

    PrintDataBuff[Datanum] = ':';//Byte_to_PRN_TextBuffer(':',10);

	Datanum++;

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_SHIJIAN,2);


	CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum + 2;	

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_GUILING归零
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_GUILING()
{

#ifdef POLVersion
    unsigned char Table_Text_GUILING_Eng[]= "Sprawdzenie zera:"; //Time:

#else
    unsigned char Table_Text_GUILING_Eng[]= "Blank:"; //Blank：
#endif    

    unsigned int Table_Text_Ling[]= //
    {
     0x0000
    };
    unsigned char Table_Text_mg_100ml[] = // "mg/100mL"
    {0x6d,0x67,0x2f,0x31,0x30,
      0x30,0x6d,0x4c};
    unsigned char Table_Text_mg_l[] = // "mg/L"
    {0x6d,0x67,0x2f,0x4c};
    unsigned char Table_Text_mg_100[] = //"%"
    {0x25};
    unsigned char Table_Text_mg_1000ml[] = //"%."
    {0x25,0x2e};
    unsigned char Table_Text_g_100ml[] = //"g/100mL"
    {0x67, 0x2F, 0x31, 0x30, 0x30,
      0x6D, 0x4c};
    unsigned char Table_Text_g_210l[] = //"g/210L"
    {0x67, 0x2F, 0x32, 0x31,
      0x30, 0x4c};
    unsigned char Table_Text_ug_100ml[] = //"ug/100mL"
    {0x75, 0x67, 0x2F, 0x31,
     0x30, 0x30, 0x6D, 0x4c};
    unsigned char Table_Text_mg_ml[] = //"mg/mL"
    {0x6D, 0x67, 0x2F, 0x6D, 0x4c};
    unsigned char Table_Text_mg100[] = //"g/L"
    {0x67, 0x2F, 0x4c};

#ifdef POLVersion
	CopyChar(&PrintDataBuff[Datanum],Table_Text_GUILING_Eng,17);
	Datanum = Datanum+17;
	PrintDataBuff[Datanum] = 0x0d;
	Datanum++;

#else
	CopyChar(&PrintDataBuff[Datanum],Table_Text_GUILING_Eng,6);
	Datanum = Datanum+6;
#endif
	

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',8);

	Datanum++;

    PrintDataBuff[Datanum] = '.';//Byte_to_PRN_TextBuffer('.',9);

	Datanum++;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',10);

	Datanum++;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',11);

	Datanum++;

    PrintDataBuff[Datanum] = '0';//Byte_to_PRN_TextBuffer('0',11);

	Datanum++;
	
    PrintDataBuff[Datanum] = ' ';//Byte_to_PRN_TextBuffer(' ',12);

	Datanum++;

	switch(Printbuff[9])
    {

         case 0://mg/100mL

		CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_100ml,8);

	    Datanum = Datanum+8;
         break;

         case 1://mg/L
 
		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_l,4);

	    Datanum = Datanum+4;
         break;

         case 2://%

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_100,1);

	    Datanum = Datanum+1;
         break;       

         case 3://g/100mL

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_g_100ml,7);

	    Datanum = Datanum+7;
         break;

         case 4://%.

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_1000ml,2);

	    Datanum = Datanum+2;
 
         break;         

         case 5://ug/100mL

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_ug_100ml,8);

	    Datanum = Datanum+8;
         break;

         case 6://mg/mL

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_ml,5);

	    Datanum = Datanum+5;	 
         break;

         case 7://mg%

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg100,3);

	    Datanum = Datanum+3;
         break;

         case 8://g/210L

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_g_210l,6);

	    Datanum = Datanum+6;		 
         break;

         default: 
         break;
		}
	
    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
}



/*---------------------------------------------------------------------
  Function Name: Last_Calibration_Date最后一次校准时间
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Last_Calibration_Date()
{

#ifdef POLVersion
    unsigned char Table_Last_Calibration_Date[]="Data kalibracji:"; //Last Calibration Date：
    CopyChar(&PrintDataBuff[Datanum],Table_Last_Calibration_Date,16);
	Datanum = Datanum+16;
#else
    unsigned char Table_Last_Calibration_Date[]="Last Calibration Date:"; //Last Calibration Date：
    CopyChar(&PrintDataBuff[Datanum],Table_Last_Calibration_Date,22);
	Datanum = Datanum+22;
 #endif 
    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
}



void Print_Last_Calibration_Date()
{
     uint Table_TestRecord[3]={0};

   unsigned char MonthtempH,MonthtempL;	 

unsigned char datetemp;
unsigned char TempCalibrationBuff[]={0x05,0x03,0x09,0};//月 日 年
datetemp=EEPROMReadByte(MarkYrDataAddress);
TempCalibrationBuff[2]=datetemp;//((datetemp/10)&0x0f)<<4+(datetemp%10);
datetemp=EEPROMReadByte(MarkMthDataAddress);
TempCalibrationBuff[0]=datetemp;//((datetemp/10)&0x0f)<<4+(datetemp%10);
datetemp=EEPROMReadByte(MarkDayDataAddress);
TempCalibrationBuff[1]=datetemp;//((datetemp/10)&0x0f)<<4+(datetemp%10);



   	MonthtempH = (TempCalibrationBuff[0]>>4)&0x0f;
	MonthtempL =  TempCalibrationBuff[0]&0x0f;

	uchar *p_TestRecord = TestRecord;

    *(p_TestRecord+n_Offset_RIQI+0) = TempCalibrationBuff[1];

    *(p_TestRecord+n_Offset_RIQI+1) = TempCalibrationBuff[0];

    *(p_TestRecord+n_Offset_RIQI+2) = TempCalibrationBuff[2];

	
    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,0);

    CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);

	Datanum = Datanum+2;//TextBlock_to_PRN_TextBuffer(Table_TestRecord,2,0);//日    

    Byte_to_PRN_TextBuffer('/',2);


    Byte_to_PRN_TextBuffer(MonthtempH+'0',3);

	Byte_to_PRN_TextBuffer(MonthtempL+'0',4);

    Byte_to_PRN_TextBuffer('/',5);

    Byte_to_PRN_TextBuffer('2',6);

    Byte_to_PRN_TextBuffer('0',7);

    BCD_to_ASCII(Table_TestRecord,1,n_Offset_RIQI,2);

    CopyChar(&PrintDataBuff[Datanum],Table_TestRecord,2);//TextBlock_to_PRN_TextBuffer(Table_TestRecord,2,8);// 年

	Datanum = Datanum+2;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
}


/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_RefuseTest
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_RefuseTest()
{
#ifdef POLVersion
    unsigned char Table_Text_JUJUECESHI_Eng[]= "Odmowa badania";//Refuse Test

	CopyChar(&PrintDataBuff[Datanum],Table_Text_JUJUECESHI_Eng,14);

    Datanum = Datanum+14;
#else
    unsigned char Table_Text_JUJUECESHI_Eng[]= "Refused";//Refuse Test

	CopyChar(&PrintDataBuff[Datanum],Table_Text_JUJUECESHI_Eng,7);

    Datanum = Datanum+7;
#endif
    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	
}
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_Discontiued
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Discontiued()
{

#ifdef POLVersion
    unsigned char Table_Text_Discontiued[]= "Odmowa badania";//Refuse Test

	CopyChar(&PrintDataBuff[Datanum],Table_Text_Discontiued,14);

    Datanum = Datanum+14;
#else	
    unsigned char Table_Text_Discontiued[]= "Discontinued";//test mode:discontinued
 
	CopyChar(&PrintDataBuff[Datanum],Table_Text_Discontiued,12);

    Datanum = Datanum+12;
#endif
    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	

}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_TestMode
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_TestMode()
{
    unsigned char Table_Text_TestMode[]= "Tryb pomiaru:";//"Test mode:"; //Test mode: Passive

	

	CopyChar(&PrintDataBuff[Datanum],Table_Text_TestMode,13);

   Datanum = Datanum +13;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;   
 
}
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_TestMode
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Passive()
{
    unsigned char Table_Text_Passive[]= "Pomiar manualny";//"Test mode:"; //Test mode: Passive

	

	CopyChar(&PrintDataBuff[Datanum],Table_Text_Passive,15);

   Datanum = Datanum +15;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++; 

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;
 
}

#ifdef SA_VERSION
void Print_Screenmode()
{
    unsigned char Table_Text_TestMode[]= "Test mode: Screen";
	unsigned char result_alcoholedetect[] = "Result: Alcohol Detect";
	unsigned char result_noalcohole[] = "Result: No Alcohol";

	

	CopyChar(&PrintDataBuff[Datanum],Table_Text_TestMode,17);

    Datanum = Datanum +17;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++; 

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

    if(Screen_mode & Printbuff[10])
    	{
			CopyChar(&PrintDataBuff[Datanum],result_alcoholedetect,22);

    		Datanum = Datanum +22;
    	}
	else
		{
			CopyChar(&PrintDataBuff[Datanum],result_noalcohole,18);

    		Datanum = Datanum +18;
		}
		
	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	
 
}
#endif

#ifndef POLVersion
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_TestModePassive
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_TestModePassive()
{
    unsigned char Table_Text_TestModePassive_Eng[]= "Test mode: Manual"; //Test mode: Passive

	

	CopyChar(&PrintDataBuff[Datanum],Table_Text_TestModePassive_Eng,17);

   Datanum = Datanum +17;

    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;   
 
}
#endif
/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_TestModeAuto
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_TestModeAuto()
{

#ifdef POLVersion
    unsigned char Table_Text_TestModeAuto[]= "Tryb pomiaru: Auto";//Test mode: Auto
	CopyChar(&PrintDataBuff[Datanum],Table_Text_TestModeAuto,18);
    Datanum = Datanum +18;
#else
    unsigned char Table_Text_TestModeAuto[]= "Test mode: Auto";//Test mode: Auto
	CopyChar(&PrintDataBuff[Datanum],Table_Text_TestModeAuto,15);
    Datanum = Datanum +15;
#endif
    PrintDataBuff[Datanum] = 0x0d;

	Datanum++;	
}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_JIUJINGHANLIANG
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_JIUJINGHANLIANG()
{
#ifdef POLVersion
    
    unsigned char Table_Text_JIUJINGHANLIANG_Eng[]= "Wynik pomiaru:";//Alcohol Content：

	CopyChar(&PrintDataBuff[Datanum],Table_Text_JIUJINGHANLIANG_Eng,14);

    Datanum = Datanum + 14;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++; 
#else

    unsigned char Table_Text_JIUJINGHANLIANG_Eng[]= "Alc Cont:";//Alcohol Content：


	CopyChar(&PrintDataBuff[Datanum],Table_Text_JIUJINGHANLIANG_Eng,9);

    Datanum = Datanum + 9;

	//PrintDataBuff[Datanum] = 0x0d;

	//Datanum++;
#endif
}

/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_NONGDU
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_NONGDU()
{
    unsigned char Table_Text_mg_100ml[] = // "mg/100mL"
    {0x6d,0x67,0x2f,0x31,0x30,
      0x30,0x6d,0x4c};
    unsigned char Table_Text_mg_l[] = // "mg/L"
    {0x6d,0x67,0x2f,0x4c};
    unsigned char Table_Text_mg_100[] = //"%"
    {0x25};
    unsigned char Table_Text_mg_1000ml[] = //"%."
    {0x25,0x2e};
    unsigned char Table_Text_g_100ml[] = //"g/100mL"
    {0x67, 0x2F, 0x31, 0x30, 0x30,
      0x6D, 0x4c};
    unsigned char Table_Text_g_210l[] = //"g/210L"
    {0x67, 0x2F, 0x32, 0x31,
      0x30, 0x4c};
    unsigned char Table_Text_ug_100ml[] = //"ug/100mL"
    {0x75, 0x67, 0x2F, 0x31,
     0x30, 0x30, 0x6D, 0x4c};
    unsigned char Table_Text_mg_ml[] = //"mg/mL"
    {0x6D, 0x67, 0x2F, 0x6D, 0x4c};
    unsigned char Table_Text_mg100[] = //"g/L"
    {0x67, 0x2F, 0x4c};

    unsigned char temp_nongduH,temp_nongduL;
    
  
    switch(Printbuff[9])
    {
        case 0://mg/100mL
        temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	 temp_nongduL = Printbuff[10]&0x0f;
      
        Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

        Byte_to_PRN_TextBuffer(temp_nongduL+'0',1);
      
        temp_nongduH = (Printbuff[11]>>4)&0x0f;
        temp_nongduL = Printbuff[11]&0x0f;

		
        Byte_to_PRN_TextBuffer(temp_nongduH+'0',2);

		Byte_to_PRN_TextBuffer('.',3);

		Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

        //TextBlock_to_PRN_TextBuffer(Table_Text_mg_100ml,9,3);

		Byte_to_PRN_TextBuffer(' ',5);

		CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_100ml,8);

        Datanum = Datanum + 8;
		
         break;

         case 1://mg/L
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

		 Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5); 

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_l,4);

		 Datanum = Datanum+4;

         break;

         case 2://%
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);
		 
		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_100,1);

         Datanum = Datanum + 1;
         break;       

         case 3://g/100mL
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         //TextBlock_to_PRN_TextBuffer(Table_Text_g_100ml,9,4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_g_100ml,7);

         Datanum = Datanum + 7;
         break;

         case 4://%.
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_1000ml,2);

         Datanum = Datanum +2;
         break;         

         case 5://ug/100mL
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	  temp_nongduL = Printbuff[10]&0x0f;

      	  Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',1);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',2);

		 Byte_to_PRN_TextBuffer('.',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_ug_100ml,8);

         Datanum = Datanum + 8;
				 
         break;

         case 6://mg/mL
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
         temp_nongduL = Printbuff[10]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg_ml,5);

         Datanum = Datanum + 5;         	 
         break;

         case 7://mg%
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	  temp_nongduL = Printbuff[10]&0x0f;

      	  Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

		  Byte_to_PRN_TextBuffer('.',1);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',2);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',3);

		 

		 Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_mg100,3);

         Datanum = Datanum + 3;
         break;

         case 8://g/210L
         temp_nongduH = (Printbuff[10]>>4)&0x0f;
      	  temp_nongduL = Printbuff[10]&0x0f;

      	  Byte_to_PRN_TextBuffer(temp_nongduH+'0',0);

         Byte_to_PRN_TextBuffer(temp_nongduL+'0',1);
      
         temp_nongduH = (Printbuff[11]>>4)&0x0f;
         temp_nongduL = Printbuff[11]&0x0f;
      
         Byte_to_PRN_TextBuffer(temp_nongduH+'0',2);

		 Byte_to_PRN_TextBuffer('.',3);

		 Byte_to_PRN_TextBuffer(temp_nongduL+'0',4);

		 Byte_to_PRN_TextBuffer(' ',5);

		 CopyChar(&PrintDataBuff[Datanum],Table_Text_g_210l,6);

         Datanum = Datanum + 6;
         break;

         default: 
         break;
    }

	 PrintDataBuff[Datanum] = 0x0d;

	Datanum++;



}


void Print_Table_Text_Testee(void)
{
#ifdef POLVersion
 unsigned char Table_Text_Alcovisor_Testee[]="Osoba badana:"; 

 CopyChar(&PrintDataBuff[Datanum],Table_Text_Alcovisor_Testee,13);//30-57

 Datanum = Datanum+13;
#else

 unsigned char Table_Text_Alcovisor_Testee[]="Testee:"; 



 CopyChar(&PrintDataBuff[Datanum],Table_Text_Alcovisor_Testee,7);//30-57

 Datanum = Datanum+7;
#endif	
   //Print_datareturn();

   Print_UnderLine();

}


void Print_Table_Text_Place(void)
{

    unsigned char Table_Text_Alcovisor_Place[]="Place:"; 



    CopyChar(&PrintDataBuff[Datanum],Table_Text_Alcovisor_Place,6);//30-57

   Datanum = Datanum+6;
	
   //Print_datareturn();

   Print_UnderLine();
}


void Print_Table_Text_Vehicleno(void)
{

#ifdef POLVersion
    unsigned char Print_Table_Text_Vehicleno[]="Numer dokumentu:"; 
    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Vehicleno,16);//30-57
    Datanum = Datanum+16;

#else
    unsigned char Print_Table_Text_Vehicleno[]="Vehicle No. :"; 
    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Vehicleno,13);//30-57
    Datanum = Datanum+13;
#endif	
   //Print_datareturn();

   Print_UnderLine();
}


void Print_Table_Text_Licenceno(void)
{
#ifdef POLVersion
    unsigned char Print_Table_Text_Licenceno[]="Numer pojazdu:"; 

    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Licenceno,14);//30-57

   Datanum = Datanum+14;
#else
    unsigned char Print_Table_Text_Licenceno[]="Licence No. :"; 



    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Licenceno,13);//30-57

   Datanum = Datanum+13;
#endif	
   //Print_datareturn();

   Print_UnderLine();
}

void Print_Table_Text_SignaturePL(void)
{

    unsigned char Print_Table_Text_Signature[]="Podpis badanego:"; 



    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Signature,16);//30-57

   Datanum = Datanum+16;
	
  // Print_datareturn();

   Print_UnderLine();
}

void Print_Table_Text_Operator(void)
{

    unsigned char Print_Table_Text_Operator[]="Operator:"; 



    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Operator,9);//30-57

   Datanum = Datanum+9;
	
  //Print_datareturn();

   Print_UnderLine();
}


void Print_Table_Text_Operatorno(void)
{

    unsigned char Print_Table_Text_Operatorno[]="Operator No. :"; 



    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Operatorno,14);//30-57

   Datanum = Datanum+14;
	
 //  Print_datareturn();

   Print_UnderLine();
}




void Print_Table_Text_Department(void)
{

   unsigned char Print_Table_Text_Department[]="Department:"; 



    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Department,11);//30-57

   Datanum = Datanum+11;
	
  // Print_datareturn();

   Print_UnderLine();
}

void Print_Table_Text_Remarks(void)
{
#ifdef POLVersion

    unsigned char Print_Table_Text_Remarks[]="Notatki - Uwagi:"; 

    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Remarks,16);//30-57

   Datanum = Datanum+16;
#else
    unsigned char Print_Table_Text_Remarks[]="Remarks:"; 



    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Remarks,8);//30-57

   Datanum = Datanum+8;
#endif	
   //Print_datareturn();

   Print_UnderLine();
}

void Print_Table_Text_Signature(void)
{
#ifdef POLVersion

    unsigned char Print_Table_Text_Signature[]="Podpis operatora:"; 

    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Signature,17);//30-57

   Datanum = Datanum+17;
#else
    unsigned char Print_Table_Text_Signature[]="Signature:"; 



    CopyChar(&PrintDataBuff[Datanum],Print_Table_Text_Signature,10);//30-57

   Datanum = Datanum+10;
#endif	
  // Print_datareturn();

   Print_UnderLine();
}



/*---------------------------------------------------------------------
  Function Name: Print_Table_Text_teperature
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_temperature()
{
  //XCHAR  recordtem;
  
    unsigned char Table_Text_temperature[]="Temperature:"; //Refuse Test

  


   	Table_Text_temperature[14]=(Printbuff[13]%10+'0');
                
	Table_Text_temperature[13]=(Printbuff[13]/10+'0');

	if(Flashbuff[14]==1)
		Table_Text_temperature[12]=0x2D;
	else
		Table_Text_temperature[12]=0x20;

	Table_Text_temperature[15]=0xc4;

	//Table_Text_temperature[16]=0x43;
	
    CopyChar(&PrintDataBuff[Datanum],Table_Text_temperature,16);

	Datanum = Datanum + 16;

	PrintDataBuff[Datanum] = 0x0d;

	Datanum++;

       
	//Print_UnderLine();
       
	
}

void Print_UnderLine(void)
{

    unsigned char  N_blank = 0;

#ifdef USECABLE
      PrintDataBuff[Datanum++]=0x0d;
  
	for(N_blank=0;N_blank<30;N_blank++)
  
      PrintDataBuff[Datanum++]='_';
	
      PrintDataBuff[Datanum++]=0x0d;
	
#else
	
	for(N_blank=0;N_blank<11;N_blank++)
  
      PrintDataBuff[Datanum++]=0x0d;
   
#endif

	  PrintDataBuff[Datanum++] = 0x0a;

	
	
}

void Print_datareturn(void)
{


	PrintDataBuff[Datanum] = 0x0a;

	Datanum++;
	
	
}

void Printdata(XCHAR *Str)

{

	uchar  pz;
		
    int m;
	
    uchar ItemString[20] = {0x00}; 

	for(m=0;m<20;m++)
    {
        ItemString[m]=0;
    }
	
	
    CopyChar(ItemString, Str, 20);

    pz=Search_zero(ItemString);			   
        		
	
	CopyChar(&PrintDataBuff[Datanum],ItemString,pz);

	Datanum = Datanum + pz;
/*
	for(m=0;m<(30-pz);m++)
    {
        PrintDataBuff[Datanum+m]=0x20;
    }

	Datanum = Datanum+m;
*/
//	PrintDataBuff[Datanum] = 0x0d;

//	Datanum++;


}

void Print_ITEM(WORD ItemAddress)
{ 

	uchar *p_buffer = textbuffer;

	uchar  pz;
		
    int m;
     uchar Loop_H;

				
	DelayMs(50);
	
    for(m=0;m<41;m++)
    {
        textbuffer[m]=0x00;
    }

    EEPROMReadArray(ItemAddress,p_buffer,20); 

	   for(Loop_H = 0; Loop_H < 19; Loop_H++)
        {
            if(*(p_buffer+Loop_H) > 0x7c)
            	{
                *(p_buffer+Loop_H)=Search_Table(*(p_buffer+Loop_H));
            	}			
        }
    	 for(Loop_H = 0; Loop_H < 20; Loop_H++)
        {
		 if(*(p_buffer+Loop_H) == 0)
			{
				*(p_buffer+Loop_H) = 0x3a;
				 Loop_H++;
				*(p_buffer+Loop_H) = 0;
				break;
				}
    	 }

	pz=Search_zero(textbuffer);

	CopyChar(&PrintDataBuff[Datanum],textbuffer,pz);

	Datanum = Datanum+pz;

	PrintDataBuff[Datanum] = 0x0d;
	
    Datanum++;
}

/*---------------------------------------------------------------------
  Function Name:  Print_Table_Text_Eng
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_Table_Text_Eng(WORD ItemAddress)
{   

  uchar *p_buffer = textbuffer;
    int m;
    XCHAR ItemString[17] = {0x00}; 

    for(m=0;m<41;m++)
    {
        textbuffer[m]=0x00;
    }

    //EEPROMReadArray(&PrintDataBuff[Datanum],p_buffer,16); 
    Print_ITEM(ItemAddress);

    switch(ItemAddress)// ItemState
    {
        case Item0Address:

			if(Printbuff[49]==0)
				
        		    Print_UnderLine(); 



        	else
        		{
                  Printdata(&Printbuff[49]);
                  Print_datareturn();
        		}
                break;
				
        case Item1Address:


			if(Printbuff[68]==0)
				
				{
				
        			 Print_UnderLine();    
                      
				}
                else
                	{
                       Printdata(&Printbuff[68]);
					   Print_datareturn();

	                   
        		    }
                break;
				
        case Item2Address:

    			if(Printbuff[87]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[87]);
                   Print_datareturn(); 

				}
                break;
				
        case Item3Address:

    			if(Printbuff[106]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[106]);
                   Print_datareturn();
				}
                break;
        case Item4Address:

    			if(Printbuff[125]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[125]);
                   Print_datareturn();
				}
                break;	
        case Item5Address:

    			if(Printbuff[144]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[144]);
				   Print_datareturn();
    				}
                break;
        case Item6Address:

    			if(Printbuff[163]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[163]);
                   Print_datareturn();
				}
                break;
        case Item7Address:

    			if(Printbuff[182]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[182]);
                   Print_datareturn();
				}
				break; 	
        case Item8Address:

    			if(Printbuff[201]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[201]);
                   Print_datareturn();
				}
				break; 	
        case Item9Address:

    			if(Printbuff[220]==0)

        			Print_UnderLine();    

    			else
    				{
                   Printdata(&Printbuff[220]);
                   Print_datareturn();

				}
				break; 	                       	
        default:
                break;
    }

}

void Inputdata(void)

{
#ifdef USECABLE
      		memset(tx1_buff,0,550);
			
			memset(rx1_buff,0,50);
#else
      		memset(tx1_buff,0,250);
			
			memset(rx1_buff,0,250);
#endif			
      		Datanum = 0;

#ifdef POLVersion
         

     		Print_Table_Text_YIQIHAO();
		  
   
            Print_Table_Text_JILUHAO();
		  
 

            Print_Table_Text_RIQI();


            Print_Table_Text_SHIJIAN();


            Print_Table_Text_GUILING();



            Last_Calibration_Date();

			Print_Last_Calibration_Date();
			

            if(Printbuff[16]&(ST_REFUSE))
                Print_Table_Text_RefuseTest();
            else if(Printbuff[16]&(ST_Discontinued))
				Print_Table_Text_Discontiued();
            else 
				{
				
					if(Printbuff[16]&(ST_Passive))

						{

						Print_Table_Text_TestMode();
						Print_Table_Text_Passive();
					}

            		else 

               			Print_Table_Text_TestModeAuto();
              

               			Print_Table_Text_JIUJINGHANLIANG();


               			Print_Table_Text_NONGDU();
            	}
            	
    		PrintDataBuff[Datanum] = 0x0d;

			Datanum++;

    		PrintDataBuff[Datanum] = 0x0d;

			Datanum++;			
		   
			Print_Table_Text_Testee();
			
			Print_Table_Text_Vehicleno();
			
			Print_Table_Text_Licenceno();

			Print_Table_Text_Operator();

			Print_Table_Text_Remarks();
			

			Print_Table_Text_SignaturePL();			

			Print_Table_Text_Signature();

#else

#ifdef DOT		 
        Print_Table_Text_Alcovisor_Mercury();
#endif          


       // Datanum  = 38;
            Print_Table_Text_YIQIHAO();
		  


       // Datanum  = 52;
            Print_Table_Text_JILUHAO();
		  
 

            Print_Table_Text_RIQI();


            Print_Table_Text_SHIJIAN();


            Print_Table_Text_GUILING();

#ifdef DOT
 if(EEPROMReadByte(ID_CHECKBOXADDRESS+25))//ID_CHECKBOX6))
 	{
 #endif
            Last_Calibration_Date();

			Print_Last_Calibration_Date();
#ifdef DOT			
 	}
#endif

#ifdef SA_VERSION
if(Screen_mode & Printbuff[16])
	Print_Screenmode();
else
#endif
{
            if(Printbuff[16]&(ST_REFUSE))
                Print_Table_Text_RefuseTest();
            else if(Printbuff[16]&(ST_Discontinued))
				Print_Table_Text_Discontiued();
            else 
				{
				#ifndef DOT
					if(Printbuff[16]&(ST_Passive))

               		Print_Table_Text_TestModePassive();

            		else 

               			Print_Table_Text_TestModeAuto();
               #endif

               			Print_Table_Text_JIUJINGHANLIANG();


               			Print_Table_Text_NONGDU();
            	}
}

#ifndef DOT			   
Print_Table_Text_Testee();
Print_Table_Text_Place();
Print_Table_Text_Vehicleno();
Print_Table_Text_Licenceno();
Print_Table_Text_Operator();
Print_Table_Text_Operatorno();
Print_Table_Text_Department();
#ifndef USECABLE
Print_Table_Text_Remarks();
#endif
Print_Table_Text_Signature();
#endif

#ifdef USECABLE
PrintDataBuff[Datanum++] = 0x0d;
PrintDataBuff[Datanum++] = 0x0d;
PrintDataBuff[Datanum++] = 0x0a;
#else
Print_Table_Text_temperature();
#endif
#endif
}
/*---------------------------------------------------------------------
  Function Name: Print_TestRecord
  Description:   None
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void Print_TestRecord()
{
#ifdef USECABLE

    memset(&tx1_buff[0],0,250); 

	
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

	tx1_buff[0] = 0x0a;

	
    CopyChar(&tx1_buff[1],PrintDataBuff,Datanum);
	
    tx1_ptrend   += Datanum; 

	UART1transtart();

	
  //  StartPrint();

    SPrint=FALSE;


    Sprinnum =0;

    Datanum = 0;


	ChoicePrint = FALSE;

	SPrint=FALSE;

	p_n =0;

	Sprinnum =0;

	Datanum = 0;
#else

if(Datanum>240)
{
    memset(&tx1_buff[0],0,250); 

	
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];
   
    tx1_ptrend    = tx1_ptr; 

	tx1_buff[0] = 0x1b;

	tx1_buff[1] = 0x10;

	tx1_buff[2] = 1;

	tx1_buff[3] = 240;
	
    CopyChar(&tx1_buff[4],&PrintDataBuff[0],240);
/************for debug******
	PrintDataBuff[240] = '*';
	************/
    Sprinnum = Sprinnum+ 240;
	
    tx1_ptrend   += 244; 

	UART1transtart();

    delayms(50000);
	
    delayms(60000);

	delayms(50000);
#ifdef POLVersion
    delayms(30000);
#endif
	//delayms(60000);
}
    memset(&tx1_buff[0],0,250); 

	
	U1STAbits.UTXEN=1;
	
    tx1_ptr       = &tx1_buff[0];

   
    tx1_ptrend    = tx1_ptr; 

	tx1_buff[0] = 0x1b;

	tx1_buff[1] = 0x10;
if(Datanum>240)
	tx1_buff[2] = 2;
else
	tx1_buff[2] = 1;


	tx1_buff[3] = Datanum-Sprinnum;
	
    CopyChar(&tx1_buff[4],&PrintDataBuff[Sprinnum],Datanum-Sprinnum);

	
    Sprinnum = Datanum;

	
    tx1_ptrend   += tx1_buff[3]+4;

	UART1transtart();

    delayms(50000);
	
    delayms(50000);

	delayms(60000);
#ifdef POLVersion
	delayms(30000);
#endif
	//delayms(60000);
	
   // endprint = TRUE;

    StartPrint();

    SPrint=FALSE;


    Sprinnum =0;

    Datanum = 0;


	ChoicePrint = FALSE;

	SPrint=FALSE;

	p_n =0;

	Sprinnum =0;

	Datanum = 0;
#endif

}

