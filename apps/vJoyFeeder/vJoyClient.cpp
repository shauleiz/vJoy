//////////////////////////////////////////////////////////////////////////////////////////
// Logical layer between vJoyInterface.dll and the application (vJoyDemo)
//
//	It is assumed that the application does not open more than one VJD for writing.
//	The handle to this device is a global paramete hDevice
//
//
//	Registration: 
//		Registeres the application main window to receive notification from vJoy driver when PNP operation is underway
//		This anables the application to close the handle to the device when the device is goind down and (optionally) to
//		re-connect when it goes up again.
//		Functions:
//		- RegistervJoyNotification():  Every time an interface of type GUID_DEVINTERFACE_VJOY is enables/disabled.
//		- RegisterHandleNotification(): Every time the system tries to remove the device to which the application has an open handle
//
//	Access to vJoy Device (VJD)
//		It is assumed that there is one device open at a time (at most).
//		The global parameter (hDevice) holdes a handle to this device or  hDevice = INVALID_HANDLE_VALUE if none is open
//		Functions:
//		- GetJoystickHandle()
//		- openDevice()
//		- CloseJoystickDevice()
//		- ReopenJoystickDevice()
//
//	Writing position data to open VJD is done by calling update_device().
//
//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <malloc.h>
#include "vjoyclient.h"

// Global
HANDLE hDevice = INVALID_HANDLE_VALUE;


HDEVNOTIFY RegistervJoyNotification(HWND hWin)
// Register event to catch changes in vJoy device interfaces
// Every time an interface of type GUID_DEVINTERFACE_VJOY is enables/disabled
// the window procedure(WndProc) will receive a window message WM_DEVICECHANGE
// Since vJoy device have 16 such interfaces - this message will be sen 16 times,
// every time the raw-PDO device is added or removed.
//	Notes:
//	1. Remember to un-register handle by calling UnregisterDeviceNotification()
//	2. (???) You can register interface even when vJoy not installed
//	3. Need some modifications if to be used with a Service
{
	HDEVNOTIFY hDeviceNotifyInterFace = NULL;
	GUID InterfaceClassGuid = GUID_DEVINTERFACE_VJOY;
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = InterfaceClassGuid;


	hDeviceNotifyInterFace = RegisterDeviceNotification( 
		hWin,                       // events recipient
		&NotificationFilter,        // type of device
		DEVICE_NOTIFY_WINDOW_HANDLE  /* type of recipient handle*/
		);

	return hDeviceNotifyInterFace;
}

HDEVNOTIFY RegisterHandleNotification(HWND hWin)
{
	HDEVNOTIFY hDeviceNotifyHandle = NULL;
	DEV_BROADCAST_HANDLE  NotificationFilterHandle;
	ZeroMemory( &NotificationFilterHandle, sizeof(NotificationFilterHandle) );
	NotificationFilterHandle.dbch_size = sizeof(DEV_BROADCAST_HANDLE);
	NotificationFilterHandle.dbch_devicetype = DBT_DEVTYP_HANDLE;
	HANDLE hJoystickHandle = GetJoystickHandle();
	if (hJoystickHandle != INVALID_HANDLE_VALUE)
	{
		NotificationFilterHandle.dbch_handle = GetJoystickHandle();


		hDeviceNotifyHandle = RegisterDeviceNotification( 
			hWin,                       // events recipient
			&NotificationFilterHandle,        // type of device
			DEVICE_NOTIFY_WINDOW_HANDLE  /* type of recipient handle*/
			);
	};

	return hDeviceNotifyHandle;
}

HANDLE GetJoystickHandle(void)
{
	return hDevice;
}



BOOL openDevice(UINT iInterFace)
	// ---------------------------------------------------------------------------------------------------------- \\
	// Opens vJoy device for writing
	// Returns a valid handle to the device if the device is responsive
	// If handle already open - NO-OP
{
	//// NO-OP
	//if (hDevice && hDevice  != INVALID_HANDLE_VALUE)
	//	return TRUE;

	//// Open
	//hDevice = OpenVJD(iInterFace);
	//if (hDevice  == INVALID_HANDLE_VALUE)
	//	return FALSE;
	//else
		return TRUE;

}

