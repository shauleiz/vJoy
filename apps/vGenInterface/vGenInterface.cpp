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
		BOOL Exist;
		if (SUCCEEDED(IX_isControllerPluggedIn(to_vXbox(rID), &Exist)) && Exist)
			return 10;
		else
			return 0;
	}
	else
		return vJoyNS::GetVJDButtonNumber(rID);
}

VGENINTERFACE_API int GetVJDDiscPovNumber(UINT rID)	// Get the number of POVs defined in the specified device
{
	if (Range_vXbox(rID))
	{
		BOOL Exist;
		if (SUCCEEDED(IX_isControllerPluggedIn(to_vXbox(rID), &Exist)) && Exist)
			return 1;
		else
			return 0;
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
		BOOL Exist;
		if (SUCCEEDED(IX_isControllerPluggedIn(to_vXbox(rID), &Exist)) && Exist)
		{
			if ((Axis == HID_USAGE_X) || (Axis == HID_USAGE_Y) || (Axis == HID_USAGE_Z) || (Axis == HID_USAGE_RX) || (Axis == HID_USAGE_RY) || (Axis == HID_USAGE_RZ))
				return TRUE;
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return (vJoyNS::GetVJDAxisExist(rID, Axis) == TRUE);
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
		BOOL Exist, Owned;
		if SUCCEEDED(IX_isControllerOwned(to_vXbox(rID), &Owned))
		{
			if (Owned)
				return VJD_STAT_OWN;
		}

		if SUCCEEDED(IX_isControllerPluggedIn(to_vXbox(rID), &Exist))
		{
			if (Exist)
				return VJD_STAT_BUSY;
		}
			return VJD_STAT_FREE;
	}
	else
		return vJoyNS::GetVJDStatus(rID);
}

VGENINTERFACE_API BOOL isVJDExists(UINT rID)					// TRUE if the specified vJoy Device exists
{
	if (Range_vXbox(rID))
	{

		BOOL Exist;
		if SUCCEEDED(IX_isControllerPluggedIn(to_vXbox(rID), &Exist))
					 return Exist;
		return FALSE;
	}
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
		return (vJoyNS::SetAxis(Value, rID, Axis) == TRUE);

	if (Range_vXbox(rID))
	{
		if (!SUCCEEDED(IX_isControllerPluggedIn(to_vXbox(rID))))
			return FALSE;

		// Remap Axes
		Axis = Axis - HID_USAGE_X + 1;

		// If Axis is X,Y,RX,RY (1,2,4,5) then remap range:
		// 0 - 32768  ==> -32768 - 32767
		SHORT vx_Value;
		if (Axis == 1 || Axis == 2 || Axis == 4 || Axis == 5)
		{
			if (Value > 32767)
				Value = 32767;
			vx_Value = (Value- 16384)*2;
			if (Axis == 1)
				return SUCCEEDED(IX_SetAxisLx(to_vXbox(rID), vx_Value));
			if (Axis == 2)
				return SUCCEEDED(IX_SetAxisLy(to_vXbox(rID), vx_Value));
			if (Axis == 4)
				return SUCCEEDED(IX_SetAxisRx(to_vXbox(rID), vx_Value));
			if (Axis == 5)
				return SUCCEEDED(IX_SetAxisRy(to_vXbox(rID), vx_Value));
		}

		// If Triggers (3,6) then remap range:
		// 0 - 32768  ==> 0 - 255
		else if (Axis == 3 || Axis == 6)
		{
			vx_Value = static_cast<SHORT>((Value - 1) /128);
			if (Axis == 3)
				return SUCCEEDED(IX_SetTriggerR(to_vXbox(rID), static_cast<BYTE>(vx_Value)));
			if (Axis == 6)
				return SUCCEEDED(IX_SetTriggerL(to_vXbox(rID), static_cast<BYTE>(vx_Value)));
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
		return SUCCEEDED(IX_SetBtn(to_vXbox(rID), Value, nBtn));

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
VGENINTERFACE_API DWORD AcquireDev(UINT DevId, DevType dType, HDEVICE * hDev)
{
	if (dType == vJoy)
	{
		*hDev = IJ_AcquireVJD(DevId);
		if (INVALID_DEV == *hDev)
			return STATUS_UNSUCCESSFUL;
		else
			return STATUS_SUCCESS;
	};

	if (dType == vXbox)
	{
		DWORD res = IX_PlugIn(DevId);
		if (STATUS_SUCCESS == res)
			res = GetDevHandle(DevId, vXbox, hDev);
		else
			*hDev = INVALID_DEV;
		return res;
	}

	return STATUS_INVALID_PARAMETER_2;
}

VGENINTERFACE_API DWORD 	RelinquishDev(HDEVICE hDev)			// Relinquish a Device.
{
	if (!ValidDev(hDev))
		return STATUS_INVALID_HANDLE;

	if (isDevice_vJoy(hDev) == STATUS_SUCCESS)
		return IJ_RelinquishVJD(hDev);

	if (isDevice_vXbox(hDev) == STATUS_SUCCESS)
		return IX_UnPlug(GetDeviceId(hDev));

	return STATUS_INVALID_HANDLE;
}

VGENINTERFACE_API  DWORD GetDevType(HDEVICE hDev, DevType * dType)			// Get device type (vJoy/vXbox)
{
	DWORD res;

	if (!dType)
		return STATUS_INVALID_PARAMETER_2;

	if (!ValidDev(hDev))
		return STATUS_INVALID_HANDLE;

	UINT id = GetDeviceId(hDev);

	res = isDevice_vJoy(hDev);
	if (res == STATUS_SUCCESS)
		*dType = vJoy;

	else
	{
		res = isDevice_vXbox(hDev);
		if (res == STATUS_SUCCESS)
			* dType = vXbox;
	}

	BOOL Owned;
	if SUCCEEDED(isDevOwned(id, *dType, &Owned))
	{
		if (!Owned)
			return STATUS_DEVICE_REMOVED;
	}

	return res;
}

VGENINTERFACE_API DWORD isDevOwned(UINT DevId, DevType dType, BOOL * Owned)
{
	DWORD res;

	if (!Owned)
		return STATUS_INVALID_PARAMETER_3;

	if (dType == vJoy)
	{
		if (vJoyNS::GetVJDStatus(DevId) == VJD_STAT_OWN)
			*Owned = TRUE;
		else
			*Owned = FALSE;
		return STATUS_SUCCESS;
	}

	if (dType == vXbox)
	{
		res = IX_isControllerOwned(DevId, Owned);
		return res;
	}

	return STATUS_UNSUCCESSFUL;
}

VGENINTERFACE_API DWORD isDevExist(UINT DevId, DevType dType, BOOL * Exist)
{
	DWORD res;

	if (!Exist)
		return STATUS_INVALID_PARAMETER_3;

	if (dType == vJoy)
	{
		VjdStat stat = vJoyNS::GetVJDStatus(DevId);
		if ((stat == VJD_STAT_OWN) || (stat == VJD_STAT_BUSY))
			*Exist = TRUE;
		else
			*Exist = FALSE;
		return STATUS_SUCCESS;
	};

	if (dType == vXbox)
	{ 
		res = IX_isControllerPluggedIn(DevId, Exist);
		return res;
	}
	
	return STATUS_UNSUCCESSFUL;
}

VGENINTERFACE_API DWORD isDevFree(UINT DevId, DevType dType, BOOL * Free)
{
	DWORD res;

	if (!Free)
		return STATUS_INVALID_PARAMETER_3;

	if (dType == vJoy)
	{
		VjdStat stat = vJoyNS::GetVJDStatus(DevId);
		if ((stat == VJD_STAT_FREE))
			*Free = TRUE;
		else
			*Free = FALSE;
		return STATUS_SUCCESS;
	};

	BOOL Exist;
	if (dType == vXbox)
	{
		res = IX_isControllerPluggedIn(DevId, &Exist);
		*Free = !Exist;
		return res;
}

	return STATUS_UNSUCCESSFUL;
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

VGENINTERFACE_API DWORD GetDevNumber(HDEVICE hDev, UINT * dNumber)// If vJoy: Number=Id; If vXbox: Number=Led#
{
	DWORD res;
	DevType dType;

	if (!dNumber)
		return STATUS_INVALID_PARAMETER_2;

	if (!ValidDev(hDev))
		return STATUS_INVALID_HANDLE;

	// Get id + Type and verify it is owned
	UINT id = GetDeviceId(hDev);
	res = GetDevType(hDev, &dType);
	if FAILED(res)
		return res;

	// If not owned - return
	BOOL Owned;
	if SUCCEEDED(isDevOwned(id, dType, &Owned))
	{
		if (!Owned)
			return STATUS_DEVICE_REMOVED;
	}

	res = isDevice_vJoy(hDev);
	if (res == STATUS_SUCCESS)
		*dNumber = id;
	else
	{
		BYTE Led = 0;
		res = IX_GetLedNumber(id, &Led);
		if (res == STATUS_SUCCESS)
			*dNumber = Led;
	}

	return res;
}

VGENINTERFACE_API DWORD GetDevId(HDEVICE hDev, UINT * dID)	// Return Device ID to be used with vXbox API and Backward compatibility API
{
	DWORD res;
	DevType dType;

	if (!dID)
		return STATUS_INVALID_PARAMETER_2;

	if (!ValidDev(hDev))
		return STATUS_INVALID_HANDLE;

	*dID = GetDeviceId(hDev);
	res = GetDevType(hDev, &dType);
	if FAILED(res)
		return res;

	// If not owned - return
	BOOL Owned;
	if SUCCEEDED(isDevOwned(*dID, dType, &Owned))
	{
		if (!Owned)
			return STATUS_DEVICE_REMOVED;
	}

	if (!dID)
		return STATUS_INVALID_HANDLE;
	else
		return STATUS_SUCCESS;
}

VGENINTERFACE_API DWORD GetDevHandle(UINT DevId, DevType dType, HDEVICE * hDev) // Return device handle from Device ID and Device type
{
	// If not owned - return
	BOOL Owned;

	if (!hDev)
		return STATUS_INVALID_PARAMETER_3;

	// Get handle from container
	*hDev = GetDevice(dType, DevId);

	// If handle is valid check that device still owned
	if ValidDev(*hDev)
	{
		if SUCCEEDED(isDevOwned(DevId, dType, &Owned))
		{
			if (Owned)
				return STATUS_SUCCESS; // Owned
		}

		// Handle is OK but device was removed so we remove the entry from the container
		DestroyDevice(*hDev);
		return STATUS_DEVICE_REMOVED; 
	}
	else
		return STATUS_UNSUCCESSFUL;
}

VGENINTERFACE_API DWORD isAxisExists(HDEVICE hDev, UINT nAxis, BOOL * Exist)	// Does Axis exist.
{
	BOOL Owned;
	DWORD res;
	UINT id;
	DevType dType;

	if (!Exist)
		return STATUS_INVALID_PARAMETER_3;

	res = GetDevId(hDev, &id);
	if FAILED(res)
		return res;

	res = GetDevType(hDev, &dType);
	if FAILED(res)
		return res;

	res = isDevOwned(id, dType, &Owned);
	if FAILED(res)
		return res;
	if (!Owned)
		return STATUS_DEVICE_REMOVED;

	if SUCCEEDED(isDevice_vJoy(hDev))
	{
		*Exist = IJ_GetVJDAxisExist(hDev, nAxis);
		return STATUS_SUCCESS;
	}


	if SUCCEEDED(isDevice_vXbox(hDev))
	{
		if (nAxis >= 1 && nAxis <= 6)
			*Exist = TRUE;
		else
			*Exist = FALSE;
		return STATUS_SUCCESS;
	}


	return STATUS_UNSUCCESSFUL;
}

VGENINTERFACE_API DWORD GetDevButtonN(HDEVICE hDev, UINT * nBtn)			// Get number of buttons in device
{
	BOOL Owned;
	DWORD res;
	UINT id;
	DevType dType;

	if (!nBtn)
		return STATUS_INVALID_PARAMETER_3;

	res = GetDevId(hDev, &id);
	if FAILED(res)
		return res;

	res = GetDevType(hDev, &dType);
	if FAILED(res)
		return res;

	res = isDevOwned(id, dType, &Owned);
	if FAILED(res)
		return res;
	if (!Owned)
		return STATUS_DEVICE_REMOVED;

	if SUCCEEDED(isDevice_vJoy(hDev))
	{ 
		* nBtn = IJ_GetVJDButtonNumber(hDev);
		return STATUS_SUCCESS;
	}

	if SUCCEEDED(isDevice_vXbox(hDev))
	{
		*nBtn = 10;
		return STATUS_SUCCESS;
	}

	return STATUS_UNSUCCESSFUL;
}

VGENINTERFACE_API DWORD GetDevHatN(HDEVICE hDev, UINT * nHat)				// Get number of Hat Switches in device
{
	BOOL Owned;
	DWORD res;
	UINT id;
	DevType dType;

	if (!nHat)
		return STATUS_INVALID_PARAMETER_3;

	res = GetDevId(hDev, &id);
	if FAILED(res)
		return res;

	res = GetDevType(hDev, &dType);
	if FAILED(res)
		return res;

	res = isDevOwned(id, dType, &Owned);
	if FAILED(res)
		return res;
	if (!Owned)
		return STATUS_DEVICE_REMOVED;

	if SUCCEEDED(isDevice_vJoy(hDev))
	{
		*nHat = (IJ_GetVJDDiscPovNumber(hDev) + IJ_GetVJDContPovNumber(hDev));
		return STATUS_SUCCESS;
	}

	if SUCCEEDED(isDevice_vXbox(hDev))
	{
		*nHat = 1;
		return STATUS_SUCCESS;

	}

	return STATUS_UNSUCCESSFUL;
}


VGENINTERFACE_API DWORD SetDevButton(HDEVICE hDev, UINT Button, BOOL Press)
{
	BOOL bRes;

	if SUCCEEDED(isDevice_vJoy(hDev))
	{
		bRes = IJ_SetBtn(Press, hDev, Button);
		if (bRes == TRUE)
			return STATUS_SUCCESS;
		else
			return STATUS_UNSUCCESSFUL;
	}

	if SUCCEEDED(isDevice_vXbox(hDev))
		return IX_SetBtn(hDev, Press, Button);

	return STATUS_INVALID_HANDLE;
}

VGENINTERFACE_API DWORD SetDevAxis(HDEVICE hDev, UINT Axis, FLOAT Value)
{		
	BOOL bRes;
	if SUCCEEDED(isDevice_vJoy(hDev))
	{
		// Convert Value from range 0-100 to range 0-32768
		LONG vj_Value = static_cast <LONG>(32768 * Value / 100);
		// Convert Axis from 1-8 to HID_USAGE_X-HID_USAGE_SL0
		UINT vj_Axis = Axis + HID_USAGE_X - 1;

		bRes = IJ_SetAxis(vj_Value, hDev, vj_Axis);
		if (bRes == TRUE)
			return STATUS_SUCCESS;
		else
			return STATUS_UNSUCCESSFUL;
	}

	if SUCCEEDED(isDevice_vXbox(hDev))
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
				return STATUS_INVALID_HANDLE;
				break;
		}
	}

	return STATUS_INVALID_HANDLE;
}

VGENINTERFACE_API DWORD  SetDevPov(HDEVICE hDev, UINT nPov, FLOAT Value)
{
	if SUCCEEDED(isDevice_vJoy(hDev))
	{
		// Don't test for type - just try

		// Continuous: Convert Value from range 0-100 to range 0-35999 (-1 means Reset)
		LONG vj_Value = -1;
		if (Value >= 0)
			vj_Value = static_cast <LONG>(35999 * Value / 100);
		BOOL res = IJ_SetContPov(vj_Value, hDev, nPov);
		if (res == TRUE)
			return STATUS_SUCCESS;

		// Discrete: Convert Value from range 0-100 to descrete values (-1 means Reset)
		int vjd_Value = -1;
		switch (static_cast <int>(Value))
		{
			case 0:
			case 100:
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

		res = IJ_SetDiscPov(vjd_Value, hDev, nPov);
		if (res == TRUE)
			return STATUS_SUCCESS;
		else
			return STATUS_UNSUCCESSFUL;
	}

	if SUCCEEDED(isDevice_vXbox(hDev))
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

	return STATUS_INVALID_HANDLE;

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

DWORD IJ_RelinquishVJD(HDEVICE hDev)			// Relinquish the specified vJoy Device.
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
	{
		vJoyNS::RelinquishVJD(GetDeviceId(hDev));
		DestroyDevice(hDev);
		return STATUS_SUCCESS;
	}
	return STATUS_UNSUCCESSFUL;
}

BOOL IJ_isVJDExists(HDEVICE hDev)
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		return vJoyNS::isVJDExists(GetDeviceId(hDev));
	else
		return FALSE;
}

VjdStat IJ_GetVJDStatus(HDEVICE hDev)
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		return vJoyNS::GetVJDStatus(GetDeviceId(hDev));
	else
		return VJD_STAT_UNKN;
}

BOOL IJ_GetVJDAxisExist(HDEVICE hDev, UINT Axis)
{	
	BOOL bRes;

	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		bRes = vJoyNS::GetVJDAxisExist(GetDeviceId(hDev), Axis+ HID_USAGE_X-1);
	if (bRes == TRUE)
		return bRes;
	
	return FALSE;
}

int	IJ_GetVJDButtonNumber(HDEVICE hDev)	// Get the number of buttons defined in the specified VDJ
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		return vJoyNS::GetVJDButtonNumber(GetDeviceId(hDev));
	return FALSE;
}

int IJ_GetVJDDiscPovNumber(HDEVICE hDev)   // Get the number of POVs defined in the specified device
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		return vJoyNS::GetVJDDiscPovNumber(GetDeviceId(hDev));
	return 0;
}

