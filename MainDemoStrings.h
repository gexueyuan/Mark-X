#include "Graphics.h"

XCHAR TouchScreenStr[] = "Touch screen to continue!";
XCHAR ButtonStr[] = "Buttons";			
XCHAR CheckBoxStr[] = "Checkbox";
XCHAR RadioButtonStr[] = "Radio buttons";	
XCHAR GroupBoxStr[] = "Group Box";
XCHAR StaticTextStr[] = "Static Text";
XCHAR SliderStr[] = "Slider";			
XCHAR ProgressBarStr[] = "Progress bar";
XCHAR ListBoxStr[] = "List box";		
XCHAR EditBoxStr[] = "Edit box";		
XCHAR MeterStr[] = "Meter";			
XCHAR DialStr[] = "Dial";			
XCHAR PictureStr[] = "Picture";			
XCHAR CustomStr[] = "Custom Control";	  
XCHAR SignatureStr[] = "Signature";		
XCHAR PlottingStr[] = "Plotting";		
XCHAR EcgStr[] = "ECG"; 
XCHAR HomeStr[] = "Home";			
XCHAR LowStr[] = "Lo";				
XCHAR HighStr[] = "Hi";			
XCHAR OnStr[] = "On";				
XCHAR OffStr[] = "Off";
XCHAR EnableStr[] = "Enabled";			
XCHAR DisabledStr[] = "Disabled";		
XCHAR TextLeftStr[] = "Text Left";						
XCHAR TextBottomStr[] = "Text Bottom";        				
XCHAR Rb1Str[] = "Rb1";
XCHAR Rb2Str[] = "Rb2";
XCHAR Rb3Str[] = "Rb3";
XCHAR Rb4Str[] = "Rb4";
XCHAR Rb5Str[] = "Rb5";
XCHAR Rb6Str[] = "Rb6";
XCHAR StaticTextLstStr[] = "Microchip\n Graphics \nLibrary\n Static Text &\n Group Box Test.";
XCHAR LeftStr[] = "L";		
XCHAR CenterStr[] = "C";			
XCHAR RightStr[] = "R";			
XCHAR PIC24SpeedStr[] = "How fast can \nPIC24 go?";
XCHAR QuestionStr[] = "?";
XCHAR SingleStr[] = "Single";			
XCHAR AlignCenterStr[] = "Center";			
XCHAR ListboxLstStr[] = "Applications \nHome Appliances\n Home Automation\n Industrial Controls\n Medical Devices\n Automotive\n and much more...";
XCHAR CallingStr[] = "Calling...";						
XCHAR HoldingStr[] = "Holding...";
XCHAR HoldStr[] = "Hold";
XCHAR AccelStr[] = "Accel";
XCHAR DecelStr[] = "Dclrt";			
XCHAR ScaleStr[] = "Scale";
XCHAR AnimateStr[] = "Animate";
XCHAR LeftArrowStr[] = "Up";//"\x1c";
XCHAR RightArrowStr[] = "Down";//"\x1d";
XCHAR UpArrowStr[] = "<--";//"\x1e";
XCHAR DownArrowStr[] = "-->";//"\x1f";
XCHAR ExitStr[] = "Update";			
XCHAR SetDateTimeStr[] = "Set Time and Date";               


XCHAR OnBulbStr[] = "On";				
XCHAR OffBulbStr[] = "Off";
XCHAR SelectionStr[] = "Selection";		
XCHAR AlignmentStr[] = "Alignment";		
XCHAR ReturnStr[] = "Return";
XCHAR Meter1Str[] = "Meter1";			
XCHAR Meter2Str[] = "Meter2";			
XCHAR ValueLabelStr[] = "Value";
XCHAR MoreStr[] = "MORE......";
XCHAR BackStr[] = "BACK......";
XCHAR Group1Str[] = "Group 1";							
XCHAR Group2Str[] = "Group 2";						    
XCHAR NormalLoadStr[] = "Normal Load";                     
XCHAR LightLoadStr[] = "Light Load";                      
XCHAR HeavyLoadStr[] = "Heavy Load";                      
XCHAR UseR6PotStr[] = "Using R6 Potentiometer"; 
XCHAR ScanModeStr[] = "Scan Mode";                       
XCHAR ScrSelList1[] = "Buttons\n" "Checkbox\n" "Radio buttons\n" "Group box\n" "Slider\n" "Progress bar\n" "List box\n" "Edit box\n" "MORE......";
XCHAR ScrSelList2[] = "BACK......\n" "Meter\n" "Dial\n" "Picture\n" "Custom control\n" "Signature\n" "Plotting\n" "ECG"; 


