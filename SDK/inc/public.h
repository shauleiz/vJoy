/*++

Copyright (c) Shaul Eizikovich.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    public.h

Abstract:

    Public header file for the vJoy project
    Developpers that need to interface with vJoy need to include this file

Author:


Environment:

    kernel mode and User mode

Notes:


Revision History:


--*/
#ifndef _PUBLIC_H
#define _PUBLIC_H
#pragma once

// Compilation directives
#define PPJOY_MODE
#undef PPJOY_MODE	// Comment-out for compatibility mode

#ifdef PPJOY_MODE
#include "PPJIoctl.h"
#endif

#include <INITGUID.H>	// Definitions for controlling GUID initialization

// Sideband comunication with vJoy Device
//{781EF630-72B2-11d2-B852-00C04FAD5101}
DEFINE_GUID(GUID_DEVINTERFACE_VJOY, 0x781EF630, 0x72B2, 0x11d2, 0xB8, 0x52, 0x00, 0xC0, 0x4F, 0xAD, 0x51, 0x01);

//
// Usage example:
//		CreateFile(TEXT("\\\\.\\vJoy"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
#ifdef PPJOY_MODE
#define DEVICENAME_STRING			"PPJoyIOCTL1"
#else
#define DEVICENAME_STRING			"vJoy"
#endif
#define NTDEVICE_NAME_STRING		"\\Device\\"DEVICENAME_STRING
#define SYMBOLIC_NAME_STRING		"\\DosDevices\\"DEVICENAME_STRING
#define	DOS_FILE_NAME				"\\\\.\\"DEVICENAME_STRING
#define VJOY_INTERFACE				L"Device_"

// Use auto-generated version file from inc/
#include "gen-versioninfo.h"
// This is a hardcoded version, not to be confused with whole {installer+libs+SDK} that
// use getversion to retrieve a 'git describe' (tag) version.
// Version parts as a serie of digits (from higher (X) significant number to lower (L))
// Will be taken from CreateVersion.exe in gen-versioninfo.h
#ifndef VER_X_
#define VER_X_	2 // Must be within 0..0XF
#define VER_H_	2 // Must be within 0..0XF
#define VER_M_	1 // Must be within 0..0XF
#define VER_L_	0 // Must be within 0..0XF
#endif


// Use to select which level of API is being used by the driver : 1 2 or 3
// See JOYSTICK_POSITION, JOYSTICK_POSITION_V2, JOYSTICK_POSITION_V3
#define USE_JOYSTICK_API_VERSION 3

#ifndef USE_JOYSTICK_API_VERSION
#error "Must select an API version!"
#endif // !USE_JOYSTICK_API_VERSION


// Select whether driver has support for FFB
#define VJOY_HAS_FFB
// For FFB, Look for Registrys Keys to cleanup: 
// HKEY_CURRENT_USER\\SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_1234& PID_0FFB"
// HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\MediaProperties\PrivateProperties\Joystick\OEM\VID_1234&PID_0FFB
// HKEY_LOCAL_MACHINE\SYSTEM\Setup\Upgrade\PnP\CurrentControlSet\Control\DeviceMigration\Devices\HID\HIDCLASS\*
// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Setup\PnpResources\Registry\HKLM\SYSTEM\CurrentControlSet\Control\MediaProperties\PrivateProperties\Joystick\OEM
// HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Setup\PnpResources\Registry\HKLM\SYSTEM\CurrentControlSet\Control\MediaProperties\PrivateProperties\Joystick\OEM\VID_1234&PID_1FFB

// For FFB: when bellow flag is enabled, a first Effect Block Index is always allocated,
// even if there is no feeder.
// This is to avoid some games being locked if a feeder with FFB was not started.
// This also enables RAM Pool emulation.
#define FIRST_EID_PREALLOCATED


#define STRINGIFY_1(x)   #x
#define STRINGIFY(x)     STRINGIFY_1(x)
#define PASTE(x, y) x##y
#define MAKEWIDE(x) PASTE(L,x)

