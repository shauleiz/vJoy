/*
	Based on:
	http://www.microchip.com/forums/tm.aspx?m=320225&mpage=2
	Post number 35
*/

// Generic defines
#include "public.h"

#define DEV				1
#define DEV_GEN			1	// The non-FFB Joystick part
#define DEV_FFB			0	// The FFB Joystick part
#define DEV_ADDITIONAL	0	// The additional part inside the non-FFB part

/*
	Input
	Collection Application:
	Joystick
	ID:	1
	X:			10bit
	Padding:	6bit
	Y:			6bit
	Padding:	2bit
	Rz:			6bit
	Padding:	2bit
	Buttons:	8X1bit
	-------------------------
	Usage Undefined:	1bit
	Padding:			7bit

	
*/
#define N_COLLECTIONS	1

char G_DefaultReportDescriptor[] = {
#define TLID 0x1 // Top Level ID
#include "hidReportDescSingle.h"
#if 0
#undef TLID
#define TLID 0x2 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0x3 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0x4 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0x5 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0x6 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0x7 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0x8 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0x9 // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0xA // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0xB // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0xC // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0xD // Top Level ID
#include "hidReportDescSingle.h"
#undef TLID
#define TLID 0xE // Top Level ID
#include "hidReportDescSingle.h"
#undef DEV_FFB
#define DEV_FFB 0
#undef TLID
#define TLID 0xF // Top Level ID
#include "hidReportDescSingle.h"    
#endif // 0

 };
