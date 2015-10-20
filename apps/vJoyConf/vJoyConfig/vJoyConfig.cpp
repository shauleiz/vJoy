// vJoyConfig.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include <stdio.h>
#include "cfgmgr32.h"
#include <setupapi.h>
#include <devguid.h>
#include "../../vJoyInstall/vJoyInstall.h"
#include "../../inc/wrapper.h"
#include "../../inc/vJoyInterface.h"
#include "../HidToken.h"
#include "../HidUsage.h"
#include "../Declarations.h"
#include "resource.h"
#include "vJoyConfig.h"

#pragma comment(lib, "vJoyInstall.lib")
#pragma comment(lib, "vJoyInterface.lib")
#pragma comment(lib, "Version.lib")

//////////////// Globals ////////////////
CMD		g_cmnd = HLP;

// Default device values
int		g_iDevice[MAX_N_DEVICES] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int		g_nAxes = 8;
int		g_nButtons = 8;
int		g_nDescretePovs = 0;
int		g_nAnalogPovs = 0;
bool	g_isAxisX = true;
bool	g_isAxisY = true;
bool	g_isAxisZ = true;
bool	g_isAxisRX = true;
bool	g_isAxisRY = true;
bool	g_isAxisRZ = true;
bool	g_isAxisSL0 = true;
bool	g_isAxisSL1 = true;

//////////////// Declarations ////////////////
bool ParseCommandLine(int argc, _TCHAR* argv[]);
int  CreateHidReportDesc(void ** data, UINT nButtons, bool * axes, int nPovHatsCont, int nPovHatsDir, BYTE ReportId);
bool WriteHidReportDescriptor(int target);
void WriteHidReportDescToReg(int target, UCHAR * Descriptor, int size, bool Overwrite);
void DeleteHidReportDescFromReg(int target);
void DisplayHelp(void);
void DisplayVersion(void);
void ReportConf(void);
bool RunGui(void);
void PrintMessage(int errn, LPCWSTR Param);
bool TestBitness(void);

// Command Line Helper functions (cl_*)
bool	cl_isFlag(LPCWSTR sItem);
bool	cl_isFlag(int argc,  _TCHAR* argv[], int iItem);
bool	cl_isCreate(int argc,  _TCHAR* argv[]);
int		cl_getTargetDevice(int argc,  _TCHAR* argv[]);
int		cl_getNextFlagIndex(int argc,  _TCHAR* argv[],int iItem);
int		cl_getParamIndex(int argc,  _TCHAR* argv[], int iFlag, int i);
CMD		cl_Flag2Cmd(int argc,  _TCHAR* argv[], int iItem);
int		cl_FindFlg(int argc,  _TCHAR* argv[], LPCWSTR flg);
bool	cl_isParamExist(int argc,  _TCHAR* argv[], LPCWSTR flg, LPCWSTR param);
bool	cl_getParamValue(int argc,  _TCHAR* argv[], LPCWSTR flg, int serial ,int *val);

int _tmain(int argc, _TCHAR* argv[])
{

	bool restart = false;

	// Test Bitness
	if (!TestBitness())
		return -1;

	// Read command line and set the global device values and set the command
	bool parse_ok = ParseCommandLine(argc, argv);

	// Call GUI
	if (parse_ok && g_cmnd == GUI)
		bool gui_ok = RunGui();

	// Help or illegal
	if (/*!parse_ok ||*/ g_cmnd == HLP)
		DisplayHelp();

	// Report configuration
	if (parse_ok && g_cmnd == RPT)
		ReportConf();

	// Version information
	if (parse_ok && g_cmnd == VER)
		DisplayVersion();

	// Reset vJoy to default configuration
	if (parse_ok && g_cmnd == RST)
	{
		DeleteHidReportDescFromReg(0);
		restart = true;
	};

	// Delete one or more devices
	if (parse_ok && g_cmnd == DEL)
	{
		for (int i=0; i<16; i++)
		{
			if (!g_iDevice[i])
				break;
			DeleteHidReportDescFromReg(g_iDevice[i]);
		};
		restart = true;
	};

	//Create a joystick device
	if (parse_ok && (g_cmnd == FRC || g_cmnd == CRT))
	{
		WriteHidReportDescriptor(g_iDevice[0]);
		restart = true;
	};

	 // Refresh
	if (restart)
		refresh_vjoy();

	return 0;
}

