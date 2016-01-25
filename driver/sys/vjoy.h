/*++

Copyright (c) Shaul Eizikovich.  All rights reserved.

	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
	PURPOSE.

Module Name:

	vjoy.h
	
Abstract:

	common header file for the driver

Author:


Environment:

	kernel mode only

Notes:


Revision History:


--*/
#ifndef _HIDUSBFX2_H_

#define _HIDUSBFX2_H_

#pragma warning(disable:4200)  // suppress nameless struct/union warning
#pragma warning(disable:4201)  // suppress nameless struct/union warning
#pragma warning(disable:4214)  // suppress bit field types other than int warning
#include <initguid.h>
#include <Ntddk.h>
#include <wdm.h>

#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)
#include <wdf.h>
#include <wdmsec.h> // for SDDLs


#pragma warning(disable:4201)  // suppress nameless struct/union warning
#pragma warning(disable:4214)  // suppress bit field types other than int warning
#include <hidport.h>

#define FFB_SUPPORTED
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>


#include "trace.h"
#include <TCHAR.H>
#include <errcodes.h>

#include "../../inc/public.h"

#define _DRIVER_NAME_					"VJOY: "
#define FEATURE_COLLECTION_REPORT_ID	3
#define COMPATIBLE_DEVICE_ID			L"hid_device_system_game"
#define VJOY_DEVICE_ID					L"{D6E55CA0-1A2E-4234-AAF3-3852170B492F}\\vJoyDevice\0"
#define VJOY_INSTANCE_ID				L"vJoyInstance\0"
#define	VJOY_READ_REPORT_PERIOD			2

#define REG_PARAM		L"SYSTEM\\CurrentControlSet\\services\\vjoy\\Parameters"
#define REG_PARAM_DEV0	L"SYSTEM\\CurrentControlSet\\services\\vjoy\\Parameters\\Device0"
#define REG_DEVICE		L"Device"
#define REG_INIT		L"Init"
#define BTN_INIT		L"BTNS"
#define DESC_NAME		L"HidReportDesctiptor"
#define DESC_SIZE		L"HidReportDesctiptorSize"
#define	MEM_TAG_DESC	'1gaT'
#define	MEM_TAG_HIDRPRT	'rDIH'

#define MAX_N_DEVICES	16 // Maximum number of vJoy devices

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;


//
// Collection object is used to store all the FilterDevice objects so
// that any event callback routine can easily walk thru the list and pick a
// specific instance of the device for filtering.
//
WDFCOLLECTION		vJoyDeviceCollection;
WDFWAITLOCK			vJoyDeviceCollectionLock;
extern WDFDEVICE	g_ControlDevice;


// Parameters that are global to the driver that keep track
// of the number of installed devices.
// The number incremented with every device installation and decremented 
// with every device removal
WDFWAITLOCK			g_DeviceCounterLock;
static int			g_DeviceCounter;



typedef struct _CONTROL_DEVICE_EXTENSION {

	PVOID   ControlData; // Store your control data here
	WDFDEVICE hParentDevice;

} CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CONTROL_DEVICE_EXTENSION, ControlGetData)

#ifdef USE_HARDCODED_HID_REPORT_DESCRIPTOR 

#define REPORT_DESCRIPTOR_SIZE	63

// Created from joystk.hid using DT.EXE
// DT.EXE package downloaded from  http://www.usb.org/developers/hidpage
CONST  HID_REPORT_DESCRIPTOR       G_DefaultReportDescriptor[REPORT_DESCRIPTOR_SIZE] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)
	0x09, 0x31,                    //     USAGE (Y)
	0x09, 0x32,                    //     USAGE (Z)
	0x09, 0x35,                    //     USAGE (Rz)
	0x09, 0x36,                    //     USAGE (Slider)
	0x09, 0x33,                    //     USAGE (Rx)
	0x09, 0x34,                    //     USAGE (Ry)
	0x09, 0x37,                    //     USAGE (Dial)
	0x15, 0x01,                    //     LOGICAL_MINIMUM (1)
	0x26, 0xff, 0x7f,              //     LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //     REPORT_SIZE (32)
	0x95, 0x08,                    //     REPORT_COUNT (8)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0xc0,                          //   END_COLLECTION
	0x05, 0x09,                    //   USAGE_PAGE (Button)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x55, 0x00,                    //   UNIT_EXPONENT (0)
	0x65, 0x00,                    //   UNIT (None)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	0xc0                           // END_COLLECTION
};




//
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//
CONST HID_DESCRIPTOR G_DefaultHidDescriptor = 
{
	0x09,   // length of HID descriptor
	0x21,   // descriptor type == HID  0x21
	0x0100, // hid spec release
	0x00,   // country code == Not Specified
	0x01,   // number of HID class descriptors
	{ 
		0x22,   // descriptor type 
		sizeof(G_DefaultReportDescriptor) // total length of report descriptor
	}  
};


#endif  // USE_HARDCODED_HID_REPORT_DESCRIPTOR


#ifdef USE_HARDCODED_HID_REPORT_DESCRIPTOR_1

#define N_COLLECTIONS	1

const char ReportId[N_COLLECTIONS] = {0x01};

// Note that this is a report device
char G_DefaultReportDescriptor[] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)
	// 0x85, 0x01,					   // REPORT_ID (1)

	/***************** Eight Axes ************************************/
	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x31,                    //     USAGE (Y)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x32,                    //     USAGE (Z)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x33,                    //     USAGE (Rx)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x34,                    //     USAGE (Ry)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x35,                    //     USAGE (Rz)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x36,                    //     USAGE (Slider)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x37,                    //     USAGE (Dial)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0xc0,                          //   END_COLLECTION
	/***************** Place holder for 4 POV Hat switches *******************/
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x04,                    //   REPORT_COUNT (4)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
	/***************** Eight buttons *******************/
	0x05, 0x09,                    //   USAGE_PAGE (Button)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x55, 0x00,                    //   UNIT_EXPONENT (0)
	0x65, 0x00,                    //   UNIT (None)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	/***************** Place holder for 24 buttons *******************/
	0x75, 0x18,                    //   REPORT_SIZE (24)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
#if 0
	/************************ FFB 01**********************/
	0xA1, 0x02,					  // Collection (Logical)
	0x26, 0xff, 0x00,			  // Logical Maximum (255)
	0x46, 0xFF, 0x00,			  // Physical Maximum (255)
	0x95, 0x07,					  // Report Count (7)
	0x75, 0x08,					  // Report Size (8)
	0x09, 0x03,					  // Usage (Vendor-Defined 3)   
	0x91, 0x02,					  // Output (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit)   
	0xC0,						  // End Collection   
	/************************ FFB 01**********************/
