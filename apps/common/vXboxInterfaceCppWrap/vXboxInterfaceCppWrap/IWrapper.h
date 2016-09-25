//// IWrapper.h
#pragma once
#include <Xinput.h>
#include "../../vjoyinterface/vjoyinterface.h"
#include <valarray>

using namespace std;
namespace vXbox {

/***
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

	XINPUT defined constants
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


		//
		// Gamepad thresholds
		//
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

*/

	public ref class IWrapper
	{
	public:
		// HID8USAGE enum
		static const unsigned int hid_X = HID_USAGE_X,
			hid_Y = HID_USAGE_Y,
			hid_Z = HID_USAGE_Z,
			hid_RX = HID_USAGE_RX,
			hid_RY = HID_USAGE_RY,
			hid_RZ = HID_USAGE_RZ,
			hid_SL0 = HID_USAGE_SL0,
			hid_SL1 = HID_USAGE_SL1,
			hid_WHL = HID_USAGE_WHL,
			hid_POV = HID_USAGE_POV;
		// XINPUT Deadzone and Threshold
		static const unsigned int xinput_LAXIS_DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
			xinput_RAXIS_DEADZONE = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
			xinput_TRIGGER_THRESHOLD = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
		static const unsigned int xinput_DPAD_UP = XINPUT_GAMEPAD_DPAD_UP, 
			xinput_DPAD_DOWN = XINPUT_GAMEPAD_DPAD_DOWN, 
			xinput_DPAD_LEFT = XINPUT_GAMEPAD_DPAD_LEFT, 
			xinput_DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT, 
			xinput_DPAD_OFF = 0;
		static const unsigned int xinput_GAMEPAD_A = XINPUT_GAMEPAD_A,
			xinput_GAMEPAD_B = XINPUT_GAMEPAD_B,
			xinput_GAMEPAD_X = XINPUT_GAMEPAD_X,
			xinput_GAMEPAD_Y = XINPUT_GAMEPAD_Y,
			xinput_GAMEPAD_BACK = XINPUT_GAMEPAD_BACK,
			xinput_GAMEPAD_START = XINPUT_GAMEPAD_START,
			xinput_GAMEPAD_LEFT_SHOULDER = XINPUT_GAMEPAD_LEFT_SHOULDER,
			xinput_GAMEPAD_RIGHT_SHOULDER = XINPUT_GAMEPAD_RIGHT_SHOULDER,
			xinput_GAMEPAD_LEFT_THUMB = XINPUT_GAMEPAD_LEFT_THUMB,
			xinput_GAMEPAD_RIGHT_THUMB = XINPUT_GAMEPAD_RIGHT_THUMB;
			// VjdStat
		static const int VJD_STAT_BUSY = vJoy::VJD_STAT_BUSY,
			VJD_STAT_FREE = vJoy::VJD_STAT_FREE,
			VJD_STAT_MISS = vJoy::VJD_STAT_MISS,
			VJD_STAT_OWN = vJoy::VJD_STAT_OWN,
			VJD_STAT_UNKN = vJoy::VJD_STAT_UNKN;
		static valarray<int> getVjdStat() {
			valarray <int> all = { VJD_STAT_BUSY,VJD_STAT_FREE,VJD_STAT_MISS, VJD_STAT_OWN, VJD_STAT_UNKN };
			return all;
		}
		/// Status
		bool isVBusExists();
		bool GetNumEmptyBusSlots(UCHAR * nSlots);
		bool isControllerExists(UINT UserIndex);
		bool isControllerOwned(UINT UserIndex);


		// Virtual device Plug-In/Unplug
		bool PlugIn(UINT UserIndex);
		bool UnPlug(UINT UserIndex);
		bool UnPlugForce(UINT UserIndex);

		// Data Transfer (Data to the device)
		bool SetBtnAny(UINT UserIndex, bool Press, UINT xBtn);
		bool SetBtnA(UINT UserIndex, bool Press);
		bool SetBtnB(UINT UserIndex, bool Press);
		bool SetBtnX(UINT UserIndex, bool Press);
		bool SetBtnY(UINT UserIndex, bool Press);
		bool SetBtnStart(UINT UserIndex, bool Press);
		bool SetBtnBack(UINT UserIndex, bool Press);
		bool SetBtnLT(UINT UserIndex, bool Press); // Left Thumb/Stick
		bool SetBtnRT(UINT UserIndex, bool Press); // Right Thumb/Stick
		bool SetBtnLB(UINT UserIndex, bool Press); // Left Bumper
		bool SetBtnRB(UINT UserIndex, bool Press); // Right Bumper
		bool SetTriggerL(UINT UserIndex, BYTE Value, UINT Threshold); // Left Trigger
		bool SetDpad(UINT UserIndex, UINT Value);
		bool SetTriggerR(UINT UserIndex, BYTE Value, UINT Threshold); // Right Trigger
		bool SetDpadUp(UINT UserIndex);
		bool SetDpadRight(UINT UserIndex);
		bool SetDpadDown(UINT UserIndex);
		bool SetDpadLeft(UINT UserIndex);
		bool SetDpadUpRight(UINT UserIndex);
		bool SetDpadDownRight(UINT UserIndex);
		bool SetDpadDownLeft(UINT UserIndex);
		bool SetDpadUpLeft(UINT UserIndex);
		bool SetDpadOff(UINT UserIndex);

		// Data Transfer (Feedback from the device)
		bool GetLedNumber(UINT UserIndex, PBYTE pLed);
		bool GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib);

#pragma region Legacy Interface Functions
		/////	General driver data
		SHORT GetvJoyVersion(void);
		bool vJoyEnabled(void);
		PVOID GetvJoyProductString(void);
		PVOID GetvJoyManufacturerString(void);
		PVOID GetvJoySerialNumberString(void);
		//BOOL	 	DriverMatch(WORD * DllVer, WORD * DrvVer);
		//VOID	 	RegisterRemovalCB(RemovalCB cb, PVOID data);
		bool vJoyFfbCap(bool * Supported);
		bool GetvJoyMaxDevices(int * n);
		bool GetNumberExistingVJD(int * n);

