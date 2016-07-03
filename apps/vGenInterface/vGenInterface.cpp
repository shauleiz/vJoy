// vGenInterface.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"

// Compilation directives
#define USE_STATIC
#define STATIC

#include <Xinput.h>
#include "vGenInterface.h"
#include "public.h"
#include "private.h"

#include "vjoyinterface.h"
#include "XOutput.h"

#pragma comment(lib, "vJoyInterfaceStat.lib")
#pragma comment(lib, "XOutputStatic_1_2.lib")


#pragma region Interface Functions (vJoy)
VGENINTERFACE_API SHORT GetvJoyVersion(void)
{
	return vJoyNS::GetvJoyVersion();
}

VGENINTERFACE_API int GetVJDButtonNumber(UINT rID)	// Get the number of buttons defined in the specified device
{
	if (Range_vXbox(rID))
	{
		if (!IX_isControllerExists(to_vXbox(rID)))
			return 0;
		else
			return 10;
	}
	else
		return vJoyNS::GetVJDButtonNumber(rID);
}

VGENINTERFACE_API int GetVJDDiscPovNumber(UINT rID)	// Get the number of POVs defined in the specified device
{
	if (Range_vXbox(rID))
	{
		if (!IX_isControllerExists(to_vXbox(rID)))
			return 0;
		else
			return 1;
	}
	else
		return vJoyNS::GetVJDDiscPovNumber(rID);
}

VGENINTERFACE_API int GetVJDContPovNumber(UINT rID)	// Get the number of POVs defined in the specified device
{
	if (Range_vXbox(rID))
		return 0;
	else
		return vJoyNS::GetVJDContPovNumber(rID);
}

VGENINTERFACE_API BOOL GetVJDAxisExist(UINT rID, UINT Axis) // Test if given axis defined in the specified VDJ
{
	if (Range_vXbox(rID))
	{
		if (!IX_isControllerExists(to_vXbox(rID)))
			return FALSE;
			
		if ((Axis == HID_USAGE_X) || (Axis == HID_USAGE_Y) || (Axis == HID_USAGE_Z) || (Axis == HID_USAGE_RX) || (Axis == HID_USAGE_RY) || (Axis == HID_USAGE_RZ))
			return TRUE;
		else
			return FALSE;
	}
	else
		return vJoyNS::GetVJDAxisExist(rID, Axis);
}

