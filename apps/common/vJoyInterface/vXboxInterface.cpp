#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <crtdbg.h>
#include <iostream>

extern "C"
{
#include <setupapi.h>
#include <Hidsdi.h>
}

#include <Dbt.h>
#include <tchar.h>
#include "vJoyInterface.h"
#include "vXboxInterface.h"

using namespace std;
using namespace vJoy;

extern "C"
{

	VJOYINTERFACE_API BOOL	__cdecl	 isVBusExists(void)
	{
		TCHAR path[MAX_PATH];

		int n = GetVXbusPath(path, MAX_PATH);

		if (n > 0)
			return TRUE;
		else
			return FALSE;
	}

	VJOYINTERFACE_API BOOL	__cdecl	 GetNumEmptyBusSlots(UCHAR * nSlots)
	{
		UCHAR output[1];
		DWORD trasfered = 0;


		if (g_hBus == INVALID_HANDLE_VALUE)
			g_hBus = GetVXbusHandle();
		if (g_hBus == INVALID_HANDLE_VALUE)
			return FALSE;

		// Send request to bus
		if (DeviceIoControl(g_hBus, IOCTL_BUSENUM_EMPTY_SLOTS, nullptr, 0, output, 1, &trasfered, nullptr))
		{
			*nSlots = *output;
			return TRUE;
		}

		return FALSE;
	}

	VJOYINTERFACE_API BOOL	__cdecl	 isControllerExists(UINT UserIndex)
	{
		BOOL out = FALSE;
		ULONG buffer[1];
		ULONG output[1];
		DWORD trasfered = 0;

		if (UserIndex < 1 || UserIndex>4)
			return out;

		if (g_hBus == INVALID_HANDLE_VALUE)
			g_hBus = GetVXbusHandle();
		if (g_hBus == INVALID_HANDLE_VALUE)
			return out;

		// Prepare the User Index for sending
		buffer[0] = UserIndex;

		// Send request to bus
		if (DeviceIoControl(g_hBus, IOCTL_BUSENUM_ISDEVPLUGGED, buffer, _countof(buffer), output, 4, &trasfered, nullptr))
		{
			if (*output != 0)
				out = TRUE;
		};

		return out;
	}

	VJOYINTERFACE_API BOOL	__cdecl	 isControllerOwned(UINT UserIndex)
	{
		ULONG OrigProcID = 0;
		ULONG ThisProcID = 0;

		// Sanity Check
		if (UserIndex < 1 || UserIndex>4)
			return FALSE;

		// Does controler exist?
		if (!isControllerExists(UserIndex))
			return FALSE;

		// Get ID of the process that created the controler?
		if (!GetCreateProcID(UserIndex, &OrigProcID) || !OrigProcID)
			return FALSE;

		// Get ID of current process
		ThisProcID = GetCurrentProcessId();
		if (!ThisProcID)
			return FALSE;

		// Compare
		if (ThisProcID != OrigProcID)
			return FALSE;

		return TRUE;

	}

	VJOYINTERFACE_API BOOL	__cdecl	 PlugIn(UINT UserIndex)
	{
		BOOL out = FALSE;


		if (UserIndex < 1 || UserIndex>4)
			return out;

		if (g_hBus == INVALID_HANDLE_VALUE)
			g_hBus = GetVXbusHandle();
		if (g_hBus == INVALID_HANDLE_VALUE)
			return out;

		DWORD trasfered = 0;
		UCHAR buffer[16] = {};

		buffer[0] = 0x10;

		buffer[4] = ((UserIndex >> 0) & 0xFF);
		buffer[5] = ((UserIndex >> 8) & 0xFF);
		buffer[6] = ((UserIndex >> 16) & 0xFF);
		buffer[8] = ((UserIndex >> 24) & 0xFF);

		if (DeviceIoControl(g_hBus, IOCTL_BUSENUM_PLUGIN_HARDWARE, buffer, _countof(buffer), nullptr, 0, &trasfered, nullptr))
		{
			out = TRUE;
			g_vDevice[UserIndex - 1] = TRUE;
		};

		//CloseHandle(h);
		DWORD error = 0;
		if (out)
		{
			//std::cout << "IOCTL_BUSENUM_PLUGIN_HARDWARE 0X" << IOCTL_BUSENUM_PLUGIN_HARDWARE << "\n" << endl;
			error = 0;
		}
		else
		{
			error = GetLastError();
			//std::cout << "IOCTL_BUSENUM_PLUGIN_HARDWARE 0X" << IOCTL_BUSENUM_PLUGIN_HARDWARE << "Failed (Error:0X" << error << ")\n" << endl;
		}

		return out;
	}

	VJOYINTERFACE_API BOOL	__cdecl	 UnPlug(UINT UserIndex)
	{
		return UnPlug_Opt(UserIndex, FALSE);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 UnPlugForce(UINT UserIndex)
	{
		return UnPlug_Opt(UserIndex, TRUE);
	}

	VJOYINTERFACE_API BOOL	__cdecl	SetDpad(UINT UserIndex, INT Value)
	{
		g_Gamepad[UserIndex - 1].wButtons &= 0xFFF0;
		g_Gamepad[UserIndex - 1].wButtons |= Value;
		return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadUp(UINT UserIndex)
	{
		return SetDpad(UserIndex, DPAD_UP);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadRight(UINT UserIndex)
	{
		return SetDpad(UserIndex, DPAD_RIGHT);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadDown(UINT UserIndex)
	{
		return SetDpad(UserIndex, DPAD_DOWN);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadLeft(UINT UserIndex)
	{
		return SetDpad(UserIndex, DPAD_LEFT);
	}
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadOff(UINT UserIndex)
	{
		return SetDpad(UserIndex, 0);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadUpRight(UINT UserIndex) {
		return SetDpad(UserIndex, DPAD_UP | DPAD_RIGHT);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadDownRight(UINT UserIndex) {
		return SetDpad(UserIndex, DPAD_DOWN | DPAD_RIGHT);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadDownLeft(UINT UserIndex) {
		return SetDpad(UserIndex, DPAD_DOWN | DPAD_LEFT);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadUpLeft(UINT UserIndex) {
		return SetDpad(UserIndex, DPAD_UP | DPAD_LEFT);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnAny(UINT UserIndex, BOOL Press, UINT xBtn)
	{
		g_Gamepad[UserIndex - 1].wButtons &= ~xBtn;
		g_Gamepad[UserIndex - 1].wButtons |= xBtn*Press;
		return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
	}
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnA(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_A);
	}
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnB(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_B);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnX(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_X);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnY(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_Y);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnStart(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_START);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnBack(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_BACK);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLT(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_LEFT_THUMB);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRT(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_RIGHT_THUMB);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLB(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_LEFT_SHOULDER);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRB(UINT UserIndex, BOOL Press)
	{
		return SetBtnAny(UserIndex, Press, (UINT)XINPUT_GAMEPAD_RIGHT_SHOULDER);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetTriggerL(UINT UserIndex, BYTE Value, UINT Threshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD) // Left Trigger
	{
		return SetTriggerLR(UserIndex, Value, 0, Threshold);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 SetTriggerR(UINT UserIndex, BYTE Value, UINT Threshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD) // Right Trigger
	{
		return SetTriggerLR(UserIndex, Value, 0, Threshold);
	}

	VJOYINTERFACE_API BOOL	__cdecl	 GetLedNumber(UINT UserIndex, PBYTE pLed)
	{
		BOOL ref = XOutputSetGetState(UserIndex, &g_Gamepad[UserIndex - 1], nullptr, nullptr, nullptr, pLed);
		if (ref)
			*pLed++;
		return ref;
	}

	VJOYINTERFACE_API BOOL GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib)
	{
		BYTE LargeMotor, SmallMotor, Vibrate;
		BOOL ref = XOutputSetGetState(UserIndex, &g_Gamepad[UserIndex - 1], &Vibrate, &LargeMotor, &SmallMotor, nullptr);
		if (ref)
		{
			if (Vibrate)
			{
				(*pVib).wLeftMotorSpeed = LargeMotor * 256;
				(*pVib).wRightMotorSpeed = SmallMotor * 256;
			}
			else
				(*pVib).wLeftMotorSpeed = (*pVib).wRightMotorSpeed = 0;
		};
		return ref;
	}
}


#ifdef XBOX
#pragma region Legacy Interface Functions
/////	General driver data
VJOYINTERFACE_API SHORT __cdecl GetvJoyVersion(void)
{
	// Hard-coded
	return 0x216;
}
VJOYINTERFACE_API BOOL	__cdecl vJoyEnabled(void)
{
	return isVBusExists();
}
VJOYINTERFACE_API PVOID	__cdecl	GetvJoyProductString(void)
{
	// TODO: Hard Coded
	return (PVOID)L"vXbox (Temporary version)";
}
VJOYINTERFACE_API PVOID	__cdecl	GetvJoyManufacturerString(void)
{
	// TODO: Hard Coded
	return (PVOID)L"Shaul Eizikovich";
}
VJOYINTERFACE_API PVOID	__cdecl	GetvJoySerialNumberString(void)
{
	// TODO: Hard Coded
	return (PVOID)L"2.1.6";
}
//VJOYINTERFACE_API BOOL	__cdecl	DriverMatch(WORD * DllVer, WORD * DrvVer);
//VJOYINTERFACE_API VOID	__cdecl	RegisterRemovalCB(RemovalCB cb, PVOID data);
VJOYINTERFACE_API BOOL	__cdecl	vJoyFfbCap(BOOL * Supported)	// Is this version of vJoy capable of FFB?
{
	*Supported = FALSE;
	return TRUE;
}
VJOYINTERFACE_API BOOL	__cdecl	GetvJoyMaxDevices(int * n)	// What is the maximum possible number of vJoy devices
{
	return 4;
}
VJOYINTERFACE_API BOOL	__cdecl	GetNumberExistingVJD(int * n)	// What is the number of vJoy devices currently enabled
{
	UINT count = 0;
	for (auto dev : g_vDevice)
		if (dev)
			count++;
	*n = count;
	return TRUE;

}


/////	vJoy Device properties
VJOYINTERFACE_API int	__cdecl  GetVJDButtonNumber(UINT rID)	// Get the number of buttons defined in the specified VDJ
{
	return 10;
}
VJOYINTERFACE_API int	__cdecl  GetVJDDiscPovNumber(UINT rID)		  // Get the number of descrete-type POV hats defined in the specified VDJ
{
	return 1;
}
VJOYINTERFACE_API int	__cdecl  GetVJDContPovNumber(UINT rID)	// Get the number of descrete-type POV hats defined in the specified VDJ
{
	return 0;
}
VJOYINTERFACE_API BOOL	__cdecl  GetVJDAxisExist(UINT rID, UINT Axis) // Test if given axis defined in the specified VDJ
{
	switch (Axis)
	{
	case HID_USAGE_X:
	case HID_USAGE_Y:
	case HID_USAGE_RX:
	case HID_USAGE_RY:
	case HID_USAGE_SL0:
	case HID_USAGE_SL1:
		return TRUE;
		break;
	default:
		return FALSE;
	};
}
VJOYINTERFACE_API BOOL	__cdecl  GetVJDAxisMax(UINT rID, UINT Axis, LONG * Max) // Get logical Maximum value for a given axis defined in the specified VDJ
{
	switch (Axis)
	{
	case HID_USAGE_X:
	case HID_USAGE_Y:
	case HID_USAGE_RX:
	case HID_USAGE_RY:
		*Max = AXIS_MAX;
		break;
	case HID_USAGE_SL0:
	case HID_USAGE_SL1:
		*Max = AXIS_SL_MAX;
		break;
	default:
		return FALSE;
	};

	return TRUE;

}
VJOYINTERFACE_API BOOL	__cdecl  GetVJDAxisMin(UINT rID, UINT Axis, LONG * Min) // Get logical Minimum value for a given axis defined in the specified VDJ
{
	switch (Axis)
	{

	case HID_USAGE_X:
	case HID_USAGE_Y:
	case HID_USAGE_RX:
	case HID_USAGE_RY:
		*Min = AXIS_MIN;
		break;
	case HID_USAGE_SL0:
	case HID_USAGE_SL1:
		*Min = AXIS_SL_MIN;
		break;
	default:
		return FALSE;
	};

	return TRUE;
}
VJOYINTERFACE_API enum vJoy::VjdStat	__cdecl	GetVJDStatus(UINT rID)			// Get the status of the specified vJoy Device.
{
	if (!isControllerExists(rID))
		return VJD_STAT_MISS;

	if (isControllerOwned(rID))
		return 	VJD_STAT_OWN;
	else
		return VJD_STAT_BUSY;

	return 	 VJD_STAT_UNKN;
}
VJOYINTERFACE_API BOOL		__cdecl	isVJDExists(UINT rID)					// TRUE if the specified vJoy Device exists
{
	return isControllerExists(rID);
}

/////	Write access to vJoy Device - Basic

VJOYINTERFACE_API BOOL		__cdecl	AcquireVJD(UINT rID)				// Acquire the specified vJoy Device.
{
	BOOL res;

	// If the device is owned by another application - unplug it
	if (GetVJDStatus(rID) == VJD_STAT_BUSY)
	{
		res = UnPlug(rID);
		if (!res)
			return res;
	}

	// If the device does not exist - plug it in
	if (GetVJDStatus(rID) == VJD_STAT_FREE || GetVJDStatus(rID) == VJD_STAT_MISS)
	{
		res = PlugIn(rID);
		if (!res)
			return res;
	}

	// If device not Owned then it is a problem
	if (GetVJDStatus(rID) != VJD_STAT_OWN)
		return 	FALSE;

	return TRUE;
}

VJOYINTERFACE_API VOID		__cdecl	RelinquishVJD(UINT rID)			// Relinquish the specified vJoy Device.
{
	UnPlug(rID);
}


//// Reset functions
VJOYINTERFACE_API BOOL		__cdecl	ResetVJD(UINT rID)			// Reset all controls to predefined values in the specified VDJ
{
	XINPUT_GAMEPAD Gamepad;

	// Init
	memset(&Gamepad, 0, sizeof(XINPUT_GAMEPAD));

	// Send joystick position structure to vJoy device
	return  XOutputSetState(rID, &Gamepad);

}
VJOYINTERFACE_API VOID		__cdecl	ResetAll(void)				// Reset all controls to predefined values in all VDJ
{
	ResetVJD(1);
	ResetVJD(2);
	ResetVJD(3);
	ResetVJD(4);
}
VJOYINTERFACE_API BOOL		__cdecl	ResetButtons(UINT rID)		// Reset all buttons (To 0) in the specified VDJ
{
	g_Gamepad[rID].wButtons &= 0x000F;
	// Send joystick position structure to vJoy device
	return  XOutputSetState(rID, &g_Gamepad[rID]);
}
VJOYINTERFACE_API BOOL		__cdecl	ResetPovs(UINT rID)		// Reset all POV Switches (To -1) in the specified VDJ
{
	g_Gamepad[rID].wButtons &= 0xFFF0;
	// Send joystick position structure to vJoy device
	return  XOutputSetState(rID, &g_Gamepad[rID]);

}

// Write data
VJOYINTERFACE_API BOOL		__cdecl	UpdateVJD(UINT rID, PVOID pData)	// Update the position data of the specified vJoy Device.
{
	PJOYSTICK_POSITION_V2 pJoyPos = (PJOYSTICK_POSITION_V2)pData;
	XINPUT_GAMEPAD Gamepad;

	// Init
	memset(&Gamepad, 0, sizeof(XINPUT_GAMEPAD));


	/////  Axes
	Gamepad.sThumbLX = 2 * (pJoyPos->wAxisX - 1) - AXIS_MAX;
	Gamepad.sThumbLY = 2 * (pJoyPos->wAxisY - 1) - AXIS_MAX;
	Gamepad.sThumbRX = 2 * (pJoyPos->wAxisXRot - 1) - AXIS_MAX;
	Gamepad.sThumbRY = 2 * (pJoyPos->wAxisYRot - 1) - AXIS_MAX;
	Gamepad.bLeftTrigger = (pJoyPos->wAxisZRot - 1) / AXIS_SL_MAX;
	Gamepad.bRightTrigger = (pJoyPos->wAxisZ - 1) / AXIS_SL_MAX;

	//// Buttons
	// Button 1 ==> Button A
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 1, XINPUT_GAMEPAD_A);
	// Button 2 ==> Button B
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 2, XINPUT_GAMEPAD_B);
	// Button 3  ==> Button X
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 3, XINPUT_GAMEPAD_X);
	// Button 4  ==> Button Y
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 4, XINPUT_GAMEPAD_Y);
	// Button 5  ==> Button LB (Left Bumper)
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 5, XINPUT_GAMEPAD_LEFT_SHOULDER);
	// Button 6  ==> Button RB (Right Bumper)
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 6, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	// Button 7  ==> Back
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 7, XINPUT_GAMEPAD_BACK);
	// Button 8  ==> Start
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 8, XINPUT_GAMEPAD_START);
	// Button 9  ==> Button LSB (Left Stick Button)
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 9, XINPUT_GAMEPAD_LEFT_THUMB);
	// Button 10 ==> Button RSB (Right Stick Button)
	Gamepad.wButtons = ConvertButton(pJoyPos->lButtons, Gamepad.wButtons, 10, XINPUT_GAMEPAD_RIGHT_THUMB);

	// Dpad / Discrete POV #1
	switch (pJoyPos->bHats)
	{
	case 0:
		Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
		break;
	case 1:
		Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
		break;
	case 2:
		Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
		break;
	case 4:
		Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
		break;
	default:
		Gamepad.wButtons &= 0xFFF0;
	}


	// Send joystick position structure to vJoy device
	return  XOutputSetState(rID, &Gamepad);
}

/**
Axis DeadZones and Trigger Thresholds function (Xinput recommends)
https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx
*/
VJOYINTERFACE_API BOOL __cdecl	SetAxisXY(UINT UserIndex, SHORT ValueX, SHORT ValueY, 
	UINT AxisX = HID_USAGE_X, UINT AxisY = HID_USAGE_Y, 
	UINT DeadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, SHORT Axis_Max = AXIS_MAX)
{

	float LX = ValueX;
	float LY = ValueY;

	//determine how far the controller is pushed
	float magnitude = sqrt(LX*LX + LY*LY);
	float normalizedMagnitude = 0;

	//check if the controller is outside a circular dead zone
	if (magnitude > DeadZone)
	{
		//clip the magnitude at its expected maximum value
		if (magnitude > Axis_Max) magnitude = Axis_Max;

		//adjust magnitude relative to the end of the dead zone
		magnitude -= DeadZone;

		//optionally normalize the magnitude with respect to its expected range
		//giving a magnitude value of 0.0 to 1.0
		normalizedMagnitude = magnitude / (Axis_Max - DeadZone);

		//determine the direction the controller is pushed
		//ValueX = (SHORT)roundf(LX / magnitude);
		//ValueY = (SHORT)roundf(LY / magnitude);
	}
	else //if the controller is in the deadzone zero out the magnitude
	{
		magnitude = 0.0;
		normalizedMagnitude = 0.0;
	}
	BOOL r = TRUE;
	if (AxisX != 0)
		r = r && SetAxis((LONG)roundf(LX*normalizedMagnitude), UserIndex, AxisX);
	if (AxisY != 0)
		r = r && SetAxis((LONG)roundf(LY*normalizedMagnitude), UserIndex, AxisY);
	return r;
}

VJOYINTERFACE_API BOOL __cdecl	SetTriggerLR(UINT UserIndex, SHORT ValueL, SHORT ValueR, UINT Threshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
{
	BOOL b = TRUE;
	b= b && SetAxisXY(UserIndex, ValueL, 0, HID_USAGE_SL0, 0, Threshold, AXIS_SL_MAX);
	b = b && SetAxisXY(UserIndex, 0, ValueR, 0, HID_USAGE_SL1, Threshold, AXIS_SL_MAX);
	return FALSE;
}

VJOYINTERFACE_API BOOL __cdecl GetTriggerLR(UINT UserIndex, PBYTE ValueL, PBYTE ValueR, UINT AxisL, UINT AxisR) {
	if (XOutputSetGetState(UserIndex, &g_Gamepad[UserIndex - 1], nullptr, nullptr, nullptr, nullptr))
	{
		XINPUT_GAMEPAD *pad = &g_Gamepad[UserIndex - 1];
		if (AxisL == HID_USAGE_SL0)
			*ValueL = (*pad).bLeftTrigger;
		if (AxisR == HID_USAGE_SL1)
			*ValueR = (*pad).bRightTrigger;
		return true;
	}
	return false;
}
VJOYINTERFACE_API BOOL __cdecl GetAxisXY(UINT UserIndex, PSHORT ValueX, PSHORT ValueY, UINT AxisX, UINT AxisY) {
	if (XOutputSetGetState(UserIndex, &g_Gamepad[UserIndex - 1], nullptr, nullptr, nullptr, nullptr))
	{
		XINPUT_GAMEPAD *pad = &g_Gamepad[UserIndex - 1];
		if (AxisX == HID_USAGE_X)
			*ValueX = (*pad).sThumbLX;
		if (AxisY == HID_USAGE_Y)
			*ValueY = (*pad).sThumbLY;

		if (AxisX == HID_USAGE_RX)
			*ValueX = (*pad).sThumbRX;
		if (AxisY == HID_USAGE_RY)
			*ValueY = (*pad).sThumbRY;
		return true;
	}
	return false;
}
/**
This external function doesn't manage DEADZONES. Xinput recommend using the function SetAxisXY().
*/
VJOYINTERFACE_API BOOL		__cdecl	SetAxis(LONG Value, UINT rID, UINT Axis)		// Write Value to a given axis defined in the specified VDJ 
{
	/** Clip as Xinput values */
	SHORT NormVal = max(AXIS_MIN, min(AXIS_MAX, Value));
	UINT TrigVal = max(AXIS_SL_MIN, min(AXIS_SL_MAX, Value));

	switch (Axis)
	{
	case HID_USAGE_X:
		g_Gamepad[rID - 1].sThumbLX = NormVal;
		break;
	case HID_USAGE_Y:
		g_Gamepad[rID - 1].sThumbLY = NormVal;
		break;
	case HID_USAGE_RX:
		g_Gamepad[rID - 1].sThumbRX = NormVal;
		break;
	case HID_USAGE_RY:
		g_Gamepad[rID - 1].sThumbRY = NormVal;
		break;
	case HID_USAGE_SL0:
		g_Gamepad[rID - 1].bLeftTrigger = TrigVal;
		break;
	case HID_USAGE_SL1:
		g_Gamepad[rID - 1].bRightTrigger = TrigVal;
		break;
	default:
		return FALSE;
	}
	return XOutputSetState(rID, &g_Gamepad[rID - 1]);

}

/**
Converts UCHAR and finds the appropriate button press method. This is for configuration.
See SetBtn(BOOL,UINT,UINT), a direct function to XINPUT.
**/
VJOYINTERFACE_API BOOL		__cdecl	SetBtn(BOOL Value, UINT rID, UCHAR nBtn)		// Write Value to a given button defined in the specified VDJ 
{
	switch (nBtn)
	{
	case 1:
		SetBtnA(rID, Value);
		break;
	case 2:
		SetBtnB(rID, Value);
		break;
	case 3:
		SetBtnX(rID, Value);
		break;
	case 4:
		SetBtnY(rID, Value);
		break;
	case 5:
		SetBtnLB(rID, Value);
		break;
	case 6:
		SetBtnRB(rID, Value);
		break;
	case 7:
		SetBtnBack(rID, Value);
		break;
	case 8:
		SetBtnStart(rID, Value);
		break;
	case 9:
		SetBtnLT(rID, Value);
		break;
	case 10:
		SetBtnRT(rID, Value);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}
/**
Converts UCHAR and finds the appropriate button press method. This is for configuration.
*/
VJOYINTERFACE_API BOOL		__cdecl	SetDiscPov(int Value, UINT rID, UCHAR nPov)	// Write Value to a given descrete POV defined in the specified VDJ 
{
	if (nPov != 1)
		return FALSE;

	switch (Value)
	{
	case 0:
		SetDpadUp(rID);
		break;
	case 1:
		SetDpadRight(rID);
		break;
	case 2:
		SetDpadDown(rID);
		break;
	case 3:
		SetDpadLeft(rID);
		break;
	case 4:
		SetDpadUpRight(rID);
		break;
	case 5:
		SetDpadDownRight(rID);
		break;
	case 6:
		SetDpadDownRight(rID);
		break;
	case 7:
		SetDpadUpLeft(rID);
		break;
	case 0xF:
	case -1:
		SetDpadOff(rID);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

/** TODO: handle bi-directional press (Up-Right, Down-Left, etc.) */
VJOYINTERFACE_API BOOL		__cdecl	SetContPov(DWORD Value, UINT rID, UCHAR nPov)	// Write Value to a given continuous POV defined in the specified VDJ 
{
	if (nPov != 1)
		return FALSE;
	INT xinput = 0; //OFF
	if (Value >= 31500 || Value < 4500)
		xinput |= DPAD_UP;
	if (Value >= 4500 && Value < 13500)
		xinput |= DPAD_RIGHT;
	if (Value >= 13500 && Value < 22500)
		xinput |= DPAD_DOWN;
	if (Value >= 22500 && Value < 31500)
		xinput |= DPAD_LEFT;
	return SetDpad(rID, xinput);
}

#pragma endregion
#endif // XBOX


///-------------------------------------------------------------------------------------------------
/// <summary>	Output set state. </summary>
///
/// <remarks>	Benjamin, 15.03.2016. </remarks>
///
/// <param name="dwUserIndex">	Zero-based index of the user. </param>
/// <param name="pGamepad">	  	[in,out] If non-null, the state. </param>
///
/// <returns>	A DWORD. </returns>
///-------------------------------------------------------------------------------------------------
BOOL XOutputSetState(DWORD UserIndex, XINPUT_GAMEPAD* pGamepad)
{
	{
		BOOL out = FALSE;

		if (UserIndex < 1 || UserIndex>4)
			return out;

		if (!g_vDevice[UserIndex - 1])
			return out;

		DWORD trasfered = 0;
		BYTE buffer[28] = {};

		buffer[0] = 0x1C;

		// encode user index
		buffer[4] = ((UserIndex >> 0) & 0xFF);
		buffer[5] = ((UserIndex >> 8) & 0xFF);
		buffer[6] = ((UserIndex >> 16) & 0xFF);
		buffer[7] = ((UserIndex >> 24) & 0xFF);

		buffer[9] = 0x14;

		// concat gamepad info to buffer
		memcpy_s(&buffer[10], _countof(buffer), pGamepad, sizeof(XINPUT_GAMEPAD));

		// vibration and LED info end up here
		BYTE output[FEEDBACK_BUFFER_LENGTH] = {};

		// send report to bus, receive vibration and LED status
		if (!DeviceIoControl(g_hBus, IOCTL_BUSENUM_REPORT_HARDWARE, buffer, _countof(buffer), output, FEEDBACK_BUFFER_LENGTH, &trasfered, nullptr))
		{
			return FALSE;
		}

		// Save last successful position data
		memcpy_s(&g_Gamepad[UserIndex - 1], sizeof(XINPUT_GAMEPAD), pGamepad, sizeof(XINPUT_GAMEPAD));

		return TRUE;
	}
}

BOOL XOutputSetGetState(DWORD UserIndex, XINPUT_GAMEPAD * pGamepad, PBYTE bVibrate, PBYTE bLargeMotor, PBYTE bSmallMotor, PBYTE bLed)
{
	BOOL out = FALSE;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	if (!g_vDevice[UserIndex - 1])
		return out;

	DWORD trasfered = 0;
	BYTE buffer[28] = {};

	buffer[0] = 0x1C;

	// encode user index
	buffer[4] = ((UserIndex >> 0) & 0xFF);
	buffer[5] = ((UserIndex >> 8) & 0xFF);
	buffer[6] = ((UserIndex >> 16) & 0xFF);
	buffer[7] = ((UserIndex >> 24) & 0xFF);

	buffer[9] = 0x14;

	// concat gamepad info to buffer
	memcpy_s(&buffer[10], _countof(buffer), pGamepad, sizeof(XINPUT_GAMEPAD));

	// vibration and LED info end up here
	BYTE output[FEEDBACK_BUFFER_LENGTH] = {};

	// send report to bus, receive vibration and LED status
	if (!DeviceIoControl(g_hBus, IOCTL_BUSENUM_REPORT_HARDWARE, buffer, _countof(buffer), output, FEEDBACK_BUFFER_LENGTH, &trasfered, nullptr))
	{
		return FALSE;
	}

	// Save last successful position data
	memcpy_s(&g_Gamepad[UserIndex - 1], sizeof(XINPUT_GAMEPAD), pGamepad, sizeof(XINPUT_GAMEPAD));

	// cache feedback
	if (bVibrate != nullptr)
	{
		*bVibrate = (output[1] == 0x08) ? 0x01 : 0x00;
	}

	if (bLargeMotor != nullptr)
	{
		*bLargeMotor = output[3];
	}

	if (bSmallMotor != nullptr)
	{
		*bSmallMotor = output[4];
	}

	if (bLed != nullptr)
	{
		*bLed = output[8];
	}

	return TRUE;
}


BOOL GetCreateProcID(DWORD UserIndex, PULONG ProcID)
{
	BOOL out = FALSE;
	ULONG buffer[1];
	ULONG output[1];
	DWORD trasfered = 0;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	if (g_hBus == INVALID_HANDLE_VALUE)
		g_hBus = GetVXbusHandle();
	if (g_hBus == INVALID_HANDLE_VALUE)
		return out;

	// Prepare the User Index for sending
	buffer[0] = UserIndex;

	// Send request to bus
	if (DeviceIoControl(g_hBus, IOCTL_BUSENUM_PROC_ID, buffer, _countof(buffer), output, 4, &trasfered, nullptr))
	{
		*ProcID = output[0];
		out = TRUE;
	};

	return out;

}

///-------------------------------------------------------------------------------------------------
/// <summary>	Get Device Interface path. </summary>
///
/// <remarks>	Shaul, 17.03.2016. </remarks>
///
/// <param name="path">	Pointer to the output buffer holding the Device Interface path string. </param>
/// <param name="size">	Size of the output buffer in WCHARs.</param>
///
/// <returns>	If sucessful: Size of the the path in WCHARs
///             Negative number indicates failure.</returns>
/// 
///-------------------------------------------------------------------------------------------------
int GetVXbusPath(LPCTSTR path, UINT size)
{
	SP_DEVICE_INTERFACE_DATA deviceInterfaceData = {};
	deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
	// GUID deviceClassGuid = { 0xF679F562, 0x3164, 0x42CE,{ 0xA4, 0xDB, 0xE7 ,0xDD ,0xBE ,0x72 ,0x39 ,0x09 } };
	GUID deviceClassGuid = GUID_DEVINTERFACE_SCPVBUS;
	DWORD memberIndex = 0;
	DWORD requiredSize = 0;

	auto deviceInfoSet = SetupDiGetClassDevs(&deviceClassGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &deviceClassGuid, memberIndex, &deviceInterfaceData))
	{
		// get required target buffer size
		SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, nullptr, 0, &requiredSize, nullptr);

		// allocate target buffer
		auto detailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(requiredSize));
		if (detailDataBuffer == NULL)
			return -1;
		detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		// get detail buffer
		if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, requiredSize, &requiredSize, nullptr))
		{
			SetupDiDestroyDeviceInfoList(deviceInfoSet);
			free(detailDataBuffer);
			return -1;
		}

		// Copy	the path to output buffer
		memcpy((void *)path, detailDataBuffer->DevicePath, requiredSize * sizeof(WCHAR));

		// Cleanup
		SetupDiDestroyDeviceInfoList(deviceInfoSet);
		free(detailDataBuffer);
	}
	else
		return -1;

	return requiredSize;
}

HANDLE GetVXbusHandle(void)
{
	TCHAR path[MAX_PATH];

	int n = GetVXbusPath(path, MAX_PATH);

	if (n < 1)
		return INVALID_HANDLE_VALUE;

	// bus found, open it and obtain handle
	g_hBus = CreateFile(path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	return g_hBus;
}

WORD ConvertButton(LONG vBtns, WORD xBtns, UINT vBtn, UINT xBtn)
{
	WORD out;
	out = ((vBtns&(1 << (vBtn - 1))) == 0) ? xBtns & ~xBtn : xBtns | xBtn;
	return out;
}

BOOL UnPlug_Opt(UINT UserIndex, BOOL Force)
{
	BOOL out = FALSE;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	if (g_hBus == INVALID_HANDLE_VALUE)
		g_hBus = GetVXbusHandle();
	if (g_hBus == INVALID_HANDLE_VALUE)
		return out;

	DWORD trasfered = 0;
	BUSENUM_UNPLUG_HARDWARE buffer = {};


	buffer.Size = sizeof(BUSENUM_UNPLUG_HARDWARE);
	buffer.SerialNo = UserIndex;

	if (Force)
		buffer.Flags = 0x0001;
	else
		buffer.Flags = 0x0000;

	if (DeviceIoControl(g_hBus, IOCTL_BUSENUM_UNPLUG_HARDWARE, (LPVOID)(&buffer), buffer.Size, nullptr, 0, &trasfered, nullptr))
	{
		out = TRUE;
		g_vDevice[UserIndex - 1] = FALSE;
	}

	return out;
}