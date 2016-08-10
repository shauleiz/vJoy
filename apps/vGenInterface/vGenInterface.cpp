// vGenInterface.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"

// Compilation directives
#define USE_STATIC
#define STATIC
#define VJOYHEADERUSED

//#include <ntstatus.h>
#include <Xinput.h>
#include "public.h"
#include "vjoyinterface.h"
#include "XOutput.h"
#include "vGenInterface.h"
#include "private.h"

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
		if (!IX_isControllerPluggedIn(to_vXbox(rID)))
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
		if (!IX_isControllerPluggedIn(to_vXbox(rID)))
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
		if (!IX_isControllerPluggedIn(to_vXbox(rID)))
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
		if (!IX_isControllerPluggedIn(to_vXbox(rID)))
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
		if (!IX_isControllerPluggedIn(to_vXbox(rID)))
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
		if (IX_isControllerPluggedIn(to_vXbox(rID)))
			return VJD_STAT_OWN;
		else
			return VJD_STAT_UNKN;
	}
	else
		return vJoyNS::GetVJDStatus(rID);
}

VGENINTERFACE_API BOOL isVJDExists(UINT rID)					// TRUE if the specified vJoy Device exists
{
	if (Range_vXbox(rID))
		return (IX_isControllerPluggedIn(to_vXbox(rID)));
	else
		return vJoyNS::isVJDExists(rID);
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

VGENINTERFACE_API BOOL SetAxis(LONG Value, UINT rID, UINT Axis)		// Write Value to a given axis defined in the specified VDJ 
{
	if (Range_vJoy(rID))
		return vJoyNS::SetAxis(Value, rID, Axis);

	if (Range_vXbox(rID))
	{
		if (!IX_isControllerPluggedIn(to_vXbox(rID)))
			return FALSE;

		// If Axis is X,Y,RX,RY (1,2,4,5) then remap range:
		// 0 - 32768  ==> -32768 - 32767
		SHORT vx_Value;
		if (Axis == 1 || Axis == 2 || Axis == 4 || Axis == 5)
		{
			vx_Value = static_cast<SHORT>(2 * (Value - 1) - 32767);
			if (Axis == 1)
				return IX_SetAxisLx(to_vXbox(rID), vx_Value);
			if (Axis == 2)
				return IX_SetAxisLy(to_vXbox(rID), vx_Value);
			if (Axis == 4)
				return IX_SetAxisRx(to_vXbox(rID), vx_Value);
			if (Axis == 5)
				return IX_SetAxisRy(to_vXbox(rID), vx_Value);
		}

		// If Triggers (3,6) then remap range:
		// 0 - 32768  ==> 0 - 255
		else if (Axis == 3 || Axis == 6)
		{
			vx_Value = static_cast<SHORT>((Value - 1) /128);
			if (Axis == 3)
				return IX_SetTriggerR(to_vXbox(rID), static_cast<BYTE>(vx_Value));
			if (Axis == 6)
				return IX_SetTriggerL(to_vXbox(rID), static_cast<BYTE>(vx_Value));
		}
		else 
			return FALSE;
		
	}
	return FALSE;
}

VGENINTERFACE_API BOOL SetBtn(BOOL Value, UINT rID, UCHAR nBtn)		// Write Value to a given button defined in the specified VDJ 
{
	if (Range_vJoy(rID))
		return vJoyNS::SetBtn(Value, rID, nBtn);

	if (Range_vXbox(rID))
		return IX_SetBtn(to_vXbox(rID), Value, nBtn);

	return FALSE;
}

VGENINTERFACE_API BOOL SetDiscPov(int Value, UINT rID, UCHAR nPov)	// Write Value to a given descrete POV defined in the specified VDJ 
{
	if (Range_vJoy(rID))
		return vJoyNS::SetDiscPov(Value, rID, nPov);

	if (Range_vXbox(rID) && (nPov==1))
	{
		switch (Value)
		{
			case 0:
				return IX_SetDpad(to_vXbox(rID), XINPUT_GAMEPAD_DPAD_UP);

			case 1:
				return IX_SetDpad(to_vXbox(rID), XINPUT_GAMEPAD_DPAD_RIGHT);

			case 2:
				return IX_SetDpad(to_vXbox(rID), XINPUT_GAMEPAD_DPAD_DOWN);

			case 3:
				return IX_SetDpad(to_vXbox(rID), XINPUT_GAMEPAD_DPAD_LEFT);

			default:
				return IX_SetDpad(to_vXbox(rID), 0);
		}
	}

	return FALSE;

}

VGENINTERFACE_API BOOL SetContPov(DWORD Value, UINT rID, UCHAR nPov)	// Write Value to a given continuous POV defined in the specified VDJ 
{
	if (Range_vJoy(rID))
		return vJoyNS::SetContPov(Value, rID, nPov);

	if (Range_vXbox(rID) && (nPov == 1))
	{
		if (Value == -1)
			return IX_SetDpad(to_vXbox(rID), 0);

		if (static_cast<LONG>(Value) < 100 || static_cast<LONG>(Value) > 35900)
			return IX_SetDpad(to_vXbox(rID), DPAD_UP);

		else if (abs(static_cast<LONG>(Value - 4500)) < 100)
			return IX_SetDpad(to_vXbox(rID), DPAD_UP | DPAD_RIGHT);

		else if (abs(static_cast<LONG>(Value - 9000)) < 100)
			return IX_SetDpad(to_vXbox(rID), DPAD_RIGHT);

		else if (abs(static_cast<LONG>(Value - 13500)) < 100)
			return IX_SetDpad(to_vXbox(rID), DPAD_DOWN | DPAD_RIGHT);

		else if (abs(static_cast<LONG>(Value - 18000)) < 100)
			return IX_SetDpad(to_vXbox(rID), DPAD_DOWN);

		else if (abs(static_cast<LONG>(Value - 22500)) < 100)
			return IX_SetDpad(to_vXbox(rID), DPAD_DOWN | DPAD_LEFT);

		else if (abs(static_cast<LONG>(Value - 27000)) < 100)
			return IX_SetDpad(to_vXbox(rID), DPAD_LEFT);

		else if (abs(static_cast<LONG>(Value - 31500)) < 100)
			return IX_SetDpad(to_vXbox(rID), DPAD_UP | DPAD_LEFT);

		else
			return FALSE;
	}

	return FALSE;

}

VGENINTERFACE_API BOOL ResetVJD(UINT rID)			// Reset all controls to predefined values in the specified VDJ
{
	if (Range_vJoy(rID))
		return vJoyNS::ResetVJD( rID);

	if (Range_vXbox(rID))
		return IX_ResetController(to_vXbox(rID));

	return FALSE;
}

VGENINTERFACE_API VOID ResetAll(void) // Reset all controls to predefined values in all VDJ
{
	vJoyNS::ResetAll();
	IX_ResetAllControllers();
}

VGENINTERFACE_API BOOL ResetButtons(UINT rID)		// Reset all buttons (To 0) in the specified VDJ
{
	if (Range_vJoy(rID))
		return vJoyNS::ResetButtons(rID);

	if (Range_vXbox(rID))
		return IX_ResetControllerBtns(to_vXbox(rID));

	return FALSE;

}

VGENINTERFACE_API BOOL ResetPovs(UINT rID)		// Reset all POV Switches (To -1) in the specified VDJ
{
	if (Range_vJoy(rID))
		return vJoyNS::ResetPovs(rID);

	if (Range_vXbox(rID))
		return IX_ResetControllerDPad(to_vXbox(rID));

	return FALSE;

}

#pragma region FFB API
#pragma warning( push )
#pragma warning( disable : 4996 )
VGENINTERFACE_API FFBEType FfbGetEffect() { return  vJoyNS::FfbGetEffect(); }
VGENINTERFACE_API VOID FfbRegisterGenCB(FfbGenCB cb, PVOID data) { return  vJoyNS::FfbRegisterGenCB( cb,  data); }
VGENINTERFACE_API BOOL 	FfbStart(UINT rID) { return  TRUE; }
VGENINTERFACE_API VOID 	FfbStop(UINT rID) { return; }
VGENINTERFACE_API BOOL 	IsDeviceFfb(UINT rID) { return  vJoyNS::IsDeviceFfb(rID); }
VGENINTERFACE_API BOOL 	IsDeviceFfbEffect(UINT rID, UINT Effect) { return  vJoyNS::IsDeviceFfbEffect(rID, Effect); }
VGENINTERFACE_API DWORD Ffb_h_DeviceID(const FFB_DATA * Packet, int *DeviceID) { return  vJoyNS::Ffb_h_DeviceID(Packet, DeviceID); }
VGENINTERFACE_API DWORD Ffb_h_Type(const FFB_DATA * Packet, FFBPType *Type) { return  vJoyNS::Ffb_h_Type(Packet, Type); }
VGENINTERFACE_API DWORD Ffb_h_Packet(const FFB_DATA * Packet, WORD *Type, int *DataSize, BYTE *Data[]) { return  vJoyNS::Ffb_h_Packet(Packet, Type, DataSize, Data); }
VGENINTERFACE_API DWORD Ffb_h_EBI(const FFB_DATA * Packet, int *Index) { return  vJoyNS::Ffb_h_EBI(Packet, Index); }
VGENINTERFACE_API DWORD Ffb_h_Eff_Report(const FFB_DATA * Packet, FFB_EFF_REPORT*  Effect) { return  vJoyNS::Ffb_h_Eff_Report(Packet, Effect); }
//VGENINTERFACE_API DWORD Ffb_h_Eff_Const(const FFB_DATA * Packet, FFB_EFF_REPORT*  Effect) { return  vJoyNS::Ffb_h_Eff_Report(Packet, Effect); }
VGENINTERFACE_API DWORD Ffb_h_Eff_Ramp(const FFB_DATA * Packet, FFB_EFF_RAMP*  RampEffect) { return  vJoyNS::Ffb_h_Eff_Ramp(Packet,   RampEffect); }
VGENINTERFACE_API DWORD Ffb_h_EffOp(const FFB_DATA * Packet, FFB_EFF_OP*  Operation) { return  vJoyNS::Ffb_h_EffOp(Packet,  Operation); }
VGENINTERFACE_API DWORD Ffb_h_DevCtrl(const FFB_DATA * Packet, FFB_CTRL *  Control) { return  vJoyNS::Ffb_h_DevCtrl(Packet,  Control); }
VGENINTERFACE_API DWORD Ffb_h_Eff_Period(const FFB_DATA * Packet, FFB_EFF_PERIOD*  Effect) { return  vJoyNS::Ffb_h_Eff_Period(Packet,  Effect); }
VGENINTERFACE_API DWORD Ffb_h_Eff_Cond(const FFB_DATA * Packet, FFB_EFF_COND*  Condition) { return  vJoyNS::Ffb_h_Eff_Cond(Packet, Condition); }
VGENINTERFACE_API DWORD Ffb_h_DevGain(const FFB_DATA * Packet, BYTE * Gain) { return  vJoyNS::Ffb_h_DevGain(Packet, Gain); }
VGENINTERFACE_API DWORD Ffb_h_Eff_Envlp(const FFB_DATA * Packet, FFB_EFF_ENVLP*  Envelope) { return  vJoyNS::Ffb_h_Eff_Envlp(Packet, Envelope); }
VGENINTERFACE_API DWORD Ffb_h_EffNew(const FFB_DATA * Packet, FFBEType * Effect) { return  vJoyNS::Ffb_h_EffNew(Packet, Effect); }
VGENINTERFACE_API DWORD Ffb_h_Eff_Constant(const FFB_DATA * Packet, FFB_EFF_CONSTANT *  ConstantEffect) { return  vJoyNS::Ffb_h_Eff_Constant(Packet, ConstantEffect); }
#pragma warning( pop )
#pragma endregion  FFB API

#pragma endregion Interface Functions (vJoy)

#pragma region Interface Functions (vXbox)
VGENINTERFACE_API DWORD isVBusExists(void)
{
	return IX_isVBusExists();
}

VGENINTERFACE_API DWORD GetNumEmptyBusSlots(UCHAR * nSlots)
{
	return IX_GetNumEmptyBusSlots(nSlots);
}

VGENINTERFACE_API DWORD isControllerPluggedIn(UINT UserIndex, PBOOL Exist)
{
	return IX_isControllerPluggedIn(UserIndex, Exist);
}

VGENINTERFACE_API DWORD isControllerOwned(UINT UserIndex, PBOOL Owned)
{
	return IX_isControllerOwned(UserIndex, Owned);
}

VGENINTERFACE_API DWORD PlugIn(UINT UserIndex)
{
	return IX_PlugIn( UserIndex);
}

VGENINTERFACE_API DWORD PlugInNext(UINT * UserIndex)
{
	return IX_PlugInNext( UserIndex);
}

VGENINTERFACE_API DWORD UnPlug(UINT UserIndex)
{
	return IX_UnPlug(UserIndex);
}

VGENINTERFACE_API DWORD UnPlugForce(UINT UserIndex)
{
	return IX_UnPlugForce(UserIndex);
}

// Reset Devices
VGENINTERFACE_API DWORD ResetController(UINT UserIndex)
{
	return IX_ResetController(UserIndex);
}

VGENINTERFACE_API DWORD ResetAllControllers()
{
	return IX_ResetAllControllers();
}

#ifdef SPECIFICRESET

VGENINTERFACE_API DWORD ResetControllerBtns(UINT UserIndex)
{
	return IX_ResetControllerBtns(UserIndex);
}

VGENINTERFACE_API DWORD ResetControllerDPad(UINT UserIndex)
{
	return IX_ResetControllerDPad(UserIndex);
}

#endif // SPECIFICRESET


VGENINTERFACE_API DWORD SetBtn(UINT UserIndex, WORD Button, BOOL Press)
{
	return IX_SetBtn(UserIndex, Press,  Button);
}

#ifdef SPECIFICBUTTONS

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

#endif // SPECIFICBUTTONS

VGENINTERFACE_API DWORD SetTriggerR(UINT UserIndex, BYTE Value) // Right Trigger
{
	return IX_SetTriggerR(UserIndex, Value);
}

VGENINTERFACE_API DWORD SetTriggerL(UINT UserIndex, BYTE Value) // Left Trigger
{
	return IX_SetTriggerL(UserIndex, Value);
}

VGENINTERFACE_API DWORD SetAxisLx(UINT UserIndex, SHORT Value) // Left Stick X
{
	return IX_SetAxisLx(UserIndex, Value);
}

VGENINTERFACE_API DWORD SetAxisLy(UINT UserIndex, SHORT Value) // Left Stick Y
{
	return IX_SetAxisLy(UserIndex, Value);
}

VGENINTERFACE_API DWORD SetAxisRx(UINT UserIndex, SHORT Value) // Right Stick X
{
	return IX_SetAxisRx(UserIndex, Value);
}

VGENINTERFACE_API DWORD SetAxisRy(UINT UserIndex, SHORT Value) // Right Stick Y
{
	return IX_SetAxisRy(UserIndex, Value);
}

VGENINTERFACE_API DWORD SetDpad(UINT UserIndex, UCHAR Value) // DPAD Set Value
{
	return IX_SetDpad(UserIndex, Value);
}

#ifdef SPECIFICBUTTONS
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
#endif // SPECIFICBUTTONS
VGENINTERFACE_API DWORD GetLedNumber(UINT UserIndex, PBYTE pLed)
{
	return IX_GetLedNumber(UserIndex, pLed);
}

VGENINTERFACE_API DWORD GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib)
{
	return IX_GetVibration(UserIndex, pVib);
}