BOOL  CloseJoystickDevice(void)
{
	if (hDevice == INVALID_HANDLE_VALUE)
		return TRUE;

	BOOL out = CloseHandle(hDevice);
	if (out)
		hDevice = INVALID_HANDLE_VALUE;
	return out;
}

//void ReopenJoystickDevice(void)
///* Reopen handle to device if nor already open */
//
//{
//	if (hDevice != INVALID_HANDLE_VALUE)
//		return;
//
//	hDevice = CreateFileA(DOS_FILE_NAME, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
//}


DWORD GetErrorString(TCHAR * Msg, int Size)
{
	TCHAR * s;
	DWORD errorcode = GetLastError();
	int nTChars = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,  NULL, errorcode, 0, (LPSTR)&s,  0, (va_list*)NULL);
	if (!nTChars)
		return errorcode;

	_tcsncpy_s(Msg, Size, s, Size);
	LocalFree(s);
	return errorcode;
}

BOOL isInstalled(void)
// ---------------------------------------------------------------------------------------------------------- \\
// Tests if vJoy device is installed 
// This call to the system requests installation information for the device by its ID (GUID_DEVINTERFACE_VJOY)
// It does not tests its functionality and it does not care if it is disabled.
{
	HDEVINFO hDeviceInfo;
	hDeviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_VJOY, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hDeviceInfo == INVALID_HANDLE_VALUE)
	{
		//GetErrorString(ErrMsg,1000);
		//_tprintf(_T("[E] SetupDiGetClassDevs failed with error: %s\n"), ErrMsg);
		return FALSE;
	}
	else
		return TRUE;

}

BOOL getDeviceAttrib(TCHAR * Msg)
// ---------------------------------------------------------------------------------------------------------- \\
// Get the driver attributes (Vendor ID, Product ID, Version Number)
{

	int res = 1;
	if (!Msg)
		return FALSE;

	PWSTR Product = (PWSTR)GetvJoyProductString();
	PWSTR Manufacturer = (PWSTR)GetvJoyManufacturerString();
	PWSTR SerialNumber = (PWSTR)GetvJoySerialNumberString();
	USHORT version = GetvJoyVersion();

	if (!version)
	{
		_stprintf_s(Msg, MSG_SIZE, _T("Failed\r\nvJoy is probably disabled or uninstalled"));
		return FALSE;
	}
	else
	{
		if (!Product)
			_stprintf_s(Msg, MSG_SIZE, _T("VendorID:0x%04X ProductID:0x%04X VersionNumber:0x%04X\r\n"), VENDOR_N_ID,  PRODUCT_N_ID, version);
		else
			_stprintf_s(Msg, MSG_SIZE, _T("Product ID: %S, Manufacturer ID: %S, Serial Number: %S\r\nVendorID:0x%04X ProductID:0x%04X VersionNumber:0x%04X"),Product, Manufacturer, SerialNumber, VENDOR_N_ID,  PRODUCT_N_ID, version);

		return TRUE;
	};

}

void update_device(JOYSTICK_POSITION_V2	* iReport)
// ---------------------------------------------------------------------------------------------------------- \\
// Update vJoy device position data
// Position includes  Axes, Buttons and one POV hat switch
// Input: Pointer to data structure
{

	PVOID pPositionMessage;
	UINT	IoCode = LOAD_POSITIONS;
	UINT	IoSize = sizeof(JOYSTICK_POSITION_V2);
	ULONG  bytes;

	// Sanity check
	if (!iReport) return;

	// Buffer hoding the joystick position (iReport) is ready
	// Cast it to PVOID
	pPositionMessage = (PVOID)(iReport);

	// Send joystick position structure to vJoy device
	DeviceIoControl (hDevice, IoCode, pPositionMessage, IoSize, NULL, 0, &bytes, NULL);

}

