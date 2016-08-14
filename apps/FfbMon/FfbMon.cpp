// FfbMon.cpp : Defines the entry point for the console application.
//

// Monitor Force Feedback (FFB) vJoy device
#include "stdafx.h"
#include "Devioctl.h"
#include "..\..\inc\public.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "..\..\inc\vjoyinterface.h"
#include "FfbMon.h"
#include "Math.h"

using namespace vJoy;

int ffb_direction = 0;
int ffb_strenght = 0;
int serial_result = 0;


JOYSTICK_POSITION_V2 iReport; // The structure that holds the full position data

int
__cdecl
_tmain(int argc, _TCHAR* argv[])
{
int stat = 0;
UINT DevID = DEV_ID;
USHORT Z = 0;

PVOID pPositionMessage;
UINT	IoCode = LOAD_POSITIONS;
UINT	IoSize = sizeof(JOYSTICK_POSITION);
// HID_DEVICE_ATTRIBUTES attrib;
BYTE id = 1;
UINT iInterface = 1;


// Set the target Joystick

if (argc>1)
DevID = _tstoi(argv[1]);

// Get the driver attributes (Vendor ID, Product ID, Version Number)
if (!vJoyEnabled())
{
_tprintf(L"Function vJoyEnabled Failed\n");
int dummy = getchar();
stat = - 2;
goto Exit;
}
else
{
_tprintf(L"Vendor: %s\nProduct :%s\nVersion Number:%s\n", static_cast<TCHAR *> (GetvJoyManufacturerString()), static_cast<TCHAR *>(GetvJoyProductString()), static_cast<TCHAR *>(GetvJoySerialNumberString()));
};

// Get FFB device
if (!AcquireVJD(DevID))
{
_tprintf(L"Failed to acquire vJoy device number %d.\n", DevID);
int dummy = getchar();
stat = -1;
goto Exit;
}
else
_tprintf(L"Acquisita vJoy device numero %d - OK\n", DevID);

VjdStat status = GetVJDStatus(DevID);

switch (status)
{
case VJD_STAT_OWN:
_tprintf(L"vJoy device %d is  owned by this feeder\n", DevID);
break;
case VJD_STAT_FREE:
_tprintf(L"vJoy device %d is free\n", DevID);
break;
case VJD_STAT_BUSY:
_tprintf(L"vJoy device %d is already owned by another feeder\nCannot continue\n", DevID);
return -3;
case VJD_STAT_MISS:
_tprintf(L"vJoy device %d is not installed or disabled\nCannot continue\n", DevID);
return -4;
default:
_tprintf(L"vJoy device %d general error\nCannot continue\n", DevID);
return -1;
};


// Acquire the target
if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(DevID))))
{
_tprintf(L"Did not acquire vJoy device number %d.\n", DevID);
// return -1;
}
else
{
_tprintf(L"Acquired: vJoy device number %d.\n", DevID);
}


// Start FFB
#pragma warning( push )
#pragma warning( disable : 4996 )
BOOL Ffbstarted = FfbStart(DevID);
if (!Ffbstarted)
{
_tprintf(L"Failed to start FFB on vJoy device number %d.\n", DevID);
int dummy = getchar();
stat = -3;
goto Exit;
}
else
_tprintf(L"Started FFB on vJoy device number %d - OK\n", DevID); 
#pragma warning( pop )


// Register Generic callback function
FfbRegisterGenCB(FfbFunction1, &DevID);


// Prepare for polling
static FFBEType FfbEffect= (FFBEType)-1;
LPCTSTR FfbEffectName[] =
{L"NONE", L"Constant Force", L"Ramp", L"Square", L"Sine", L"Triangle", L"Sawtooth Up",\
L"Sawtooth Down", L"Spring", L"Damper", L"Inertia", L"Friction", L"Custom Force"};


while (1)

{

	id = (BYTE)DevID;
	iReport.bDevice = id;

	Sleep(2);

	Z += 350;

	iReport.wAxisZ = Z;

	pPositionMessage = (PVOID)(&iReport);

	if (!UpdateVJD(DevID, pPositionMessage))
	{
		printf("Feeding vJoy device number %d failed - try to enable device then press enter\n", DevID);
		int ch = getchar();
		AcquireVJD(DevID);
	}
}

Exit:
RelinquishVJD(DevID);
return 0;
}