#pragma endregion Interface Functions (vXbox)

#pragma region Interface Functions (Common)
VGENINTERFACE_API HDEVICE AcquireDev(UINT DevId, DevType dType)
{
	if (dType == vJoy)
		return IJ_AcquireVJD(DevId);

	if (dType == vXbox)
	{
		if (STATUS_SUCCESS == IX_PlugIn(DevId))
			return GetDevHandle(DevId, vXbox);
		else
			return INVALID_DEV;
	}

	return INVALID_DEV;
}

VGENINTERFACE_API VOID 	RelinquishDev(HDEVICE hDev)			// Relinquish a Device.
{
	if (isDevice_vJoy(hDev))
		IJ_RelinquishVJD(hDev);

	if (isDevice_vXbox(hDev))
		IX_UnPlug(GetDeviceId(hDev));
}

VGENINTERFACE_API DevType GetDevType(HDEVICE hDev)			// Get device type (vJoy/vXbox)
{
	if (isDevice_vJoy(hDev))
		return vJoy;
	else
		return vXbox;
}

VGENINTERFACE_API BOOL isDevOwned(UINT DevId, DevType dType)
{
	if (dType == vJoy)
		return (vJoyNS::GetVJDStatus(DevId) == VJD_STAT_OWN);

	if (dType == vXbox)
		return IX_isControllerOwned(DevId);

	return FALSE;
}

