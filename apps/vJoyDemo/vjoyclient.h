#include <public.h>
#include <vJoyInterface.h>


//DEFINE_GUID(GUID_DEVINTERFACE_HID, 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00,0x11, 0x11, 0x00, 0x00, 0x30);

#define MSG_SIZE 1000
#define WHILE(constant) \
__pragma(warning(disable: 4127)) while(constant); __pragma(warning(default: 4127))
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

// Prototypes
DWORD GetErrorString(TCHAR * Msg, int Size);
HANDLE OpenJoystickDevice(void);
HANDLE GetJoystickHandle(void);
BOOL CloseJoystickDevice(void);
//void ReopenJoystickDevice(void);
HDEVNOTIFY RegistervJoyNotification(HWND hWin);
HDEVNOTIFY RegisterHandleNotification(HWND hWin);
#ifdef PPJOY_MODE
VOID Convert2CompatibleFormat(PJOYSTICK_STATE pPosCompat, PJOYSTICK_POSITION pPosNative);
#endif

// Interface Functions
BOOL LoadJoystickPos();
BOOL isInstalled();
BOOL openDevice(UINT iInterface);
BOOL getDeviceAttrib(TCHAR * Msg);
void update_device(JOYSTICK_POSITION	* iReport);


