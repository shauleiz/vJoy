/****************************************************************************
*																			*
*   vGenInterface.h -- This header file defines the Generic interface		*
*   that includes vJoy and vXbox											*
*		                                                                    *
*                                                                           *
*   Copyright (c)  Shaul Eizikovich									        *
*																			*
*****************************************************************************/

// Must Include <Xinput.h>

#define DPAD_UP XINPUT_GAMEPAD_DPAD_UP
#define DPAD_DOWN XINPUT_GAMEPAD_DPAD_DOWN
#define DPAD_LEFT XINPUT_GAMEPAD_DPAD_LEFT
#define DPAD_RIGHT XINPUT_GAMEPAD_DPAD_RIGHT
#define DPAD_OFF 0

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VGENINTERFACE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VGENINTERFACE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef VGENINTERFACE_EXPORTS
#define VGENINTERFACE_API __declspec(dllexport)
#else
#define VGENINTERFACE_API __declspec(dllimport)
#endif

typedef UINT HDEVICE;

//////////////////////////////////////////////////////////////////////////////////////
/// 
///  vJoy interface fuctions (Native vJoy)
///  If you wish to write GENERIC code (Used for vJoy AND vXbox) 
///  then you can use this set of interface functions
///
///  vJoy device range: 1-16
///  vXbox device range: 1001-1004
///
//////////////////////////////////////////////////////////////////////////////////////
	/////	vJoy/vXbox Device properties
	VGENINTERFACE_API int	__cdecl  GetVJDButtonNumber(UINT rID);	// Get the number of buttons defined in the specified VDJ
	VGENINTERFACE_API int	__cdecl  GetVJDDiscPovNumber(UINT rID);	// Get the number of descrete-type POV hats defined in the specified VDJ
	VGENINTERFACE_API int	__cdecl  GetVJDContPovNumber(UINT rID);	// Get the number of descrete-type POV hats defined in the specified VDJ
	VGENINTERFACE_API BOOL	__cdecl  GetVJDAxisExist(UINT rID, UINT Axis); // Test if given axis defined in the specified VDJ
	VGENINTERFACE_API BOOL	__cdecl  GetVJDAxisMax(UINT rID, UINT Axis, LONG * Max); // Get logical Maximum value for a given axis defined in the specified VDJ
	VGENINTERFACE_API BOOL	__cdecl  GetVJDAxisMin(UINT rID, UINT Axis, LONG * Min); // Get logical Minimum value for a given axis defined in the specified VDJ
	VGENINTERFACE_API enum VjdStat	__cdecl	GetVJDStatus(UINT rID);			// Get the status of the specified vJoy Device.
	VGENINTERFACE_API BOOL	__cdecl	isVJDExists(UINT rID);					// TRUE if the specified vJoy Device exists

	/////	Write access to vJoy Device - Basic
	VGENINTERFACE_API BOOL		__cdecl	AcquireVJD(UINT rID);				// Acquire the specified vJoy Device.
	VGENINTERFACE_API VOID		__cdecl	RelinquishVJD(UINT rID);			// Relinquish the specified vJoy Device.
	VGENINTERFACE_API BOOL		__cdecl	UpdateVJD(UINT rID, PVOID pData);	// Update the position data of the specified vJoy Device.

	/////	Write access to vJoy Device - Modifyiers
	// This group of functions modify the current value of the position data
	// They replace the need to create a structure of position data then call UpdateVJD

	//// Reset functions
	VGENINTERFACE_API BOOL		__cdecl	ResetVJD(UINT rID);			// Reset all controls to predefined values in the specified VDJ
	VGENINTERFACE_API VOID		__cdecl	ResetAll(void);				// Reset all controls to predefined values in all VDJ
	VGENINTERFACE_API BOOL		__cdecl	ResetButtons(UINT rID);		// Reset all buttons (To 0) in the specified VDJ
	VGENINTERFACE_API BOOL		__cdecl	ResetPovs(UINT rID);		// Reset all POV Switches (To -1) in the specified VDJ

	// Write data
	VGENINTERFACE_API BOOL		__cdecl	SetAxis(LONG Value, UINT rID, UINT Axis);		// Write Value to a given axis defined in the specified VDJ 
	VGENINTERFACE_API BOOL		__cdecl	SetBtn(BOOL Value, UINT rID, UCHAR nBtn);		// Write Value to a given button defined in the specified VDJ 
	VGENINTERFACE_API BOOL		__cdecl	SetDiscPov(int Value, UINT rID, UCHAR nPov);	// Write Value to a given descrete POV defined in the specified VDJ 
	VGENINTERFACE_API BOOL		__cdecl	SetContPov(DWORD Value, UINT rID, UCHAR nPov);	// Write Value to a given continuous POV defined in the specified VDJ 



	VGENINTERFACE_API	SHORT		__cdecl GetvJoyVersion(void);

	// vXbox Interface
	VGENINTERFACE_API	BOOL		__cdecl isVBusExists(void);
	VGENINTERFACE_API	BOOL		__cdecl GetNumEmptyBusSlots(UCHAR * nSlots);
	VGENINTERFACE_API	BOOL		__cdecl isControllerOwned(UINT UserIndex);
	VGENINTERFACE_API	BOOL		__cdecl PlugIn(UINT UserIndex);
	VGENINTERFACE_API	BOOL		__cdecl UnPlug(UINT UserIndex);
	VGENINTERFACE_API	BOOL		__cdecl UnPlugForce(UINT UserIndex);

	VGENINTERFACE_API	BOOL		__cdecl SetBtnA(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnB(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnX(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnY(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnLT(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnRT(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnLB(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnRB(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnStart(UINT UserIndex, BOOL Press);
	VGENINTERFACE_API	BOOL		__cdecl SetBtnBack(UINT UserIndex, BOOL Press);

	VGENINTERFACE_API	BOOL		__cdecl SetTriggerL(UINT UserIndex, BYTE Value);
	VGENINTERFACE_API	BOOL		__cdecl SetTriggerR(UINT UserIndex, BYTE Value);
	VGENINTERFACE_API	BOOL		__cdecl	SetAxisX(UINT UserIndex, SHORT Value); // Left Stick X
	VGENINTERFACE_API	BOOL		__cdecl	SetAxisY(UINT UserIndex, SHORT Value); // Left Stick Y
	VGENINTERFACE_API	BOOL		__cdecl	SetAxisRx(UINT UserIndex, SHORT Value); // Right Stick X
	VGENINTERFACE_API	BOOL		__cdecl	SetAxisRy(UINT UserIndex, SHORT Value); // Right Stick Y
	VGENINTERFACE_API	BOOL		__cdecl	SetDpad(UINT UserIndex, INT Value); // DPAD Set Value
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadUp(UINT UserIndex); // DPAD Up
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadRight(UINT UserIndex); // DPAD Right
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadDown(UINT UserIndex); // DPAD Down
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadLeft(UINT UserIndex); // DPAD Left
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadOff(UINT UserIndex); // DPAD Off
	VGENINTERFACE_API	BOOL		__cdecl	GetLedNumber(UINT UserIndex, PBYTE pLed);
	VGENINTERFACE_API	BOOL		__cdecl	GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib);
