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

// Definition to Device Handle
typedef INT HDEVICE;
#define  INVALID_DEV (HDEVICE)0
#define ValidDev(x) ((x == INVALID_DEV)?FALSE:TRUE)

#ifndef VJDSTAT
#define VJDSTAT
enum VjdStat  /* Declares an enumeration data type */
{
	VJD_STAT_OWN,	// The  vJoy Device is owned by this application.
	VJD_STAT_FREE,	// The  vJoy Device is NOT owned by any application (including this one).
	VJD_STAT_BUSY,	// The  vJoy Device is owned by another application. It cannot be acquired by this application.
	VJD_STAT_MISS,	// The  vJoy Device is missing. It either does not exist or the driver is down.
	VJD_STAT_UNKN	// Unknown
};
#endif

// Device Type
enum DevType { vJoy, vXbox };

#ifndef VJOYHEADERUSED

#pragma region HID
// HID Descriptor definitions - Axes
#define HID_USAGE_X		0x30
#define HID_USAGE_Y		0x31
#define HID_USAGE_Z		0x32
#define HID_USAGE_RX	0x33
#define HID_USAGE_RY	0x34
#define HID_USAGE_RZ	0x35
#define HID_USAGE_SL0	0x36
#define HID_USAGE_SL1	0x37
#define HID_USAGE_WHL	0x38
#define HID_USAGE_POV	0x39
#pragma endregion HID

#pragma region FFB Re-Definitions
// HID Descriptor definitions - FFB Report IDs
#define HID_ID_STATE	0x02	// Usage PID State report
#define HID_ID_EFFREP	0x01	// Usage Set Effect Report
#define HID_ID_ENVREP	0x02	// Usage Set Envelope Report
#define HID_ID_CONDREP	0x03	// Usage Set Condition Report
#define HID_ID_PRIDREP	0x04	// Usage Set Periodic Report
#define HID_ID_CONSTREP	0x05	// Usage Set Constant Force Report
#define HID_ID_RAMPREP	0x06	// Usage Set Ramp Force Report
#define HID_ID_CSTMREP	0x07	// Usage Custom Force Data Report
#define HID_ID_SMPLREP	0x08	// Usage Download Force Sample
#define HID_ID_EFOPREP	0x0A	// Usage Effect Operation Report
#define HID_ID_BLKFRREP	0x0B	// Usage PID Block Free Report
#define HID_ID_CTRLREP	0x0C	// Usage PID Device Control
#define HID_ID_GAINREP	0x0D	// Usage Device Gain Report
#define HID_ID_SETCREP	0x0E	// Usage Set Custom Force Report
#define HID_ID_NEWEFREP	0x01	// Usage Create New Effect Report
#define HID_ID_BLKLDREP	0x02	// Usage Block Load Report
#define HID_ID_POOLREP	0x03	// Usage PID Pool Report


enum FFBEType // FFB Effect Type
{

	// Effect Type
	ET_NONE = 0,	  //    No Force
	ET_CONST = 1,    //    Constant Force
	ET_RAMP = 2,    //    Ramp
	ET_SQR = 3,    //    Square
	ET_SINE = 4,    //    Sine
	ET_TRNGL = 5,    //    Triangle
	ET_STUP = 6,    //    Sawtooth Up
	ET_STDN = 7,    //    Sawtooth Down
	ET_SPRNG = 8,    //    Spring
	ET_DMPR = 9,    //    Damper
	ET_INRT = 10,   //    Inertia
	ET_FRCTN = 11,   //    Friction
	ET_CSTM = 12,   //    Custom Force Data
};

enum FFBPType // FFB Packet Type
{
	// Write
	PT_EFFREP = HID_ID_EFFREP,	// Usage Set Effect Report
	PT_ENVREP = HID_ID_ENVREP,	// Usage Set Envelope Report
	PT_CONDREP = HID_ID_CONDREP,	// Usage Set Condition Report
	PT_PRIDREP = HID_ID_PRIDREP,	// Usage Set Periodic Report
	PT_CONSTREP = HID_ID_CONSTREP,	// Usage Set Constant Force Report
	PT_RAMPREP = HID_ID_RAMPREP,	// Usage Set Ramp Force Report
	PT_CSTMREP = HID_ID_CSTMREP,	// Usage Custom Force Data Report
	PT_SMPLREP = HID_ID_SMPLREP,	// Usage Download Force Sample
	PT_EFOPREP = HID_ID_EFOPREP,	// Usage Effect Operation Report
	PT_BLKFRREP = HID_ID_BLKFRREP,	// Usage PID Block Free Report
	PT_CTRLREP = HID_ID_CTRLREP,	// Usage PID Device Control
	PT_GAINREP = HID_ID_GAINREP,	// Usage Device Gain Report
	PT_SETCREP = HID_ID_SETCREP,	// Usage Set Custom Force Report