VGENINTERFACE_API BOOL GetVJDAxisMax(UINT rID, UINT Axis, LONG * Max) // Get logical Maximum value for a given axis defined in the specified VDJ
{
	if (Range_vXbox(rID))
	{
		if (!IX_isControllerExists(to_vXbox(rID)))
			return FALSE;

		if ((Axis == HID_USAGE_X) || (Axis == HID_USAGE_Y) || (Axis == HID_USAGE_RX) || (Axis == HID_USAGE_RY))
		{
			*Max = 32767;
			return TRUE;
		}
		else if ((Axis == HID_USAGE_Z) || (Axis == HID_USAGE_RZ))
		{
			*Max = 255;
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		return vJoyNS::GetVJDAxisMax(rID, Axis, Max);

}

VGENINTERFACE_API BOOL GetVJDAxisMin(UINT rID, UINT Axis, LONG * Min) // Get logical Minimum value for a given axis defined in the specified VDJ
{
	if (Range_vXbox(rID))
	{
		if (!IX_isControllerExists(to_vXbox(rID)))
			return FALSE;

		if ((Axis == HID_USAGE_X) || (Axis == HID_USAGE_Y)  || (Axis == HID_USAGE_RX) || (Axis == HID_USAGE_RY))
		{
			*Min = -32768;
			return TRUE;
		}
		else if ((Axis == HID_USAGE_Z) || (Axis == HID_USAGE_RZ))
		{
			*Min = 0;
			return TRUE;
		}
		else
			return FALSE;
	}
	else
		return vJoyNS::GetVJDAxisMin(rID, Axis, Min);

}

VGENINTERFACE_API enum VjdStat GetVJDStatus(UINT rID)			// Get the status of the specified vJoy Device.
{
	if (Range_vXbox(rID))
	{
		if (!IX_isControllerOwned(to_vXbox(rID)))
			return VJD_STAT_OWN;
		if (IX_isControllerExists(to_vXbox(rID)))
			return VJD_STAT_BUSY;
		else
			return VJD_STAT_MISS;
	}
	else
		return vJoyNS::GetVJDStatus(rID);
}

VGENINTERFACE_API BOOL isVJDExists(UINT rID)					// TRUE if the specified vJoy Device exists
{
	if (Range_vXbox(rID))
		return (IX_isControllerExists(to_vXbox(rID)));
	else
		return vJoyNS::GetVJDStatus(rID);
}

VGENINTERFACE_API BOOL AcquireVJD(UINT rID)				// Acquire the specified vJoy Device.
{
	if (Range_vXbox(rID))
		return (IX_PlugIn(to_vXbox(rID)));
	else
		return vJoyNS::AcquireVJD(rID);
}

VGENINTERFACE_API VOID RelinquishVJD(UINT rID)			// Relinquish the specified vJoy Device.
{
	if (Range_vXbox(rID))
		 IX_UnPlug(to_vXbox(rID));
	else
		 vJoyNS::RelinquishVJD(rID);
}

VGENINTERFACE_API BOOL UpdateVJD(UINT rID, PVOID pData)	// Update the position data of the specified vJoy Device.
{
	if (Range_vXbox(rID))
	{
		UINT UserIndex = to_vXbox(rID);
		HDEVICE h = GetDevice(vXbox, UserIndex);
		if (!h)
			return FALSE;

		// Get  position
		XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
		if (!position)
			return FALSE;

		// Convert position (in place)
		if (!ConvertPosition_vJoy2vXbox(pData, position))
			return FALSE;

		// Send data to device
		if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
			return FALSE;
		else
			return TRUE;
	}
	else
		 return vJoyNS::UpdateVJD(rID, pData);
}

#pragma endregion Interface Functions (vJoy)

#pragma region Interface Functions (vXbox)
VGENINTERFACE_API BOOL isVBusExists(void)
{
	return IX_isVBusExists();
}

VGENINTERFACE_API BOOL GetNumEmptyBusSlots(UCHAR * nSlots)
{
	return IX_GetNumEmptyBusSlots(nSlots);
}

VGENINTERFACE_API BOOL isControllerExists(UINT UserIndex)
{
	return (UserIndex);
}

VGENINTERFACE_API BOOL isControllerOwned(UINT UserIndex)
{
	return IX_isControllerOwned(UserIndex);
}

VGENINTERFACE_API BOOL PlugIn(UINT UserIndex)
{
	return IX_PlugIn( UserIndex);
}

VGENINTERFACE_API BOOL UnPlug(UINT UserIndex)
{
	return IX_UnPlug(UserIndex);
}

VGENINTERFACE_API BOOL UnPlugForce(UINT UserIndex)
{
	return IX_UnPlugForce(UserIndex);
}

// Reset Devices
VGENINTERFACE_API BOOL ResetController(UINT UserIndex)
{
	return IX_ResetController(UserIndex);
}

VGENINTERFACE_API BOOL ResetAllControllers()
{
	return IX_ResetAllControllers();
}

VGENINTERFACE_API BOOL ResetControllerBtns(UINT UserIndex)
{
	return IX_ResetControllerBtns(UserIndex);
}

VGENINTERFACE_API BOOL ResetControllerDPad(UINT UserIndex)
{
	return IX_ResetControllerDPad(UserIndex);
}


VGENINTERFACE_API BOOL SetBtnA(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnA(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnB(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnB(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnX(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnX(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnY(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnY(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnStart(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnStart(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnBack(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnBack(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnLT(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnLT(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnRT(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnRT(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnLB(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnLB(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetBtnRB(UINT UserIndex, BOOL Press)
{
	return IX_SetBtnRB(UserIndex, Press);
}

VGENINTERFACE_API BOOL SetTriggerR(UINT UserIndex, BYTE Value) // Right Trigger
{
	return IX_SetTriggerR(UserIndex, Value);
}

VGENINTERFACE_API BOOL SetTriggerL(UINT UserIndex, BYTE Value) // Left Trigger
{
	return IX_SetTriggerL(UserIndex, Value);
}

VGENINTERFACE_API BOOL SetAxisX(UINT UserIndex, SHORT Value) // Left Stick X
{
	return IX_SetAxisX(UserIndex, Value);
}

VGENINTERFACE_API BOOL SetAxisY(UINT UserIndex, SHORT Value) // Left Stick Y
{
	return IX_SetAxisY(UserIndex, Value);
}

VGENINTERFACE_API BOOL SetAxisRx(UINT UserIndex, SHORT Value) // Right Stick X
{
	return IX_SetAxisRx(UserIndex, Value);
}

VGENINTERFACE_API BOOL SetAxisRy(UINT UserIndex, SHORT Value) // Right Stick Y
{
	return IX_SetAxisRy(UserIndex, Value);
}

VGENINTERFACE_API BOOL SetDpad(UINT UserIndex, INT Value) // DPAD Set Value
{
	return IX_SetDpad(UserIndex, Value);
}

VGENINTERFACE_API BOOL SetDpadUp(UINT UserIndex) // DPAD Up
{
	return IX_SetDpadUp(UserIndex);
}

VGENINTERFACE_API BOOL SetDpadRight(UINT UserIndex) // DPAD Right
{
	return IX_SetDpadRight(UserIndex);
}

VGENINTERFACE_API BOOL SetDpadDown(UINT UserIndex) // DPAD Down
{
	return IX_SetDpadDown(UserIndex);
}

VGENINTERFACE_API BOOL SetDpadLeft(UINT UserIndex) // DPAD Left
{
	return IX_SetDpadLeft(UserIndex);
}

VGENINTERFACE_API BOOL SetDpadOff(UINT UserIndex) // DPAD Off
{
	return IX_SetDpadOff(UserIndex);
}

VGENINTERFACE_API BOOL GetLedNumber(UINT UserIndex, PBYTE pLed)
{
	return IX_GetLedNumber(UserIndex, pLed);
}

VGENINTERFACE_API BOOL GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib)
{
	return IX_GetVibration(UserIndex, pVib);
}



#pragma endregion Interface Functions (vXbox)

#pragma region Interface Functions (Common)
VGENINTERFACE_API HDEVICE AcquireDev(UINT DevId, DevType dType)
{
	return  HDEVICE();
}
#pragma endregion

#pragma region Internal vXbox
BOOL	IX_isVBusExists(void)
{
	DWORD Version;

	DWORD res = XOutputGetBusVersion(&Version);
	if (res == XOUTPUT_VBUS_NOT_CONNECTED)
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_GetNumEmptyBusSlots(UCHAR * nSlots)
{
	if (ERROR_SUCCESS == XOutputGetFreeSlots(1, nSlots))
		return TRUE;
	else
		return FALSE;
}

BOOL	IX_isControllerExists(UINT UserIndex)
{
	BOOL Exist;
	if (ERROR_SUCCESS == XOutputIsPluggedIn(UserIndex - 1, &Exist))
		return Exist;
	else
		return FALSE;
}

BOOL	IX_isControllerOwned(UINT UserIndex)
{
	BOOL Owned;
	if (ERROR_SUCCESS == XOutputIsOwned(UserIndex - 1, &Owned))
		return Owned;
	else
		return FALSE;
}

BOOL	IX_PlugIn(UINT UserIndex)
{
	// Plug-in
	if (ERROR_SUCCESS != XOutputPlugIn(UserIndex - 1))
		return FALSE;

	// Wait for device to start - try up to 1 second
	DWORD res;
	BYTE Led;
	for (int i = 0; i < 1000; i++)
	{
		res = XoutputGetLedNumber(UserIndex - 1, &Led);

		// If device not ready then wait and try again
		if (res == XOUTPUT_VBUS_DEVICE_NOT_READY)
		{
			Sleep(1);
			continue;
		}

		// Device is ready or error occured
		break;
	}


	// Create the device data structure and insert it into the device-container
	HDEVICE h = CreateDevice(vXbox, UserIndex);
	if (h)
		return TRUE;
	
	// Failed to create device
	XOutputUnPlug(UserIndex - 1);
	return FALSE;

}

BOOL	IX_UnPlug(UINT UserIndex)
{
	// Unplug
	if (ERROR_SUCCESS != XOutputUnPlug(UserIndex - 1))
		return FALSE;

	// Wait for device to be unplugged
	for (int i = 0; i < 1000; i++)
	{
		if (!IX_isControllerExists(UserIndex))
			break;
		Sleep(1);
	}

	Sleep(1000); // Temporary - replace with detection code

	// If still exists - error
	if (IX_isControllerExists(UserIndex))
		return FALSE;
	

	// Get handle to device and destroy it
	HDEVICE h = GetDevice(vXbox, UserIndex);
	DestroyDevice(h);
	if (h)
		return TRUE;
	else
		return FALSE;
}

BOOL	IX_UnPlugForce(UINT UserIndex)
{
	// Unplug
	if (ERROR_SUCCESS != XOutputUnPlugForce(UserIndex - 1))
		return FALSE;

	// Get handle to device and destroy it
	HDEVICE h = GetDevice(vXbox, UserIndex);
	DestroyDevice(h);
	if (h)
		return TRUE;
	else
		return FALSE;

}

BOOL	IX_ResetController(HDEVICE hDev)
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	memset(&position, 0, sizeof(XINPUT_GAMEPAD));
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_ResetController(UINT UserIndex)
{
	return IX_ResetController(GetDevice(vXbox, UserIndex));
}

BOOL	IX_ResetAllControllers()
{
	int c = 0;
	c += IX_ResetController(1);
	c += IX_ResetController(2);
	c += IX_ResetController(3);
	c += IX_ResetController(4);

	return (c == 0 ? FALSE : TRUE);
}

BOOL	IX_ResetControllerBtns(HDEVICE hDev)
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->wButtons &= 0x000F;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_ResetControllerBtns(UINT UserIndex)
{
	return IX_ResetControllerBtns(GetDevice(vXbox, UserIndex));
}

BOOL	IX_ResetControllerDPad(HDEVICE hDev)
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->wButtons &= 0xFFF0;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_ResetControllerDPad(UINT UserIndex)
{
	return IX_ResetControllerDPad(GetDevice(vXbox, UserIndex));
}

BOOL	IX_SetBtn(HDEVICE hDev, BOOL Press, WORD Button)
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->wButtons &= ~Button;
	position->wButtons |= Button*Press;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetBtn(UINT UserIndex, BOOL Press, WORD Button)
{
	return IX_SetBtn(GetDevice(vXbox, UserIndex), Press, Button);
}

BOOL	IX_SetBtnA(HDEVICE hDev, BOOL Press)
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_A);
}

BOOL	IX_SetBtnB(HDEVICE hDev, BOOL Press)
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_B);
}

BOOL	IX_SetBtnX(HDEVICE hDev, BOOL Press)
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_X);
}

BOOL	IX_SetBtnY(HDEVICE hDev, BOOL Press)
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_Y);
}

BOOL	IX_SetBtnStart(HDEVICE hDev, BOOL Press)
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_START);
}

BOOL	IX_SetBtnBack(HDEVICE hDev, BOOL Press)
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_BACK);
}

BOOL	IX_SetBtnLT(HDEVICE hDev, BOOL Press) // Left Thumb/Stick 
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_LEFT_THUMB);
}

BOOL	IX_SetBtnRT(HDEVICE hDev, BOOL Press) // Right Thumb/Stick 
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_RIGHT_THUMB);
}

BOOL	IX_SetBtnLB(HDEVICE hDev, BOOL Press) // Left Bumper 
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_LEFT_SHOULDER);
}

BOOL	IX_SetBtnRB(HDEVICE hDev, BOOL Press) // Right Bumper 
{
	return IX_SetBtn(hDev, Press, XINPUT_GAMEPAD_RIGHT_SHOULDER);
}

BOOL	IX_SetBtnA(UINT UserIndex, BOOL Press)
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_A);
}