// Device Attributes
// Vendor ID
#define VENDOR_N_ID		0x1234
// Product ID changes if FFB is enabled or not (so we will have 2 different products and drivers)
#ifdef VJOY_HAS_FFB
// Product ID: FFB with registry keys in INF file
#define	PRODUCT_N_ID	0xBEAD
// Product ID: FFB without registry keys in INF file
//#define	PRODUCT_N_ID	0x1FFB
#else
#define	PRODUCT_N_ID	0x0FFB
#endif

// Complete driver version on 16bits for HID and driver: v0.0.0.0 (4 nibbles)
// To avoid bugfix numbering to be reported in vjoy.inx file each time we have a new driver, 
// just leave the 4th number off.
// /!\ This number must matched what is written in the vjoy.inx file (root\VID_1234&PID_BEAD&REV_0XHM)
#define	VERSION_N	(0x100*VER_X_ + 0x10*VER_H_ + VER_M_)
// In case we want to move to 4digits, use following and change vjoy.inx accordingly
//#define	VERSION_N	(0x1000*VER_X_ + 0x100*VER_H_ + 0*010*VER_M_ + VER_L_)

// Device Strings
//
#define VENDOR_STR_ID		L"Shaul Eizikovich"
#define PRODUCT_STR_ID		L"vJoy - Virtual Joystick"
#define	SERIALNUMBER_STR	MAKEWIDE(STRINGIFY(VER_X_)) L"." MAKEWIDE(STRINGIFY(VER_H_)) L"." MAKEWIDE(STRINGIFY(VER_M_)) L"."  MAKEWIDE(STRINGIFY(VER_L_))

// Function codes;
//#define LOAD_POSITIONS	0x910
//#define GETATTRIB		0x911
// #define GET_FFB_DATA	0x00222912	// METHOD_OUT_DIRECT + FILE_DEVICE_UNKNOWN	+ FILE_ANY_ACCESS
//#define SET_FFB_STAT	0x913	 // METHOD_NEITHER
//#define GET_FFB_STAT	0x916

#define F_LOAD_POSITIONS	0x910
#define F_GETATTRIB			0x911
#define F_GET_FFB_DATA		0x912
#define F_SET_FFB_STAT		0x913
#define F_GET_FFB_PID_DATA	0x914
#define F_SET_FFB_PID_DATA	0x915
#define F_GET_FFB_STAT		0x916
#define F_GET_DEV_INFO      0x917
#define F_IS_DRV_FFB_CAP	0x918
#define F_IS_DRV_FFB_EN		0x919
#define F_GET_DRV_DEV_MAX	0x91A
#define F_GET_DRV_DEV_EN	0x91B
#define F_IS_DEV_FFB_START	0x91C
#define F_GET_DEV_STAT		0x91D
#define F_GET_DRV_INFO		0x91E
#define F_RESET_DEV			0x91F
#define F_GET_POSITIONS		0x920