									// Feature
									PT_NEWEFREP = HID_ID_NEWEFREP + 0x10,	// Usage Create New Effect Report
									PT_BLKLDREP = HID_ID_BLKLDREP + 0x10,	// Usage Block Load Report
									PT_POOLREP = HID_ID_POOLREP + 0x10,		// Usage PID Pool Report
};

enum FFBOP
{
	EFF_START = 1, // EFFECT START
	EFF_SOLO = 2, // EFFECT SOLO START
	EFF_STOP = 3, // EFFECT STOP
};

enum FFB_CTRL
{
	CTRL_ENACT = 1,	// Enable all device actuators.
	CTRL_DISACT = 2,	// Disable all the device actuators.
	CTRL_STOPALL = 3,	// Stop All Effects­ Issues a stop on every running effect.
	CTRL_DEVRST = 4,	// Device Reset– Clears any device paused condition, enables all actuators and clears all effects from memory.
	CTRL_DEVPAUSE = 5,	// Device Pause– The all effects on the device are paused at the current time step.
	CTRL_DEVCONT = 6,	// Device Continue– The all effects that running when the device was paused are restarted from their last time step.
};

enum FFB_EFFECTS {
	Constant = 0x0001,
	Ramp = 0x0002,
	Square = 0x0004,
	Sine = 0x0008,
	Triangle = 0x0010,
	Sawtooth_Up = 0x0020,
	Sawtooth_Dn = 0x0040,
	Spring = 0x0080,
	Damper = 0x0100,
	Inertia = 0x0200,
	Friction = 0x0400,
	Custom = 0x0800,
};

typedef struct _FFB_DATA {
	ULONG	size;
	ULONG	cmd;
	UCHAR	*data;
} FFB_DATA, *PFFB_DATA;

typedef struct _FFB_EFF_CONSTANT {
	BYTE EffectBlockIndex;
	LONG Magnitude; 			  // Constant force magnitude: 	-10000 - 10000
} FFB_EFF_CONSTANT, *PFFB_EFF_CONSTANT;

typedef struct _FFB_EFF_RAMP {
	BYTE		EffectBlockIndex;
	LONG 		Start;             // The Normalized magnitude at the start of the effect (-10000 - 10000)
	LONG 		End;               // The Normalized magnitude at the end of the effect	(-10000 - 10000)
} FFB_EFF_RAMP, *PFFB_EFF_RAMP;

//typedef struct _FFB_EFF_CONST {
typedef struct _FFB_EFF_REPORT {
	BYTE		EffectBlockIndex;
	FFBEType	EffectType;
	WORD		Duration;// Value in milliseconds. 0xFFFF means infinite
	WORD		TrigerRpt;
	WORD		SamplePrd;
	BYTE		Gain;
	BYTE		TrigerBtn;
	BOOL		Polar; // How to interpret force direction Polar (0-360°) or Cartesian (X,Y)
	union
	{
		BYTE	Direction; // Polar direction: (0x00-0xFF correspond to 0-360°)
		BYTE	DirX; // X direction: Positive values are To the right of the center (X); Negative are Two's complement
	};
	BYTE		DirY; // Y direction: Positive values are below the center (Y); Negative are Two's complement
} FFB_EFF_REPORT, *PFFB_EFF_REPORT;
//} FFB_EFF_CONST, *PFFB_EFF_CONST;

typedef struct _FFB_EFF_OP {
	BYTE		EffectBlockIndex;
	FFBOP		EffectOp;
	BYTE		LoopCount;
} FFB_EFF_OP, *PFFB_EFF_OP;

typedef struct _FFB_EFF_PERIOD {
	BYTE		EffectBlockIndex;
	DWORD		Magnitude;			// Range: 0 - 10000
	LONG 		Offset;				// Range: –10000 - 10000
	DWORD 		Phase;				// Range: 0 - 35999
	DWORD 		Period;				// Range: 0 - 32767
} FFB_EFF_PERIOD, *PFFB_EFF_PERIOD;

typedef struct _FFB_EFF_COND {
	BYTE		EffectBlockIndex;
	BOOL		isY;
	LONG 		CenterPointOffset; // CP Offset:  Range -­10000 ­- 10000
	LONG 		PosCoeff; // Positive Coefficient: Range -­10000 ­- 10000
	LONG 		NegCoeff; // Negative Coefficient: Range -­10000 ­- 10000
	DWORD 		PosSatur; // Positive Saturation: Range 0 – 10000
	DWORD 		NegSatur; // Negative Saturation: Range 0 – 10000
	LONG 		DeadBand; // Dead Band: : Range 0 – 1000
} FFB_EFF_COND, *PFFB_EFF_COND;