XCHAR NextStr[] = "Next";
XCHAR MainStr[] = "Main";
XCHAR SubjectNameStr[] = "Subject Name";
XCHAR TesteeNameStr[] = "Testee Name";
XCHAR TesterNameStr[] = "Tester Name";
XCHAR NAStr[] = "NA";
XCHAR PlaceStr[] = "Place";
XCHAR DriverName[]="Driver Name";
XCHAR VehicleNoStr[] = "Vehicle Number";
XCHAR LicenceNumber[]="Licence Number";
XCHAR PoliceName[]="Police Name";
XCHAR BadgeNoStr[] = "Badge Number";
XCHAR OperatorNameStr[] = "Operator Name";
XCHAR DepartmentStr[] = "Department";
XCHAR Signature[]="Signature";
XCHAR PreprocessStr[] = "Preprocess";
XCHAR BlowStr[] = "Please Blow";
XCHAR BlowAgainStr[] = "Blow... Again";
XCHAR ManualStr[] = "Manual";//{'P','a','s','s','i','v','e',0};//;    // "±»¶¯²âÊÔ";
XCHAR TestagainStr[] = "Test Again";//	    {'T','e','s','t',' ','a','g','a','i','n',0};//;
XCHAR RefuseStr[] = 	"Refuse";//    {'R','e','f','u','s','e',0};
XCHAR UnitsStr[] = 	"Units";
XCHAR AutoStr[] = 	"Auto";//  Ö÷¶¯²âÊÔ

XCHAR TestReadyStr[] = "Test Ready";    // ²âÊÔ×¼±¸;
XCHAR DemarWendStr[] = "Adjust";//;    // ÎÂÐ£;
XCHAR DemarcateStr[] ="Cal.";//; //±ê¶¨
XCHAR MarkconsStr[] ="Cons";//;//±ê¶¨Å¨¶È
XCHAR MarktemStr[] ="Temperture";//;//±ê¶¨ÎÂ¶È
XCHAR MarkvaluStr[] ="Value";



XCHAR PrintStr[] = "Print";
XCHAR PrinttingStr[] = "Printting...";
XCHAR BlowTimePressStr[] = "Blow";//;// ´µÆøÊ±¼äºÍÑ¹Á¦

XCHAR OtherStr[]="Other";// Other
XCHAR AgentStr[]="Advanced Setting";// agent
XCHAR MasterStr[]="Advanced Setting";
XCHAR DeleteStr[]="Delete";// delete
XCHAR ResetStr[]="Default";// Reset
XCHAR Yes_NoStr[]="Yes_No";
XCHAR YesStr[]="Yes";
XCHAR NoStr[]="No";
XCHAR SoundStr[]="Sound";

XCHAR TestStr[] = "TEST";
XCHAR RecordStr[] = "RECORD";
XCHAR JumpStr[] = "Jump";
XCHAR ScreenStr[] = "SCREEN";
XCHAR SettingStr[] = "SETTING";
XCHAR GPSStr[] = "GPS";
XCHAR LongitudeStr[]="Long:";
XCHAR LatitudeStr[]="Lat:";
XCHAR TimeStr[] = "Time";
XCHAR DateStr[] ="Date";
XCHAR InitiativeStr[] ="Test Mode";
XCHAR NumberStr[] ="Record No.";

XCHAR LanguageStr[] = "Language";
XCHAR AuthorStr[] = "Author";
XCHAR BacklightStr[] = "Auto Off";
XCHAR FirmwareStr[] = "Firmware";
XCHAR MemoryStr[] = "Memory";
XCHAR CheckScreenStr[]="Screen";
XCHAR ABCStr[] = "ABC";
XCHAR abcStr[] = "abc";

XCHAR OneMinStr[] = "1 Minutes";
XCHAR TwoMinStr[] = " 2  Minutes";
XCHAR FineMinStr[] = "5 Minutes";//;    //"Fine Mins";
XCHAR TenMinStr[] = "10 Minutes";//;    // "Ten Mins";
XCHAR TestResultStr[] = "Test Result";
XCHAR TestFailStr[] = "Test Fail";

