#ifndef __PPJIOCTL_H__
#define __PPJIOCTL_H__

/* Define to use byte-size values for joystick axes, else dword size */
#undef UCHAR_AXES

#define	PPJOY_AXIS_MIN				1
#ifdef UCHAR_AXES
#define	PPJOY_AXIS_MAX				127
#else
#define	PPJOY_AXIS_MAX				32767
#endif

#define FILE_DEVICE_PPORTJOY			FILE_DEVICE_UNKNOWN

#define PPORTJOY_IOCTL(_index_)	\
	CTL_CODE (FILE_DEVICE_PPORTJOY, _index_, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PPORTJOY_SET_STATE		PPORTJOY_IOCTL (0x0)

#define	JOYSTICK_STATE_V1	0x53544143

typedef struct
{
 ULONG	Version;
 UCHAR	Data[1];
} JOYSTICK_SET_STATE, *PJOYSTICK_SET_STATE;

#define	NUM_ANALOG	8		/* Number of analog values which we will provide */
#define	NUM_DIGITAL	8		/* Number of digital values which we will provide */


#pragma pack(push,1)		/* All fields in structure must be byte aligned. */
typedef struct
{
 unsigned long	Signature;				/* Signature to identify packet to PPJoy IOCTL */
 char			NumAnalog;				/* Num of analog values we pass */
 long			Analog[NUM_ANALOG];		/* Analog values */
 char			NumDigital;				/* Num of digital values we pass */
 char			Digital[NUM_DIGITAL];	/* Digital values */
}	JOYSTICK_STATE, *PJOYSTICK_STATE;
#pragma pack(pop)


#endif