/*
	Parse command line
	Sets global parameters including device ID, controls and command
	Returns false if command line was illegal
*/
bool ParseCommandLine(int argc, _TCHAR* argv[])
{
	g_cmnd = UNK;

	// No arguments - This is a call to the GUI version
	if (argc == 1)
	{
		g_cmnd = GUI;
		return true;
	}

	// Create
	if (cl_isCreate( argc, argv))
	{
		g_iDevice[0] = cl_getTargetDevice(argc, argv);
		if (g_iDevice[0]<1 || g_iDevice[0]>16)
		{
			PrintMessage(IDS_ERR_ID_RANGE, NULL);
			return false;
		};

		if (cl_FindFlg( argc, argv, L"-f"))
			g_cmnd = FRC;
		else
			g_cmnd = CRT;

		// Parse axes (Following '-a')
		if (cl_FindFlg(argc, argv, L"-a"))
		{
			g_isAxisX   = cl_isParamExist(argc, argv, L"-a", L"x");
			g_isAxisY   = cl_isParamExist(argc, argv, L"-a", L"y");
			g_isAxisZ   = cl_isParamExist(argc, argv, L"-a", L"z");
			g_isAxisRX  = cl_isParamExist(argc, argv, L"-a", L"rx");
			g_isAxisRY  = cl_isParamExist(argc, argv, L"-a", L"ry");
			g_isAxisRZ  = cl_isParamExist(argc, argv, L"-a", L"rz");
			g_isAxisSL0 = cl_isParamExist(argc, argv, L"-a", L"sl0");
			g_isAxisSL1 = cl_isParamExist(argc, argv, L"-a", L"sl1");
		};

		// Parse buttons (Following '-b')
		int nButtons;
		if (cl_getParamValue(argc, argv, L"-b", 1, &nButtons))
		{
			if (nButtons>=0 && nButtons<=128)
				g_nButtons = nButtons;
			else 
			{
				PrintMessage(IDS_ERR_BTN, NULL);
				return false;
			};
		};

		// Parse POV (Analog)
		int nAnalogPovs;
		if (cl_getParamValue(argc, argv, L"-p", 1, &nAnalogPovs))
		{
			if (nAnalogPovs>=0 && nAnalogPovs<=4)
				g_nAnalogPovs = nAnalogPovs;
			else 
			{
				PrintMessage(IDS_ERR_POV, NULL);
				return false;
			};
		};

		// Parse POV (Descrete)
		if (!g_nAnalogPovs)
		{
			int nDescretePovs;
			if (cl_getParamValue(argc, argv, L"-s", 1, &nDescretePovs))
			{
				if (nDescretePovs>=0 && nDescretePovs<=4)
					g_nDescretePovs = nDescretePovs;
				else 
				{
					PrintMessage(IDS_ERR_POV, NULL);
					return false;
				};
			};
		}
		return true;
	};

	// Delete
	int iDel = cl_FindFlg( argc,   argv, L"-d");
	if (iDel)
	{
		g_cmnd = DEL;
		int i;
		int val;
		for (i=0; iDel<argc && i<16; i++)
		{
			if (cl_getParamValue( argc,   argv, L"-d", i+1 ,&val))
				g_iDevice[i] = val;
			else
				break;
		};
		return (i>0);
	};

	// Reset
	int iRst = cl_FindFlg( argc,   argv, L"-r");
	if (iRst)
	{
		g_cmnd = RST;
		return true;
	};

	// Report
	if (cl_FindFlg( argc,   argv, L"-t"))
	{
		g_cmnd = RPT;
		return true;
	};

	// Version
	if (cl_FindFlg( argc,   argv, L"-v"))
	{
		g_cmnd = VER;
		return true;
	};

	// Help
	if (cl_FindFlg( argc,   argv, L"-h"))
	{
		g_cmnd = HLP;
		return true;
	};


	PrintMessage(IDS_ERR_FLAG, NULL);
	return false;
}

