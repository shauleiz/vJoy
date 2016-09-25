#pragma once
#include <Xinput.h>

//////////// Definitions /////////////////////////

typedef struct _BUSENUM_UNPLUG_HARDWARE {

	__in ULONG Size;

	__in ULONG SerialNo;

	__in ULONG Flags;

	ULONG Reserved[1];

} BUSENUM_UNPLUG_HARDWARE, *PBUSENUM_UNPLUG_HARDWARE;



//{F679F562-3164-42CE-A4DB-E7DDBE723909}  
DEFINE_GUID(GUID_DEVINTERFACE_SCPVBUS, 0xf679f562, 0x3164, 0x42ce, 0xa4, 0xdb, 0xe7, 0xdd, 0xbe, 0x72, 0x39, 0x9);

#define VBOX_BUS
#define FEEDBACK_BUFFER_LENGTH 9
#define MAX_NUMBER_XBOX_CTRLS 4

#define DPAD_UP XINPUT_GAMEPAD_DPAD_UP
#define DPAD_DOWN XINPUT_GAMEPAD_DPAD_DOWN
#define DPAD_LEFT XINPUT_GAMEPAD_DPAD_LEFT
#define DPAD_RIGHT XINPUT_GAMEPAD_DPAD_RIGHT
#define DPAD_OFF 0

/** As of Xinput definitions
typedef struct _XINPUT_GAMEPAD
{
WORD                                wButtons;
SHORT                               sThumbLX;
SHORT                               sThumbLY;
SHORT                               sThumbRX;
SHORT                               sThumbRY;
*/
#define AXIS_MAX	32767
#define AXIS_MIN	-32767
/**
BYTE                                bLeftTrigger;
BYTE                                bRightTrigger;
}
*/
#define AXIS_SL_MAX	255
#define AXIS_SL_MIN	0


#define FILE_DEVICE_BUSENUM		FILE_DEVICE_BUS_EXTENDER
#define BUSENUM_IOCTL(_index_)	CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_READ_DATA)
#define BUSENUM_W_IOCTL(_index_)	CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_WRITE_DATA)
#define BUSENUM_R_IOCTL(_index_)	CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_READ_DATA)
#define BUSENUM_RW_IOCTL(_index_)	CTL_CODE(FILE_DEVICE_BUSENUM, _index_, METHOD_BUFFERED, FILE_WRITE_DATA | FILE_READ_DATA)

#define IOCTL_BUSENUM_BASE 0x801

#ifdef VBOX_BUS
#define IOCTL_BUSENUM_PLUGIN_HARDWARE	BUSENUM_W_IOCTL(IOCTL_BUSENUM_BASE+0x0)
#define IOCTL_BUSENUM_UNPLUG_HARDWARE	BUSENUM_W_IOCTL(IOCTL_BUSENUM_BASE+0x1)
#define IOCTL_BUSENUM_EJECT_HARDWARE	BUSENUM_W_IOCTL(IOCTL_BUSENUM_BASE+0x2)
#define IOCTL_BUSENUM_REPORT_HARDWARE	BUSENUM_RW_IOCTL(IOCTL_BUSENUM_BASE+0x3)
#else
#define IOCTL_BUSENUM_PLUGIN_HARDWARE	BUSENUM_IOCTL(0x0)
#define IOCTL_BUSENUM_UNPLUG_HARDWARE	BUSENUM_IOCTL(0x1)
#define IOCTL_BUSENUM_EJECT_HARDWARE	BUSENUM_IOCTL(0x2)
#define IOCTL_BUSENUM_REPORT_HARDWARE	BUSENUM_IOCTL(0x3)
#endif

#define IOCTL_BUSENUM_ISDEVPLUGGED	BUSENUM_RW_IOCTL(IOCTL_BUSENUM_BASE+0x100)
#define IOCTL_BUSENUM_EMPTY_SLOTS	BUSENUM_RW_IOCTL(IOCTL_BUSENUM_BASE+0x101)
#define IOCTL_BUSENUM_PROC_ID		BUSENUM_RW_IOCTL(IOCTL_BUSENUM_BASE+0x102)



//////////// Globals /////////////////////////
XINPUT_GAMEPAD g_Gamepad[MAX_NUMBER_XBOX_CTRLS];
BOOL g_vDevice[MAX_NUMBER_XBOX_CTRLS] = { FALSE };
HANDLE g_hBus = INVALID_HANDLE_VALUE;