// IO Device Control codes;
#define IOCTL_VJOY_GET_ATTRIB       CTL_CODE (FILE_DEVICE_UNKNOWN, GETATTRIB, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define LOAD_POSITIONS              CTL_CODE (FILE_DEVICE_UNKNOWN, F_LOAD_POSITIONS, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define GET_POSITIONS               CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_POSITIONS, METHOD_BUFFERED, FILE_READ_ACCESS)
#define GET_FFB_DATA                CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_FFB_DATA, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define GET_FFB_PID_DATA            CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_FFB_PID_DATA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SET_FFB_PID_DATA            CTL_CODE (FILE_DEVICE_UNKNOWN, F_SET_FFB_PID_DATA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SET_FFB_STAT                CTL_CODE (FILE_DEVICE_UNKNOWN, F_SET_FFB_STAT, METHOD_NEITHER, FILE_ANY_ACCESS)
#define GET_FFB_STAT                CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_FFB_STAT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GET_DEV_INFO                CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_DEV_INFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IS_DRV_FFB_CAP              CTL_CODE (FILE_DEVICE_UNKNOWN, F_IS_DRV_FFB_CAP, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IS_DRV_FFB_EN               CTL_CODE (FILE_DEVICE_UNKNOWN, F_IS_DRV_FFB_EN, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GET_DRV_DEV_MAX             CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_DRV_DEV_MAX, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GET_DRV_DEV_EN              CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_DRV_DEV_EN, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IS_DEV_FFB_START            CTL_CODE (FILE_DEVICE_UNKNOWN, F_IS_DEV_FFB_START, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GET_DEV_STAT                CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_DEV_STAT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GET_DRV_INFO                CTL_CODE (FILE_DEVICE_UNKNOWN, F_GET_DRV_INFO, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define RESET_DEV                   CTL_CODE (FILE_DEVICE_UNKNOWN, F_RESET_DEV, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#ifndef __HIDPORT_H__
// Copied from hidport.h
#define IOCTL_HID_SET_FEATURE	0xB0191
#define IOCTL_HID_WRITE_REPORT	0xB000F



typedef struct _HID_DEVICE_ATTRIBUTES {

    ULONG           Size;
    //
    // sizeof (struct _HID_DEVICE_ATTRIBUTES)
    //

    //
    // Vendor ids of this hid device
    //
    USHORT          VendorID;
    USHORT          ProductID;
    USHORT          VersionNumber;
    USHORT          Reserved[11];

} HID_DEVICE_ATTRIBUTES, * PHID_DEVICE_ATTRIBUTES;
#endif

// Device Type
//enum DevType { vJoy, vXbox };

// Error levels for status report
enum ERRLEVEL { INFO, WARN, ERR, FATAL, APP };
// Status report function prototype
#ifdef WINAPI
typedef BOOL(WINAPI* StatusMessageFunc)(void* output, TCHAR* buffer, enum ERRLEVEL level);
#endif

///////////////////////////////////////////////////////////////

/////////////////////// Joystick Position ///////////////////////
//
// This structure holds data that is passed to the device from
// an external application such as SmartPropoPlus.
//
// Usage example:
//	JOYSTICK_POSITION iReport;
//	:
//	DeviceIoControl (hDevice, 100, &iReport, sizeof(HID_INPUT_REPORT), NULL, 0, &bytes, NULL)


#if USE_JOYSTICK_API_VERSION == 1

typedef struct _JOYSTICK_POSITION
{
    BYTE	bDevice;	// Index of device. 1-based.
    LONG	wThrottle;
    LONG	wRudder;
    LONG	wAileron;
    LONG	wAxisX;
    LONG	wAxisY;
    LONG	wAxisZ;
    LONG	wAxisXRot;
    LONG	wAxisYRot;
    LONG	wAxisZRot;
    LONG	wSlider;
    LONG	wDial;
    LONG	wWheel;
    LONG	wAxisVX;
    LONG	wAxisVY;
    LONG	wAxisVZ;
    LONG	wAxisVBRX;
    LONG	wAxisVBRY;
    LONG	wAxisVBRZ;
    LONG	lButtons;	// 32 buttons: 0x00000001 means button1 is pressed, 0x80000000 -> button32 is pressed
    DWORD	bHats;		// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
    DWORD	bHatsEx1;	// 16-bit of continuous HAT switch
    DWORD	bHatsEx2;	// 16-bit of continuous HAT switch
    DWORD	bHatsEx3;	// 16-bit of continuous HAT switch
} JOYSTICK_POSITION, * PJOYSTICK_POSITION;

#define VJOY_MAX_N_DEVICES  16 // Maximum number of vJoy devices
#define VJOY_NUMBER_OF_AXES (8) // Maximum number of axes
#define VJOY_NUMBER_OF_HAT (4) // Maximum number of hats
#define VJOY_NUMBER_OF_BUTTONS (128) // Maximum number of hat
#define VJOY_AXIS_MAX_VALUE (0x7FFF) // Maximum value for an axis