XCHAR ThicknessStr[] = "Unit";
XCHAR mg_100mlStr[] = "mg/100ml";
XCHAR mg_100Str[] 	 = "%";
XCHAR mg_1000mlStr[] = "%o";
XCHAR g_100mlStr[] = {0x0067, 0x002F, 0x0031, 0x0030, 0x0030, 0x006D, 'L', 0x0000};    //"g/100ml";
XCHAR g_210lStr[] = {0x0067, 0x002F, 0x0032, 0x0031, 0x0030, 'L', 0x0000};    //"g/210l";
XCHAR ug_100mlStr[] = {0x0075, 0x0067, 0x002F, 0x0031, 0x0030, 0x0030, 0x006D, 'L', 0x0000};    //"ug/100ml";
XCHAR mg_mlStr[] = {0x006D, 0x0067, 0x002F, 0x006D, 'L', 0x0000};    //"mg/ml";
XCHAR mg_mg100Str[] = {0x0067, 0x002F, 'L', 0x0000};    //"g/l";
XCHAR mg_lStr[] = {0x006D, 0x0067, 0x002F, 'L', 0x0000};    //"mg/l";
XCHAR Celsius_Str[]  = {0x007D, 0x0000}; 

// K Setting
XCHAR KSettingStr[] ="K Ratio";//"KSetting";
XCHAR KselectStr[] = "2000";
XCHAR K2050StrA[] = "2050";
XCHAR K2100StrA[] = "2100";
XCHAR K2150StrA[] = "2150";
XCHAR K2200StrA[] = "2200";
XCHAR K2250StrA[] = "2250";
XCHAR K2300StrA[] = "2300";
XCHAR K2350StrA[] = "2350";
XCHAR K2400StrA[] = "2400";
XCHAR K2450StrA[] = "2450";



// Print setting
XCHAR PrintSettingStr[] ="Print";
XCHAR PrintLotStr[] ="Lot";
XCHAR BigValueStr[] ="Value";
XCHAR AutoPrintStr[] ="Auto Print";
//XCHAR PaperNumStr[] = "PrintOut";//No. of PrintOut"·ÝÊý";
XCHAR ZeroStr[] = "=0";
XCHAR BeyondZeroStr[] = "Number";//{0x003E, 0X0030, 0X0000};
//XCHAR Big20dStr[] = ">20";

//For ±ê¶¨
XCHAR INPUTStr[] = "mg/100mL";    // ±ê¶¨;

// ÃÜÂë
XCHAR PasswordStr[] ="Password";
XCHAR PasswordSettingStr[] ="Password Setting";
XCHAR EnterPasswordStr[] ="Enter Password";
XCHAR WrongPasswordStr[] ="Invalid";
XCHAR ScreenPasswordStr[] ="Screen Calibration";
XCHAR Confirmpasswordstr[] = "Confirm Password";
XCHAR SettingsuccessStr[] = "Password Setting \n \n Succeed";
XCHAR NotmatchStr[] ="The passwords \n \n you enter didn't match";
// PwoerOff
XCHAR PowerOffStr[] = "Power OFF";


// Blow time and Press setting 
XCHAR BlowSettingStr[] = "Blow";		// ´µÆø
XCHAR BlowTimeStr[] = "Time";		//(Sec) ´µÆøÊ±¼ä
XCHAR TwoSecStr[] = "2";    	//	"2 Ãë";
XCHAR TwoFineSecStr[] = "2.5";    //	"2.5 Ãë";
XCHAR ThreeSecStr[] = "3";    	//	"3 Ãë";
XCHAR ThreeFineSecStr[] = "3.5";  //	"3.5 Ãë";
XCHAR FourSecStr[] = "4";   		//	"4 Ãë";
XCHAR FourFineSecStr[] = "5";    //	"4.5 Ãë";
XCHAR FineSecStr[] = "5.5";    	//	"5 Ãë";

// Press setting
XCHAR BlowPressStr[] = "Pressure";		// ´µÆøÑ¹Á¦
XCHAR OneStr[] = "1";	// 1 ¼¶
XCHAR TwoStr[] = "2";	// 2 ¼¶
XCHAR ThreeStr[] = "3";	// 3 ¼¶
XCHAR FourStr[] = "4";	// 4 ¼¶
XCHAR FineStr[] = "5";	// 5 ¼¶
XCHAR SixStr[] = "6";		// 6 ¼