/*
	Write the HID Report Descriptor to the registry
	Key:	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\vjoy\Parameters\Device0
	Name:	HidReportDesctiptor
	Type:	REG_BINARY
*/
void WriteHidReportDescToReg(int target, UCHAR * Descriptor, int size, bool Overwrite)
{
	/* Open registry - Most of the path should exist */
	HKEY hParams, hDev0;
	LONG RegRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PARAM, 0, KEY_ALL_ACCESS , &hParams);
	if (RegRes != ERROR_SUCCESS)
		return;	// Error

	// Create target key string from target number
	std::wstring  RegParam;
	WCHAR cTarget[3] = {'0', '0', '\0'};

	swprintf_s(cTarget, 3, L"%02d" , target);
	RegParam = REG_PARAM_DEV +  std::wstring(cTarget);
	LPCWSTR lpRegParam = RegParam.c_str();
	
	DWORD dwDisposition;
	RegRes = RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpRegParam, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS , NULL, &hDev0, &dwDisposition);
	if (RegRes != ERROR_SUCCESS)
	{
		RegCloseKey(hParams);
		return;	// Error
	};

	if (!Overwrite && (dwDisposition==REG_OPENED_EXISTING_KEY))
	{
		PrintMessage(IDS_WRN_CREATE, NULL);
		RegCloseKey(hParams);
		return;	// Error
	};


	/* Write the data */
	RegRes = RegSetValueEx(hDev0, DESC_NAME, 0, REG_BINARY, Descriptor, size);
	if (RegRes != ERROR_SUCCESS)
	{
		RegCloseKey(hParams);
		RegCloseKey(hDev0);
		return;	// Error
	};

	/* Write the data Size*/
	DWORD dSize = (DWORD)size;
	RegRes = RegSetValueEx(hDev0, DESC_SIZE, 0, REG_DWORD, (const BYTE *)(&dSize), 4);
	RegCloseKey(hParams);
	RegCloseKey(hDev0);
}