// Generic callback function
void CALLBACK FfbFunction(PVOID data)
{
FFB_DATA * FfbData = (FFB_DATA *)data;
int size = FfbData->size;
_tprintf(L"\nFFB Size %d\n", size);

_tprintf(L"Cmd:%08.8X ", FfbData->cmd);
_tprintf(L"ID:%02.2X ", FfbData->data[0]);
_tprintf(L"Size:%02.2d ", static_cast<int>(FfbData->size - 8));
_tprintf(L" - ");
for (UINT i = 0; i < FfbData->size - 8; i++)
_tprintf(L" %02.2X", (UINT)FfbData->data[i]);
_tprintf(L"\n");
}

void CALLBACK FfbFunction1(PVOID data, PVOID userdata)
{
// Packet Header
_tprintf(L"\n ============= FFB Packet size Size %d =============\n", static_cast<int>(((FFB_DATA *)data)->size));

/////// Packet Device ID, and Type Block Index (if exists)
#pragma region Packet Device ID, and Type Block Index
int DeviceID, BlockIndex;
FFBPType	Type;
TCHAR	TypeStr[100];

if (ERROR_SUCCESS == Ffb_h_DeviceID((FFB_DATA *)data, &DeviceID))
_tprintf(L"\n > Device ID: %d", DeviceID);
if (ERROR_SUCCESS == Ffb_h_Type((FFB_DATA *)data, &Type))
{
if (!PacketType2Str(Type, TypeStr))
_tprintf(L"\n > Packet Type: %d", Type);
else
_tprintf(L"\n > Packet Type: %s", TypeStr);

}
if (ERROR_SUCCESS == Ffb_h_EBI((FFB_DATA *)data, &BlockIndex))
_tprintf(L"\n > Effect Block Index: %d", BlockIndex);

#pragma endregion


/////// Effect Report
#pragma region Effect Report
#pragma warning( push )
#pragma warning( disable : 4996 )
FFB_EFF_CONST Effect;
if (ERROR_SUCCESS == Ffb_h_Eff_Const((FFB_DATA *)data, &Effect))
{
if (!EffectType2Str(Effect.EffectType, TypeStr))
_tprintf(L"\n >> Effect Report: %02x", Effect.EffectType);
else
_tprintf(L"\n >> Effect Report: %s", TypeStr);
#pragma warning( push )

if (Effect.Polar)
{
_tprintf(L"\n >> Direction: %d deg (%02x)", Polar2Deg(Effect.Direction), Effect.Direction);


}
else
{
_tprintf(L"\n >> X Direction: %02x", Effect.DirX);
_tprintf(L"\n >> Y Direction: %02x", Effect.DirY);
};

if (Effect.Duration == 0xFFFF)
_tprintf(L"\n >> Duration: Infinit");
else
_tprintf(L"\n >> Duration: %d MilliSec", static_cast<int>(Effect.Duration));

if (Effect.TrigerRpt == 0xFFFF)
_tprintf(L"\n >> Trigger Repeat: Infinit");
else
_tprintf(L"\n >> Trigger Repeat: %d", static_cast<int>(Effect.TrigerRpt));

if (Effect.SamplePrd == 0xFFFF)
_tprintf(L"\n >> Sample Period: Infinit");
else
_tprintf(L"\n >> Sample Period: %d", static_cast<int>(Effect.SamplePrd));


_tprintf(L"\n >> Gain: %d%%", Byte2Percent(Effect.Gain));

};
#pragma endregion
#pragma region PID Device Control
FFB_CTRL	Control;
TCHAR	CtrlStr[100];
if (ERROR_SUCCESS == Ffb_h_DevCtrl((FFB_DATA *)data, &Control) && DevCtrl2Str(Control, CtrlStr))
_tprintf(L"\n >> PID Device Control: %s", CtrlStr);

#pragma endregion
#pragma region Effect Operation
FFB_EFF_OP	Operation;
TCHAR	EffOpStr[100];
if (ERROR_SUCCESS == Ffb_h_EffOp((FFB_DATA *)data, &Operation) && EffectOpStr(Operation.EffectOp, EffOpStr))
{
_tprintf(L"\n >> Effect Operation: %s", EffOpStr);
if (Operation.LoopCount == 0xFF)
_tprintf(L"\n >> Loop until stopped");
else
_tprintf(L"\n >> Loop %d times", static_cast<int>(Operation.LoopCount));

};
#pragma endregion
#pragma region Global Device Gain
BYTE Gain;
if (ERROR_SUCCESS == Ffb_h_DevGain((FFB_DATA *)data, &Gain))
_tprintf(L"\n >> Global Device Gain: %d", Byte2Percent(Gain));

#pragma endregion
#pragma region Condition
FFB_EFF_COND Condition;
if (ERROR_SUCCESS == Ffb_h_Eff_Cond((FFB_DATA *)data, &Condition))
{
if (Condition.isY)
_tprintf(L"\n >> Y Axis");
else
_tprintf(L"\n >> X Axis");
_tprintf(L"\n >> Center Point Offset: %d", TwosCompWord2Int((WORD)Condition.CenterPointOffset)/**10000/127*/);
_tprintf(L"\n >> Positive Coefficient: %d", TwosCompWord2Int((WORD)Condition.PosCoeff)/**10000/127*/);
_tprintf(L"\n >> Negative Coefficient: %d", TwosCompWord2Int((WORD)Condition.NegCoeff)/**10000/127*/);
_tprintf(L"\n >> Positive Saturation: %d", Condition.PosSatur/**10000/255*/);
_tprintf(L"\n >> Negative Saturation: %d", Condition.NegSatur/**10000/255*/);
_tprintf(L"\n >> Dead Band: %d", Condition.DeadBand/**10000/255*/);
}
#pragma endregion
#pragma region Envelope
FFB_EFF_ENVLP Envelope;
if (ERROR_SUCCESS == Ffb_h_Eff_Envlp((FFB_DATA *)data, &Envelope))
{
	_tprintf(L"\n >> Attack Level: %d", TwosCompWord2Int((WORD)Envelope.AttackLevel));
	_tprintf(L"\n >> Fade Level: %d", TwosCompWord2Int((WORD)Envelope.FadeLevel));
	_tprintf(L"\n >> Attack Time: %d", static_cast<int>(Envelope.AttackTime));
	_tprintf(L"\n >> Fade Time: %d", static_cast<int>(Envelope.FadeTime));
};

#pragma endregion
#pragma region Periodic
FFB_EFF_PERIOD EffPrd;
if (ERROR_SUCCESS == Ffb_h_Eff_Period((FFB_DATA *)data, &EffPrd))
{
_tprintf(L"\n >> Magnitude: %d", EffPrd.Magnitude );
_tprintf(L"\n >> Offset: %d", TwosCompWord2Int(static_cast<WORD>(EffPrd.Offset)));
_tprintf(L"\n >> Phase: %d", EffPrd.Phase);
_tprintf(L"\n >> Period: %d", static_cast<int>(EffPrd.Period));
};
#pragma endregion

#pragma region Effect Type
FFBEType EffectType;
if (ERROR_SUCCESS == Ffb_h_EffNew((FFB_DATA *)data, &EffectType))
{
if (EffectType2Str(EffectType, TypeStr))
_tprintf(L"\n >> Effect Type: %s", TypeStr);
else
_tprintf(L"\n >> Effect Type: Unknown");
}

#pragma endregion

#pragma region Ramp Effect
FFB_EFF_RAMP RampEffect;
if (ERROR_SUCCESS == Ffb_h_Eff_Ramp((FFB_DATA *)data, &RampEffect))
{
	_tprintf(L"\n >> Ramp Start: %d", TwosCompWord2Int((WORD)RampEffect.Start) /** 10000 / 127*/);
	_tprintf(L"\n >> Ramp End: %d", TwosCompWord2Int((WORD)RampEffect.End) /** 10000 / 127*/);
};

#pragma endregion

#pragma region Constant Effect
FFB_EFF_CONSTANT ConstantEffect;
if (ERROR_SUCCESS == Ffb_h_Eff_Constant((FFB_DATA *)data, &ConstantEffect))
{
	_tprintf(L"\n >> Constant Magnitude: %d", TwosCompWord2Int((WORD)ConstantEffect.Magnitude));
};

#pragma endregion

_tprintf(L"\n");
FfbFunction(data);
_tprintf(L"\n ====================================================\n");

}