typedef struct _FFB_EFF_ENVLP {
	BYTE		EffectBlockIndex;
	DWORD 		AttackLevel;   // The Normalized magnitude of the stating point: 0 - 10000
	DWORD 		FadeLevel;	   // The Normalized magnitude of the stopping point: 0 - 10000
	DWORD 		AttackTime;	   // Time of the attack: 0 - 4294967295
	DWORD 		FadeTime;	   // Time of the fading: 0 - 4294967295
} FFB_EFF_ENVLP, *PFFB_EFF_ENVLP;

#define FFB_DATA_READY	 WM_USER+31

typedef void (CALLBACK *FfbGenCB)(PVOID, PVOID);
#pragma endregion

#endif // !VJOYHEADERUSED
//////////////////////////////////////////////////////////////////////////////////////
/// 
///  vJoy interface fuctions (Native vJoy)
///  If you wish to write GENERIC code (Used for vJoy AND vXbox) 
///  then you can use this set of interface functions
///
///  vJoy device ID range: 1-16
///  vXbox device ID range: 1001-1004
///
///  Axis & Button Mapping:
///  According to https://msdn.microsoft.com/en-us/library/windows/desktop/hh405052(v=vs.85).aspx
///  (Assuming XUSB subtype  1 - Gamepad)
///  
///  vJoy		|	vXbox
///  -----------------------
///  X			|	X
///  Y			|	Y
///  Z			|	Trigger (R)	
///  Rx			|	Rx
///  Ry			|	Ry
///  Rz			|	Trigger (L)
///  Button 1	|	A
///  Button 2	|	B
///  Button 3	|	X
///  Button 4	|	Y
///  Button 5	|	Left Bumper (LB) A.K.A Left Shoulder
///  Button 6	|	Right Bumper (RB) A.K.A Right Shoulder
///  Button 7	|	Back
///  Button 8	|	Start
///  Button 9	|	Left Thumb (LT)
///  Button 10	|	Right Thumb (RT)
///  
///  Axis Serial number is:
///		| vJoy		|	vXbox
///  --------------------------
///  1	| X			|	X
///  2	| Y			|	Y
///  3	| Z			|	Trigger (R)	
///  4	| Rx		|	Rx
///  5	| Ry		|	Ry
///  6	| Rz		|	Trigger (L)
///  7	| Slider0	|	-
///  8	| Slider1	|	-
///  