/*
	Create an HID Report Descriptor
	Inputs:
		data:	Pointer to the pointer to array of bytes that will hold the descriptor
		buttons: Array of buttons. Every entry represents a button. True = Implemented button.
		axes: Array of axes. Every entry represents an axis. True = Implemented axis.
		nPovHatsCont: Number of implemented Continuous POV Hat switches
		nPovHatsDir: Number of implemented 5-state POV Hat switches

	Return:
		Positive value: Size of HID Report Descriptor (output buffer) in bytes.

*/
int CreateHidReportDesc(void ** data, UINT nButtons, bool * axes, int nPovHatsCont, int nPovHatsDir, BYTE ReportId)
{
	/* normalize if illegal parameters are passed */
	// Buttons: 0-128
	if (nButtons>128)
		nButtons = 128;

	// POV (Cont): 0-4
	if (nPovHatsCont<0)
		nPovHatsCont = 0;
	if (nPovHatsCont>4)
		nPovHatsCont = 4;
	// POV (Dir): 0-4
	if (nPovHatsDir<0)
		nPovHatsDir = 0;
	if (nPovHatsDir>4)
		nPovHatsDir = 4;

	/* Create a data buffer large enough to hold the resulting HID Report Descriptor */
	//int buffer_size = 1023;
	static std::vector<BYTE> buffer;
	buffer.clear();

	/* Create standard header */
	NEXT_BYTE(buffer, HIDP_GLOBAL_USAGE_PAGE_1)		// USAGE_PAGE(Generic Desktop):		05 01
		NEXT_BYTE(buffer, HID_USAGE_PAGE_GENERIC)
		NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1)			// LOGICAL_MINIMUM(0):				15 00
		NEXT_BYTE(buffer, 0)
		NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1)			// USAGE (Joystick):				09 04
		NEXT_BYTE(buffer, HID_USAGE_GENERIC_JOYSTICK)
		NEXT_BYTE(buffer, HIDP_MAIN_COLLECTION)			// COLLECTION( Application):		A1 01
		NEXT_BYTE(buffer, HIDP_MAIN_COLLECTION_APP)

		/* Collection 1 */
		NEXT_BYTE(buffer, HIDP_GLOBAL_USAGE_PAGE_1)		// USAGE_PAGE(Generic Desktop):		05 01
		NEXT_BYTE(buffer, HID_USAGE_PAGE_GENERIC)
		NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_ID)			//	REPORT_ID (x)					85 ID
		NEXT_BYTE(buffer, ReportId)
		NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1)			// USAGE(Pointer):					09 01
		NEXT_BYTE(buffer, HID_USAGE_GENERIC_POINTER)
		NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1)			// LOGICAL_MINIMUM(0):				15 00
		NEXT_BYTE(buffer, 0)
		NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_2)			// LOGICAL_MAXIMUM(32767):			26 FF 7F
		NEXT_SHORT(buffer, 0x7FFF)
		NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE)		// REPORT_SIZE(32):					75 20
		NEXT_BYTE(buffer, 0x20)
		NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(1):					95 01
		NEXT_BYTE(buffer, 0x01)
		NEXT_BYTE(buffer, HIDP_MAIN_COLLECTION)			// COLLECTION(Physical):			A1 00
		NEXT_BYTE(buffer, 0x00)

		/** Collection 2 **/
		/* Loop on fitst 8 Axes */
		for (int i = 0; i<8; i++)
		{
			if (axes[i])
			{
				// Insert Axis
				NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1)	// USAGE(X+offset):					0x09 0x30+i
					NEXT_BYTE(buffer, HID_USAGE_GENERIC_X + i)
					NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)		// INPUT (Data,Var,Abs):			0x81 0x02
					NEXT_BYTE(buffer, 0x02)
			}
			else
			{
				// Insert place holder
				NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)		// INPUT (Cnst,Ary,Abs):			0x81 0x01
					NEXT_BYTE(buffer, 0x01)
			};
		};
	NEXT_BYTE(buffer, HIDP_MAIN_ENDCOLLECTION)		// END_COLLECTION:					0xC0

		if (nPovHatsDir)
		{
			// POV - supported: One switch at most, 5-state only
			NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1)			// LOGICAL_MINIMUM(0):		15 00
				NEXT_BYTE(buffer, 0x00)
				NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_1)			// LOGICAL_MAXIMUM(3):		25 03
				NEXT_BYTE(buffer, 0x03)
				NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MIN_1)			// PHYSICAL_MINIMUM (0):	35 00
				NEXT_BYTE(buffer, 0x00)
				NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MAX_2)			// PHYSICAL_MAXIMUM (270):	46 0e 01
				NEXT_SHORT(buffer, 0x010e)
				NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_1)			// UNIT (Eng Rot:Angular Pos):	65 14
				NEXT_BYTE(buffer, 0x14)
				// One 4-bit data  + 31 4-bit padding
				NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE)		// REPORT_SIZE(4):			75 04
				NEXT_BYTE(buffer, 0x04)
				NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(1):			95 01
				NEXT_BYTE(buffer, 0x01)

				// Insert 1-4 5-state POVs
				for (int i = 1; i <= nPovHatsDir; i++)
				{
					NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1)			// USAGE(Hat switch):		0x09 0x39
						NEXT_BYTE(buffer, HID_USAGE_GENERIC_HATSWITCH)
						NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)				// INPUT (Data,Var,Abs):	0x81 0x02
						NEXT_BYTE(buffer, 0x02)
				};

			// Insert 5-state POV place holders
			NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(31):		95 1F
				NEXT_BYTE(buffer, 0x20 - nPovHatsDir)
				NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)				// INPUT (Cnst,Ary,Abs):	0x81 0x01
				NEXT_BYTE(buffer, 0x01)
		}
		else if (nPovHatsCont)
		{
			// Continuous POV
			NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1)			// LOGICAL_MINIMUM(0):		15 00
				NEXT_BYTE(buffer, 0x00)
				NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_4)			// LOGICAL_MAXIMUM(35900):	27 3c 8c 00 00
				NEXT_SHORT(buffer, 0x8C3C)
				NEXT_SHORT(buffer, 0x0000)
				NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MIN_1)			// PHYSICAL_MINIMUM (0):	35 00
				NEXT_BYTE(buffer, 0x00)
				NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MAX_4)			// PHYSICAL_MAXIMUM (35900):	47 3c 8c 00 00
				NEXT_SHORT(buffer, 0x8C3C)
				NEXT_SHORT(buffer, 0x0000)
				NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_1)			// UNIT (Eng Rot:Angular Pos):	65 14
				NEXT_BYTE(buffer, 0x14)
				//
				NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE)		// REPORT_SIZE(32):			75 20
				NEXT_BYTE(buffer, 0x20)
				NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(1):			95 01
				NEXT_BYTE(buffer, 0x01)

				// Insert 1-4 continuous POVs
				for (int i = 1; i <= nPovHatsCont; i++)
				{
					NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1)			// USAGE(Hat switch):		0x09 0x39
						NEXT_BYTE(buffer, HID_USAGE_GENERIC_HATSWITCH)
						NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)				// INPUT (Data,Var,Abs):	0x81 0x02
						NEXT_BYTE(buffer, 0x02)
				};

			// Insert 1-3 continuous POV place holders
			if (nPovHatsCont<4)
			{
				NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(3):		95 03
					NEXT_BYTE(buffer, 0x04 - nPovHatsCont)
					NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)				// INPUT (Cnst,Ary,Abs):	0x81 0x01
					NEXT_BYTE(buffer, 0x01)
			};
		}
		else
		{
			// Sixteen 4-bit padding
			NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE)		// REPORT_SIZE(32):			75 20
				NEXT_BYTE(buffer, 0x20)
				NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(4):			95 04
				NEXT_BYTE(buffer, 0x04)
				NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)				// INPUT (Cnst,Ary,Abs):	0x81 0x01
				NEXT_BYTE(buffer, 0x01)
		};


		// Buttons - up to 32 buttons supported. Only the NUMBER of buttons can be set
		//int nButtons = 0;
		//for (int i=0; i<MAX_BUTTONS; i++)
		//	if (buttons[i]) 
		//		nButtons++;

		// There are buttons
		NEXT_BYTE(buffer, HIDP_GLOBAL_USAGE_PAGE_1)		// USAGE_PAGE(Buttons):		05 09
			NEXT_BYTE(buffer, HID_USAGE_PAGE_BUTTON)
			NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1)			// LOGICAL_MINIMUM(0):		15 00
			NEXT_BYTE(buffer, 0x00)
			NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_1)			// LOGICAL_MAXIMUM(0):		25 01
			NEXT_BYTE(buffer, 0x01)
			NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_EXP_1)		// UNIT_EXPONENT(0):		55 00
			NEXT_BYTE(buffer, 0x00)
			NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_1)			// UNIT (None):				65 00
			NEXT_BYTE(buffer, 0x00)
			NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_MIN_1)		// USAGE_MINIMUM(1):		19 01
			NEXT_BYTE(buffer, 0x01)
			NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_MAX_1)		// USAGE_MAXIMUM(nButtons):	29 nButtons
			NEXT_BYTE(buffer, nButtons)
			NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE)		// REPORT_SIZE(1):			75 01
			NEXT_BYTE(buffer, 0x01)
			NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(nButtons):	95 nButtons
			NEXT_BYTE(buffer, nButtons)
			NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)				// INPUT (Data,Var,Abs):	0x81 0x02
			NEXT_BYTE(buffer, 0x02)

			// Padding, if there are less than 32 buttons
			if (nButtons < MAX_BUTTONS)
			{
				NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE)	// REPORT_SIZE(x):		75 32-nButtons
					NEXT_BYTE(buffer, MAX_BUTTONS - nButtons)
					NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1)	// REPORT_COUNT(1):	95 nButtons
					NEXT_BYTE(buffer, 0x01)
					NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1)			// INPUT (Cnst,Ary,Abs):0x81 0x01
					NEXT_BYTE(buffer, 0x01)
			};


		NEXT_BYTE(buffer, HIDP_MAIN_ENDCOLLECTION)			// END_COLLECTION:					0xC0

			UCHAR * orig = &buffer[0];
		*data = (void **)orig;

		return (int)buffer.size();

}

