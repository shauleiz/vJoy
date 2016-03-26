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
#include "vJoyInterface.h"
#include "vXboxInterface.h"



VJOYINTERFACE_API BOOL	__cdecl	 isVBusExists(void)
{
	TCHAR path[MAX_PATH];

	int n = GetVXbusPath( path, MAX_PATH);

	if (n > 0)
		return TRUE;
	else
		return FALSE;
}


VJOYINTERFACE_API BOOL	__cdecl	 isControllerExists(UINT UserIndex)
{
	return FALSE;
}

VJOYINTERFACE_API BOOL	__cdecl	 PlugIn(UINT UserIndex)
{
	BOOL out = FALSE;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	HANDLE h = GetVXbusHandle();
	if (h == INVALID_HANDLE_VALUE)
		return out;

	DWORD trasfered = 0;
	UCHAR buffer[16] = {};

	buffer[0] = 0x10;

	buffer[4] = ((UserIndex >> 0) & 0xFF);
	buffer[5] = ((UserIndex >> 8) & 0xFF);
	buffer[6] = ((UserIndex >> 16) & 0xFF);
	buffer[8] = ((UserIndex >> 24) & 0xFF);

	if (DeviceIoControl(h, IOCTL_BUSENUM_PLUGIN_HARDWARE, buffer, _countof(buffer), nullptr, 0, &trasfered, nullptr))
		out = TRUE;

	CloseHandle(h);
	return out;
}

VJOYINTERFACE_API BOOL	__cdecl	 UnPlug(UINT UserIndex)
{
	BOOL out = FALSE;

	if (UserIndex < 1 || UserIndex>4)
		return out;

	HANDLE h = GetVXbusHandle();
	if (h == INVALID_HANDLE_VALUE)
		return out;

	DWORD trasfered = 0;
	UCHAR buffer[16] = {};

	buffer[0] = 0x10;

	buffer[4] = ((UserIndex >> 0) & 0xFF);
	buffer[5] = ((UserIndex >> 8) & 0xFF);
	buffer[6] = ((UserIndex >> 16) & 0xFF);
	buffer[8] = ((UserIndex >> 24) & 0xFF);

	if (DeviceIoControl(h, IOCTL_BUSENUM_UNPLUG_HARDWARE, buffer, _countof(buffer), nullptr, 0, &trasfered, nullptr))
		out = TRUE;

	CloseHandle(h);
	return out;
}

VJOYINTERFACE_API BOOL	__cdecl	 SetAxisX(UINT UserIndex, LONG Value) // Left Stick X
{
	g_Gamepad[UserIndex - 1].sThumbLX = Value;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetAxisY(UINT UserIndex, LONG Value) // Left Stick X
{
	g_Gamepad[UserIndex - 1].sThumbLY = Value;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetAxisRx(UINT UserIndex, LONG Value) // Left Stick X
{
	g_Gamepad[UserIndex - 1].sThumbRX = Value;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetAxisRy(UINT UserIndex, LONG Value) // Left Stick X
{
	g_Gamepad[UserIndex - 1].sThumbRY = Value;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetDpad(UINT UserIndex, INT Value)
{
	g_Gamepad[UserIndex - 1].wButtons &= 0xFFF0;
	g_Gamepad[UserIndex - 1].wButtons |= Value;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnA(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_A;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnB(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_B;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnX(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_X;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnY(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_Y;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnStart(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_START;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnBack(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_BACK;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLT(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_LEFT_THUMB;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRT(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_RIGHT_THUMB;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLB(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_LEFT_SHOULDER;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}

VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRB(UINT UserIndex, BOOL Press)
{
	UINT Btn = XINPUT_GAMEPAD_RIGHT_SHOULDER;
	g_Gamepad[UserIndex - 1].wButtons &= ~Btn;
	g_Gamepad[UserIndex - 1].wButtons |= Btn*Press;
	return XOutputSetState(UserIndex, &g_Gamepad[UserIndex - 1]);
}


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

		HANDLE h = GetVXbusHandle();
		if (h == INVALID_HANDLE_VALUE)
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
		if (!DeviceIoControl(h, 0x2A400C, buffer, _countof(buffer), output, FEEDBACK_BUFFER_LENGTH, &trasfered, nullptr))
		{
			return FALSE;
		}

		// Save last successful position data
		memcpy_s(&g_Gamepad[UserIndex-1], sizeof(XINPUT_GAMEPAD), pGamepad, sizeof(XINPUT_GAMEPAD));

#ifdef FEEDBACK_SUPPORTED
		// cache feedback
		memcpy_s(g_Feedback[(UserIndex - 1)], FEEDBACK_BUFFER_LENGTH, output, FEEDBACK_BUFFER_LENGTH);

#endif // FEEDBACK_SUPPORTED

		return TRUE;
	}
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
		detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		// get detail buffer
		if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, requiredSize, &requiredSize, nullptr))
		{
			SetupDiDestroyDeviceInfoList(deviceInfoSet);
			free(detailDataBuffer);
			return -1;
		}

		// Copy	the path to output buffer
		memcpy((void *)path, detailDataBuffer->DevicePath, requiredSize*sizeof(WCHAR));

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

	if (n<1)
		return INVALID_HANDLE_VALUE;

	// bus found, open it and obtain handle
	HANDLE hScpVBus = CreateFile(path,
								 GENERIC_READ | GENERIC_WRITE,
								 FILE_SHARE_READ | FILE_SHARE_WRITE,
								 nullptr,
								 OPEN_EXISTING,
								 FILE_ATTRIBUTE_NORMAL,
								 nullptr);
	return hScpVBus;
}