VGENINTERFACE_API BOOL isDevExist(UINT DevId, DevType dType)
{
	if (dType == vJoy)
	{
		VjdStat stat = vJoyNS::GetVJDStatus(DevId);
		if ((stat == VJD_STAT_OWN) || (stat == VJD_STAT_BUSY))
			return TRUE;
		else
			return FALSE;
	};

	if (dType == vXbox)
		return IX_isControllerOwned(DevId);

	return FALSE;
}

// Cannot implement isDevOwned(h) because only an OWNED device has a handle
// BUSY device is is owned by another feeder so it does not have a handle 
#if 0
VGENINTERFACE_API BOOL isDevOwned(HDEVICE hDev)
{
	if (isDevice_vJoy(hDev))
	{
		VjdStat stat = IJ_GetVJDStatus(hDev);
		if (stat == VJD_STAT_OWN)
			return TRUE;
		else
			return FALSE;
	}

	if (isDevice_vXbox(hDev))
		return IX_isControllerOwned(hDev);

	return FALSE;
}
#endif // 0

VGENINTERFACE_API UINT GetDevNumber(HDEVICE hDev)// If vJoy: Number=Id; If vXbox: Number=Led#
{
	UINT id = GetDeviceId(hDev);
	if (isDevice_vJoy(hDev))
		return id;
	
	BYTE Led = 0;
	BOOL res = IX_GetLedNumber(id, &Led);
	if (res)
		return Led;
	else
		return 0;
}