#endif
#if 1
	/************************ FFB http://forums.obdev.at/viewtopic.php?f=8&t=4313#p14541 **********************/
	  /// force feed back...
	  0x05,  0x0F,   // USAGE_PAGE (Physical Interface)   
	  0x09,  0x21,   // USAGE (Set Effect Report)
	  0xA1,  0x02,   // COLLECTION (effect)   
	  // 0x85, 0x01,	 // REPORT_ID (1)
		 0x09,0x22,   // USAGE (Effect Block Index)
		 0x25,0x7F,    //    Logical Maximum 127
		 0x75,0x07,    //    Report Size 7 (( should be a 7 ))?
		 0x95,0x01,    //    Report Count 1
		 0x91,0x02,   // OUTPUT (Data,Var,Abs)
		 0x09,0x24,//   USAGE (ROM Flag)
		 0x25,0x01,//    LOGICAL_MAXIMUM
		 0x75,0x01,//    Report Size 1
		 0x91,0x02,   // OUTPUT (Data,Var,Abs)

		 0x09,  0x25,   /* USAGE (Effect Type)         */
		 0xA1,  0x02,   /* COLLECTION (Logical2)         */
			0x09,0x26,    //    Usage ET Constant Force
			0x15,0x01,    //    Logical Minimum 1
			0x25,0x01,    //    Logical Maximum 1
			0x75,0x08,    //    Report Size 8
			0x91,0x00,    //    Output
		 0xC0,            //   END_COLLECTION (Logical2)
			
		 //other needed info
		 0x09,0x50,         //    Usage Duration
		 0x09,0x54,         //    Usage Trigger Repeat Interval
		 0x15,0x00,         //    Logical Minimum 0
		 0x26,0x10,0x27,    //    Logical Maximum 10000
		 0x46,0x10,0x27,    //    physical Maximum 10000
		 0x75,0x10,         //    REPORT_SIZE (16)
		 0x66,0x03,0x10,    //    UNIT (Eng Lin:Time)
		 0x55,0x0D,         //    Unit Exponent -3
		 0x95,0x02,         //    Report Count 2
		 0x91,0x02,         //    Output (Data,Var,Abs)

		 0x55,0x0A,         //    UNIT_EXPONENT (-6)
		 0x09,0x51,         //    Usage Sample Period
		 0x95,0x01,         //    Report Count 1
		 0x91,0x02,         //    OUTPUT (Data,Var,Abs) 91 02
		 0x45,0x00,          //    Physical Maximum 0
		 0x55,0x00,         //    Unit Exponent 0
		 0x65,0x00,         //    Unit 0

		 0x09,0x52,         //    Usage Gain
		 0x09,0x53,         //    Usage Trigger Button
		 0x25,0x7F,         //    Logical Maximum 127
		 0x75,0x08,         //    Report Size 8
		 0x95,0x02,         //    Report Count 2
		 0x91,0x02,         //    Output (Variable)

		 0x09,0x55,         //    Usage Axes Enable
		 0xA1,0x02,         //    Collection (Logical)
			0x05,0x01,    //    Usage Page Generic Desktop
			0x09,0x01,    //    USAGE (Pointer)
			0xA1,0x00,         //    Collection (Physical)
			   0x09,0x30,    //    Usage X
			   0x09,0x31,    //    Usage Y
			   0x25,0x01,    //    Logical Maximum 1
			   0x75,0x01,    //    Report Size 1
			   0x95,0x02,    //    Report Count 2
			   0x91,0x02,    //    Output (Variable)
			0xC0     ,    // End Collection (Physical)
		 0xC0     ,    // End Collection(Logical)

		 0x95,0x06,         //    6-bit pad 95 06
		 0x91,0x03,         //   OUTPUT (Cnst,Var,Abs)

		 0x05,  0x0F,   // USAGE_PAGE (Physical Interface)   
		 0x09,0x57,    //    USAGE (Direction))
		 0xA1,0x02,         //    Collection logical 3
			0x05,0x01, //USAGE_PAGE (Generic Desktop)
			0x09,0x01,    //    USAGE (Pointer)
			0xA1,0x00,         //    Collection (Physical)
			   0x09,0x30,    //    Usage X
			   0x09,0x31,    //    Usage Y
			   0x15,0x00,     //    Logical Minimum 0
			   0x26,0xff,0x00,//   Logical Maximum 255
			   0x46,0x68,0x01,//   PHYSICAL_MAXIMUM (360)
			   0x66,0x14,0x00,//   UNIT (Eng Rot:Angular Pos)
			   0x75,0x08,    //    Report Size 8
			   0x95,0x02,    //    Report Count 2
			   0x91,0x02,    //    Output (Variable)
			   0x65,0x00,      //  Unit 0
			   0x45,0x00,      //  PHYSICAL_MAXIMUM (0)
			0xC0     ,    // End Collection (Physical)
		 0xC0     ,    // End Collection (logical 3)


		 0x05,  0x0F,   // USAGE_PAGE (Physical Interface)   
		 0x09,  0x58,   // USAGE (Type Specific Block Offset)
		 0xA1,0x02,    //    Collection 1
			0x0B, 0x01, 0x00, 0x0A, 0x00,//USAGE (Ordinals:Instance 1) 
			0x0B, 0x02, 0x00, 0x0A, 0x00,//USAGE (Ordinals:Instance 2) 
			0x26,0xFD, 0x7F,             //Logical Maximum LOGICAL_MAXIMUM (32765) ; 32K RAM or ROM max.
			0x75,0x10,    //    Report Size 8
			0x95,0x02,    //    Report Count 2
			0x91,0x02,   // OUTPUT (Data,Var,Abs)
		 0xC0     ,    // End  Collection 1
	  0xc0,   //   END_COLLECTION effect 

	  //and a few more settings out side of effect

	  0x09,0x5A,    //    Usage Set Envelope Report
	  0xA1,0x02,    //    Collection Datalink
		0x09,0x23,         //    Usage Effect Block 
		0x26,0xFD, 0x7F,   //    LOGICAL_MAXIMUM (32765) ; 32K RAM or ROM max
		0x75,0x0F,         //    Report Size 15
		0x95,0x01,         //    Report Count 1
		0x91,0x02,         //    Output (Variable)

		0x09,0x24,         //   USAGE (ROM Flag)
		0x25,0x01,         //    LOGICAL_MAXIMUM 1
		0x75,0x01,         //    Report Size 1
		0x91,0x02,         //    Output

		0x09,0x5B,         //    Usage Attack Level
		0x09,0x5D,         //    Usage Fade Level
		0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
		0x75,0x08,         //    Report size 8 
		0x95,0x02,         //    Report Count 2
		0x91,0x02,         //    Output (Variable)

		0x09,0x5C,         //    Usage Attack Time
		0x09,0x5E,         //    Usage Fade Time
		0x26,0x10,0x27,    //    Logical Maximum 7FFFh (32767d)
		0x46,0x10,0x27,    //    Physical Maximum 7FFFh (32767d)
		0x66,0x03,0x10,    //    Unit 1003h (4099d)
		0x55,0x0D,         //    Unit Exponent FDh (253d)
		0x75,0x10,         //    Report Size 10h (16d)
		0x91,0x02,         //    Output (Variable)
		0x45,0x00,         //    Physical Maximum 0
		0x65,0x00,        //    Unit 0
		0x55,0x00,         //    Unit Exponent 0
	  0xC0     ,            //    End Collection



	  0x09,0x5F,    //    USAGE (Set Condition Report)
	  0xA1,0x02,    //    Collection Datalink
		0x09,0x23,         //    Usage Effect Block 
		0x26,0xFD, 0x7F,   //    LOGICAL_MAXIMUM (32765) ; 32K RAM or ROM max
		0x75,0x0F,         //    Report Size 15
		0x95,0x01,         //    Report Count 1
		0x91,0x02,         //    Output (Variable)

		0x09,0x24,         //   USAGE (ROM Flag)
		0x25,0x01,         //    LOGICAL_MAXIMUM 1
		0x75,0x01,         //    Report Size 1
		0x91,0x02,         //    Output

		0x09,0x60,         //    USAGE (CP Offset)
		0x09,0x61,         //    USAGE (Positive Coefficient)
		0x09,0x62,          //     USAGE (Negative Coefficient)
		0x09,0x63,          //     USAGE (Positive Saturation)
		0x09,0x64,         //    USAGE (Negative Saturation)
		0x09,0x65,         //    USAGE (Dead Band)
		0x26,0xFF, 0x00,   //    LOGICAL_MAXIMUM (255)
		0x75,0x08,         //    Report Size 8
		0x95,0x06,         //    Report Count 6
		0x91,0x02,         //    Output (Variable)
	  0xC0     ,            //    End Collection

   0x09,0x5F,    //    USAGE (Set Periodic Report)
   0xA1,0x02,    //    Collection Datalink
	 0x09,0x23,         //    (Parameter Block Offset)
	 0x26,0xFD, 0x7F,   //    LOGICAL_MAXIMUM (32765) ; 32K RAM or ROM max
	 0x75,0x0F,         //    Report Size 15
	 0x95,0x01,         //    Report Count 1
	 0x91,0x02,         //    Output (Variable)

	 0x09,0x24,         //   USAGE (ROM Flag)
	 0x25,0x01,         //    LOGICAL_MAXIMUM 1
	 0x75,0x01,         //    Report Size 1
	 0x91,0x02,         //    Output

	 0x09,0x70,         //    USAGE (Magnitude) 
	 0x09,0x6F,         //    USAGE USAGE (Offset) 
	 0x09,0x71,          //     USAGE (Phase) 
	 0x26,0xFF, 0x00,   //    LOGICAL_MAXIMUM (255)
	 0x75,0x08,         //    Report Size 8
	 0x95,0x03,         //    Report Count 3
	 0x91,0x02,         //    Output (Variable)

	 0x09,0x72,          //     USAGE (Phase) 
	 0x26,0x10, 0x27,   //    LOGICAL_MAXIMUM 
	 0x46,0x10, 0x27,   //    PHYSICAL_MAXIMUM 
	 0x66,0x03, 0x10,   //    UNIT (Eng Lin:Time)
	 0x55,0x0D,       //    UNIT_EXPONENT (-3)
	 0x75,0x10,         //    Report Size 16
	 0x95,0x01,         //    Report Count 1
	 0x91,0x02,         //    Output (Variable)
	 0x45,0x00,         //    PHYSICAL_MAXIMUM 
	 0x65,0x00,       //    UNIT (None)
	 0x55,0x00,          //    UNIT_EXPONENT (0)
   0xC0     ,            //    End Collection


   0x09,0x73,    //    Usage Set Constant Force Rep...
   0xA1,0x02,    //    Collection Datalink
	  0x09,0x23,         //    Usage Effect Block Index
	  0x26,0xFD, 0x7F,   //    Logical Maximum 32765)
	  0x75,0x0F,         //    Report Size 15
	  0x95,0x01,         //    Report Count 1
	  0x91,0x02,         //    Output (Variable)

	  0x09,0x24,         //    Usage Rom flag
	  0x25,0x01,        //    LOGICAL_MAXIMUM (1)
	  0x75,0x01,         //    Report Size
	  0x91,0x02,         //    Output (Variable)

	  0x09,0x70,         //    Usage magnitude
	  0x26,0xFF, 0x00,   //    LOGICAL_MAXIMUM (255)
	  0x75,0x08,         //    Report Size
	  0x91,0x02,         //    Output (Variable)
   0xC0     ,    //    End Collection (Datalink)


   0x09,0x77,    //    USAGE (Effect Operation Report) 
   0xA1,0x02,    //    Collection Datalink
	  0x09,0x22,         //    Usage Effect Block Index
	  0x25,0x7f,         //    Logical Maximum 127)
	  0x75,0x07,         //    Report Size 7
	  0x95,0x01,         //    Report Count 1
	  0x91,0x02,         //    Output (Variable)

	  0x09,0x24,         //    Usage Rom flag
	  0x25,0x01,        //    LOGICAL_MAXIMUM (1)
	  0x75,0x01,         //    Report Size 1
	  0x91,0x02,         //    Output (Variable)

	  0x09,0x78,         //    USAGE (Effect Operation)
	  0xA1,0x02,    //    Collection Datalink
		 0x09,0x79,         //  USAGE (Op Effect Start) 
		 0x09,0x7A,         //  USAGE (Op Effect Start SOLO) 
		 0x09,0x7B,         //  USAGE (Op Effect STOP) 
		 0x15,0x01,         //    LOGICAL_MINIMUM 1
		 0x25,0x03,         //    LOGICAL_MAXIMUM 3
		 0x75,0x08,         //    Report Size
		 0x91,0x00,         //    OUTPUT (Data,Ary,Abs) 91 00
	  0xC0     ,    //    End Collection (Datalink)

	  0x09,0x7C,         //   USAGE (Loop Count)
	  0x15,0x00,         //    LOGICAL_MINIMUM 1
	  0x26,0xFF, 0x00,    //    LOGICAL_MAXIMUM 3
	  0x91,0x02,         //    OUTPUT (Data,Var,Abs)

   0xC0     ,    //    End Collection (Datalink)

   0x09,0x7F,    //    USAGE (PID Pool Report)
   0xA1,0x02,    //    Collection Datalink
	  0x09,0x80,         // USAGE (RAM Pool Size) 
	  0x09,0x81,         // USAGE (RoM Pool Size) 
	  0x09,0x82,         // USAGE (ROM Effect Block Count)
	  0x26,0xFD, 0x7F,   // LOGICAL_MAXIMUM (32765)
	  0x95,0x03,         //    REPORT_COUNT (3)
	  0x75,0x10,         //    REPORT_SIZE (16)
	  0xB1,0x02,         //    FEATURE (Data,Var,Abs)

	  0x09, 0xA8,   //USAGE (Parameter Block Size) 
	  0xA1,0x02,    //    Collection Datalink
		 0x09,0x73,         // USAGE (Set Constant Force Report)
		 0x26,0xFF, 0x00,   // LOGICAL_MAXIMUM (255)
		 0x75,0x08,         // REPORT_SIZE (8)
		 0x95,0x01,         // REPORT_COUNT (1)
		 0xB1,0x02,         //    FEATURE (Data,Var,Abs)
	  0xC0     ,    //    End Collection (Datalink)

	  0x25, 0x01,//LOGICAL_MAXIMUM (1) 
	  0x75, 0x07,//REPORT_SIZE (7) 
	  0x95, 0x01,//REPORT_COUNT (1) 
	  0xB1, 0x03,//FEATURE (Cnst,Var,Abs) ; 7-bit pad 
	  0x09, 0x67,//USAGE (Isoch Custom Force Enable) 
	  0x75, 0x01,//REPORT_SIZE (1)
	  0xB1, 0x02,//FEATURE (Data,Var,Abs) 
   0xC0     ,    //    End Collection (Datalink)


   0x09,0x92,    //   USAGE (PID State Report)
   0xA1,0x02,    //    Collection Datalink
	  0x09, 0x22,         //    USAGE (Effect Block Index)
	  0x25, 0x7F,       //    Logical Maximum (127)
	  0x75, 0x07,         //    Report Size 7
	  0x81, 0x02,         //   INPUT (Data,Var,Abs)

	  0x09, 0x24,//USAGE (ROM Flag) 
	  0x25, 0x01,//LOGICAL_MAXIMUM (1)
	  0x75, 0x01,//REPORT_SIZE (1) 
	  0x95, 0x01,//REPORT_COUNT (1) 
	  0x81, 0x02,//   INPUT (Data,Var,Abs)

	  0x09, 0x94,   // USAGE (Effect Playing) 
	  0x09, 0xA0,   // USAGE (Actuators Enabled)
	  0x09, 0xA4,   // USAGE (Safety Switch) 
	  0x09, 0xA6,   // USAGE (Actuator Power)
	  0x75, 0x01,//REPORT_SIZE (1) 
	  0x95, 0x04,//REPORT_COUNT (4)
	  0x81, 0x02,//   INPUT (Data,Var,Abs)
	  0x81, 0x03,//  INPUT (Cnst,Var,Abs) ; 4-bit pad 
   0xC0     ,    //    End Collection (Datalink)

   0x09,0x95,    //   USAGE (PID Device Control Report)
   0xA1,0x02,    //    Collection Datalink
	  0x09,0x96,         //    USAGE (PID Device Control) 
	  0xA1,0x02,    //    Collection Datalink
		 0x09,0x97,         //    USAGE (DC Enable Actuators) 
		 0x09,0x98,         //    USAGE (DC Disable Actuators) 
		 0x09,0x99,         //    USAGE (DC Stop All Effects) 
		 0x09,0x9A,         //    USAGE (DC Reset) 
		 0x09,0x9B,         //    USAGE (DC Pause) 
		 0x09,0x9C,         //    USAGE (DC Continue) 
		 0x15,0x01,        //    Logical Maximum 1)
		 0x25,0x06,         //    Logical Maximum 6)
		 0x95,0x08,         //    Report Count 8 ( wrong in usb pdf )
		 0x75,0x01,         //    Report Size 1 ( wrong in usb pdf )
		 0x91,0x02,         //   OUTPUT (Data,Var,Abs)
	  0xC0     ,    //    End Collection (Datalink)
   0xC0     ,    //    End Collection (Datalink)


   0x09,0x85,    //   USAGE (PID Pool Move Report)
   0xA1,0x02,    //    Collection Datalink
	  0x09,0x86,         //    USAGE (Move Source)
	  0x09,0x87,         //    USAGE (Move Destination)
	  0x09,0x88,         //    USAGE (Move Length)
	  0x26,0xFF, 0x7F,   //    Logical Maximum 2767)
	  0x75,0x10,         //    Report Size 16
	  0x95,0x03,         //    Report Count 3
	  0x92,0x02,0x01,       //   OUTPUT (Data,Var,Abs,Buf)
   0xC0     ,    //    End Collection (Datalink)


   0x09,0x7D,    //   Device Gain Report)
   0xA1,0x02,    //    Collection Datalink
	  0x09,0x7E,         //    USAGE (Device Gain)
	  0x26,0xFF, 0x00,   //    Logical Maximum 255)
	  0x75,0x08,         //    Report Size 8
	  0x95,0x01,         //    Report Count 1
	  0xB1,0x02,         //    FEATURE (Data,Var,Abs)
   0xC0     ,    //    End Collection (Datalink)

		/************************ FFB http://forums.obdev.at/viewtopic.php?f=8&t=4313#p14541 **********************/
