// Test vGenInterface - Unified API for vJoy and vXbox
#include "../stdafx.h"
#include <Xinput.h>
#include "../vGenInterface.h"

int main()
{
	SHORT vJoyVer = GetvJoyVersion();
	BOOL exist = isVBusExists();
	BYTE Led[4];
	XINPUT_VIBRATION Vib;
	
	UnPlugForce(1);
	UnPlugForce(2);
	UnPlugForce(3);
	UnPlugForce(4);

	BOOL Plugged, UnPlugged, Pressed, GetN;
	UCHAR nSlots;

	int res1 = GetVJDButtonNumber(1);
	int res80 = GetVJDButtonNumber(80);
	int res2 = GetVJDButtonNumber(2);

	Plugged = PlugIn(1);
	int resX1 = GetVJDButtonNumber(1001);
	int resX2 = GetVJDButtonNumber(1002);

	Pressed = SetBtnA(1, TRUE);
	GetN  = GetNumEmptyBusSlots(&nSlots);
	UnPlugged = UnPlug(1);	
	Plugged = PlugIn(1);
	SetBtnA(1, TRUE);
	SetBtnA(1, FALSE);
	SetDpad(1, DPAD_UP);
	UnPlugged = UnPlug(2);
	Plugged = PlugIn(4);
	SetTriggerR(4, 100);
	SetTriggerL(4, 200);
	Plugged = PlugIn(5);
	SetDpadLeft(1);
	SetDpadOff(1);
	GetLedNumber(1, &(Led[0]));
	GetLedNumber(4, &(Led[3]));
	GetVibration(1, &Vib);
	GetVibration(4, &Vib);
}