extern "C" {
	///
#pragma region Backward compatibility API
//////////////////////////////////////////////////////////////////////////////////////
// Version
	VGENINTERFACE_API	SHORT		__cdecl GetvJoyVersion(void);
	VGENINTERFACE_API	BOOL		__cdecl vJoyEnabled(void);

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

	//// Device-Reset functions
	VGENINTERFACE_API BOOL		__cdecl	ResetVJD(UINT rID);			// Reset all controls to predefined values in the specified VDJ
	VGENINTERFACE_API VOID		__cdecl	ResetAll(void);				// Reset all controls to predefined values in all VDJ
	VGENINTERFACE_API BOOL		__cdecl	ResetButtons(UINT rID);		// Reset all buttons (To 0) in the specified VDJ
	VGENINTERFACE_API BOOL		__cdecl	ResetPovs(UINT rID);		// Reset all POV Switches (To -1) in the specified VDJ

	// Write data
	VGENINTERFACE_API BOOL		__cdecl	SetAxis(LONG Value, UINT rID, UINT Axis);		// Write Value to a given axis defined in the specified VDJ 
	VGENINTERFACE_API BOOL		__cdecl	SetBtn(BOOL Value, UINT rID, UCHAR nBtn);		// Write Value to a given button defined in the specified VDJ 
	VGENINTERFACE_API BOOL		__cdecl	SetDiscPov(int Value, UINT rID, UCHAR nPov);	// Write Value to a given descrete POV defined in the specified VDJ 
	VGENINTERFACE_API BOOL		__cdecl	SetContPov(DWORD Value, UINT rID, UCHAR nPov);	// Write Value to a given continuous POV defined in the specified VDJ 

	// FFB function
	VGENINTERFACE_API FFBEType	__cdecl	FfbGetEffect();	// Returns effect serial number if active, 0 if inactive
	VGENINTERFACE_API VOID		__cdecl	FfbRegisterGenCB(FfbGenCB cb, PVOID data);
	__declspec(deprecated("** FfbStart function was deprecated - you can remove it from your code **")) \
		VGENINTERFACE_API BOOL		__cdecl	FfbStart(UINT rID);				  // Start the FFB queues of the specified vJoy Device.
	__declspec(deprecated("** FfbStop function was deprecated - you can remove it from your code **")) \
		VGENINTERFACE_API VOID		__cdecl	FfbStop(UINT rID);				  // Stop the FFB queues of the specified vJoy Device.

																			  // Added in 2.1.6
	VGENINTERFACE_API BOOL		__cdecl	IsDeviceFfb(UINT rID);
	VGENINTERFACE_API BOOL		__cdecl	IsDeviceFfbEffect(UINT rID, UINT Effect);

	//  Force Feedback (FFB) helper functions
	VGENINTERFACE_API DWORD 	__cdecl	Ffb_h_DeviceID(const FFB_DATA * Packet, int *DeviceID);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_Type(const FFB_DATA * Packet, FFBPType *Type);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_Packet(const FFB_DATA * Packet, WORD *Type, int *DataSize, BYTE *Data[]);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_EBI(const FFB_DATA * Packet, int *Index);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_Eff_Report(const FFB_DATA * Packet, FFB_EFF_REPORT*  Effect);
	//__declspec(deprecated("** Ffb_h_Eff_Const function was deprecated - Use function Ffb_h_Eff_Report **")) \
	//VGENINTERFACE_API DWORD 	__cdecl Ffb_h_Eff_Const(const FFB_DATA * Packet, FFB_EFF_REPORT*  Effect);
	VGENINTERFACE_API DWORD		__cdecl Ffb_h_Eff_Ramp(const FFB_DATA * Packet, FFB_EFF_RAMP*  RampEffect);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_EffOp(const FFB_DATA * Packet, FFB_EFF_OP*  Operation);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_DevCtrl(const FFB_DATA * Packet, FFB_CTRL *  Control);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_Eff_Period(const FFB_DATA * Packet, FFB_EFF_PERIOD*  Effect);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_Eff_Cond(const FFB_DATA * Packet, FFB_EFF_COND*  Condition);
	VGENINTERFACE_API DWORD 	__cdecl Ffb_h_DevGain(const FFB_DATA * Packet, BYTE * Gain);
	VGENINTERFACE_API DWORD		__cdecl Ffb_h_Eff_Envlp(const FFB_DATA * Packet, FFB_EFF_ENVLP*  Envelope);
	VGENINTERFACE_API DWORD		__cdecl Ffb_h_EffNew(const FFB_DATA * Packet, FFBEType * Effect);

	// Added in 2.1.6
	VGENINTERFACE_API DWORD		__cdecl Ffb_h_Eff_Constant(const FFB_DATA * Packet, FFB_EFF_CONSTANT *  ConstantEffect);
#pragma endregion

#pragma region vXbox API

	//////////////////////////////////////////////////////////////////////////////////////
	/// 
	///  vXbox interface fuctions
	///
	///  Device range: 1-4 (Not necessarily related to Led number)
	///
	//////////////////////////////////////////////////////////////////////////////////////

	// Virtual vXbox bus information
	VGENINTERFACE_API	DWORD		__cdecl isVBusExist(void);
	VGENINTERFACE_API	DWORD		__cdecl GetNumEmptyBusSlots(UCHAR * nSlots);

	// Device Status (Plugin/Unplug and check ownership)
	VGENINTERFACE_API	DWORD		__cdecl isControllerPluggedIn(UINT UserIndex, PBOOL Exist);
	VGENINTERFACE_API	DWORD		__cdecl isControllerOwned(UINT UserIndex, PBOOL Exist);
	VGENINTERFACE_API	DWORD		__cdecl PlugIn(UINT UserIndex);
	VGENINTERFACE_API	DWORD		__cdecl PlugInNext(UINT * UserIndex);
	VGENINTERFACE_API	DWORD		__cdecl UnPlug(UINT UserIndex);
	VGENINTERFACE_API	DWORD		__cdecl UnPlugForce(UINT UserIndex);

	// Reset Devices
	VGENINTERFACE_API	DWORD		__cdecl ResetController(UINT UserIndex);
	VGENINTERFACE_API	DWORD		__cdecl ResetAllControllers();
#ifdef SPECIFICRESET
	VGENINTERFACE_API	DWORD		__cdecl ResetControllerBtns(UINT UserIndex);
	VGENINTERFACE_API	DWORD		__cdecl ResetControllerDPad(UINT UserIndex);

#endif // SPECIFICRESET

	// Button functions: Per-button Press/Release
	VGENINTERFACE_API	DWORD		__cdecl SetButton(UINT UserIndex, WORD Button, BOOL Press);
#ifdef SPECIFICBUTTONS
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

#endif // SPECIFICBUTTONS

	// Trigger/Axis functions: Set value in the range
	VGENINTERFACE_API	DWORD		__cdecl SetTriggerL(UINT UserIndex, BYTE Value);
	VGENINTERFACE_API	DWORD		__cdecl SetTriggerR(UINT UserIndex, BYTE Value);
	VGENINTERFACE_API	DWORD		__cdecl	SetAxisLx(UINT UserIndex, SHORT Value); // Left Stick X
	VGENINTERFACE_API	DWORD		__cdecl	SetAxisLy(UINT UserIndex, SHORT Value); // Left Stick Y
	VGENINTERFACE_API	DWORD		__cdecl	SetAxisRx(UINT UserIndex, SHORT Value); // Right Stick X
	VGENINTERFACE_API	DWORD		__cdecl	SetAxisRy(UINT UserIndex, SHORT Value); // Right Stick Y

	// DPAD Functions
	VGENINTERFACE_API	DWORD		__cdecl	SetDpad(UINT UserIndex, UCHAR Value); // DPAD Set Value
#ifdef SPECIFICBUTTONS
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadUp(UINT UserIndex); // DPAD Up
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadRight(UINT UserIndex); // DPAD Right
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadDown(UINT UserIndex); // DPAD Down
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadLeft(UINT UserIndex); // DPAD Left
	VGENINTERFACE_API	BOOL		__cdecl	SetDpadOff(UINT UserIndex); // DPAD Off
#endif // SPECIFICBUTTONS

	// Feedback Polling: Assigned Led number / Vibration values
	VGENINTERFACE_API	DWORD		__cdecl	GetLedNumber(UINT UserIndex, PBYTE pLed);
	VGENINTERFACE_API	DWORD		__cdecl	GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib);