#endif
  0xc0                           // END_COLLECTION
};

// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//
HID_DESCRIPTOR G_DefaultHidDescriptor = 
{
	0x09,   // length of HID descriptor
	0x21,   // descriptor type == HID  0x21
	0x0101, // hid spec release
	0x00,   // country code == Not Specified
	0x01,   // number of HID class descriptors
	{ 
		0x22,   // descriptor type 
		sizeof(G_DefaultReportDescriptor) // total length of report descriptor
	}  
};






#endif  // USE_HARDCODED_HID_REPORT_DESCRIPTOR_1

#ifdef USE_HARDCODED_HID_REPORT_DESCRIPTOR_2
// C:\WinDDK\vjoy1\driver\sys\joystk4Ax32Bt-02.h
// Good
// Axes: 4/8
// Buttons: 12/32
#define REPORT_DESCRIPTOR_SIZE	75


// Created from joystk.hid using DT.EXE
// DT.EXE package downloaded from  http://www.usb.org/developers/hidpage
CONST  HID_REPORT_DESCRIPTOR       G_DefaultReportDescriptor[REPORT_DESCRIPTOR_SIZE] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x31,                    //     USAGE (Y)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x32,                    //     USAGE (Z)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x35,                    //     USAGE (Rz)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0xc0,                          //   END_COLLECTION
	0x05, 0x09,                    //   USAGE_PAGE (Button)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x55, 0x00,                    //   UNIT_EXPONENT (0)
	0x65, 0x00,                    //   UNIT (None)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x0c,                    //   USAGE_MAXIMUM (Button 12)
	0x95, 0x0c,                    //   REPORT_COUNT (12)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x14,                    //   REPORT_SIZE (20)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
	0xc0                           // END_COLLECTION
};