bool WriteHidReportDescriptor(int target)
{

			 // Axes: Creat C-language array
			 int nAxes = g_nAxes;
			 if (nAxes < 8)
				 return false;

			 bool * Axes = new bool[nAxes];
			 Axes[0] = g_isAxisX;
			 Axes[1] = g_isAxisY;
			 Axes[2] = g_isAxisZ;
			 Axes[3] = g_isAxisRX;
			 Axes[4] = g_isAxisRY;
			 Axes[5] = g_isAxisRZ;
			 Axes[6] = g_isAxisSL0;
			 Axes[7] = g_isAxisSL1;
			 

			 // Call external C-function that creats an array of bytes that holds
			 // the HID Report Descriptor
			 UCHAR **out = (UCHAR **)malloc(sizeof(UCHAR *));
			 if (!out)
				 return false;
			 int desc_size = CreateHidReportDesc((void **)out, g_nButtons, Axes, g_nAnalogPovs, g_nDescretePovs, (int)target);

			 if ((desc_size<=0))
			 {
				 MessageBox(NULL, L"Failed to create HID Report Descriptor",  NULL, MB_OK);
				 return false;
			 };


			 WriteHidReportDescToReg((int)target, *out, desc_size, g_cmnd == FRC);
			 return true;
		 }

/*
	Delete the HID Report Descriptor from the registry
	Key:	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\vjoy\Parameters\Device0
*/
void DeleteHidReportDescFromReg(int target)
{
	/* Open registry - Most of the path should exist */
	HKEY hParams;
	LONG RegRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PARAM, 0, KEY_ALL_ACCESS , &hParams);
	if (RegRes != ERROR_SUCCESS)
		return;	// Error

	// Determine the key(s) to delete
	int top=16, bottom=1;
	if (target<0 || target>16)
		return;
	if (target)
		top=bottom=target;

	for (int i=bottom; i<=top; i++)
	{
		// Create target key string from target number
		// Create target key string from target number
		std::wstring  RegParam;
		WCHAR cTarget[3] = {'0', '0', '\0'};

		swprintf_s(cTarget, 3, L"%02d" , i);
		RegParam = REG_PARAM_DEV +  std::wstring(cTarget);
		LPCWSTR lpRegParam = RegParam.c_str();

		// Delete
		RegRes = RegDeleteKey(HKEY_LOCAL_MACHINE, lpRegParam);

	};

	RegCloseKey(hParams);
	return;
}

