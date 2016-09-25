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

#include "..\..\vXboxInterfaceCppWrap\vXboxInterfaceCppWrap\IWrapper.h"
#include "..\..\vXboxInterfaceCppWrap\vXboxInterfaceCppWrap\IWrapper.cpp"


using namespace std;
using namespace vXbox;
#ifdef XBOX
#pragma comment(lib, "vJoyInterface")
int main()
{
	BOOL res;
	UCHAR nEmpty;
	IWrapper w;

	// Test if bus exists
	BOOL bus = isVBusExists();
	if (bus)
		printf("Virtual Xbox bus exists\n\n");
	else
	{
		printf("Virtual Xbox bus does NOT exist - Aborting\n\n");
		getchar();
		return -1;
	}

	res = AcquireVJD(1);
	printf("\n\nAcquireVJD(%d): %d\n", 1, res);
	//Sleep(500);

	res = AcquireVJD(1);
	printf("\n\nAcquireVJD(%d): %d\n", 1, res);
	//Sleep(500);

	res = AcquireVJD(1);
	printf("\n\nAcquireVJD(%d): %d\n", 1, res);
	//Sleep(500);

	res = AcquireVJD(1);
	printf("\n\nAcquireVJD(%d): %d\n", 1, res);
	//Sleep(500);




	printf("Press any key to continue \n");
	getchar();



	// Install Virtual Devices
	for (UINT i = 0; i < 6; i++)
	{
		res = GetNumEmptyBusSlots(&nEmpty);
		if (res)
			printf("\n\nNumber of Empty Slots: %d\n", nEmpty);
		else
			printf("\n\nCannot determine Number of Empty Slots");

		res = isControllerExists(i);
		printf("\nisControllerExists(%d): %X\n", i, res);
		res = isControllerOwned(i);
		printf("\nisControllerOwned(%d): %X\n", i, res);
		res = PlugIn(i);
		printf("Plug-in device %d: %X\n", i, res);
		res = isControllerExists(i);
		printf("isControllerExists(%d): %X\n", i, res);
		res = isControllerOwned(i);
		printf("\nisControllerOwned(%d): %X\n", i, res);
	}
	printf("\n");
	SetTriggerL(2, 0, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	SetTriggerR(2, 0, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	for (int T = 0; T < 100; T++)
	{
		SetAxisXY(1, (BYTE)(T * 300), (BYTE)((T * 300)*-1), HID_USAGE_X, HID_USAGE_Y, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		SetAxisRxy(1, (BYTE)(T * 100), (BYTE)((T * 100)*-1), XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		switch (T)
		{
		case 0:
			SetDpadUp(1);
			SetBtnA(1, FALSE);
			SetBtnB(1, TRUE);
			break;
		case 20:
			SetDpadRight(1);
			SetBtnB(1, FALSE);
			break;
		case 40:
			SetDpadDown(1);
			break;
		case 60:
			SetDpadLeft(1);
			break;
		case 80:
			SetDpadOff(1);
			SetBtnA(1, TRUE);
			break;
		}

		SetTriggerL(1, 2 * T, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		SetTriggerR(1, 57 + (2 * T), XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		Sleep(100);
	}
	SetTriggerL(1, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	SetTriggerR(1, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

	printf("Press any key to detect device feedback \n");
	getchar();

	UCHAR Led;
	BOOL Led_Ok, Vib_Ok;
	XINPUT_VIBRATION Vib;
	int iDev = 0;
	while (getchar() != 'q')
	{
		for (iDev = 1; iDev < 5; iDev++)
		{
			Led_Ok = GetLedNumber(iDev, &Led);
			Vib_Ok = GetVibration(iDev, &Vib);
			printf("LED: %d; Left Motor: %d; Right Motor: %d  \n", Led, Vib.wLeftMotorSpeed, Vib.wRightMotorSpeed);
		};
	}

	printf("Press any key to remove devices \n");
	getchar();

#if 1
	// UnInstall Virtual Devices
	for (UINT i = 0; i < 6; i++)
	{
		res = UnPlugForce(i);
		printf("UnPlug (Forced) device %d: %X\n", i, res);
	}
#endif // 0

	printf("Press any key to exit \n");
	getchar();

	return 0;

}
#else // !XBOX
int main()
{
	printf("NOT Xbox mode\n");
	return 0;
}

#endif // !XBOX 