//
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//
CONST HID_DESCRIPTOR G_DefaultHidDescriptor = 
{
	0x09,   // length of HID descriptor
	0x21,   // descriptor type == HID  0x21
	0x0101, // hid spec release
	0x00,   // country code == Not Specified
	0x01,   // number of HID class descriptors
	{ 
		0x22,   // descriptor type 
		sizeof(G_DefaultReportDescriptor) // total length of report descriptor
	}  
};



#endif  // USE_HARDCODED_HID_REPORT_DESCRIPTOR_2


#ifdef USE_HARDCODED_HID_REPORT_DESCRIPTOR_3

// C:\WinDDK\vjoy1\driver\sys\joystk4Ax12Bt4Hat-01.h
// Good
// Axes:		4/8
// Buttons:		12/32
// POV Hats:	1/4 (5-state)
#define REPORT_DESCRIPTOR_SIZE	98


// Created from joystk.hid using DT.EXE
// DT.EXE package downloaded from  http://www.usb.org/developers/hidpage
CONST  HID_REPORT_DESCRIPTOR       G_DefaultReportDescriptor[REPORT_DESCRIPTOR_SIZE] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x31,                    //     USAGE (Y)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x32,                    //     USAGE (Z)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x35,                    //     USAGE (Rz)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)
	0xc0,                          //   END_COLLECTION
	
	
	0x09, 0x39,                    //   USAGE (Hat switch)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x03,                    //   LOGICAL_MAXIMUM (3)
	0x35, 0x00,                    //   PHYSICAL_MINIMUM (0)
	0x46, 0x0e, 0x01,              //   PHYSICAL_MAXIMUM (270)
	0x65, 0x14,                    //   UNIT (Eng Rot:Angular Pos)
	0x75, 0x04,                    //   REPORT_SIZE (4)
	0x95, 0x02,                    //   REPORT_COUNT (2)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	0x95, 0x02,                    //   REPORT_COUNT (2)
	0x75, 0x04,                    //   REPORT_SIZE (4)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)

	0x05, 0x09,                    //   USAGE_PAGE (Button)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x55, 0x00,                    //   UNIT_EXPONENT (0)
	0x65, 0x00,                    //   UNIT (None)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x20,                    //   USAGE_MAXIMUM (Button 322)
	0x95, 0x20,                    //   REPORT_COUNT (20)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	//0x95, 0x01,                    //   REPORT_COUNT (1)
	//0x75, 0x14,                    //   REPORT_SIZE (20)
	//0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)

	
	
	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	0xc0                           // END_COLLECTION
};