BOOL	IX_SetBtnB(UINT UserIndex, BOOL Press)
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_B);
}

BOOL	IX_SetBtnX(UINT UserIndex, BOOL Press)
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_X);
}

BOOL	IX_SetBtnY(UINT UserIndex, BOOL Press)
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_Y);
}

BOOL	IX_SetBtnStart(UINT UserIndex, BOOL Press)
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_START);
}

BOOL	IX_SetBtnBack(UINT UserIndex, BOOL Press)
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_BACK);
}

BOOL	IX_SetBtnLT(UINT UserIndex, BOOL Press) // Left Thumb/Stick 
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_LEFT_THUMB);
}

BOOL	IX_SetBtnRT(UINT UserIndex, BOOL Press) // Right Thumb/Stick 
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_RIGHT_THUMB);
}

BOOL	IX_SetBtnLB(UINT UserIndex, BOOL Press) // Left Bumper 
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_LEFT_SHOULDER);
}

BOOL	IX_SetBtnRB(UINT UserIndex, BOOL Press) // Right Bumper 
{
	return IX_SetBtn(UserIndex, Press, XINPUT_GAMEPAD_RIGHT_SHOULDER);
}

BOOL	IX_SetTriggerL(HDEVICE hDev, BYTE Value) // Left Trigger
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->bLeftTrigger = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetTriggerL(UINT UserIndex, BYTE Value) // Left Trigger
{
	return IX_SetTriggerL(GetDevice(vXbox, UserIndex), Value);
}

