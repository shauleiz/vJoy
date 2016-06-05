#pragma once
#include <map>
//////////////////////////////////////////////////////////
//
// vGenInterface private header file
// 
// Use it to declare internal global variables
// and internal function declarations
//
//////////////////////////////////////////////////////////

//////////////////////////////////
// Device Type
enum DevType { vJoy, vXbox };

// Device Structure
typedef struct 
	{ 
		DevType Type;	// vJoy or vXbox
		UINT Id;		// vJoy ID or vXbox Index
		union 
		{
			XINPUT_GAMEPAD * vXboxPos;
			JOYSTICK_POSITION_V2 * vJoyPos;
		} PPosition;
	} DEVICE, *PDEVICE;

std::map<HDEVICE, DEVICE> DevContainer;

// Macros
#define Range_vJoy(x) (x<=16 && x>0):TRUE?FALSE
#define Range_vXbox(x) (x<=1004 && x>1000):TRUE?FALSE
#define to_vXbox(x) (x-1000)

//
// Constants for gamepad buttons
//
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000


#pragma region vXbox Internal Functions
				//////////// vXbox Internal Functions ////////////

/// Status
BOOL	IX_isVBusExists(void);
BOOL	IX_GetNumEmptyBusSlots(UCHAR * nSlots);
BOOL	IX_isControllerExists(UINT UserIndex);
BOOL	IX_isControllerOwned(UINT UserIndex);
// Virtual device Plug-In/Unplug
BOOL	IX_PlugIn(UINT UserIndex);
BOOL	IX_UnPlug(UINT UserIndex);
BOOL	IX_UnPlugForce(UINT UserIndex);
// Data Transfer (Data to the device)
BOOL	IX_SetBtn(UINT UserIndex, BOOL Press, WORD Button);
BOOL	IX_SetBtnA(UINT UserIndex, BOOL Press);
BOOL	IX_SetBtnB(UINT UserIndex, BOOL Press);
BOOL	IX_SetBtnX(UINT UserIndex, BOOL Press);
BOOL	IX_SetBtnY(UINT UserIndex, BOOL Press);
BOOL	IX_SetBtnStart(UINT UserIndex, BOOL Press);
BOOL	IX_SetBtnBack(UINT UserIndex, BOOL Press);
BOOL	IX_SetBtnLT(UINT UserIndex, BOOL Press); // Left Thumb/Stick
BOOL	IX_SetBtnRT(UINT UserIndex, BOOL Press); // Right Thumb/Stick
BOOL	IX_SetBtnLB(UINT UserIndex, BOOL Press); // Left Bumper
BOOL	IX_SetBtnRB(UINT UserIndex, BOOL Press); // Right Bumper
BOOL	IX_SetTriggerL(UINT UserIndex, BYTE Value); // Left Trigger
BOOL	IX_SetTriggerR(UINT UserIndex, BYTE Value); // Right Trigger
BOOL	IX_SetAxisX(UINT UserIndex, SHORT Value); // Left Stick X
BOOL	IX_SetAxisY(UINT UserIndex, SHORT Value); // Left Stick Y
BOOL	IX_SetAxisRx(UINT UserIndex, SHORT Value); // Right Stick X
BOOL	IX_SetAxisRy(UINT UserIndex, SHORT Value); // Right Stick Y
BOOL	IX_SetDpad(UINT UserIndex, INT Value);
BOOL	IX_SetDpadUp(UINT UserIndex);
BOOL	IX_SetDpadRight(UINT UserIndex);
BOOL	IX_SetDpadDown(UINT UserIndex);
BOOL	IX_SetDpadLeft(UINT UserIndex);
BOOL	IX_SetDpadOff(UINT UserIndex);
// Data Transfer (Feedback from the device)
BOOL	IX_GetLedNumber(UINT UserIndex, PBYTE pLed);
BOOL	IX_GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib);

#pragma endregion // vXbox Internal Functions

//// Device Container and Device Handle functions
HDEVICE CreateDevice(DevType Type, UINT i);
void DestroyDevice(HDEVICE dev);
HDEVICE GetDevice(DevType Type, UINT i);
void * GetDevicePos(HDEVICE h);