VGENINTERFACE_API UINT GetDevId(HDEVICE hDev)	// Return Device ID to be used with vXbox API and Backward compatibility API
{
	return GetDeviceId(hDev);
}

VGENINTERFACE_API HDEVICE GetDevHandle(UINT DevId, DevType dType) // Return device handle from Device ID and Device type
{
	return GetDevice(dType, DevId);
}

VGENINTERFACE_API BOOL isAxisExists(HDEVICE hDev, UINT nAxis)	// Does Axis exist.
{
	if (isDevice_vJoy(hDev))
		return IJ_GetVJDAxisExist(hDev, nAxis);

	if (isDevice_vXbox(hDev) && nAxis >= 1 && nAxis <= 6)
		return TRUE;

	return FALSE;
}

VGENINTERFACE_API UINT GetDevButtonN(HDEVICE hDev)			// Get number of buttons in device
{
	if (isDevice_vJoy(hDev))
		return IJ_GetVJDButtonNumber(hDev);

	if (isDevice_vXbox(hDev))
		return 10;

	return 0;
}

VGENINTERFACE_API UINT GetDevHatN(HDEVICE hDev)				// Get number of Hat Switches in device
{
	if (isDevice_vJoy(hDev))
		return (IJ_GetVJDDiscPovNumber(hDev) + IJ_GetVJDContPovNumber(hDev));

	if (isDevice_vXbox(hDev))
		return 1;

	return 0;
}