BOOL	IX_SetTriggerR(HDEVICE hDev, BYTE Value) // Right Trigger
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->bRightTrigger = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetTriggerR(UINT UserIndex, BYTE Value) // Left Trigger
{
	return IX_SetTriggerR(GetDevice(vXbox, UserIndex), Value);
}

BOOL	IX_SetAxisX(HDEVICE hDev, SHORT Value) // Left Stick X
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbLX = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisX(UINT UserIndex, SHORT Value) // Left Stick X
{
	return IX_SetAxisX(GetDevice(vXbox, UserIndex), Value);
}

BOOL	IX_SetAxisY(HDEVICE hDev, SHORT Value) // Left Stick Y
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbLY = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisY(UINT UserIndex, SHORT Value) // Left Stick Y
{
	return IX_SetAxisY(GetDevice(vXbox, UserIndex), Value);
}

BOOL	IX_SetAxisRx(HDEVICE hDev, SHORT Value) // Right Stick X
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbRX = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisRx(UINT UserIndex, SHORT Value) // Right Stick X
{
	return IX_SetAxisRx(GetDevice(vXbox, UserIndex), Value);
}

BOOL	IX_SetAxisRy(HDEVICE hDev, SHORT Value) // Right Stick Y
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbRY = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisRy(UINT UserIndex, SHORT Value) // Right Stick Y
{
	return IX_SetAxisRy(GetDevice(vXbox, UserIndex), Value);
}