//
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//
CONST HID_DESCRIPTOR G_DefaultHidDescriptor = 
{
	0x09,   // length of HID descriptor
	0x21,   // descriptor type == HID  0x21
	0x0101, // hid spec release
	0x00,   // country code == Not Specified
	0x01,   // number of HID class descriptors
	{ 
		0x22,   // descriptor type 
		sizeof(G_DefaultReportDescriptor) // total length of report descriptor
	}  
};



#endif  // USE_HARDCODED_HID_REPORT_DESCRIPTOR_3


#ifdef USE_HARDCODED_HID_REPORT_DESCRIPTOR_4

// C:\WinDDK\vjoy1\driver\sys\joystk4Ax12Bt4Hat-01.h
// Good
// Axes:		4/8
// Buttons:		12/32
// POV Hats:	1/4 (5-state)
#define REPORT_DESCRIPTOR_SIZE	182


// Created from joystk.hid using DT.EXE
// DT.EXE package downloaded from  http://www.usb.org/developers/hidpage
char G_DefaultReportDescriptor[REPORT_DESCRIPTOR_SIZE] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)


////////////////////////////////////////////////////////////////////////////////////
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x85, 0x01,					   // REPORT_ID (1)				[1]
	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x39,                    //     USAGE (Hat switch)
	0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	0x25, 0x03,                    //     LOGICAL_MAXIMUM (3)
	0x75, 0x04,                    //     REPORT_SIZE (4)
	0x95, 0x02,                    //     REPORT_COUNT (2)
	0x65, 0x14,                    //     UNIT (Eng Rot:Angular Pos) 
	0x81, 0x42,                    //     INPUT (Data,Var,Abs,Null)  [1]
	0x95, 0x02,                    //     REPORT_COUNT (2)
	0x81, 0x41,                    //   INPUT (Cnst,Ary,Abs,Null) [1]
	0xc0,                          //   END_COLLECTION
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)				
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x09, 0x31,                    //     USAGE (Y)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x09, 0x32,                    //     USAGE (Z)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x09, 0x35,                    //     USAGE (Rz)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0xc0,                          //   END_COLLECTION

	0xc0,                          // END_COLLECTION

////////////////////////////////////////////////////////////////////////////////////
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x85, 0x02,					   // REPORT_ID (2)		[1]
	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	
	
	0x09, 0x39,                    //   USAGE (Hat switch)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x03,                    //   LOGICAL_MAXIMUM (3)
	0x35, 0x00,                    //   PHYSICAL_MINIMUM (0)
	0x46, 0x0e, 0x01,              //   PHYSICAL_MAXIMUM (270)
	0x65, 0x14,                    //   UNIT (Eng Rot:Angular Pos)
	0x75, 0x04,                    //   REPORT_SIZE (4)
	0x95, 0x02,                    //   REPORT_COUNT (2)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs) [1]
	0x95, 0x02,                    //   REPORT_COUNT (2)
	0x75, 0x04,                    //   REPORT_SIZE (4)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs) [1]

	0x05, 0x09,                    //   USAGE_PAGE (Button)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x55, 0x00,                    //   UNIT_EXPONENT (0)
	0x65, 0x00,                    //   UNIT (None)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x20,                    //   USAGE_MAXIMUM (Button 32)
	0x95, 0x20,                    //   REPORT_COUNT (20)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs) [4]
	//0x95, 0x01,                    //   REPORT_COUNT (1)
	//0x75, 0x14,                    //   REPORT_SIZE (20)
	//0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)

	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x09, 0x01,                    //   USAGE (Pointer)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)				
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x09, 0x31,                    //     USAGE (Y)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x09, 0x32,                    //     USAGE (Z)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x09, 0x35,                    //     USAGE (Rz)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs)	[4]
	0xc0,                          //   END_COLLECTION

	
	0xc0,                           // END_COLLECTION

///////////////////////////////////////////////////////////////////////////////////////////////////////////


};




//
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//
HID_DESCRIPTOR G_DefaultHidDescriptor = 
{
	0x09,   // length of HID descriptor
	0x21,   // descriptor type == HID  0x21
	0x0101, // hid spec release
	0x00,   // country code == Not Specified
	0x01,   // number of HID class descriptors
	{ 
		0x22,   // descriptor type 
		sizeof(G_DefaultReportDescriptor) // total length of report descriptor
	}  
};



#endif  // USE_HARDCODED_HID_REPORT_DESCRIPTOR_4

#ifdef USE_HARDCODED_HID_REPORT_DESCRIPTOR_5

#define N_COLLECTIONS	2			
//#define REPORT_DESCRIPTOR_SIZE	182