// Convert Packet type to String
BOOL PacketType2Str(FFBPType Type, LPTSTR OutStr)
{
BOOL stat = TRUE;
LPTSTR Str=L"";

switch (Type)
{
case PT_EFFREP:
Str = L"Effect Report";
break;
case PT_ENVREP:
Str = L"Envelope Report";
break;
case PT_CONDREP:
Str = L"Condition Report";
break;
case PT_PRIDREP:
Str = L"Periodic Report";
break;
case PT_CONSTREP:
Str = L"Constant Force Report";
break;
case PT_RAMPREP:
Str = L"Ramp Force Report";
break;
case PT_CSTMREP:
Str = L"Custom Force Data Report";
break;
case PT_SMPLREP:
Str = L"Download Force Sample";
break;
case PT_EFOPREP:
Str = L"Effect Operation Report";
break;
case PT_BLKFRREP:
Str = L"PID Block Free Report";
break;
case PT_CTRLREP:
Str = L"PID Device Control";
break;
case PT_GAINREP:
Str = L"Device Gain Report";
break;
case PT_SETCREP:
Str = L"Set Custom Force Report";
break;
case PT_NEWEFREP:
Str = L"Create New Effect Report";
break;
case PT_BLKLDREP:
Str = L"Block Load Report";
break;
case PT_POOLREP:
Str = L"PID Pool Report";
break;
default:
stat = FALSE;
break;
}

if (stat)
_tcscpy_s(OutStr, 100, Str);

return stat;
}