/*
	Call vJoyConf if in the same folder
*/
bool RunGui(void)
{
	// Test if "vJoyConf.exe" exists
	FILE *file;


#if 0
	if (fopen_sx(&file, "vJoyConf.exe", "r"))
	{
		PrintMessage(IDS_ERR_NOGUI, NULL);
		return false;
	}
	else
		fclose(file);
#else
#pragma warning(disable : 4996)
	file = fopen("vJoyConf.exe", "r");
	if (!file)
	{
		PrintMessage(IDS_ERR_NOGUI, NULL);
		return false;
	}
	else
		fclose(file);

#endif // 0

	// Call it
	HINSTANCE  h = ShellExecute(NULL, L"open", L"vJoyConf.exe", L"", NULL, SW_SHOW);

	return false;
}

/*
	Print message to console
	errn:	Message serial number
	Param:	Optional parameter (NULL is ignored)
*/
void PrintMessage(int errn,  LPCWSTR Param)
{
	WCHAR msg[1000];

	int size = sizeof(msg)/sizeof(msg[0]);

	int loaded = LoadString(GetModuleHandle(NULL), errn, &(msg[0]), size);
	if (Param)
		wprintf_s(msg, Param);
	else
		wprintf_s(msg);
	wprintf_s(L"\n");
}

void DisplayHelp(void)
{
	DisplayVersion();
	wprintf_s(HelpStr);
}