//////////// Interface Functions /////////////////////////
extern "C"
{

	/// Status
	VJOYINTERFACE_API BOOL	__cdecl	 isVBusExists(void);
	VJOYINTERFACE_API BOOL	__cdecl	 GetNumEmptyBusSlots(UCHAR * nSlots);
	VJOYINTERFACE_API BOOL	__cdecl	 isControllerExists(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 isControllerOwned(UINT UserIndex);


	// Virtual device Plug-In/Unplug
	VJOYINTERFACE_API BOOL	__cdecl	 PlugIn(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 UnPlug(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 UnPlugForce(UINT UserIndex);

	// Data Transfer (Data to the device)
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnAny(UINT UserIndex, BOOL Press, UINT xBtn);
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnA(UINT UserIndex, BOOL Press);
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnB(UINT UserIndex, BOOL Press);
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnX(UINT UserIndex, BOOL Press);
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnY(UINT UserIndex, BOOL Press);
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnStart(UINT UserIndex, BOOL Press);
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnBack(UINT UserIndex, BOOL Press);
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLT(UINT UserIndex, BOOL Press); // Left Thumb/Stick
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRT(UINT UserIndex, BOOL Press); // Right Thumb/Stick
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnLB(UINT UserIndex, BOOL Press); // Left Bumper
	VJOYINTERFACE_API BOOL	__cdecl	 SetBtnRB(UINT UserIndex, BOOL Press); // Right Bumper
	VJOYINTERFACE_API BOOL	__cdecl	 SetTriggerL(UINT UserIndex, BYTE Value, UINT Threshold); // Left Trigger
	VJOYINTERFACE_API BOOL	__cdecl	 SetTriggerR(UINT UserIndex, BYTE Value, UINT Threshold); // Right Trigger
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpad(UINT UserIndex, INT Value);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadUp(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadRight(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadDown(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadLeft(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadUpRight(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadDownRight(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadDownLeft(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadUpLeft(UINT UserIndex);
	VJOYINTERFACE_API BOOL	__cdecl	 SetDpadOff(UINT UserIndex);

	// Data Transfer (Feedback from the device)
	VJOYINTERFACE_API BOOL	__cdecl	 GetLedNumber(UINT UserIndex, PBYTE pLed);
	VJOYINTERFACE_API BOOL	__cdecl	 GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib);

	/**
	Axis DeadZones and Trigger Thresholds function (Xinput recommends)
	https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx
	*/
	VJOYINTERFACE_API BOOL	__cdecl	 SetAxisXY(UINT UserIndex, SHORT ValueX, SHORT ValueY, UINT AxisX, UINT AxisY, UINT DeadZone, SHORT Axis_Max);
	VJOYINTERFACE_API BOOL __cdecl GetAxisXY(UINT UserIndex, PSHORT ValueX, PSHORT ValueY, UINT AxisX, UINT AxisY);
	VJOYINTERFACE_API BOOL __cdecl GetTriggerLR(UINT UserIndex, PBYTE ValueL, PBYTE ValueR, UINT AxisL, UINT AxisR);

	VJOYINTERFACE_API BOOL __cdecl	SetTriggerLR(UINT UserIndex, SHORT ValueL, SHORT ValueR, UINT Threshold);
}  // extern "C"

//////////// Helper Functions /////////////////////////
int GetVXbusPath(LPCTSTR path, UINT size);
HANDLE GetVXbusHandle(void);
BOOL GetCreateProcID(DWORD UserIndex, PULONG ProcID);
BOOL XOutputSetState(DWORD UserIndex, XINPUT_GAMEPAD* pGamepad);
BOOL XOutputSetGetState(DWORD UserIndex, XINPUT_GAMEPAD* pGamepad, PBYTE bVibrate, PBYTE bLargeMotor, PBYTE bSmallMotor, PBYTE bLed);
BOOL SetBtn(UINT UserIndex, BOOL Press, UCHAR nBtn);
WORD ConvertButton(LONG vBtns, WORD xBtns, UINT vBtn, UINT xBtn);
BOOL UnPlug_Opt(UINT UserIndex, BOOL Force);