VGENINTERFACE_API BOOL SetDevButton(HDEVICE hDev, UINT Button, BOOL Press)
{
	if (isDevice_vJoy(hDev))
		return IJ_SetBtn(Press, hDev, Button);

	if (isDevice_vXbox(hDev))
		return IX_SetBtn( hDev,  Press,  Button);

	return FALSE;
}

VGENINTERFACE_API BOOL SetDevAxis(HDEVICE hDev, UINT Axis, FLOAT Value)
{
	if (isDevice_vJoy(hDev))
	{
		// Convert Value from range 0-100 to range 0-32768
		LONG vj_Value = static_cast <LONG>(32768 * Value / 100);
		// Convert Axis from 1-8 to HID_USAGE_X-HID_USAGE_SL0
		UINT vj_Axis = Axis + HID_USAGE_X - 1;

		return IJ_SetAxis(vj_Value, hDev, vj_Axis);
	}

	if (isDevice_vXbox(hDev))
	{
		// Convert Value from range (0 - 100) to range (-32768 - 32767) for axes X,Y,RX,RY
		SHORT vx_Value = static_cast <SHORT>((65535 * Value / 100) - 32768);
		// Convert Value from range (0 - 100) to range (0 - 255) for Triggers
		BYTE vx_TValue = static_cast <BYTE>(255 * Value / 100);

		switch (Axis)
		{
			case 1:
				return IX_SetAxisLx(hDev, vx_Value);

			case 2:
				return IX_SetAxisLy(hDev, vx_Value);

			case 3:
				return IX_SetTriggerR(hDev, vx_TValue);

			case 4:
				return IX_SetAxisRx(hDev, vx_Value);

			case 5:
				return IX_SetAxisRy(hDev, vx_Value);

			case 6:
				return IX_SetTriggerL(hDev, vx_TValue);

			default:
				return FALSE;
				break;
		}
	}

	return FALSE;
}

VGENINTERFACE_API BOOL  SetDevPov(HDEVICE hDev, UINT nPov, FLOAT Value)
{
	if (isDevice_vJoy(hDev))
	{
		// Don't test for type - just try

		// Continuous: Convert Value from range 0-100 to range 0-35999 (-1 means Reset)
		LONG vj_Value = -1;
		if (Value >= 0)
			vj_Value = static_cast <LONG>(35999 * Value / 100);
		BOOL res = IJ_SetContPov(vj_Value, hDev, nPov);
		if (res == TRUE)
			return TRUE;

		// Discrete: Convert Value from range 0-100 to descrete values (-1 means Reset)
		int vjd_Value = -1;
		switch (static_cast <int>(Value))
		{
			case 0:
				vjd_Value = 0;
				break;

			case 25:
				vjd_Value = 1;
				break;

			case 50:
				vjd_Value = 2;
				break;

			case 75:
				vjd_Value = 3;
				break;

			default:
				break;
		}

		return IJ_SetDiscPov(vjd_Value, hDev, nPov);
	}

	if (isDevice_vXbox(hDev))
	{
		if (nPov != 1)
			return FALSE;

		if (Value == 0)
			return IX_SetDpad(hDev, DPAD_UP);
		if (Value == 12.5)
			return IX_SetDpad(hDev, DPAD_UP | DPAD_RIGHT);
		if (Value == 25)
			return IX_SetDpad(hDev, DPAD_RIGHT);
		if (Value == 37.5)
			return IX_SetDpad(hDev, DPAD_DOWN | DPAD_RIGHT);
		if (Value == 50)
			return IX_SetDpad(hDev, DPAD_DOWN);
		if (Value == 62.5)
			return IX_SetDpad(hDev, DPAD_DOWN | DPAD_LEFT);
		if (Value == 75)
			return IX_SetDpad(hDev, DPAD_LEFT);
		if (Value == 87.5)
			return IX_SetDpad(hDev, DPAD_UP | DPAD_LEFT);

		return IX_SetDpad(hDev, DPAD_OFF);
	}

	return FALSE;

}

#pragma endregion

#pragma region Internal vXbox
DWORD	IX_isVBusExists(void)
{
	DWORD Version;

	DWORD res = XOutputGetBusVersion(&Version);
	if (res == XOUTPUT_VBUS_NOT_CONNECTED)
		return STATUS_NO_SUCH_DEVICE;
	else
		return STATUS_SUCCESS;
}

DWORD	IX_GetNumEmptyBusSlots(UCHAR * nSlots)
{
	DWORD res = XOutputGetFreeSlots(1, nSlots);

	if (res == ERROR_SUCCESS) return STATUS_SUCCESS;
	if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
	if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
	
	else return STATUS_IO_DEVICE_ERROR;		
}

