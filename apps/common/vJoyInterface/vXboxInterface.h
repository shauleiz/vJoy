#pragma once
#include <Xinput.h>

//////////// Definitions /////////////////////////

// Position Data structure



//{F679F562-3164-42CE-A4DB-E7DDBE723909}  
DEFINE_GUID(GUID_DEVINTERFACE_SCPVBUS, 0xf679f562, 0x3164, 0x42ce, 0xa4, 0xdb, 0xe7, 0xdd, 0xbe, 0x72, 0x39, 0x9);

#define FEEDBACK_BUFFER_LENGTH 9
#define MAX_NUMBER_XBOX_CTRLS 4

#define DPAD_UP XINPUT_GAMEPAD_DPAD_UP
#define DPAD_DOWN XINPUT_GAMEPAD_DPAD_DOWN
#define DPAD_LEFT XINPUT_GAMEPAD_DPAD_LEFT
#define DPAD_RIGHT XINPUT_GAMEPAD_DPAD_RIGHT
#define DPAD_OFF 0

#define AXIS_MAX	32767
#define AXIS_MIN	-32768


#define FILE_DEVICE_BUSENUM		FILE_DEVICE_BUS_EXTENDER
#define BUSENUM_IOCTL(_index_)	CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_BUSENUM_PLUGIN_HARDWARE	BUSENUM_IOCTL(0x0)
#define IOCTL_BUSENUM_UNPLUG_HARDWARE	BUSENUM_IOCTL(0x1)
#define IOCTL_BUSENUM_EJECT_HARDWARE	BUSENUM_IOCTL(0x2)
#define IOCTL_BUSENUM_REPORT_HARDWARE	BUSENUM_IOCTL(0x3)

//////////// Globals /////////////////////////
XINPUT_GAMEPAD g_Gamepad[MAX_NUMBER_XBOX_CTRLS];
BOOL g_vDevice[MAX_NUMBER_XBOX_CTRLS] = { FALSE };
HANDLE g_hBus = INVALID_HANDLE_VALUE;

//////////// Interface Functions /////////////////////////

/// Status
VJOYINTERFACE_API BOOL	__cdecl	 isVBusExists(void);
VJOYINTERFACE_API BOOL	__cdecl	 isControllerExists(UINT UserIndex);
VJOYINTERFACE_API BOOL	__cdecl	 isControllerOwned(UINT UserIndex);

// Virtual device Plug-In/Unplug
VJOYINTERFACE_API BOOL	__cdecl	 PlugIn(UINT UserIndex);
VJOYINTERFACE_API BOOL	__cdecl	 UnPlug(UINT UserIndex);

// Data Transfer
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnA(UINT UserIndex, BOOL Press);
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnB(UINT UserIndex, BOOL Press);
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnX(UINT UserIndex, BOOL Press);
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnY(UINT UserIndex, BOOL Press);
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnStart(UINT UserIndex, BOOL Press);
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnBack(UINT UserIndex, BOOL Press);
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLT(UINT UserIndex, BOOL Press); // Left Trigger
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRT(UINT UserIndex, BOOL Press); // Right Trigger
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLB(UINT UserIndex, BOOL Press); // Left Bumper
VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRB(UINT UserIndex, BOOL Press); // Right Bumper
VJOYINTERFACE_API BOOL	__cdecl	 SetAxisX(UINT UserIndex, SHORT Value); // Left Stick X
VJOYINTERFACE_API BOOL	__cdecl	 SetAxisY(UINT UserIndex, SHORT Value); // Left Stick Y
VJOYINTERFACE_API BOOL	__cdecl	 SetAxisRx(UINT UserIndex, SHORT Value); // Right Stick X
VJOYINTERFACE_API BOOL	__cdecl	 SetAxisRy(UINT UserIndex, SHORT Value); // Right Stick Y
VJOYINTERFACE_API BOOL	__cdecl	 SetDpadUp(UINT UserIndex);
VJOYINTERFACE_API BOOL	__cdecl	 SetDpadRight(UINT UserIndex);
VJOYINTERFACE_API BOOL	__cdecl	 SetDpadDown(UINT UserIndex);
VJOYINTERFACE_API BOOL	__cdecl	 SetDpadLeft(UINT UserIndex);
VJOYINTERFACE_API BOOL	__cdecl	 SetDpadOff(UINT UserIndex);

//////////// Helper Functions /////////////////////////
int GetVXbusPath(LPCTSTR path, UINT size);
HANDLE GetVXbusHandle(void);
BOOL XOutputSetState(DWORD UserIndex, XINPUT_GAMEPAD* pGamepad);
BOOL SetDpad(UINT UserIndex, INT Value);
WORD ConvertButton(LONG vBtns, WORD xBtns, UINT vBtn, UINT xBtn);