// Convert Effect type to String
BOOL EffectType2Str(FFBEType Type, LPTSTR OutStr)
{
BOOL stat = TRUE;
LPTSTR Str=L"";

switch (Type)
{
case ET_NONE:
stat = FALSE;
break;
case ET_CONST:
Str=L"Constant Force";
break;
case ET_RAMP:
Str=L"Ramp";
break;
case ET_SQR:
Str=L"Square";
break;
case ET_SINE:
Str=L"Sine";
break;
case ET_TRNGL:
Str=L"Triangle";
break;
case ET_STUP:
Str=L"Sawtooth Up";
break;
case ET_STDN:
Str=L"Sawtooth Down";
break;
case ET_SPRNG:
Str=L"Spring";
break;
case ET_DMPR:
Str=L"Damper";
break;
case ET_INRT:
Str=L"Inertia";
break;
case ET_FRCTN:
Str=L"Friction";
break;
case ET_CSTM:
Str=L"Custom Force";
break;
default:
stat = FALSE;
break;
};

if (stat)
_tcscpy_s(OutStr, 100, Str);

return stat;
}

// Convert PID Device Control to String
BOOL DevCtrl2Str(FFB_CTRL Ctrl, LPTSTR OutStr)
{
BOOL stat = TRUE;
LPTSTR Str=L"";

switch (Ctrl)
{
case CTRL_ENACT:
Str=L"Enable Actuators";
break;
case CTRL_DISACT:
Str=L"Disable Actuators";
break;
case CTRL_STOPALL:
Str=L"Stop All Effects";
break;
case CTRL_DEVRST:
Str=L"Device Reset";
break;
case CTRL_DEVPAUSE:
Str=L"Device Pause";
break;
case CTRL_DEVCONT:
Str=L"Device Continue";
break;
default:
stat = FALSE;
break;
}
if (stat)
_tcscpy_s(OutStr, 100, Str);

return stat;
}

// Convert Effect operation to string
BOOL EffectOpStr(FFBOP Op, LPTSTR OutStr)
{
BOOL stat = TRUE;
LPTSTR Str=L"";

switch (Op)
{
case EFF_START:
Str=L"Effect Start";
break;
case EFF_SOLO:
Str=L"Effect Solo Start";
break;
case EFF_STOP:
Str=L"Effect Stop";
break;
default:
stat = FALSE;
break;
}

if (stat)
_tcscpy_s(OutStr, 100, Str);

return stat;
}

// Polar values (0x00-0xFF) to Degrees (0-360)
int Polar2Deg(BYTE Polar)
{
return ((UINT)Polar*360)/255;
}

// Convert range 0x00-0xFF to 0%-100%
int Byte2Percent(BYTE InByte)
{
return ((UINT)InByte*100)/255;
}

// Convert One-Byte 2's complement input to integer
int TwosCompByte2Int(BYTE in)
{
	int tmp;
	BYTE inv = ~in;
	BOOL isNeg = in >> 7;
	if (isNeg)
	{
		tmp = (int)(inv);
		tmp = -1 * tmp;
		return tmp;
	}
	else
		return (int)in;
}

// Convert One-Byte 2's complement input to integer
int TwosCompWord2Int(WORD in)
{
	int tmp;
	WORD inv = ~in;
	BOOL isNeg = in >> 15;
	if (isNeg)
	{
		tmp = (int)(inv);
		tmp = -1 * tmp;
		return tmp-1;
	}
	else
		return (int)in;
}