DWORD	IX_isControllerPluggedIn(UINT UserIndex, PBOOL Exist)
{
	DWORD res = XOutputIsPluggedIn(UserIndex - 1, Exist);

	if (res == ERROR_SUCCESS) return STATUS_SUCCESS;
	if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
	if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;

	else return STATUS_IO_DEVICE_ERROR;
}

BOOL	IX_isControllerPluggedIn(HDEVICE hDev)
{
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;

	BOOL Exist;
	if (ERROR_SUCCESS == XOutputIsPluggedIn(UserIndex - 1, &Exist))
		return Exist;
	else
		return FALSE;
}

DWORD	IX_isControllerOwned(UINT UserIndex, PBOOL Owned)
{
	DWORD res = XOutputIsOwned(UserIndex - 1, Owned);

	if (res == ERROR_SUCCESS) return STATUS_SUCCESS;
	if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
	if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;

	else return STATUS_IO_DEVICE_ERROR;
}

BOOL	IX_isControllerOwned(HDEVICE hDev)
{
	BOOL Owned;
	if (!hDev)
		return FALSE;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return FALSE;
	
	if (ERROR_SUCCESS == XOutputIsOwned(UserIndex - 1, &Owned))
		return Owned;
	else
		return FALSE;
}

DWORD	IX_PlugIn(UINT UserIndex)
{
	// Test is it is possible to Plug-In
	BOOL Exist;
	DWORD res;

	res = IX_isControllerPluggedIn(UserIndex, &Exist);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	if (Exist) return STATUS_DEVICE_ALREADY_ATTACHED;



	// Plug-in
	res = XOutputPlugIn(UserIndex - 1);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}	
		
	// Wait for device to start - try up to 1 second
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

	// If still not ready
	if (res == XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;

	// Create the device data structure and insert it into the device-container
	HDEVICE h = CreateDevice(vXbox, UserIndex);
	if (h)
		return STATUS_SUCCESS;
	
	// Failed to create device
	XOutputUnPlug(UserIndex - 1);
	return STATUS_INVALID_HANDLE;

}

DWORD	IX_PlugInNext(UINT * UserIndex)
{
	// Look for an empty slot
	BOOL Exist;
	UINT i = 0;
	DWORD res;
	do {
		res = IX_isControllerPluggedIn(++i, &Exist);
		if (!Exist)
		{
			*UserIndex = i;
			break;
		}
	} while (res == STATUS_SUCCESS);

	// Slot not found?
	if (res != STATUS_SUCCESS)
		return res;

	// Found, now plugin
	return IX_PlugIn(i);
}

DWORD	IX_UnPlug(UINT UserIndex)
{
	BOOL Exist;
	DWORD res;

	// Exists?
	res = IX_isControllerPluggedIn(UserIndex, &Exist);
	if (res != STATUS_SUCCESS)
		return res;
	if (!Exist)
		return STATUS_DEVICE_DOES_NOT_EXIST;

	// Owned?
	res = IX_isControllerOwned(UserIndex, &Exist);
	if (res != STATUS_SUCCESS)
		return res;
	if (!Exist)
		return STATUS_RESOURCE_NOT_OWNED;

	// Unplug
	res = XOutputUnPlug(UserIndex - 1);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}

	// Wait for device to be unplugged
	for (int i = 0; i < 1000; i++)
	{
		if (!IX_isControllerPluggedIn(UserIndex))
			break;
		Sleep(1);
	}

	Sleep(1000); // Temporary - replace with detection code

	// If still exists - error
	if (IX_isControllerPluggedIn(UserIndex))
		return STATUS_TIMEOUT;
	

	// Get handle to device and destroy it
	HDEVICE h = GetDevice(vXbox, UserIndex);
	DestroyDevice(h);
	return STATUS_SUCCESS;
}

DWORD	IX_UnPlugForce(UINT UserIndex)
{
	DWORD res;
	BOOL Exist;

	// Exists?
	res = IX_isControllerPluggedIn(UserIndex, &Exist);
	if (res != STATUS_SUCCESS)
		return res;
	if (!Exist)
		return STATUS_DEVICE_DOES_NOT_EXIST;

	// Unplug
	res = XOutputUnPlugForce(UserIndex - 1);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}

	// Wait for device to be unplugged
	for (int i = 0; i < 1000; i++)
	{
		if (!IX_isControllerPluggedIn(UserIndex))
			break;
		Sleep(1);
	}

	Sleep(1000); // Temporary - replace with detection code

				 // If still exists - error
	if (IX_isControllerPluggedIn(UserIndex))
		return STATUS_TIMEOUT;


	// Get handle to device and destroy it
	HDEVICE h = GetDevice(vXbox, UserIndex);
	DestroyDevice(h);
	return STATUS_SUCCESS;
}

