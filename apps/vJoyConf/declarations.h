// General declarations

// Axes:
// The following define the Internal names and fixed order of the axes (Based on structure DIJOYSTATE2)
#define AxesIDs {\
	L"X", L"Y", L"Z", L"Rx", L"Ry", L"Rz", L"S1", L"S2",\
	L"VX", L"VY", L"VZ", L"VRx", L"VRy", L"VRz", L"VS1", L"VS2",\
	L"AX", L"AY", L"AZ", L"ARx", L"ARy", L"ARz", L"AS1", L"AS2",\
	L"FX", L"FY", L"FZ", L"FRx", L"FRy", L"FRz", L"FS1", L"FS2"}




// // // Strings

// Tooltip stringsTT_ 
#define TT_POVDIR L"Four Directions:\r\nNorth, East, South, West\r\nand a neutral state"
#define TT_TABAXES L"Set the active Axes and POV Hat type"

//////////////////////////////////////////////////////////

 /* These macros are used to build the report by adding a byte/word  */
 /* at atime while checking for buffer overflows. */
//#define UNALIGNED __unaligned
 //#define NEXT_BYTE(pReport,Data)	if ((ReportSize+=1)<=buffer_size) *pReport++ = (Data);    
 //#define NEXT_BYTE(pReport,Data)	if ((ReportSize+=1)>buffer_size) pReport = (UCHAR *)realloc((void *)pReport, buffer_size+1000);    *pReport++ = (Data);    
 //#define NEXT_LONG(pReport,Data)	if ((ReportSize+=4)<=buffer_size) *(((LONG UNALIGNED*)(pReport))++) = (Data);
 //#define NEXT_LONG(pReport,Data)	if ((ReportSize+=4)>buffer_size) pReport = (UCHAR *)realloc(pReport, buffer_size+1000); *((LONG UNALIGNED*)pReport++) = (Data);*pReport++;
 //#define NEXT_SHORT(pReport,Data)	if ((ReportSize+=2)>buffer_size) pReport = (UCHAR *)realloc(pReport, buffer_size+1000); *((USHORT *)pReport++) = (Data); *pReport++;

/* These macros are used to build the report by adding a byte/word at a time*/
#ifndef	 NEXT_BYTE
#define NEXT_BYTE(vReport,Data)   vReport.push_back(Data);
#endif

#ifndef	 NEXT_SHORT
#define NEXT_SHORT(vReport,Data)  vReport.push_back(Data&0xFF); vReport.push_back((Data&0xFF00)>>8);
#endif


#define MAX_N_FFB 	15 // Maximum number of vJoy devices that support FFB
#define MAX_N_DEVICES	16 // Maximum number of vJoy devices
#define MAX_BUTTONS	128
#define MAX_AXES	8



////////////////////////////////////////////////////////////////////
#define REG_PARAM		L"SYSTEM\\CurrentControlSet\\services\\vjoy\\Parameters"
//#define REG_PARAM_DEV0	L"SYSTEM\\CurrentControlSet\\services\\vjoy\\Parameters\\Device0"
#define REG_PARAM_DEV		L"SYSTEM\\CurrentControlSet\\services\\vjoy\\Parameters\\Device"
#define REG_PARAM_FFBDEV	L"SYSTEM\\CurrentControlSet\\services\\vjoy\\Parameters\\FFBDevice"
#define DESC_NAME			L"HidReportDesctiptor"
#define DESC_SIZE			L"HidReportDesctiptorSize"

enum ACTION {config, del, del_all};


// HID Descriptor definitions
#define HID_USAGE_X		0x30
#define HID_USAGE_Y		0x31
#define HID_USAGE_Z		0x32
#define HID_USAGE_RX	0x33
#define HID_USAGE_RY	0x34
#define HID_USAGE_RZ	0x35
#define HID_USAGE_SL0	0x36
#define HID_USAGE_SL1	0x37
#define HID_USAGE_WHL	0x38
#define HID_USAGE_POV	0x39
