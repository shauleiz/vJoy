#pragma once
#include "Stdafx.h"

#include "IWrapper.h"

#include  "../../vjoyinterface/vXboxInterface.h"

using namespace std;

using namespace vXbox;
inline bool vXbox::IWrapper::isVBusExists(void) {
	return ::isVBusExists() == TRUE;
}
inline bool vXbox::IWrapper::GetNumEmptyBusSlots(UCHAR * nSlots) {
	return ::GetNumEmptyBusSlots(nSlots) == TRUE;
}
inline bool vXbox::IWrapper::isControllerExists(UINT UserIndex) {
	return ::isControllerExists(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::isControllerOwned(UINT UserIndex) {
	return ::isControllerOwned(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::PlugIn(UINT UserIndex) {
	return ::PlugIn(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::UnPlug(UINT UserIndex) {
	return ::UnPlug(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::UnPlugForce(UINT UserIndex) {
	return ::UnPlugForce(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpad(UINT UserIndex, UINT Value) {
	return ::SetDpad(UserIndex, Value) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadUp(UINT UserIndex) {
	return ::SetDpadUp(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadRight(UINT UserIndex) {
	return ::SetDpadRight(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadDown(UINT UserIndex) {
	return ::SetDpadDown(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadLeft(UINT UserIndex) {
	return ::SetDpadLeft(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadUpRight(UINT UserIndex) {
	return ::SetDpadUpRight(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadDownRight(UINT UserIndex) {
	return ::SetDpadDownRight(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadDownLeft(UINT UserIndex) {
	return ::SetDpadDownLeft(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadUpLeft(UINT UserIndex) {
	return ::SetDpadUpLeft(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetDpadOff(UINT UserIndex) {
	return ::SetDpadOff(UserIndex) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnAny(UINT UserIndex, bool Press, UINT xBtn) {
	return ::SetBtnAny(UserIndex, (Press ? TRUE : FALSE), xBtn) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnA(UINT UserIndex, bool Press) {
	return ::SetBtnA(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnB(UINT UserIndex, bool Press) {
	return ::SetBtnB(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnX(UINT UserIndex, bool Press) {
	return ::SetBtnX(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnY(UINT UserIndex, bool Press) {
	return ::SetBtnY(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnStart(UINT UserIndex, bool Press) {
	return ::SetBtnStart(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnBack(UINT UserIndex, bool Press) {
	return ::SetBtnBack(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnLT(UINT UserIndex, bool Press) {
	return ::SetBtnLT(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnRT(UINT UserIndex, bool Press) {
	return ::SetBtnRT(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnLB(UINT UserIndex, bool Press) {
	return ::SetBtnLB(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetBtnRB(UINT UserIndex, bool Press) {
	return ::SetBtnRB(UserIndex, (Press ? TRUE : FALSE)) == TRUE;
}
inline bool vXbox::IWrapper::SetTriggerL(UINT UserIndex, BYTE Value, UINT Threshold) {
	return ::SetTriggerL(UserIndex, Value, Threshold) == TRUE;
}
inline bool vXbox::IWrapper::SetTriggerR(UINT UserIndex, BYTE Value, UINT Threshold) {
	return ::SetTriggerR(UserIndex, Value, Threshold) == TRUE;
}
inline bool vXbox::IWrapper::GetTriggerLR(UINT UserIndex, PBYTE ValueL, PBYTE ValueR) {
	return ::GetTriggerLR(UserIndex, ValueL, ValueR, HID_USAGE_SL1, HID_USAGE_SL1);
}
inline bool vXbox::IWrapper::GetLedNumber(UINT UserIndex, PBYTE pLed) {
	return ::GetLedNumber(UserIndex, pLed) == TRUE;
}

inline bool vXbox::IWrapper::GetVibration(UINT UserIndex, PXINPUT_VIBRATION pVib) {
	return ::GetVibration(UserIndex, pVib) == TRUE;
}


#pragma region Legacy Interface Functions
/////	General driver data
inline SHORT vXbox::IWrapper::GetvJoyVersion(void) {
	return ::GetvJoyVersion();
}

inline bool vXbox::IWrapper::vJoyEnabled(void) {
	return ::vJoyEnabled() == TRUE;
}
inline PVOID vXbox::IWrapper::GetvJoyProductString(void) {
	return ::GetvJoyProductString();
}
inline PVOID vXbox::IWrapper::GetvJoyManufacturerString(void) {
	return ::GetvJoyManufacturerString();
}
inline PVOID vXbox::IWrapper::GetvJoySerialNumberString(void) {
	return ::GetvJoySerialNumberString();
}
//BOOL	 	DriverMatch(WORD * DllVer, WORD * DrvVer);
//VOID	 	RegisterRemovalCB(RemovalCB cb, PVOID data);
bool vXbox::IWrapper::vJoyFfbCap(bool * Supported) {
	BOOL s = Supported ? TRUE : FALSE;
	return ::vJoyFfbCap(&s) == TRUE;
}
inline bool vXbox::IWrapper::GetvJoyMaxDevices(int * n) {
	return ::GetvJoyMaxDevices(n) == TRUE;
}
inline bool vXbox::IWrapper::GetNumberExistingVJD(int * n) {
	return ::GetNumberExistingVJD(n) == TRUE;
}

/////	vJoy Device properties
inline int vXbox::IWrapper::GetVJDButtonNumber(UINT rID) {
	return ::GetVJDButtonNumber(rID);
}
inline int vXbox::IWrapper::GetVJDDiscPovNumber(UINT rID) {
	return ::GetVJDDiscPovNumber(rID);
}
inline int vXbox::IWrapper::GetVJDContPovNumber(UINT rID) {
	return ::GetVJDContPovNumber(rID);
}
inline bool vXbox::IWrapper::GetVJDAxisExist(UINT rID, UINT Axis) {
	return ::GetVJDAxisExist(rID, Axis) == TRUE;
}
inline bool vXbox::IWrapper::GetVJDAxisMax(UINT rID, UINT Axis, LONG * Max) {
	return ::GetVJDAxisMax(rID, Axis, Max) == TRUE;
}
inline bool vXbox::IWrapper::GetVJDAxisMin(UINT rID, UINT Axis, LONG * Min) {
	return ::GetVJDAxisMin(rID, Axis, Min) == TRUE;
}
int vXbox::IWrapper::GetVJDStatus(UINT rID) {
	valarray <int> all = vXbox::IWrapper::getVjdStat();
	// return value match with "CLR" enum
	vJoy::VjdStat ret = ::GetVJDStatus(rID);
	for (int i = 0; i < all.size(); i++) {
		if (all[i] == ret)
			return all[i];
	};
	return VJD_STAT_UNKN;
}

inline bool vXbox::IWrapper::isVJDExists(UINT rID) {
	return ::isVJDExists(rID) == TRUE;
}

/////	Write access to vJoy Device - Basic
inline bool vXbox::IWrapper::AcquireVJD(UINT rID) {
	return ::AcquireVJD(rID) == TRUE;
}


inline VOID  vXbox::IWrapper::RelinquishVJD(UINT rID) {
	::RelinquishVJD(rID);
}

//// Reset functions
inline bool vXbox::IWrapper::ResetVJD(UINT rID) {
	return ::ResetVJD(rID) == TRUE;
}
inline VOID vXbox::IWrapper::ResetAll(void) {
	::ResetAll();
}
inline bool vXbox::IWrapper::ResetButtons(UINT rID) {
	return ::ResetButtons(rID) == TRUE;
}
inline bool vXbox::IWrapper::ResetPovs(UINT rID) {
	return ::ResetPovs(rID) == TRUE;
}

// Write data
inline bool vXbox::IWrapper::UpdateVJD(UINT rID, PVOID pData) {
	return ::UpdateVJD(rID, pData) == TRUE;
}
inline bool vXbox::IWrapper::SetTriggerLR(UINT UserIndex, SHORT ValueL, SHORT ValueR, UINT Threshold) {
	return ::SetTriggerLR(UserIndex, ValueL, ValueR, Threshold);
}

inline bool vXbox::IWrapper::SetAxisXY(UINT UserIndex, SHORT ValueX, SHORT ValueY, UINT AxisX, UINT AxisY, UINT DeadZone, SHORT Axis_Max) {
	return ::SetAxisXY(UserIndex, ValueX, ValueY, AxisX, AxisY, DeadZone, Axis_Max);
}
inline bool vXbox::IWrapper::GetAxisXY(UINT UserIndex, PSHORT ValueX, PSHORT ValueY, UINT AxisX, UINT AxisY) {
	return ::GetAxisXY(UserIndex, ValueX, ValueY, AxisX, AxisY);
}
inline bool vXbox::IWrapper::SetBtn(bool Value, UINT rID, UCHAR nBtn) {
	return ::SetBtn((Value ? TRUE : FALSE), rID, nBtn) == TRUE;
}
inline bool vXbox::IWrapper::SetDiscPov(int Value, UINT rID, UCHAR nPov) {
	return ::SetDiscPov(Value, rID, nPov) == TRUE;
}
inline bool vXbox::IWrapper::SetContPov(DWORD Value, UINT rID, UCHAR nPov) {
	return ::SetContPov(Value, rID, nPov) == TRUE;
}
#pragma endregion