DWORD	IX_ResetController(HDEVICE hDev)
{
	DWORD res;

	if (!hDev)
		return STATUS_INVALID_PARAMETER;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	memset(position, 0, sizeof(XINPUT_GAMEPAD));
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_ResetController(UINT UserIndex)
{
	return IX_ResetController(GetDevice(vXbox, UserIndex));
}

DWORD	IX_ResetAllControllers()
{
	DWORD res[4];
	res[0] = IX_ResetController(1);
	res[1] = IX_ResetController(2);
	res[2] = IX_ResetController(3);
	res[3] = IX_ResetController(4);

	for (int i = 0; i < 4; i++)
		if (res[i] != STATUS_SUCCESS)
			return res[i];
	return STATUS_SUCCESS;
}

DWORD	IX_ResetControllerBtns(HDEVICE hDev)
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;


	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->wButtons &= 0x000F;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_ResetControllerBtns(UINT UserIndex)
{
	return IX_ResetControllerBtns(GetDevice(vXbox, UserIndex));
}

DWORD	IX_ResetControllerDPad(HDEVICE hDev)
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;


	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->wButtons &= 0xFFF0;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_ResetControllerDPad(UINT UserIndex)
{
	return IX_ResetControllerDPad(GetDevice(vXbox, UserIndex));
}

DWORD	IX_SetBtn(HDEVICE hDev, BOOL Press, WORD Button)
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;


	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	WORD Mask = g_xButtons[Button - 1];
	// Change position value
	position->wButtons &= ~Mask;
	position->wButtons |= Mask*Press;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetBtn(UINT UserIndex, BOOL Press, WORD Button)
{
	return IX_SetBtn(GetDevice(vXbox, UserIndex), Press, Button);
}

#ifdef SPECIFICBUTTONS
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

#endif // SPECIFICBUTTONS

DWORD	IX_SetTriggerL(HDEVICE hDev, BYTE Value) // Left Trigger
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->bLeftTrigger = Value;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetTriggerL(UINT UserIndex, BYTE Value) // Left Trigger
{
	return IX_SetTriggerL(GetDevice(vXbox, UserIndex), Value);
}

DWORD	IX_SetTriggerR(HDEVICE hDev, BYTE Value) // Right Trigger -- position->bRightTrigger = Value;
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->bRightTrigger = Value;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetTriggerR(UINT UserIndex, BYTE Value) // Left Trigger
{
	return IX_SetTriggerR(GetDevice(vXbox, UserIndex), Value);
}

DWORD	IX_SetAxisLx(HDEVICE hDev, SHORT Value) // Left Stick X
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->sThumbLX = Value;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetAxisLx(UINT UserIndex, SHORT Value) // Left Stick X
{
	return IX_SetAxisLx(GetDevice(vXbox, UserIndex), Value);
}

DWORD	IX_SetAxisLy(HDEVICE hDev, SHORT Value) // Left Stick Y
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->sThumbLY = Value;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetAxisLy(UINT UserIndex, SHORT Value) // Left Stick Y
{
	return IX_SetAxisLy(GetDevice(vXbox, UserIndex), Value);
}

DWORD	IX_SetAxisRx(HDEVICE hDev, SHORT Value) // Right Stick X
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->sThumbRX = Value;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetAxisRx(UINT UserIndex, SHORT Value) // Right Stick X
{
	return IX_SetAxisRx(GetDevice(vXbox, UserIndex), Value);
}

DWORD	IX_SetAxisRy(HDEVICE hDev, SHORT Value) // Right Stick Y
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->sThumbRY = Value;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetAxisRy(UINT UserIndex, SHORT Value) // Right Stick Y
{
	return IX_SetAxisRy(GetDevice(vXbox, UserIndex), Value);
}

DWORD	IX_SetDpad(HDEVICE hDev, UCHAR Value) // DPAD Set Value
{
	DWORD res;

	UINT UserIndex = GetDeviceId(hDev);
	if (!UserIndex)
		return STATUS_INVALID_PARAMETER_1;

	// Get  position
	XINPUT_GAMEPAD * position = (XINPUT_GAMEPAD *)GetDevicePos(hDev);
	if (!position)
		return STATUS_MEMORY_NOT_ALLOCATED;

	// Change position value
	position->wButtons &= 0xFFF0;
	position->wButtons |= Value;
	res = XOutputSetState(UserIndex - 1, position);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (XOUTPUT_VBUS_DEVICE_NOT_READY) return STATUS_DEVICE_NOT_READY;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_SetDpad(UINT UserIndex, UCHAR Value) // DPAD Set Value
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

DWORD	IX_GetLedNumber(UINT UserIndex, PBYTE pLed)
{
	DWORD res;
	BOOL Exist;

	// Test if device is plugged-in
	res = IX_isControllerPluggedIn(UserIndex, &Exist);
	if (res != STATUS_SUCCESS)
		return res;
	if (!Exist)
		return STATUS_DEVICE_DOES_NOT_EXIST;

	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return STATUS_INVALID_HANDLE;

	if (!pLed)
		return STATUS_INVALID_PARAMETER_2;

	res = XoutputGetLedNumber(UserIndex - 1, pLed);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}

DWORD	IX_GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib)
{
	DWORD res;

	HDEVICE h = GetDevice(vXbox, UserIndex);
	if (!h)
		return STATUS_INVALID_HANDLE;

	if (!pVib)
		return STATUS_INVALID_PARAMETER_2;

	res = XoutputGetVibration(UserIndex - 1, pVib);
	if (res != ERROR_SUCCESS)
	{
		if (res == XOUTPUT_VBUS_NOT_CONNECTED) return STATUS_NO_SUCH_DEVICE;
		if (res == XOUTPUT_VBUS_INDEX_OUT_OF_RANGE) return STATUS_INVALID_PARAMETER;
		else return STATUS_IO_DEVICE_ERROR;
	}
	else
		return STATUS_SUCCESS;
}


#pragma endregion Internal vXbox

#pragma region Internal vJoy
HDEVICE	IJ_AcquireVJD(UINT rID)
{
	BOOL acq = vJoyNS::AcquireVJD(rID);
	if (acq)
		return CreateDevice(vJoy, rID);

	return INVALID_DEV;
}

VOID IJ_RelinquishVJD(HDEVICE hDev)			// Relinquish the specified vJoy Device.
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
	{
		vJoyNS::RelinquishVJD(GetDeviceId(hDev));
		DestroyDevice(hDev);
	}
}