#pragma endregion

#pragma region Common API
	// Device Administration, Manipulation and Information
	VGENINTERFACE_API DWORD 		__cdecl	AcquireDev(UINT DevId, DevType dType, HDEVICE * hDev);	// Acquire a Device.
	VGENINTERFACE_API DWORD			__cdecl	RelinquishDev(HDEVICE hDev);			// Relinquish a Device.
	VGENINTERFACE_API DWORD			__cdecl GetDevType(HDEVICE hDev, DevType * dType);	// Get device type (vJoy/vXbox)
	VGENINTERFACE_API DWORD			__cdecl GetDevNumber(HDEVICE hDev, UINT * dNumber);	// If vJoy: Number=Id; If vXbox: Number=Led#
	VGENINTERFACE_API DWORD			__cdecl GetDevId(HDEVICE hDev, UINT * dID);					// Return Device ID to be used with vXbox API and Backward compatibility API
	VGENINTERFACE_API DWORD			__cdecl isDevOwned(UINT DevId, DevType dType, BOOL * Owned);	// Is device plugged-in/Configured by this feeder
	VGENINTERFACE_API DWORD			__cdecl isDevExist(UINT DevId, DevType dType, BOOL * Exist);	// Is device plugged-in/Configured
	VGENINTERFACE_API DWORD			__cdecl isDevFree(UINT DevId, DevType dType, BOOL * Free);	// Is device unplugged/Free
	VGENINTERFACE_API DWORD			__cdecl	GetDevHandle(UINT DevId, DevType dType, HDEVICE * hDev);// Return device handle from Device ID and Device type
	VGENINTERFACE_API DWORD			__cdecl isAxisExist(HDEVICE hDev, UINT nAxis, BOOL * Exist);	// Does Axis exist. See above table
	VGENINTERFACE_API DWORD			__cdecl GetDevButtonN(HDEVICE hDev, UINT * nBtn);			// Get number of buttons in device
	VGENINTERFACE_API DWORD			__cdecl GetDevHatN(HDEVICE hDev, UINT * nHat);				// Get number of Hat Switches in device

	// Position Setting
	VGENINTERFACE_API DWORD			__cdecl SetDevButton(HDEVICE hDev, UINT Button, BOOL Press);
	VGENINTERFACE_API DWORD			__cdecl SetDevAxis(HDEVICE hDev, UINT Axis, FLOAT Value);
	VGENINTERFACE_API DWORD			__cdecl SetDevPov(HDEVICE hDev, UINT nPov, FLOAT Value);
#pragma endregion
} // extern "C"