char G_DefaultReportDescriptor[] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)

	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x85, 0x01,			           //	REPORT_ID (1)
	/***************** Eight Axes ************************************/
	0x09, 0x01,                    //   USAGE (Pointer)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x31,                    //     USAGE (Y)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x32,                    //     USAGE (Z)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x33,                    //     USAGE (Rx)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x34,                    //     USAGE (Ry)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x35,                    //     USAGE (Rz)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x36,                    //     USAGE (Slider)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x37,                    //     USAGE (Dial)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0xc0,                          //   END_COLLECTION
	/***************** Place holder for 4 POV Hat switches *******************/
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x04,                    //   REPORT_COUNT (4)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
	/***************** Eight buttons *******************/
	0x05, 0x09,                    //   USAGE_PAGE (Button)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x55, 0x00,                    //   UNIT_EXPONENT (0)
	0x65, 0x00,                    //   UNIT (None)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	/***************** Place holder for 120 buttons *******************/
	0x75, 0x78,                    //   REPORT_SIZE (120)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
	0xc0,                          // END_COLLECTION

	/****************** Second collection *******************************/

	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x15, 0x00,                    // LOGICAL_MINIMUM (0)
	0x09, 0x04,                    // USAGE (Joystick)
	0xa1, 0x01,                    // COLLECTION (Application)

	0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
	0x85, 0x02,			           //	REPORT_ID (2)
	/***************** Eight Axes ************************************/
	0x09, 0x01,                    //   USAGE (Pointer)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x7f,              //   LOGICAL_MAXIMUM (32767)
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x31,                    //     USAGE (Y)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x32,                    //     USAGE (Z)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x33,                    //     USAGE (Rx)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x34,                    //     USAGE (Ry)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x35,                    //     USAGE (Rz)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x36,                    //     USAGE (Slider)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x09, 0x37,                    //     USAGE (Dial)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0xc0,                          //   END_COLLECTION
	/***************** Place holder for 4 POV Hat switches *******************/
	0x75, 0x20,                    //   REPORT_SIZE (32)
	0x95, 0x04,                    //   REPORT_COUNT (4)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
	/***************** Eight buttons *******************/
	0x05, 0x09,                    //   USAGE_PAGE (Button)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x55, 0x00,                    //   UNIT_EXPONENT (0)
	0x65, 0x00,                    //   UNIT (None)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	/***************** Place holder for 120 buttons *******************/
	0x75, 0x78,                    //   REPORT_SIZE (120)
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
	0xc0,                          // END_COLLECTION
};

// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//
HID_DESCRIPTOR G_DefaultHidDescriptor = 
{
	0x09,   // length of HID descriptor
	0x21,   // descriptor type == HID  0x21
	0x0101, // hid spec release
	0x00,   // country code == Not Specified
	0x01,   // number of HID class descriptors
	{ 
		0x22,   // descriptor type 
		sizeof(G_DefaultReportDescriptor) // total length of report descriptor
	}  
};






#endif  // USE_HARDCODED_HID_REPORT_DESCRIPTOR_5

#ifdef USE_EXTERNAL_HARDCODED_HID_REPORT_DESCRIPTOR
//const char ReportId[N_COLLECTIONS] = {0x01};
#include "hidReportDesc.h"
// This is the default HID descriptor returned by the mini driver
// in response to IOCTL_HID_GET_DEVICE_DESCRIPTOR. The size
// of report descriptor is currently the size of G_DefaultReportDescriptor.
//
HID_DESCRIPTOR G_DefaultHidDescriptor = 
{
	0x09,   // length of HID descriptor
	0x21,   // descriptor type == HID  0x21
	0x0101, // hid spec release
	0x00,   // country code == Not Specified
	0x01,   // number of HID class descriptors
	{ 
		0x22,   // descriptor type 
		sizeof(G_DefaultReportDescriptor) // total length of report descriptor
	}  
};

#endif // USE_EXTERNAL_HARDCODED_HID_REPORT_DESCRIPTOR
/////////////////////// HID Input Report ///////////////////////
// 
// This structure is filled by vJoy device and copied into the
// buffer supplied by the IOCTL_HID_READ_REPORT.
// It containes the (virtual) joystick positions
// The structure of its 'InputReport' member is derived from the structure
// of HID_REPORT_DESCRIPTOR: The order and size of the members are derived from
// the those in 'COLLECTION (Application)'
//
#include <pshpack1.h>

typedef struct _HID_INPUT_REPORT {
	union {
		struct {
			UCHAR	CollectionId;
			LONG	bAxisX;
			LONG	bAxisY;
			LONG	bAxisZ;
			LONG	bAxisRX;
			LONG	bAxisRY;
			LONG	bAxisRZ;
			LONG	bSlider;
			LONG	bDial;
			DWORD	bHats;		// 4 HATs, 4bit each OR one 16-bit continuous HAT
			DWORD	bHatsEx1;	// Extra 16-bit continuous HAT
			DWORD	bHatsEx2;	// Extra 16-bit continuous HAT
			DWORD	bHatsEx3;	// Extra 16-bit continuous HAT
			ULONG	bButtons;	// 32 Buttons
			UCHAR	FFBVal;		// FFB Value
		} InputReport;
		UCHAR RawData[54];
	};
} HID_INPUT_REPORT, *PHID_INPUT_REPORT;

typedef struct _HID_INPUT_REPORT_V2 {
	union {
		struct {
			UCHAR	CollectionId;
			LONG	bAxisX;
			LONG	bAxisY;
			LONG	bAxisZ;
			LONG	bAxisRX;
			LONG	bAxisRY;
			LONG	bAxisRZ;
			LONG	bSlider;
			LONG	bDial;
			DWORD	bHats;		// 4 HATs, 4bit each OR one 16-bit continuous HAT
			DWORD	bHatsEx1;	// Extra 16-bit continuous HAT
			DWORD	bHatsEx2;	// Extra 16-bit continuous HAT
			DWORD	bHatsEx3;	// Extra 16-bit continuous HAT
			ULONG	bButtons;	// Buttons 1-32

			// Higher buttons
			ULONG	bButtonsEx1;	// Buttons 33-64
			ULONG	bButtonsEx2;	// Buttons 65-96
			ULONG	bButtonsEx3;	// Buttons 97-128
			// DEBUGGING UCHAR	FFBVal;		    // FFB Value
		} InputReport;
		UCHAR RawData[65];
	};
} HID_INPUT_REPORT_V2, *PHID_INPUT_REPORT_V2;

// Structure that holds the Joystick values
typedef struct _DEVICE_POSITION {
	LONG	ValThrottle, ValRudder, ValAileron;
	LONG	ValX, ValY, ValZ;
	LONG	ValRX, ValRY, ValRZ;
	LONG	ValSlider, ValDial, ValWheel;
	DWORD	ValHats;
	DWORD	ValHatsEx1;
	DWORD	ValHatsEx2;
	DWORD	ValHatsEx3;
	DWORD	ValButtons;
} DEVICE_POSITION, *PDEVICE_POSITION;

typedef struct _DEVICE_POSITION_V2 {
	LONG	ValThrottle, ValRudder, ValAileron;
	LONG	ValX, ValY, ValZ;
	LONG	ValRX, ValRY, ValRZ;
	LONG	ValSlider, ValDial, ValWheel;
	DWORD	ValHats;
	DWORD	ValHatsEx1;
	DWORD	ValHatsEx2;
	DWORD	ValHatsEx3;
	DWORD	ValButtons;

	// Upper buttons
	DWORD	ValButtonsEx1;
	DWORD	ValButtonsEx2;
	DWORD	ValButtonsEx3;
} DEVICE_POSITION_V2, *PDEVICE_POSITION_V2;

#include <poppack.h>