BOOL IJ_isVJDExists(HDEVICE hDev)
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::isVJDExists(GetDeviceId(hDev));
	else
		return FALSE;
}

VjdStat IJ_GetVJDStatus(HDEVICE hDev)
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::GetVJDStatus(GetDeviceId(hDev));
	else
		return VJD_STAT_UNKN;
}

BOOL IJ_GetVJDAxisExist(HDEVICE hDev, UINT Axis)
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::GetVJDAxisExist(GetDeviceId(hDev), Axis+ HID_USAGE_X-1);
	return FALSE;
}

int	IJ_GetVJDButtonNumber(HDEVICE hDev)	// Get the number of buttons defined in the specified VDJ
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::GetVJDButtonNumber(GetDeviceId(hDev));
	return FALSE;
}

int IJ_GetVJDDiscPovNumber(HDEVICE hDev)   // Get the number of POVs defined in the specified device
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::GetVJDDiscPovNumber(GetDeviceId(hDev));
	return 0;
}

int IJ_GetVJDContPovNumber(HDEVICE hDev)	// Get the number of descrete-type POV hats defined in the specified VDJ
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::GetVJDContPovNumber(GetDeviceId(hDev));
	return 0;
}

BOOL IJ_SetAxis(LONG Value, HDEVICE hDev, UINT Axis)		// Write Value to a given axis defined in the specified VDJ 
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::SetAxis(Value,GetDeviceId(hDev), Axis);
	return FALSE;
}

BOOL IJ_SetBtn(BOOL Value, HDEVICE hDev, UCHAR nBtn)		// Write Value to a given button defined in the specified VDJ 
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::SetBtn(Value,GetDeviceId(hDev), nBtn);
	return FALSE;
}

BOOL IJ_SetDiscPov(int Value, HDEVICE hDev, UCHAR nPov)	// Write Value to a given descrete POV defined in the specified VDJ 
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::SetDiscPov(Value, GetDeviceId(hDev), nPov);
	return FALSE;
}

BOOL IJ_SetContPov(DWORD Value, HDEVICE hDev, UCHAR nPov)	// Write Value to a given continuous POV defined in the specified VDJ 
{
	if (ValidDev(hDev) && isDevice_vJoy(hDev))
		return vJoyNS::SetContPov(Value, GetDeviceId(hDev), nPov);
	return FALSE;
}

#pragma endregion


#pragma region Helper Functions
HDEVICE CreateDevice(DevType Type, UINT i)
{
	// If found then exit
	if (GetDevice(Type, i))
		return INVALID_DEV;
	// Not found

	// Create structure
	HDEVICE h = INVALID_DEV;
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

	h+= (rand() % 1000 + 1)<<16;

	// Insert in container
	std::pair <std::map<HDEVICE, DEVICE>::iterator, bool> res;
	res = DevContainer.emplace(h, *pdev);
	if (res.second)
		return h;
	else
		return INVALID_DEV;
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

BOOL isDevice_vJoy(HDEVICE h)
{
	// Get the device structure
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(h);
	if (it == DevContainer.end())
		return 0;

	// Get the device ID
	if (it->second.Type == vJoy)
		return TRUE;
	else
		return FALSE;
}

BOOL isDevice_vXbox(HDEVICE h)
{
	// Get the device structure
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(h);
	if (it == DevContainer.end())
		return 0;

	// Get the device ID
	if (it->second.Type == vXbox)
		return TRUE;
	else
		return FALSE;
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