BOOL	IX_SetDpad(HDEVICE hDev, INT Value) // DPAD Set Value
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return FALSE;

	// Change position value
	position->wButtons &= 0xFFF0;
	position->wButtons |= Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;

	return IX_SetBtn(UserIndex, TRUE, Value);
}

BOOL	IX_SetDpad(UINT UserIndex, INT Value) // DPAD Set Value
{
	return IX_SetDpad(GetDevice(vXbox, UserIndex), Value);
}

BOOL	IX_SetDpadUp(HDEVICE hDev)
{
	return IX_SetDpad(hDev, DPAD_UP);
}

BOOL	IX_SetDpadUp(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_UP);
}

BOOL	IX_SetDpadRight(HDEVICE hDev)
{
	return IX_SetDpad(hDev, DPAD_RIGHT);
}

BOOL	IX_SetDpadRight(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_RIGHT);
}

BOOL	IX_SetDpadDown(HDEVICE hDev)
{
	return IX_SetDpad(hDev, DPAD_DOWN);
}

BOOL	IX_SetDpadDown(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_DOWN);
}

BOOL	IX_SetDpadLeft(HDEVICE hDev)
{
	return IX_SetDpad(hDev, DPAD_LEFT);
}

BOOL	IX_SetDpadLeft(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_LEFT);
}

BOOL	IX_SetDpadOff(HDEVICE hDev)
{
	return IX_SetDpad(hDev, DPAD_OFF);
}

BOOL	IX_SetDpadOff(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_OFF);
}

BOOL	IX_GetLedNumber(UINT UserIndex, PBYTE pLed)
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	if (!pLed)
		return FALSE;

	if (ERROR_SUCCESS != XoutputGetLedNumber(UserIndex - 1, pLed))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib)
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	if (!pVib)
		return FALSE;

	if (ERROR_SUCCESS != XoutputGetVibration(UserIndex - 1, pVib))
		return FALSE;
	else
		return TRUE;
}


#pragma endregion Internal vXbox

