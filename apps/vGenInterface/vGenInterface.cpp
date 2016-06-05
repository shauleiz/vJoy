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

VGENINTERFACE_API SHORT GetvJoyVersion(void)
{
	return vJoyNS::GetvJoyVersion();
}

#pragma region Interface Functions (vXbox)
VGENINTERFACE_API BOOL isVBusExists(void)
{
	return IX_isVBusExists();
}

VGENINTERFACE_API BOOL	GetNumEmptyBusSlots(UCHAR * nSlots)
{
	return IX_GetNumEmptyBusSlots(nSlots);
}

VGENINTERFACE_API BOOL isControllerExists(UINT UserIndex)
{
	return IX_isControllerExists(UserIndex);
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

BOOL	IX_SetBtn(UINT UserIndex, BOOL Press, WORD Button)
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
	if (!position)
		return FALSE;

	// Change position value
	position->wButtons &= ~Button;
	position->wButtons |= Button*Press;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex-1, position))
		return FALSE;
	else
		return TRUE;
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

BOOL	IX_SetTriggerL(UINT UserIndex, BYTE Value) // Left Trigger
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
	if (!position)
		return FALSE;

	// Change position value
	position->bLeftTrigger = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetTriggerR(UINT UserIndex, BYTE Value) // Right Trigger
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
	if (!position)
		return FALSE;

	// Change position value
	position->bRightTrigger = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisX(UINT UserIndex, SHORT Value) // Left Stick X
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbLX = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisY(UINT UserIndex, SHORT Value) // Left Stick Y
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbLY = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisRx(UINT UserIndex, SHORT Value) // Right Stick X
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbRX = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetAxisRy(UINT UserIndex, SHORT Value) // Right Stick Y
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
	if (!position)
		return FALSE;

	// Change position value
	position->sThumbRY = Value;
	if (ERROR_SUCCESS != XOutputSetState(UserIndex - 1, position))
		return FALSE;
	else
		return TRUE;
}

BOOL	IX_SetDpad(UINT UserIndex, INT Value) // DPAD Set Value
{
	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return FALSE;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(h);
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

BOOL	IX_SetDpadUp(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_UP);
}

BOOL	IX_SetDpadRight(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_RIGHT);
}

BOOL	IX_SetDpadDown(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_DOWN);
}

BOOL	IX_SetDpadLeft(UINT UserIndex)
{
	return IX_SetDpad(UserIndex, DPAD_LEFT);
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

#pragma endregion // Helper Functions