// This is the structure used by the DEVICE to hold internal data
// The way to access the data is by creating the accessor method GetDeviceContext()
// GetDeviceContext input is the handle to the device (WDFDEVICE). The return value 
// is a pointer to the device context structure (PDEVICE_EXTENSION)
typedef struct _DEVICE_EXTENSION{ // Needs to be changed


	//
	// This variable stores state for the swicth that got toggled most recently
	// (the device returns the state of all the switches and not just the 
	// one that got toggled).
	//
	UCHAR    LatestToggledSwitch;
	
	//
	// WDF Queue for read IOCTLs from hidclass that get satisfied from 
	// USB interrupt endpoint
	//
	WDFQUEUE   ReadReportMsgQueue;

	// WDF Queue to handle IOCTLs that will be forwarded to us from
	// the rawPDO. 
	//
	WDFQUEUE   rawPdoQueue;

	// WDF I/O Target - Raw PDO will redirect control I/O to here
	WDFIOTARGET IoTargetToSelf;

	//
	// Array of Joystick Values - One for each joystick (Index = ID-1)
	PDEVICE_POSITION_V2 positions[MAX_N_DEVICES];
	int nDevices;

	// Array that tells the driver which of the 16 vJoy devices is actually implemented
	BOOLEAN DeviceImplemented[MAX_N_DEVICES];

	// Array of File Objects to the opened handles - One for each joystick (Index = ID-1) 
	// There's one handle per device as we allow opening a device only once
	WDFFILEOBJECT DeviceFileObject[MAX_N_DEVICES];


	// Pointer to valid HID report descriptor
	// By default, it holds a copy of the hard-coded report descriptor
	// If there's valid data in the registry then it holds
	// a copy of the report descriptor from the registry
	HID_REPORT_DESCRIPTOR * ReportDescriptor;

	// Use this lock handle to synch access to positions[i]
	// It will be created when the raw device is created
	// Its parent object will be the raw device
	WDFWAITLOCK positionLock;

	// Handle to the timer that completes IOCTL_HID_READ_REPORT pending requests
	// Calback function will be called by this timer every VJOY_READ_REPORT_PERIOD (=500) mS
	WDFTIMER	timerHandle;

	// If vJoy device set is the hard-coded case then this variable is TRUE
	// If vJoy device set is configured from the registry then this variable is FALSE
	BOOLEAN		isHardcodedDevice;

	///////////////  Force Feedback Section (FFB) (Start) ///////////////////////////////////////////
	/*
	Two FFB sets of manual queues.
	FfbWriteQ[] (WriteQ): DirectInptut requests IOCTL_HID_SET_FEATURE and IOCTL_HID_WRITE_REPORT
	are forwarded to one of these manual queue according to the device ID.
	FfbReadQ[] (ReadQ): Feeder GET_FFB_DATA is always forwarded to one of these manual queue according to the device ID.
	The index of the Queue is ID-1 (Range 0-15)
	*/
	WDFQUEUE   FfbReadQ[MAX_N_DEVICES]; 
	WDFQUEUE   FfbWriteQ[MAX_N_DEVICES];


	/* 
	FfbEnable: Per joystick (Index = ID-1):
	False(default): FFB is Inactive
		All DirectInptut requests IOCTL_HID_SET_FEATURE and IOCTL_HID_WRITE_REPORT completes with error
		IOCTL_HID_GET_FEATURE returns 'no ffb'
	True: FFB is active
	*/
	BOOLEAN FfbEnable[MAX_N_DEVICES];
	/*
	FfbXferLock:
	Wait lock to ensure that request transfer function will not be re-entrant
	Aquire when starting and release when transfer completes
	*/
	WDFWAITLOCK FfbXferLock;
	///////////////  Force Feedback Section (FFB) (End)   ///////////////////////////////////////////

} DEVICE_EXTENSION, * PDEVICE_EXTENSION;

// Declare Accessor function GetDeviceContext()
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, GetDeviceContext)

// FFB Read Queue and Write Queue (FfbReadQ, FfbWriteQ) context area
typedef struct _FFB_QUEUE_EXTENSION	 {
	int		DeviceID;
	BOOLEAN	isRead;
}	FFB_QUEUE_EXTENSION, *PFFB_QUEUE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FFB_QUEUE_EXTENSION, GetFfbQueuContext)

// Structure that holds initial values of device controls
typedef struct _DEVICE_INIT_VALS {
	UCHAR cb;				// Size in bytes of this structure
	USHORT id;				// Device ID
	UCHAR InitValAxis[8];	// Initial Value of axes (X, Y, Z, RX, RY, RZ, SL1, SL2)  in %
	UCHAR InitValPov[4];	// Initial Value of POVs in % (0xFF means neutral point)
	UCHAR ButtonMask[16];	// Each bit represents a button
} DEVICE_INIT_VALS, *PDEVICE_INIT_VALS;

//
// driver routine declarations
//
// This type of function declaration is for Prefast for drivers. 
// Because this declaration specifies the function type, PREfast for Drivers
// does not need to infer the type or to report an inference. The declaration
// also prevents PREfast for Drivers from misinterpreting the function type 
// and applying inappropriate rules to the function. For example, PREfast for
// Drivers would not apply rules for completion routines to functions of type
// DRIVER_CANCEL. The preferred way to avoid Warning 28101 is to declare the
// function type explicitly. In the following example, the DriverEntry function
// is declared to be of type DRIVER_INITIALIZE.
//
DRIVER_INITIALIZE DriverEntry;

// Event handler prototypes
#if KMDF_MINOR_VERSION == 005
NTSTATUS
vJoyEvtDeviceAdd(
	IN WDFDRIVER       Driver,
	IN PWDFDEVICE_INIT DeviceInit
	);

VOID
vJoyEvtInternalDeviceControl(
	IN WDFQUEUE     Queue,
	IN WDFREQUEST   Request,
	IN size_t       OutputBufferLength,
	IN size_t       InputBufferLength,
	IN ULONG        IoControlCode
	);

VOID
vJoyEvtDriverContextCleanup(
	IN WDFDRIVER Driver
	);

VOID
vJoyEvtTimerFunction(
	IN WDFTIMER  Timer
	);

VOID
vJoyEvtDeviceContextCleanup(
	IN WDFDEVICE Device
	);

VOID
vJoyEvtIoDeviceControl(
	IN WDFQUEUE     Queue,
	IN WDFREQUEST   Request,
	IN size_t       OutputBufferLength,
	IN size_t       InputBufferLength,
	IN ULONG        IoControlCode
	);

NTSTATUS 
vJoyEvtDeviceReleaseHardware(
	IN  WDFDEVICE    Device,
	IN  WDFCMRESLIST ResourcesTranslated
	);

NTSTATUS 
vJoyEvtDevicePrepareHardware(
	IN  WDFDEVICE    Device,
	IN  WDFCMRESLIST ResourcesRaw,
	IN  WDFCMRESLIST ResourcesTranslated
	);

VOID
vJoyEvtDeviceSelfManagedIoFlush(
	IN  WDFDEVICE    Device
	);

VOID
vJoyEvtDevicePnpStateChange(
	IN  WDFDEVICE                          Device,
	IN  PCWDF_DEVICE_PNP_NOTIFICATION_DATA NotificationData
	);