		/////	vJoy Device properties
		int GetVJDButtonNumber(UINT rID);
		int GetVJDDiscPovNumber(UINT rID);
		int GetVJDContPovNumber(UINT rID);
		bool GetVJDAxisExist(UINT rID, UINT Axis);
		bool GetVJDAxisMax(UINT rID, UINT Axis, LONG * Max);
		bool GetVJDAxisMin(UINT rID, UINT Axis, LONG * Min);
		int GetVJDStatus(UINT rID);
		bool isVJDExists(UINT rID);

		/////	Write access to vJoy Device - Basic

		bool AcquireVJD(UINT rID);
		VOID RelinquishVJD(UINT rID);


		//// Reset functions
		bool ResetVJD(UINT rID);
		VOID ResetAll(void);
		bool ResetButtons(UINT rID);
		bool ResetPovs(UINT rID);

		// Write data
		bool UpdateVJD(UINT rID, PVOID pData);
		bool SetBtn(bool Value, UINT rID, UCHAR nBtn);
		bool SetDiscPov(int Value, UINT rID, UCHAR nPov);
		bool SetContPov(DWORD Value, UINT rID, UCHAR nPov);

		bool SetAxisXY(UINT UserIndex, SHORT ValueX, SHORT ValueY, UINT AxisX, UINT AxisY, UINT DeadZone,SHORT Axis_Max);
		// Read data
		bool GetAxisXY(UINT UserIndex, PSHORT ValueX, PSHORT ValueY, UINT AxisX, UINT AxisY);
		bool GetTriggerLR(UINT UserIndex, PBYTE ValueL, PBYTE ValueR);
		bool SetTriggerLR(UINT UserIndex, SHORT ValueL, SHORT ValueR, UINT Threshold);
#pragma endregion
	};
}