int IJ_GetVJDContPovNumber(HDEVICE hDev)	// Get the number of descrete-type POV hats defined in the specified VDJ
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		return vJoyNS::GetVJDContPovNumber(GetDeviceId(hDev));
	return 0;
}

BOOL IJ_SetAxis(LONG Value, HDEVICE hDev, UINT Axis)		// Write Value to a given axis defined in the specified VDJ 
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		return vJoyNS::SetAxis(Value,GetDeviceId(hDev), Axis);
	return FALSE;
}

BOOL IJ_SetBtn(BOOL Value, HDEVICE hDev, UCHAR nBtn)		// Write Value to a given button defined in the specified VDJ 
{
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
		return vJoyNS::SetBtn(Value,GetDeviceId(hDev), nBtn);
	return FALSE;
}

BOOL IJ_SetDiscPov(int Value, HDEVICE hDev, UCHAR nPov)	// Write Value to a given descrete POV defined in the specified VDJ 
{
	UINT id;
	
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
	{ 
		id = GetDeviceId(hDev);
		if (vJoyNS::GetVJDDiscPovNumber(id) == 0)
			return FALSE;
		return vJoyNS::SetDiscPov(Value, id, nPov);
	}
	return FALSE;
}

BOOL IJ_SetContPov(DWORD Value, HDEVICE hDev, UCHAR nPov)	// Write Value to a given continuous POV defined in the specified VDJ 
{
	UINT id;
	
	if (ValidDev(hDev) && SUCCEEDED(isDevice_vJoy(hDev)))
	{ 
		id = GetDeviceId(hDev);
		if (vJoyNS::GetVJDContPovNumber(id) == 0)
			return FALSE;
		return vJoyNS::SetContPov(Value, id, nPov);
	}
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

void DestroyDevice(HDEVICE & dev)
{
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(dev);
	if (it != DevContainer.end())
		DevContainer.erase(it);

	dev = INVALID_DEV;
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

DWORD isDevice_vJoy(HDEVICE h)
{
	// Get the device structure
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(h);
	if (it == DevContainer.end())
		return STATUS_INVALID_HANDLE;

	// Get the device ID
	if (it->second.Type == vJoy)
		return STATUS_SUCCESS;
	else
		return STATUS_UNSUCCESSFUL;
}

DWORD isDevice_vXbox(HDEVICE h)
{
	// Get the device structure
	std::map<HDEVICE, DEVICE>::iterator it;
	it = DevContainer.find(h);
	if (it == DevContainer.end())
		return STATUS_INVALID_HANDLE;

	// Get the device ID
	if (it->second.Type == vXbox)
		return STATUS_SUCCESS;
	else
		return STATUS_UNSUCCESSFUL;
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