//for the periods and number
XCHAR CalibrationPeriodStr[]="Calibration Period";//{'C','a','l','i','b','r','a','t','i','o','n',' ','P','e','r','i','o','d',0};
XCHAR CalPerStr[]="Cal.Per";
XCHAR AdjustStr[]="Adjust";
XCHAR TestNumberStr[]="Tests";//{'T','i','m','e','s',0};
XCHAR OneNumStr[] = "300";//"100";//{'1','0','0',0};	// 100 ¼¶
XCHAR ThreeNumStr[] = "500";//"300";//{'3','0','0',0};	// 300 ¼¶
XCHAR FineNumStr[] = "700";//"500";//{'5','0','0',0};	// 500 ¼¶
XCHAR SevenNumStr[] = "1000";//{'7','0','0',0};	// 700 ¼¶
//XCHAR NineNumStr[] = "1000";//{'9','0','0',0};	// 900 ¼¶
//XCHAR OtherNumtr[] = {'1','0','0','0',0};		// 6 ¼
XCHAR TestMthStr[]="Days";//{'M','o','n','t','h','s',0};
XCHAR ThreeMthStr[] = "90";//{0x0033, 0x0000};	// 3 ¼¶
XCHAR SixMthStr[] = "180";//{0x0036, 0x0000};	// 6 ¼¶
XCHAR NineMthStr[] = "270";//{0x0039, 0x0000};	// 9 ¼¶
XCHAR ElevenMthStr[] = "360";//{0x0031, 0x0032,0x0000};	// 12 ¼¶
//XCHAR ThreeMthStr[] = "5";//{0x0035, 0x0000};	// 5 ¼¶
//XCHAR ThreeMthStr[] = "6";//{0x0036, 0x0000};		// 6 ¼

// For the Item
XCHAR ItemStr[ItemMax] = "Field";
XCHAR Item0Str[ItemMax] = "NA0";
XCHAR Item1Str[ItemMax] = "NA1";
XCHAR Item2Str[ItemMax] = "NA2";
XCHAR Item3Str[ItemMax] = "NA3";
XCHAR Item4Str[ItemMax] = "NA4";
XCHAR Item5Str[ItemMax] = "NA5";
XCHAR Item6Str[ItemMax] = "NA6";
XCHAR Item7Str[ItemMax] = "NA7";
XCHAR Item8Str[ItemMax] = "NA8";
XCHAR Item9Str[ItemMax] = "NA9";

XCHAR WellStr[] = "ShenZhen Well-co LTD";
XCHAR WatNameStr[] = "Breath alcohol analyzer";//ºô³öÆøÌå¾Æ¾«º¬Á¿¼ì²âÒÇ


XCHAR UintboxLstStr[] = "%\n" "¡ë%o\n" "mg/L\n" "g/210L\n" "mg/100ml\n" "g/100mL\n" "mg/mL\n" "mg%\n" "ug/100mL\n";
XCHAR DownloadStr[] = "Download Records";
XCHAR ConnectToPCStr[] = 	  "Connect To  PC";

//Wait
XCHAR WaitStr[] = 	 "Wait";//×¼±¸ÖÐ
//Debug
XCHAR DebugStr[] = "Debug"	 ;//µ÷ÊÔ
XCHAR DemarcateshowStr[] = "Demarcateshow" ;//ÎÂ¶ÈÐ£ÕýÊý¾Ý

//lock
XCHAR LockStr[] = 	"LOCK";//Ëø¶¨
XCHAR UnlockStr[] = "Unlock"	 ;//½âËø

XCHAR Aircheckstr[] = "Aircheck Processing";
XCHAR Alcoholdetected[] = "Resident Alcohol Detected!";//{'R','e','s','i','d','e','n','t',0x000a ,'a','l','c','o','h','o','l',' ','d','e','t','e','c','t','e','d','!',0x0000};//
XCHAR Calibrationneed[]={'C','a','l','i','b','r','a','t','i','o','n',0x000a,0x000a,0x000a,'N','e','e','d','e','d','!',0x0000};
XCHAR DelectAllRecords[]="Delect All Records";
XCHAR Calibrationstr[]="Calibration";
XCHAR Cancel[]="Cancel";
XCHAR EEPROMEDIT[]="EEPROM EDIT";
XCHAR DemarcateEDIT[]="Demarcate EDIT";	
XCHAR Pointr[]="***";
XCHAR Totalstr[]="Total:";
XCHAR Beginningstr[]="First";
XCHAR Endstr[]="End";
XCHAR Jumpstr[]="Jump";
XCHAR AlocholDetectedstr[]="Alcohol Detected";
XCHAR NoAlocholstr[]="No Alcohol";
XCHAR Setpasswordstr[]="Password Setting ";
XCHAR BlowDiscontinuedstr[]="Blow  Discontinued";
XCHAR Discontinuedstr[]="Discontinued";
XCHAR Advancestr[] = "Advanced";
XCHAR EditIDstr[] = "EditID";
XCHAR PrintID[] = "PrintID";
XCHAR Keyboard[] = "Keyboard";
XCHAR TimeType[] = "Use  MDY(restart)";
#ifdef Argentina
XCHAR EDITDATEstr[] = {'E','d','i','t',' ','M','a','r','k',' ','D','a','t','e',0};
XCHAR EDITCALDATEdstr[] = {'C','A','L','.','D',0};
#endif