// Display Configuration report
void ReportConf(void) 
{
	// Get general driver info
	BOOL Enabled = vJoyEnabled();
	if (!Enabled)
	{
		wprintf_s(L"\nvJoy driver is not installed or disabled\n\n");
		return;
	};
		
	PWSTR Product = (PWSTR)GetvJoyProductString();
	PWSTR Manufacturer = (PWSTR)GetvJoyManufacturerString();
	PWSTR SerialNumber = (PWSTR)GetvJoySerialNumberString();
	USHORT version = GetvJoyVersion();

	if (!version)
	{
		wprintf_s( _T("Failed\r\nvJoy is probably disabled or uninstalled"));
		return ;
	}
	else
		wprintf_s( _T("\nProduct ID: %s\nManufacturer ID: %s\nVersion Number: %s\n(VendorID:0x%04X ProductID:0x%04X VersionNumber:0x%04X)\n\n"),Product, Manufacturer, SerialNumber, VENDOR_N_ID,  PRODUCT_N_ID, version);


	// Preparations for formatting of text
	LPCTSTR StrStat[5];
	StrStat[VJD_STAT_OWN] = TEXT("OWNED");
	StrStat[VJD_STAT_FREE] = TEXT("FREE");
	StrStat[VJD_STAT_BUSY] = TEXT("BUSY");
	StrStat[VJD_STAT_MISS] = TEXT("MISSING");
	StrStat[VJD_STAT_UNKN] = TEXT("UNKNOWN");
	enum VjdStat DevStat;	
	// Loop on all 16 vJoy Devices
	for (UINT iDev=1; iDev<=MAX_N_DEVICES; iDev++)
	{
		DevStat = GetVJDStatus(iDev);
		wprintf_s(L"\nDevice %d:\tStatus: %s",iDev,  StrStat[DevStat]);
	};
	wprintf_s(L"\n\n");
}

/*
	Extract file name and version from the resource file
	Make sure to keep resource file parameters intact
*/
// Based on http://stackoverflow.com/questions/13941837/how-to-get-version-info-from-resources
void DisplayVersion(void)
{
	HRSRC hResInfo;
	DWORD dwSize;
	HGLOBAL hResData;
	LPVOID pRes, pResCopy;
	UINT uLen;
	LPCWSTR InternalName, FileVersion;

	HINSTANCE  hInst = GetModuleHandle(NULL);
	hResInfo = FindResource(hInst, MAKEINTRESOURCE(1), RT_VERSION);
	if (!hResInfo)
	{
		DWORD err = GetLastError();
		return;
	};
	dwSize = SizeofResource(hInst, hResInfo);
	hResData = LoadResource(hInst, hResInfo);
	pRes = LockResource(hResData);
	pResCopy = LocalAlloc(LMEM_FIXED, dwSize);
	CopyMemory(pResCopy, pRes, dwSize);
	FreeResource(hResData);

	if (!VerQueryValue(pResCopy, L"\\StringFileInfo\\040904B0\\InternalName", (LPVOID*)&InternalName, &uLen))
		InternalName = L"<Unknown Name>";
	if (!VerQueryValue(pResCopy, L"\\StringFileInfo\\040904B0\\FileVersion", (LPVOID*)&FileVersion, &uLen))
		FileVersion = L"0.0.0.0";

	wprintf_s(L"\n%s version %s\n", InternalName, FileVersion);

	LocalFree(pResCopy);
}

