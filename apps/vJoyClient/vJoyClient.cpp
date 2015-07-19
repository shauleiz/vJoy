// vJoyClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "Devioctl.h"
#include <public.h>
#include "vjoyinterface.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#define WAIT_TIME 2

DWORD GetErrorString(TCHAR * Msg, int Size);
int __cdecl _tmain(__in int argc, __in PZPWSTR argv)
{

	HDEVINFO hDeviceInfo;
	BOOL bRet;
	SP_DEVICE_INTERFACE_DATA  DeviceInterfaceData;
	TCHAR ErrMsg[1000];
	ULONG  bytes;
	USHORT X, Y, Z, ZR;
	JOYSTICK_POSITION	iReport;
	PVOID pPositionMessage;
	UINT	IoCode = LOAD_POSITIONS;
	UINT	IoSize = sizeof(JOYSTICK_POSITION);
	HID_DEVICE_ATTRIBUTES attrib;
	BYTE id=1;
	UINT iInterface=1;

	if (argc>1 && wcslen(argv[1]))
		sscanf((char *)(argv[1]), "%d", &iInterface);


	// Debugging
	UINT data = 0;
	BOOL ok = FALSE;
	int n;

	//ResetAll();

	///// GET_DRV_DEV_EN  /////
	ok = GetNumberExistingVJD(&n);
	if (ok)
 		_tprintf("GetNumberExistingVJD(): Number of enabled devices=%d\n",n);
	else
 		_tprintf("GetNumberExistingVJD() Failed\n");

	//// GET_DRV_DEV_MAX  ///
	ok = GetvJoyMaxDevices(&n);
	if (ok)
 		_tprintf("GetvJoyMaxDevices(): Max number of devices=%d\n", n);
	else
 		_tprintf("GetvJoyMaxDevices() Failed\n");

	//// IS_DRV_FFB_CAP ///
	BOOL Supported;
	ok = vJoyFfbCap(&Supported);
	if (ok)
	{
		if (Supported)
			_tprintf("vJoy Driver supports FFB (vJoyFfbCap())\n");
		else
			_tprintf("vJoy Driver does NOT support FFB (vJoyFfbCap())\n" );
	}
	else
 		_tprintf("vJoyFfbCap() Failed\n");

	_tprintf("\n");
	// Debugging - end

	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled())
	{
		_tprintf("Failed Getting vJoy attributes.\n");
		return -2;
	}
	else
	{
		_tprintf("Vendor: %S\nProduct :%S\nVersion Number:%S\n", TEXT(GetvJoyManufacturerString()),  TEXT(GetvJoyProductString()), TEXT(GetvJoySerialNumberString()));
	};

	if (iInterface)
	{
		// Get the status of the required interface
		VjdStat status = GetVJDStatus(iInterface);
		switch (status) 
		{
		case VJD_STAT_OWN: _tprintf("vJoy Device %d is already owned by this feeder\n", iInterface); 		
			break;
		case VJD_STAT_FREE: _tprintf("vJoy Device %d is free\n", iInterface); 
			break; 
		case VJD_STAT_BUSY: _tprintf("vJoy Device %d is already owned by another feeder\nCannot continue\n", iInterface); 
			return -3;
		case VJD_STAT_MISS: _tprintf("vJoy Device %d is not installed or disabled\nCannot continue\n", iInterface); return -4;
		default: _tprintf("vJoy Device %d general error\nCannot continue\n", iInterface); 
			return -1; 
		};
	};

	getchar();
	if (!AcquireVJD(iInterface))
	{
		_tprintf("Failed to acquire vJoy device number %d.\n", iInterface);
		return -1;
	};

	ResetVJD(iInterface);

	_tprintf("Feeding Period: %dmS\n", WAIT_TIME);

	getchar();

	//ResetAll();

	X = 20;
	Y = 30;
	Z = 40;
	ZR = 80;


	while (1)
	{

		/*** Create the data packet that holds the entire position info ***/
		id = (BYTE)iInterface;
		iReport.bDevice = id;

		Sleep(WAIT_TIME);

		X+=150;
		Y+=250;
		Z+=350;
		ZR-=200;

		iReport.wAxisX=X;
		iReport.wAxisY=Y;
		iReport.wAxisZ=Z;
		iReport.wAxisZRot=ZR;

		if (iReport.bDevice == 2)
			iReport.wAxisXRot=ZR;
		else
			iReport.wAxisXRot=111;


		if (X>0		&& X<2000)		iReport.lButtons = 0x1;
		if (X>2000	&& X<4000)		iReport.lButtons = 0x2;
		if (X>4000	&& X<8000)		iReport.lButtons = 0x4;
		if (X>8000	&& X<12000)		iReport.lButtons = 0x8;
		if (X>12000 && X<16000)		iReport.lButtons = 0x10;
		if (X>16000 && X<20000)		iReport.lButtons = 0x20;
		if (X>20000 && X<24000)		iReport.lButtons = 0x40;
		if (X>24000 && X<32000)		iReport.lButtons = 0X80;

		/***	Debug and printouts ***/
		iReport.lButtons |= (1 << (id-1)); // Debug - Mark id as a constant button
		pPositionMessage = (PVOID)(&iReport);
		_tprintf("Input: X=%4x, Y=%4x, Buttons=%X; ", iReport.wAxisX, iReport.wAxisY, iReport.lButtons);
		_tprintf("\n");

		/*** Feed the driver with the position packet - is fails then wait for input then try to re-acquire device ***/
		if (!UpdateVJD(iInterface, pPositionMessage))
		{
			_tprintf("Feeding vJoy device number %d failed - try to enable device then press enter\n", iInterface);
			getchar();
			AcquireVJD(iInterface);
		}
	};

	_tprintf("OK\n");

	return 0;
}


/* Helper Functions */
DWORD GetErrorString(TCHAR * Msg, int Size)
{
	TCHAR * s;
	DWORD errorcode = GetLastError();
	int nTChars = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,  NULL, errorcode, 0, (LPTSTR)&s,  0, NULL);
	if (!nTChars)
		return errorcode;

	_tcsncpy_s(Msg, Size, s, Size);
	LocalFree(s);
	return errorcode;
}