// Test-vXboxInterface.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <crtdbg.h>

extern "C"
{
#include <setupapi.h>
#include <Hidsdi.h>
}

#include <Dbt.h>
#include <tchar.h>

#include "..\..\..\..\inc\public.h"
#include "..\vJoyInterface.h"
#include "..\vXboxInterface.h"

#ifndef XBOX
#pragma comment(lib, "vJoyInterface")
int main()
{
	BOOL res;


	// Test if bus exists
	BOOL bus =	 isVBusExists();
	if (bus)
		printf("Virtual Xbox bus exists\n\n");
	else
	{
		printf("Virtual Xbox bus does NOT exist - Aborting\n\n");
		getchar();
		return -1;
	}


	// Install Virtual Devices
	for (UINT i = 0; i < 6; i++)
	{
		res = PlugIn(i);
		printf("Plug-in device %d: %X\n", i, res);
	}
	printf("\n");

	for (int T = 0; T < 100; T++)
	{
		SetAxisX(1, T * 300);
		SetAxisY(1, (T * 300)*-1);
		SetAxisRx(1, T * 100);
		SetAxisRy(1, (T * 100)*-1);
		switch (T)
		{
			case 0:
				SetDpad(1, DPAD_UP);
				SetBtnA(1, FALSE);
				SetBtnB(1, TRUE);
				break;
			case 20:
				SetDpad(1, DPAD_RIGHT);
				SetBtnB(1, FALSE);
				break;
			case 40:
				SetDpad(1, DPAD_DOWN);
				break;
			case 60:
				SetDpad(1, DPAD_LEFT);
				break;
			case 80:
				SetDpad(1, DPAD_OFF);
				SetBtnA(1, TRUE);
				break;
		}
		
		Sleep(100);
	}

	printf("Press any key to remove devices \n");
	getchar();

#if 1
	// UnInstall Virtual Devices
	for (UINT i = 0; i < 6; i++)
	{
		res = UnPlug(i);
		printf("UnPlug device %d: %X\n", i, res);
	}
#endif // 0

	printf("Press any key to exit \n");
	getchar();

	return 0;

}
#else // XBOX
int main()
{
	printf("NOT Xbox mode\n");
	return 0;
}

#endif // XBOX 