bool TestBitness(void)
{
#ifdef X64
	int ImmageBitness=64;
#else
	int ImmageBitness=32;
#endif

	int TargetBitness;
	SYSTEM_INFO SystemInfo;

	// Get Data
	GetNativeSystemInfo(&SystemInfo);
	if (SystemInfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
		TargetBitness = 64;
	else
		TargetBitness = 32;

	// Compare
	if (TargetBitness == ImmageBitness)
		return true;

	if (ImmageBitness == 32)
		PrintMessage(IDS_ERR_32ON64, NULL);
	else
		PrintMessage(IDS_ERR_64ON32, NULL);

	return false;
}

#pragma region Command Line Helper functions (cl_*)
// Test if the given string - return true if it is a flag
bool	cl_isFlag(LPCWSTR sItem)
{
	if (sItem[0] == L'-')
		return true;
	else
		return false;
}

// Test if a given item (1-based index) is a flag
bool	cl_isFlag(int argc,  _TCHAR* argv[], int iItem)
{
	// Range check
	if (iItem<1 || iItem>=argc)
		return false;

	// Test the string
	return cl_isFlag(argv[iItem]);
}

// Given an item (1-based index), return the index of the next flag
// If not found return 0
int		cl_getNextFlagIndex(int argc,  _TCHAR* argv[],int iItem)
{
	// Range check
	if (iItem<1 || iItem>=argc)
		return 0;

	for (int i=iItem+1; iItem<argc; i++)
	{
		if (cl_isFlag(argv[i]))
			return i;
	};

	return 0;
}

// Given a flag item (1-based index) and a parameter (1-based) serial number 
// return the index of the next parameter (Not flag)
// Return 0 if not found
/// Example: cl_getNextParamIndex(int argc,  _TCHAR* argv[], 1, int 2);
///  Command Line: vJoyConfig -b W -a X Y Z
//   To get 'Y' : cl_getNextParamIndex(int argc,  _TCHAR* argv[], 3, int 2);
///  
int		cl_getParamIndex(int argc,  _TCHAR* argv[], int iFlag, int i)
{
	// Range check
	if (iFlag<1 || iFlag>=argc || i<1 || (iFlag + i)>=argc )
		return 0;


	if (!cl_isFlag(argv[iFlag + i]))
		return iFlag + i;

	return 0;
}

// Given a flag item (1-based index) 
// Return corresponding command
CMD		cl_Flag2Cmd(int argc,  _TCHAR* argv[], int iItem)
{
	if (!cl_isFlag( argc, argv,  iItem))
		return UNK;

	if (cl_isCreate( argc, argv))
		return CRT;

	switch (argv[iItem][1])
	{
	case 'h':
	case 'H':
		return HLP;

	case 'v':
	case 'V':
		return VER;

	case 'r':
	case 'R':
		return RST;

	case 'd':
	case 'D':
		return DEL;

	case 'f':
	case 'F':
		return FRC;

	case 'a':
	case 'A':
		return AXS;

	case 'b':
	case 'B':
		return BTN;

	case 'p':
	case 'P':
		return PVA;

	case 's':
	case 'S':
		return PVD;

	default:
		return UNK;
	};

	return UNK;
}

// Tests if the first argument is an integer in the range 1-16
bool	cl_isCreate(int argc,  _TCHAR* argv[])
{
	if (argc<2)
		return false;

	if (cl_isFlag(argv[1]))
		return false;

	int id = _wtoi(argv[1]);
	if (id>0 || id<=16)
		return true;
	else
		return false;
}

// Tests if the first argument is an integer in the range 1-16
// Retern the device ID if OK
// Return 0 otherwize
int		cl_getTargetDevice(int argc,  _TCHAR* argv[])
{
	if (argc<2)
		return 0;

	if (cl_isFlag(argv[1]))
		return 0;

	int id = _wtoi(argv[1]);
	if (id>0 || id<=16)
		return id;
	else
		return 0;
}

// Find the first flag to conform with 'flg' (Case insensitive)
// Return item index (1-based) if found
// Otherwise return 0
int		cl_FindFlg(int argc,  _TCHAR* argv[], LPCWSTR flg)
{
	if (argc < 2)
		return 0;

	for (int i=1; i<argc; i++)
	{
		if (!_tcsicmp(flg, argv[i]))
			return i;
	};

	return 0;
}

// Find the first flag to conform with 'flg' (Case insensitive)
// Search the list of following parameters for the given param
bool	cl_isParamExist(int argc,  _TCHAR* argv[], LPCWSTR flg, LPCWSTR param)
{
	// Get the index of the flag
	int iFlg = cl_FindFlg(argc, argv, flg);
	if (!iFlg)
		return false;

	// Start searching from the next item
	int i = iFlg;
	while (i<argc-1)
	{
		i++;
		if (cl_isFlag(argv[i]))
			return false;
				
		if (!_tcsicmp(param, argv[i]))
			return true;
	};

	return false;
}

// Get (int) Value of a parameter of the flag
// The exact parameter is defined by the 0-based 'serial'
// Return true only if value is valid
bool	cl_getParamValue(int argc,  _TCHAR* argv[], LPCWSTR flg, int serial ,int *val)
{
	// Get the index of the flag
	int iFlg = cl_FindFlg(argc, argv, flg);
	if (!iFlg)
		return false;

	int	i =	cl_getParamIndex(argc, argv, iFlg, serial);
	if (i)
	{
		*val = _wtoi(argv[i]);
		return true;
	};

	return false;
}

#pragma endregion