#pragma region Helper Functions
HDEVICE CreateDevice(DevType Type, UINT i)
{
	// If found then exit
	if (GetDevice(Type, i))
		return 0;
	// Not found

	// Create structure
	HDEVICE h = 0;
	DEVICE * pdev = new DEVICE;
	pdev->Id = i;
	pdev->Type = Type;
	if (Type == vJoy)
	{
		pdev->PPosition.vJoyPos = new JOYSTICK_POSITION_V2;
		h = i;
	}
	else
	{
		pdev->PPosition.vXboxPos = new XINPUT_GAMEPAD;
		*(pdev->PPosition.vXboxPos) = { 0 };
		h = i + 1000;
	};

	// Insert in container
	std::pair <std::map<HDEVICE, DEVICE>::iterator, bool> res;
	res = DevContainer.emplace(h, *pdev);
	if (res.second)
		return h;
	else
		return 0;
}

void DestroyDevice(HDEVICE dev)
{
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(dev);
	if (it != DevContainer.end())
		DevContainer.erase(it);
}

HDEVICE GetDevice(DevType Type, UINT i)
{
	// Search the device-container for an existing structure
	// that fits the description.
	for (std::map<HDEVICE, DEVICE>::iterator it = DevContainer.begin(); it != DevContainer.end(); ++it)
	{
		if ((it->second.Id == i) && (it->second.Type == Type))
			return it->first;
	}
	return 0;
}

UINT GetDeviceId(HDEVICE h)
{
	// Get the device structure
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(h);
	if (it == DevContainer.end())
		return 0;

	// Get the device ID
	return it->second.Id;
}

void * GetDevicePos(HDEVICE h)
{
	// Get the device structure
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(h);
	if (it == DevContainer.end())
		return NULL;

	// Get the device position
	return (void *)it->second.PPosition.vJoyPos;
}

BOOL ConvertPosition_vJoy2vXbox(void *vJoyPos, void *vXboxPos)
{
	if (!vJoyPos || !vXboxPos)
		return FALSE;

	// Convert the input position
	JOYSTICK_POSITION_V2 * inPos = (JOYSTICK_POSITION_V2 *)vJoyPos;

	// Convert the output position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)vXboxPos;

	///////// Convert values from vJoy to vXbox
	/////  Axes
	position->sThumbLX = 2 * ((SHORT)inPos->wAxisX - 1) - 32767;
	position->sThumbLY = 2 * ((SHORT)inPos->wAxisY - 1) - 32767;
	position->sThumbRX = 2 * ((SHORT)inPos->wAxisXRot - 1) - 32767;
	position->sThumbRY = 2 * ((SHORT)inPos->wAxisYRot - 1) - 32767;
	position->bLeftTrigger = ((SHORT)inPos->wAxisZRot - 1) / 128;
	position->bRightTrigger = ((SHORT)inPos->wAxisZ - 1) / 128;

	//// Buttons
	// Button 1 ==> Button A
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 1, XINPUT_GAMEPAD_A);
	// Button 2 ==> Button B
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 2, XINPUT_GAMEPAD_B);
	// Button 3  ==> Button X
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 3, XINPUT_GAMEPAD_X);
	// Button 4  ==> Button Y
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 4, XINPUT_GAMEPAD_Y);
	// Button 5  ==> Button LB (Left Bumper)
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 5, XINPUT_GAMEPAD_LEFT_SHOULDER);
	// Button 6  ==> Button RB (Right Bumper)
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 6, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	// Button 7  ==> Back
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 7, XINPUT_GAMEPAD_BACK);
	// Button 8  ==> Start
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 8, XINPUT_GAMEPAD_START);
	// Button 9  ==> Button LSB (Left Stick Button)
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 9, XINPUT_GAMEPAD_LEFT_THUMB);
	// Button 10 ==> Button RSB (Right Stick Button)
	position->wButtons = ConvertButton(inPos->lButtons, position->wButtons, 10, XINPUT_GAMEPAD_RIGHT_THUMB);

	// Dpad / Discrete POV #1
	switch (inPos->bHats)
	{
		case 0:
			position->wButtons |= XINPUT_GAMEPAD_DPAD_UP;
			break;
		case 1:
			position->wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
			break;
		case 2:
			position->wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
			break;
		case 4:
			position->wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
			break;
		default:
			position->wButtons &= 0xFFF0;
	}

	return TRUE;
}

WORD ConvertButton(LONG vBtns, WORD xBtns, UINT vBtn, UINT xBtn)
{
	WORD out;
	out = ((vBtns&(1 << (vBtn - 1))) == 0) ? xBtns & ~xBtn : xBtns | xBtn;
	return out;
}

#pragma endregion // Helper Functions