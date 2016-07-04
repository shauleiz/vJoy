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
#define Range_vJoy(x) ((x)<=16 && (x)>0)?TRUE:FALSE
#define Range_vXbox(x) ((x)<=1004 && (x)>1000)?TRUE:FALSE
#define to_vXbox(x) ((x)-1000)
#define to_vJoy(x) ((x)+1000)

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
BOOL	IX_isControllerExists(HDEVICE hDev);
BOOL	IX_isControllerOwned(UINT UserIndex);
BOOL	IX_isControllerOwned(HDEVICE hDev);
// Virtual device Plug-In/Unplug
BOOL	IX_PlugIn(UINT UserIndex);
BOOL	IX_UnPlug(UINT UserIndex);
BOOL	IX_UnPlugForce(UINT UserIndex);
// Reset Devices
BOOL	IX_ResetController(UINT UserIndex);
BOOL	IX_ResetController(HDEVICE hDev);
BOOL	IX_ResetAllControllers();
BOOL	IX_ResetControllerBtns(UINT UserIndex);
BOOL	IX_ResetControllerBtns(HDEVICE hDev);
BOOL	IX_ResetControllerDPad(UINT UserIndex);
BOOL	IX_ResetControllerDPad(HDEVICE hDev);
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
BOOL	IX_SetBtn(HDEVICE hDev, BOOL Press, WORD Button);
BOOL	IX_SetBtnA(HDEVICE hDev, BOOL Press);
BOOL	IX_SetBtnB(HDEVICE hDev, BOOL Press);
BOOL	IX_SetBtnX(HDEVICE hDev, BOOL Press);
BOOL	IX_SetBtnY(HDEVICE hDev, BOOL Press);
BOOL	IX_SetBtnStart(HDEVICE hDev, BOOL Press);
BOOL	IX_SetBtnBack(HDEVICE hDev, BOOL Press);
BOOL	IX_SetBtnLT(HDEVICE hDev, BOOL Press); // Left Thumb/Stick
BOOL	IX_SetBtnRT(HDEVICE hDev, BOOL Press); // Right Thumb/Stick
BOOL	IX_SetBtnLB(HDEVICE hDev, BOOL Press); // Left Bumper
BOOL	IX_SetBtnRB(HDEVICE hDev, BOOL Press); // Right Bumper
BOOL	IX_SetTriggerL(HDEVICE hDev, BYTE Value); // Left Trigger
BOOL	IX_SetTriggerR(HDEVICE hDev, BYTE Value); // Right Trigger
BOOL	IX_SetAxisX(HDEVICE hDev, SHORT Value); // Left Stick X
BOOL	IX_SetAxisY(HDEVICE hDev, SHORT Value); // Left Stick Y
BOOL	IX_SetAxisRx(HDEVICE hDev, SHORT Value); // Right Stick X
BOOL	IX_SetAxisRy(HDEVICE hDev, SHORT Value); // Right Stick Y
BOOL	IX_SetDpad(HDEVICE hDev, INT Value);
BOOL	IX_SetDpadUp(HDEVICE hDev);
BOOL	IX_SetDpadRight(HDEVICE hDev);
BOOL	IX_SetDpadDown(HDEVICE hDev);
BOOL	IX_SetDpadLeft(HDEVICE hDev);
BOOL	IX_SetDpadOff(HDEVICE hDev);
// Data Transfer (Feedback from the device)
BOOL	IX_GetLedNumber(UINT UserIndex, PBYTE pLed);
BOOL	IX_GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib);

#pragma endregion // vXbox Internal Functions

#pragma region vJoy Internal Functions
HDEVICE	IJ_AcquireVJD(UINT rID);				// Acquire the specified vJoy Device.
VOID IJ_RelinquishVJD(HDEVICE hDev);			// Relinquise the specified vJoy Device.
BOOL IJ_isVJDExists(HDEVICE hDev);
enum VjdStat IJ_GetVJDStatus(HDEVICE hDev);			// Get the status of the specified vJoy Device.
BOOL IJ_GetVJDAxisExist(HDEVICE hDev, UINT Axis); // Test if given axis defined in the specified VDJ
int	IJ_GetVJDButtonNumber(HDEVICE hDev);	// Get the number of buttons defined in the specified VDJ
int IJ_GetVJDDiscPovNumber(HDEVICE hDev);   // Get the number of POVs defined in the specified device
int IJ_GetVJDContPovNumber(HDEVICE hDev);	// Get the number of descrete-type POV hats defined in the specified VDJ
BOOL IJ_SetAxis(LONG Value, HDEVICE hDev, UINT Axis);		// Write Value to a given axis defined in the specified VDJ 
BOOL IJ_SetBtn(BOOL Value, HDEVICE hDev, UCHAR nBtn);		// Write Value to a given button defined in the specified VDJ 
BOOL IJ_SetDiscPov(int Value, HDEVICE hDev, UCHAR nPov);	// Write Value to a given descrete POV defined in the specified VDJ 
BOOL IJ_SetContPov(DWORD Value, HDEVICE hDev, UCHAR nPov);	// Write Value to a given continuous POV defined in the specified VDJ 

#pragma endregion

//// Device Container and Device Handle functions
HDEVICE CreateDevice(DevType Type, UINT i);
void DestroyDevice(HDEVICE dev);
HDEVICE GetDevice(DevType Type, UINT i);
UINT GetDeviceId(HDEVICE h);
BOOL isDevice_vJoy(HDEVICE h);
BOOL isDevice_vXbox(HDEVICE h);
void * GetDevicePos(HDEVICE h);

// Other helper functions
BOOL ConvertPosition_vJoy2vXbox(void *vJoyPos, void *vXboxPos);
WORD ConvertButton(LONG vBtns, WORD xBtns, UINT vBtn, UINT xBtn);