#else
EVT_WDF_DRIVER_DEVICE_ADD vJoyEvtDeviceAdd;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL vJoyEvtInternalDeviceControl;
EVT_WDF_OBJECT_CONTEXT_CLEANUP vJoyEvtDriverContextCleanup;
EVT_WDF_TIMER vJoyEvtTimerFunction;
//EVT_WDF_DEVICE_CONTEXT_CLEANUP vJoyEvtDeviceContextCleanup;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL vJoyEvtIoDeviceControl;
EVT_WDF_TRACE_CALLBACK WppTraceCallback;
EVT_WDF_DEVICE_RELEASE_HARDWARE  vJoyEvtDeviceReleaseHardware;
EVT_WDF_DEVICE_PREPARE_HARDWARE vJoyEvtDevicePrepareHardware;
EVT_WDF_DEVICE_SELF_MANAGED_IO_FLUSH vJoyEvtDeviceSelfManagedIoFlush;
EVT_WDF_DEVICE_PNP_STATE_CHANGE_NOTIFICATION vJoyEvtDevicePnpStateChange; 

_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
vJoyEvtDeviceContextCleanup(
	_In_ WDFDEVICE Device
	);

#endif

// Function prototypes
NTSTATUS
vJoyGetHidDescriptor(
	IN WDFDEVICE Device,
	IN WDFREQUEST Request
	);

NTSTATUS
vJoyGetReportDescriptor(
	IN WDFDEVICE Device,
	IN WDFREQUEST Request
	);

NTSTATUS
vJoyGetHidString(
	IN WDFREQUEST Request
	);

NTSTATUS
vJoyGetDeviceAttributes(
	IN WDFREQUEST Request
	);

NTSTATUS
HidReadReport_Static(    
	IN WDFREQUEST Request
	);
	
PCHAR
DbgDevicePowerString(
	IN WDF_POWER_DEVICE_STATE Type
	);

NTSTATUS
vJoyConfigContReaderForInterruptEndPoint(
	PDEVICE_EXTENSION DeviceContext
	);

NTSTATUS
vJoyCompleteWriteReport(
	WDFREQUEST request
	);

NTSTATUS
vJoyCompleteReadReport(
	WDFDEVICE	Device,
	UCHAR		id
	);

VOID
vJoyGetPositionData(
	IN HID_INPUT_REPORT_V2	*HidReport, 
	IN DEVICE_EXTENSION		*pDevContext,
	IN UCHAR				id,
	IN size_t				size
	);


PCHAR
DbgHidInternalIoctlString(
	IN ULONG        IoControlCode
	);

#if (OSVER(NTDDI_VERSION) > NTDDI_WIN2K)

NTSTATUS
vJoySendIdleNotification(
	IN WDFREQUEST Request
	);

#endif  //(OSVER(NTDDI_VERSION) > NTDDI_WIN2K)

NTSTATUS
vJoySetFeature(
	IN WDFREQUEST Request
	);


NTSTATUS
vJoyGetFeature(
	IN WDFREQUEST Request
	);

NTSTATUS
vJoyWriteReport(
	IN WDFDEVICE device,
	IN WDFREQUEST Request
	);

NTSTATUS
vJoyGetSwitchState(
	IN WDFDEVICE Device,
	OUT PUCHAR SwitchState
	);

NTSTATUS
SendVendorCommand(
	IN WDFDEVICE Device,
	IN UCHAR VendorCommand,
	IN PUCHAR CommandData
	);


NTSTATUS
vJoyCreateControlDevice(
	WDFDEVICE Device
	);

VOID
vJoyDeleteControlDevice(
	WDFDEVICE Device
	);

VOID
LoadPositions(
	PJOYSTICK_POSITION_V2 pReport,
	PDEVICE_EXTENSION	pDevContext,
	size_t				buffsize
	);

#ifdef PPJOY_MODE
VOID
Convert2NativeFormat(
	PJOYSTICK_POSITION pPosNative, 
	PJOYSTICK_STATE pPosCompat
	);
#endif

int
DeviceCount(
	BOOLEAN Relative,
	int Value
	);

UCHAR
GetDeviceCount(
	PWSTR RegistryPath
	);

NTSTATUS 
GetHwKeyName(
	PWDFDEVICE_INIT  DeviceInit, 
	PWCHAR HwKeyName
	);

NTSTATUS 
GetHwKeySerialNumber(
	PWDFDEVICE_INIT  DeviceInit,
	PLONG SerialNumber
	);

VOID FfbDisableAll(
	PDEVICE_EXTENSION devContext
	);

PVOID
GetReportDescriptor(
	size_t * Size
	);

void 
UpdateHidDescriptor(
	void
	);

void 
UpdateCollections(
	WDFDEVICE Device
	);

void
InitializeDeviceContext(
	PDEVICE_EXTENSION   devContext
	);

void InitializeDefaultDev(
	PDEVICE_EXTENSION   devContext
	);

void InitializeDev(
	PDEVICE_EXTENSION   devContext,
	USHORT Mask,
	BOOLEAN ResetOnly
	);


void CleanUpDev(
	PDEVICE_EXTENSION   devContext
	);

PVOID 
GetReportDescriptorFromRegistry(
	USHORT * Size,
	USHORT * IdMask
	);

unsigned int
GetInitValueFromRegistry(
	USHORT		id,
	PDEVICE_INIT_VALS data_buf
);

void
CalcInitValue(
	USHORT		id,
	PDEVICE_INIT_VALS data_buf
);

PVOID 
ReAllocatePoolWithTag(
	PVOID orig, 
	SIZE_T prevSize, 
	POOL_TYPE PoolType, 
	SIZE_T NumberOfBytes, 
	ULONG Tag
	);

VOID LogEvent(
	NTSTATUS code,
	PWSTR msg,
	PVOID pObj
	);

VOID LogEventWithStatus(
	NTSTATUS code,
	PWSTR msg,
	PVOID pObj,
	NTSTATUS stat
	);

USHORT ParseIdInDescriptor(
	BYTE * desc,
	DWORD size
	);

VOID FfbActiveSet(
	BOOLEAN active,
	int     id,
	PDEVICE_EXTENSION    pDevContext
	);

BOOLEAN FfbActiveGet(
	int     id,
	PDEVICE_EXTENSION    pDevContext
	);

VOID FfbNotifyWrite(
	WDFQUEUE Queue,
	WDFCONTEXT Context
	);

VOID FfbNotifyRead(
	WDFQUEUE Queue,
	WDFCONTEXT Context
	);

VOID FfbTransferData(
	PDEVICE_EXTENSION devContext,
	int 			  id
	);

int FfbGetDevIDfromFfbRequest(
	IN WDFREQUEST   Request
	);

USHORT GetIdFromRawPdoRequest(
	WDFREQUEST    Request,
	PVOID pExtension
	);

NTSTATUS  ResetDevice(
	USHORT	id, 
	PDEVICE_EXTENSION pDevContext
	);

void ResetDeviceControls(
	int id, 
	PDEVICE_EXTENSION pDevContext, 
	PDEVICE_INIT_VALS pdata_buf
	);

#endif   //_HIDUSBFX2_H_
