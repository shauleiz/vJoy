#pragma once

#include "resource.h"

// HID Descriptor definitions
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

/* These macros are used to build the report by adding a byte/word at a time*/
#ifndef	 NEXT_BYTE
#define NEXT_BYTE(vReport,Data)   vReport.push_back(Data);
#endif

#ifndef	 NEXT_SHORT
#define NEXT_SHORT(vReport,Data)  vReport.push_back(Data&0xFF); vReport.push_back((Data&0xFF00)>>8);
#endif

// Internal messages
#define WM_VJOYCHANGED	WM_USER+1

// Timer ID
#define TIMER_ID 24553