#elif USE_JOYSTICK_API_VERSION == 2

// Superset of JOYSTICK_POSITION
// Extension of JOYSTICK_POSITION with Buttons 33-128 appended to the end of the structure.
typedef struct _JOYSTICK_POSITION_V2
{
    /// JOYSTICK_POSITION
    BYTE	bDevice;	// Index of device. 1-based.
    LONG	wThrottle;
    LONG	wRudder;
    LONG	wAileron;
    LONG	wAxisX;
    LONG	wAxisY;
    LONG	wAxisZ;
    LONG	wAxisXRot;
    LONG	wAxisYRot;
    LONG	wAxisZRot;
    LONG	wSlider;
    LONG	wDial;
    LONG	wWheel;
    LONG	wAxisVX;
    LONG	wAxisVY;
    LONG	wAxisVZ;
    LONG	wAxisVBRX;
    LONG	wAxisVBRY;
    LONG	wAxisVBRZ;

    LONG	lButtons;	// 32 buttons: 0x00000001 means button1 is pressed, 0x80000000 -> button32 is pressed
    DWORD	bHats;		// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
    DWORD	bHatsEx1;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
    DWORD	bHatsEx2;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
    DWORD	bHatsEx3;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch LONG lButtonsEx1; // Buttons 33-64

    /// JOYSTICK_POSITION_V2 Extenssion
    LONG lButtonsEx1; // Buttons 33-64
    LONG lButtonsEx2; // Buttons 65-96
    LONG lButtonsEx3; // Buttons 97-128
} JOYSTICK_POSITION_V2, * PJOYSTICK_POSITION_V2;
typedef JOYSTICK_POSITION_V2 JOYSTICK_POSITION;
typedef PJOYSTICK_POSITION_V2 PJOYSTICK_POSITION;

#define VJOY_MAX_N_DEVICES  16 // Maximum number of vJoy devices
#define VJOY_NUMBER_OF_AXES (8) // Maximum number of axes
#define VJOY_NUMBER_OF_HAT (4) // Maximum number of hats
#define VJOY_NUMBER_OF_BUTTONS (128) // Maximum number of hat
#define VJOY_AXIS_MAX_VALUE (0x7FFF) // Maximum value for an axis


#elif USE_JOYSTICK_API_VERSION == 3

// Extension of JOYSTICK_POSITION for more axes
typedef struct _JOYSTICK_POSITION_V3
{
    /// JOYSTICK_POSITION
    BYTE    bDevice;	// Index of device. 1-based.

    LONG    wThrottle;
    LONG    wRudder;
    LONG    wAileron;

    LONG    wAxisX;
    LONG    wAxisY;
    LONG    wAxisZ;
    LONG    wAxisXRot;
    LONG    wAxisYRot;
    LONG    wAxisZRot;
    LONG    wSlider;
    LONG    wDial;

    LONG    wWheel;
    LONG    wAccelerator;
    LONG    wBrake;
    LONG    wClutch;
    LONG    wSteering;

    LONG    wAxisVX;
    LONG    wAxisVY;

    LONG    lButtons;	// 32 buttons: 0x00000001 means button1 is pressed, 0x80000000 -> button32 is pressed

    DWORD   bHats;		// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
    DWORD   bHatsEx1;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
    DWORD   bHatsEx2;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
    DWORD   bHatsEx3;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch LONG lButtonsEx1; // Buttons 33-64

    /// JOYSTICK_POSITION_V2 Extenssion
    LONG    lButtonsEx1; // Buttons 33-64
    LONG    lButtonsEx2; // Buttons 65-96
    LONG    lButtonsEx3; // Buttons 97-128

    // JOYSTICK Extension V3: replacing old slots and moving them at the tail
    LONG    wAxisVZ;
    LONG    wAxisVBRX;
    LONG    wAxisVBRY;
    LONG    wAxisVBRZ;

} JOYSTICK_POSITION_V3, * PJOYSTICK_POSITION_V3;
typedef JOYSTICK_POSITION_V3 JOYSTICK_POSITION;
typedef PJOYSTICK_POSITION_V3 PJOYSTICK_POSITION;

