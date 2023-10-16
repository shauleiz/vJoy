#include "stdafx.h"
#include "Public.h"
#include "vJoyInterface.h"
#include "vJoyConf.h"
#include "vJoyDevice.h"


CvJoyDevice::CvJoyDevice() :
    status(VJD_STAT_UNKN),
    ButtonNumber(0), DiscPovNumber(0), ContPovNumber(0),
    X_Exist(FALSE), Y_Exist(FALSE), Z_Exist(FALSE), RX_Exist(FALSE), RY_Exist(FALSE), RZ_Exist(FALSE), SL0_Exist(FALSE), SL1_Exist(FALSE),
    Wheel_Exist(FALSE), Accelerator_Exist(FALSE), Brake_Exist(FALSE), Clutch_Exist(FALSE), Steering_Exist(FALSE), Throttle_Exist(FALSE), Rudder_Exist(FALSE), Aileron_Exist(FALSE)
    , Id(0)
{
    CvJoyDevice(1);
}

CvJoyDevice::CvJoyDevice(int id) :
    status(VJD_STAT_UNKN),
    ButtonNumber(0), DiscPovNumber(0), ContPovNumber(0),
    X_Exist(FALSE), Y_Exist(FALSE), Z_Exist(FALSE), RX_Exist(FALSE), RY_Exist(FALSE), RZ_Exist(FALSE), SL0_Exist(FALSE), SL1_Exist(FALSE),
    Wheel_Exist(FALSE), Accelerator_Exist(FALSE), Brake_Exist(FALSE), Clutch_Exist(FALSE), Steering_Exist(FALSE), Throttle_Exist(FALSE), Rudder_Exist(FALSE), Aileron_Exist(FALSE)
{

    if (id <= 0 || id > VJOY_MAX_N_DEVICES)
        Id = 0;
    else
        Id = id;

}


CvJoyDevice::~CvJoyDevice()
{
}


// Get current status of the vJoy device and initialize status variables accordingly
BOOL CvJoyDevice::GetStatus(int id)
{
    if (id <= 0 || id > VJOY_MAX_N_DEVICES)
        return FALSE;

    status = GetVJDStatus(id);
    if (status != VJD_STAT_UNKN && status != VJD_STAT_MISS) {
        ButtonNumber = GetVJDButtonNumber(id);	// Get the number of buttons defined in the specified VDJ
        DiscPovNumber = GetVJDDiscPovNumber(id);	// Get the number of descrete-type POV hats defined in the specified VDJ
        ContPovNumber = GetVJDContPovNumber(id);	// Get the number of descrete-type POV hats defined in the specified VDJ
        X_Exist = GetVJDAxisExist(id, HID_USAGE_X); // Test if given axis defined in the specified VDJ
        Y_Exist = GetVJDAxisExist(id, HID_USAGE_Y); // Test if given axis defined in the specified VDJ
        Z_Exist = GetVJDAxisExist(id, HID_USAGE_Z); // Test if given axis defined in the specified VDJ
        RX_Exist = GetVJDAxisExist(id, HID_USAGE_RX); // Test if given axis defined in the specified VDJ
        RY_Exist = GetVJDAxisExist(id, HID_USAGE_RY); // Test if given axis defined in the specified VDJ
        RZ_Exist = GetVJDAxisExist(id, HID_USAGE_RZ); // Test if given axis defined in the specified VDJ
        SL0_Exist = GetVJDAxisExist(id, HID_USAGE_SL0); // Test if given axis defined in the specified VDJ
        SL1_Exist = GetVJDAxisExist(id, HID_USAGE_SL1); // Test if given axis defined in the specified VDJ
       
        Wheel_Exist = GetVJDAxisExist(id, HID_USAGE_WHL);
        Accelerator_Exist = GetVJDAxisExist(id, HID_USAGE_ACCELERATOR);
        Brake_Exist = GetVJDAxisExist(id, HID_USAGE_BRAKE);
        Clutch_Exist = GetVJDAxisExist(id, HID_USAGE_CLUTCH);
        Steering_Exist = GetVJDAxisExist(id, HID_USAGE_STEERING);
        Throttle_Exist = GetVJDAxisExist(id, HID_USAGE_THROTTLE);
        Rudder_Exist = GetVJDAxisExist(id, HID_USAGE_RUDDER);
        Aileron_Exist = GetVJDAxisExist(id, HID_USAGE_AILERON);
        return TRUE;
    };
    return FALSE;
}


// Tests if vJoy device exists (Input is ID number)
BOOL CvJoyDevice::Exists()
{
    if (Id <= 0 || Id > VJOY_MAX_N_DEVICES)
        return FALSE;

    status = GetVJDStatus(Id);
    if (status != VJD_STAT_UNKN && status != VJD_STAT_MISS)
        return TRUE;
    else
        return FALSE;

    //return GetStatus(Id);
}

// Tests if axis implemented exists (Input is ID number)
BOOL CvJoyDevice::AxisExists(UINT Axis)
{
    return  (GetVJDAxisExist(Id, Axis)==TRUE);
}

// Get the number of buttons
int CvJoyDevice::GetButtonNumber(void)
{
    int n = GetVJDButtonNumber(Id);
    if (n < 0)
        n = 0;
    return n;
}

// Get the number of discrete POV buttons
int CvJoyDevice::GetDiscPovNumber(void)
{
    int n = GetVJDDiscPovNumber(Id);
    if (n < 0)
        n = 0;
    return n;
}

// Get the number of Continuous POV buttons
int CvJoyDevice::GetContPovNumber(void)
{
    int n = GetVJDContPovNumber(Id);
    if (n < 0)
        n = 0;
    return n;
}