#pragma once
using namespace vJoy;
class CvJoyDevice
{
public:
	CvJoyDevice();
	CvJoyDevice(int);
	virtual ~CvJoyDevice();

private:
	VjdStat	status;
	int ButtonNumber, DiscPovNumber, ContPovNumber;
	BOOL X_Exist, Y_Exist, Z_Exist, RX_Exist, RY_Exist, RZ_Exist, SL0_Exist, SL1_Exist;
protected:
	// Get current status of the vJoy device and initialize status variables accordingly
	BOOL GetStatus(int id);
public:
	// Tests if vJoy device exists (Input is ID number)
	BOOL Exists();

	// Tests if axis implemented exists (Input is ID number)
	BOOL AxisExists(UINT Axis);

	// Get the number of buttons
	int GetButtonNumber(void);

	// Get the number of discrete POV buttons
	int GetDiscPovNumber(void);

	// Get the number of Continuous POV buttons
	int GetContPovNumber(void);

protected:
	// ID of this vJoy device
	int Id;
};