#define VJOY_MAX_N_DEVICES  (16) // Maximum number of vJoy devices
#define VJOY_NUMBER_OF_AXES (16) // Maximum number of axes
#define VJOY_NUMBER_OF_HAT (4) // Maximum number of hats
#define VJOY_NUMBER_OF_BUTTONS (128) // Maximum number of hat
#define VJOY_AXIS_MAX_VALUE (0x7FFF) // Maximum value for an axis

#endif


//----------------------------------------------------------
// FFB Features to be placed in vJoy's driver memory context

// Uncomment this to have a wrong HID descriptor for the FFB
// This allows to make the device to not being reported as a
// FFB device!
//#define VJOY_FORCE_WRONG_FFB_HID

// Max 1..128 effect block index, 10 simultaneously played
#define VJOY_FFB_FIRST_EFFECT_ID            (1)
#define VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX    (100)
#define VJOY_FFB_MAX_SIMULTANEOUS_EFFECTS   (10)

#define VJOY_FFB_EFFECT_FREE                (0x00)
#define VJOY_FFB_EFFECT_ALLOCATED           (0x01)


// For HID descriptor : use only X, or X/Y axes for FFB
// Can be 1 (X) or 2 (X and Y) - see ESP Wheel source code
#define FFB_USE_XY_AXES                     (2)

// Turn on 1 byte packing of struct, dummy fields will be added when needed
#include <pshpack1.h>

// FFB: PID Block Load Feature Report=2
// Only one per device
typedef struct _FFB_PID_BLOCK_LOAD_REPORT
{
    // 1..100. If 0, then error (LoadStatus below is 2 or 3)
    BYTE    EffectBlockIndex;
    // 0 ongoing, 1=Success,2=Full,3=Error
    BYTE	LoadStatus;
    // equals =0 if full, or sizeof(FFB_PID_EFFECT_STATE_REPORT) * (100 - created) max is VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX
    USHORT	RAMPoolAvailable;
} FFB_PID_BLOCK_LOAD_REPORT, * PFFB_PID_BLOCK_LOAD_REPORT;

// FFB: PID Pool Feature Report=3
typedef struct _FFB_PID_POOL_REPORT
{
    // equals sizeof(FFB_PID_EFFECT_STATE_REPORT) * 100, max is 0xFFFF
    USHORT	RAMPoolSize;
    // 0..10
    BYTE	MaxSimultaneousEffects;
    // Bit 0=DeviceManagedPool, bit 1=SharedParameterBlocks
    BYTE	MemoryManagement;
} FFB_PID_POOL_REPORT, * PFFB_PID_POOL_REPORT;

// FFB: PID Effect State Report
// Up to MAX_FFB_EFFECTS_BLOCK_INDEX per device
typedef struct _FFB_PID_EFFECT_STATE_REPORT
{
    // Bitfield for PID Effect State Report, bit:
    // 0: Effect Playing
    // 1: Device Paused
    // 2: Actuators Enabled
    // 3: Safety Switch
    // 4: Actuator Override Switch
    // 5: Actuator Power
    BYTE    PIDEffectStateReport;
    // For CreateEffect/GetNextFree(), internal to driver side
    // 0: free, 1 allocated (in use)
    BYTE    InUse;
} FFB_PID_EFFECT_STATE_REPORT, * PFFB_PID_EFFECT_STATE_REPORT;

// All FFB PID data, one per device
// This struct will be transfered between vJoy and client application
typedef struct _FFB_DEVICE_PID
{
    FFB_PID_BLOCK_LOAD_REPORT   PIDBlockLoad;
    FFB_PID_POOL_REPORT         PIDPool;
    // All Effect States, as 2-bytes
    FFB_PID_EFFECT_STATE_REPORT EffectStates[VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX];
    // Index to next free slot, between 1 and VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX+1
    // If >VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX => full
    BYTE                        NextFreeEID;
    // Index to last valid slot that has been used, between 1 to VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX
    // 0 if no effect yet created/used
    BYTE                        LastEID;
} FFB_DEVICE_PID, * PFFB_DEVICE_PID;

// Turn off 1-byte packing of struct
#include <poppack.h>

// HID Descriptor definitions - Axes
#define HID_USAGE_X             0x30
#define HID_USAGE_Y             0x31
#define HID_USAGE_Z             0x32
#define HID_USAGE_RX            0x33
#define HID_USAGE_RY            0x34
#define HID_USAGE_RZ            0x35
#define HID_USAGE_SL0           0x36
#define HID_USAGE_SL1           0x37
#define HID_USAGE_WHL           0x38
#define HID_USAGE_POV           0x39
#define HID_USAGE_ACCELERATOR   0xC4
#define HID_USAGE_BRAKE         0xC5
#define HID_USAGE_CLUTCH        0xC6
#define HID_USAGE_STEERING      0xC8
#define HID_USAGE_AILERON       0xB0
#define HID_USAGE_RUDDER        0xBA
#define HID_USAGE_THROTTLE      0xBB



// HID Descriptor definitions - FFB Effects
#define HID_USAGE_CONST     0x26    //    Usage ET Constant Force
#define HID_USAGE_RAMP      0x27    //    Usage ET Ramp
#define HID_USAGE_SQUR      0x30    //    Usage ET Square
#define HID_USAGE_SINE      0x31    //    Usage ET Sine
#define HID_USAGE_TRNG      0x32    //    Usage ET Triangle
#define HID_USAGE_STUP      0x33    //    Usage ET Sawtooth Up
#define HID_USAGE_STDN      0x34    //    Usage ET Sawtooth Down
#define HID_USAGE_SPRNG     0x40    //    Usage ET Spring
#define HID_USAGE_DMPR      0x41    //    Usage ET Damper
#define HID_USAGE_INRT      0x42    //    Usage ET Inertia
#define HID_USAGE_FRIC      0x43    //    Usage ET Friction
#define HID_USAGE_CUSTM     0x28    //    Usage ET Custom Force Data
#define HID_USAGE_RESERVD   0x29    //    Usage ET Reserved (unused)

#define HID_USAGE_BLKLD_SUCCESS 0x8C // Usage Block Load Success
#define HID_USAGE_BLKLD_FULL    0x8D // Usage Block Load Full
#define HID_USAGE_BLKLD_ERROR   0x8E // Usage Block Load Error

// HID Descriptor definitions - FFB Report IDs
#define HID_ID_EFFREP       0x01	// Usage Set Effect Report
#define HID_ID_ENVREP       0x02	// Usage Set Envelope Report
#define HID_ID_CONDREP      0x03	// Usage Set Condition Report
#define HID_ID_PRIDREP      0x04	// Usage Set Periodic Report
#define HID_ID_CONSTREP     0x05	// Usage Set Constant Force Report
#define HID_ID_RAMPREP      0x06	// Usage Set Ramp Force Report
#define HID_ID_CSTMREP      0x07	// Usage Custom Force Data Report
#define HID_ID_SMPLREP      0x08	// Usage Download Force Sample
#define HID_ID_EFOPREP      0x0A	// Usage Effect Operation Report
#define HID_ID_BLKFRREP     0x0B	// Usage PID Block Free Report
#define HID_ID_CTRLREP      0x0C	// Usage PID Device Control
#define HID_ID_GAINREP      0x0D	// Usage Device Gain Report
#define HID_ID_SETCREP      0x0E	// Usage Set Custom Force Report
#define HID_ID_NEWEFREP     0x01	// Usage Create New Effect Report
#define HID_ID_BLKLDREP     0x02	// Usage Block Load Report
#define HID_ID_POOLREP      0x03	// Usage PID Pool Report
#define HID_ID_STATEREP     0x04	// Usage PID State Report

#endif
