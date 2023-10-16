// vJoyClient.cpp : Simple feeder application with a FFB demo
//


// Monitor Force Feedback (FFB) vJoy device
#include "stdafx.h"
//#include "Devioctl.h"
#include "public.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "vjoyinterface.h"
#include "Math.h"

// Default device ID (Used when ID not specified)
#define DEV_ID		1

// Prototypes
void  CALLBACK FfbFunction(PVOID data);
void  CALLBACK FfbFunction1(PVOID cb, PVOID data);

BOOL PacketType2Str(FFBPType Type, LPTSTR Str);
BOOL EffectType2Str(FFBEType Ctrl, LPTSTR Str);
BOOL DevCtrl2Str(FFB_CTRL Type, LPTSTR Str);
BOOL EffectOpStr(FFBOP Op, LPTSTR Str);
int  Polar2Deg(BYTE Polar);
int  Byte2Percent(BYTE InByte);
int TwosCompByte2Int(BYTE in);
int TwosCompInt2Int(USHORT in);

int ffb_direction = 0;
int ffb_strenght = 0;
int serial_result = 0;


JOYSTICK_POSITION iReport; // The structure that holds the full position data


int
__cdecl
_tmain(int argc, _TCHAR* argv[])
{
    int stat = 0;
    UINT DevID = DEV_ID;
    USHORT X = 0;
    USHORT Y = 0;
    USHORT Z = 0;
    LONG   Btns = 0;


    PJOYSTICK_POSITION pPositionMessage;
    UINT	IoCode = LOAD_POSITIONS;
    UINT	IoSize = sizeof(JOYSTICK_POSITION);
    // HID_DEVICE_ATTRIBUTES attrib;
    BYTE id = 1;
    UINT iInterface = 1;

    // Define the effect names
    static FFBEType FfbEffect = (FFBEType)-1;
    LPCTSTR FfbEffectName[] =
    { "NONE", "Constant Force", "Ramp", "Square", "Sine", "Triangle", "Sawtooth Up",\
    "Sawtooth Down", "Spring", "Damper", "Inertia", "Friction", "Custom Force" };

    // Set the target Joystick - get it from the command-line 
    if (argc>1)
        DevID = _tstoi(argv[1]);

    // Get the driver attributes (Vendor ID, Product ID, Version Number)
    if (!vJoyEnabled()) {
        _tprintf("Function vJoyEnabled Failed - make sure that vJoy is installed and enabled\n");
        int dummy = getchar();
        stat = -2;
        goto Exit;
    } else {
        wprintf(L"Vendor: %s\nProduct :%s\nVersion Number:%s\n", static_cast<TCHAR*> (GetvJoyManufacturerString()), static_cast<TCHAR*>(GetvJoyProductString()), static_cast<TCHAR*>(GetvJoySerialNumberString()));
    };

    // Get the status of the vJoy device before trying to acquire it
    VjdStat status = GetVJDStatus(DevID);

    switch (status) {
        case VJD_STAT_OWN:
            _tprintf("vJoy device %d is already owned by this feeder\n", DevID);
            break;
        case VJD_STAT_FREE:
            _tprintf("vJoy device %d is free\n", DevID);
            break;
        case VJD_STAT_BUSY:
            _tprintf("vJoy device %d is already owned by another feeder\nCannot continue\n", DevID);
            return -3;
        case VJD_STAT_MISS:
            _tprintf("vJoy device %d is not installed or disabled\nCannot continue\n", DevID);
            return -4;
        default:
            _tprintf("vJoy device %d general error\nCannot continue\n", DevID);
            return -1;
    };

    // Acquire the vJoy device
    if (!AcquireVJD(DevID)) {
        _tprintf("Failed to acquire vJoy device number %d.\n", DevID);
        int dummy = getchar();
        stat = -1;
        goto Exit;
    } else
        _tprintf("Acquired device number %d - OK\n", DevID);



    // Start FFB
#if 1
    BOOL Ffbstarted = FfbStart(DevID);
    if (!Ffbstarted) {
        _tprintf("Failed to start FFB on vJoy device number %d.\n", DevID);
        int dummy = getchar();
        stat = -3;
        goto Exit;
    } else
        _tprintf("Started FFB on vJoy device number %d - OK\n", DevID);

#endif // 1

    // Register Generic callback function
    // At this point you instruct the Receptor which callback function to call with every FFB packet it receives
    // It is the role of the designer to register the right FFB callback function
    FfbRegisterGenCB(FfbFunction1, NULL);




    // Start endless loop
    // The loop injects position data to the vJoy device
    // If it fails it let's the user try again
    //
    // FFB Note:
    // All FFB activity is performed in a separate thread created when registered the callback function   
    while (1) {

        // Set destenition vJoy device
        id = (BYTE)DevID;
        iReport.bDevice = id;

        // Set position data of 3 first axes
        if (Z>35000) Z = 0;
        Z += 200;
        iReport.wAxisZ = Z;
        iReport.wAxisX = 32000-Z;
        iReport.wAxisY = Z/2+7000;

        // Set position data of first 8 buttons
        Btns = 1<<(Z/4000);
        iReport.lButtons = Btns;

        // Send position data to vJoy device
        pPositionMessage = &iReport;
        if (!UpdateVJD(DevID, pPositionMessage)) {
            printf("Feeding vJoy device number %d failed - try to enable device then press enter\n", DevID);
            getchar();
            AcquireVJD(DevID);
        }
        Sleep(2);
    }

Exit:
    RelinquishVJD(DevID);
    return 0;
}


// Generic callback function
void CALLBACK FfbFunction(PVOID data)
{
    FFB_DATA* FfbData = (FFB_DATA*)data;
    int size = FfbData->size;
    _tprintf("\n Cmd:%08.8X ", FfbData->cmd);
    _tprintf("ID:%02.2X ", FfbData->data[0]);
    _tprintf("Size:%02.2d ", static_cast<int>(FfbData->size - 8));
    _tprintf(" - ");
    for (UINT i = 0; i < FfbData->size - 8; i++)
        _tprintf(" %02.2X", (UINT)FfbData->data[i]);
}

void CALLBACK FfbFunction1(PVOID data, PVOID userdata)
{
    // Packet Header
    _tprintf("\n ============= FFB Packet Size %d =============", static_cast<int>(((FFB_DATA*)data)->size));
    if (userdata!=nullptr)
        _tprintf("\n Got userdata ptr=%p", userdata);

    /////// Packet Device ID, and Type Block Index (if exists)
#pragma region Packet Device ID, and Type Block Index
    unsigned int DeviceID, BlockIndex;
    FFBPType	Type;
    TCHAR	TypeStr[100];

    if (ERROR_SUCCESS == Ffb_h_DeviceID((FFB_DATA*)data, &DeviceID))
        _tprintf("\n > Device ID: %d", DeviceID);

    if (ERROR_SUCCESS == Ffb_h_EffectBlockIndex((FFB_DATA*)data, &BlockIndex))
        _tprintf("\n > Effect Block Index: %d", BlockIndex);

    if (ERROR_SUCCESS == Ffb_h_Type((FFB_DATA*)data, &Type)) {
        if (!PacketType2Str(Type, TypeStr))
            _tprintf("\n > Packet Type: %d", Type);
        else
            _tprintf("\n > Packet Type: %s", TypeStr);
        switch (Type) {
            case PT_POOLREP:
                _tprintf("\n > Pool report handled by driver side");
                break;
            case PT_BLKLDREP:
                _tprintf("\n > Block Load report handled by driver side");
                break;
            case PT_BLKFRREP:
                // FreeEffect(BlockIndex)
                _tprintf("\n > Block Free effect id %d", BlockIndex);
                // Read updated PID to get more information from driver side
                FFB_DEVICE_PID PIDBlock;
                FfbReadPID(DeviceID, &PIDBlock);
                _tprintf("\n >>  Freed effect state %d", PIDBlock.EffectStates[BlockIndex-1]);
                _tprintf("\n >>  Next free slot becomes %d", PIDBlock.NextFreeEID);
                _tprintf("\n >>  RAM Pool available %d (total is %d, one element is %d)", PIDBlock.PIDBlockLoad.RAMPoolAvailable, PIDBlock.PIDPool.RAMPoolSize, sizeof(FFB_PID_EFFECT_STATE_REPORT));
                break;
        }
    }
#pragma endregion

#pragma region PID Device Control
    FFB_CTRL	Control;
    TCHAR	CtrlStr[100];
    if (ERROR_SUCCESS == Ffb_h_DevCtrl((FFB_DATA*)data, &Control) && DevCtrl2Str(Control, CtrlStr)) {
        _tprintf("\n >> PID Device Control: %s", CtrlStr);
        switch (Control) {
            case CTRL_DEVRST:
                // device reset
                break;
            case CTRL_ENACT:
                // device enable
                break;
            case CTRL_DISACT:
                // device disable
                break;
            case CTRL_STOPALL:
                // Stop all effects
                break;
        }
    }

#pragma endregion

#pragma region Create New Effect
    FFBEType EffectType;
    unsigned int NewBlockIndex = 0;
    if (ERROR_SUCCESS == Ffb_h_CreateNewEffect((FFB_DATA*)data, &EffectType, &NewBlockIndex)) {
        // Received a Create a New Effect
        if (!EffectType2Str(EffectType, TypeStr))
            _tprintf("\n > Create New Effect Report, Type %02x, Id=%d", EffectType, NewBlockIndex);
        else
            _tprintf("\n > Create New Effect Report, Type %s, Id=%d", TypeStr, NewBlockIndex);
        

        // Read updated PID to get more information from driver side
        FFB_DEVICE_PID PIDBlock;
        if (FfbReadPID(DeviceID, &PIDBlock)) {
            _tprintf("\n >>  PID Block load effect %d", PIDBlock.PIDBlockLoad.EffectBlockIndex);
            _tprintf("\n >>  Next free slot becomes %d", PIDBlock.NextFreeEID);
            _tprintf("\n >>  RAM Pool available %d (total is %d, one element is %d)", PIDBlock.PIDBlockLoad.RAMPoolAvailable, PIDBlock.PIDPool.RAMPoolSize, sizeof(FFB_PID_EFFECT_STATE_REPORT));

            if (NewBlockIndex != PIDBlock.PIDBlockLoad.EffectBlockIndex) {
                _tprintf("\n !!!BUG NewBlockIndex=%d <> PID.PIDBlockLoad=%d", NewBlockIndex, PIDBlock.PIDBlockLoad.EffectBlockIndex);
            }
            _tprintf("\n >> LoadStatus {0}", PIDBlock.PIDBlockLoad.LoadStatus);
        }
    }

#pragma endregion

#pragma region Condition
    FFB_EFF_COND Condition;
    if (ERROR_SUCCESS == Ffb_h_Eff_Cond((FFB_DATA*)data, &Condition)) {
        if (Condition.isY)
            _tprintf("\n >> Y Axis");
        else
            _tprintf("\n >> X Axis");
        _tprintf("\n >> Center Point Offset: %d", TwosCompInt2Int(Condition.CenterPointOffset));
        _tprintf("\n >> Positive Coefficient: %d", TwosCompInt2Int(Condition.PosCoeff));
        _tprintf("\n >> Negative Coefficient: %d", TwosCompInt2Int(Condition.NegCoeff));
        _tprintf("\n >> Positive Saturation: %d", TwosCompInt2Int(Condition.PosSatur));
        _tprintf("\n >> Negative Saturation: %d", TwosCompInt2Int(Condition.NegSatur));
        _tprintf("\n >> Dead Band: %d", TwosCompInt2Int(Condition.DeadBand));
    }
#pragma endregion

    /////// Effect Report
#pragma region Effect Report
    FFB_EFF_CONST Effect;
    if (ERROR_SUCCESS == Ffb_h_Eff_Report((FFB_DATA*)data, &Effect)) {
        if (!EffectType2Str(Effect.EffectType, TypeStr))
            _tprintf("\n >> Effect Report: %02x", Effect.EffectType);
        else
            _tprintf("\n >> Effect Report: %s", TypeStr);
        _tprintf("\n >> AxisEnabledDirection: %d", (unsigned short)Effect.AxesEnabledDirection);

        if (Effect.Polar) {
            _tprintf("\n >> Direction: %d deg (%02x)", Polar2Deg(Effect.Direction), Effect.Direction);
        } else {
            _tprintf("\n >> X Direction: %02x", Effect.DirX);
            _tprintf("\n >> Y Direction: %02x", Effect.DirY);
        };

        if (Effect.Duration == 0xFFFF)
            _tprintf("\n >> Duration: Infinit");
        else
            _tprintf("\n >> Duration: %d MilliSec", static_cast<int>(Effect.Duration));

        if (Effect.TrigerRpt == 0xFFFF)
            _tprintf("\n >> Trigger Repeat: Infinit");
        else
            _tprintf("\n >> Trigger Repeat: %d", static_cast<int>(Effect.TrigerRpt));

        if (Effect.SamplePrd == 0xFFFF)
            _tprintf("\n >> Sample Period: Infinit");
        else
            _tprintf("\n >> Sample Period: %d", static_cast<int>(Effect.SamplePrd));

        if (Effect.StartDelay == 0xFFFF)
            _tprintf("\n >> Start Delay: max");
        else
            _tprintf("\n >> Start Delay: %d", static_cast<int>(Effect.StartDelay));


        _tprintf("\n >> Gain: %d%%", Byte2Percent(Effect.Gain));

    };
#pragma endregion

#pragma region Effect Operation
    FFB_EFF_OP	Operation;
    TCHAR	EffOpStr[100];
    if (ERROR_SUCCESS == Ffb_h_EffOp((FFB_DATA*)data, &Operation) && EffectOpStr(Operation.EffectOp, EffOpStr)) {
        _tprintf("\n >> Effect Operation: %s", EffOpStr);
        if (Operation.LoopCount == 0xFF)
            _tprintf("\n >> Loop until stopped");
        else
            _tprintf("\n >> Loop %d times", static_cast<int>(Operation.LoopCount));
        switch (Operation.EffectOp) {
            case EFF_START:
                // Start the effect identified by the Effect Handle.
                break;
            case EFF_STOP:
                // Stop the effect identified by the Effect Handle.
                break;
            case EFF_SOLO:
                // Start the effect identified by the Effect Handle and stop all other effects.
                break;
        }
    };
#pragma endregion
#pragma region Global Device Gain
    BYTE Gain;
    if (ERROR_SUCCESS == Ffb_h_DevGain((FFB_DATA*)data, &Gain))
        _tprintf("\n >> Global Device Gain: %d", Byte2Percent(Gain));

#pragma endregion
#pragma region Envelope
    FFB_EFF_ENVLP Envelope;
    if (ERROR_SUCCESS == Ffb_h_Eff_Envlp((FFB_DATA*)data, &Envelope)) {
        _tprintf("\n >> Attack Time: %d", static_cast<int>(Envelope.AttackTime));
        _tprintf("\n >> Attack Level: %d", Envelope.AttackLevel);
        _tprintf("\n >> Fade Time: %d", static_cast<int>(Envelope.FadeTime));
        _tprintf("\n >> Fade Level: %d", Envelope.FadeLevel);
    };

#pragma endregion
#pragma region Periodic
    FFB_EFF_PERIOD EffPrd;
    if (ERROR_SUCCESS == Ffb_h_Eff_Period((FFB_DATA*)data, &EffPrd)) {
        _tprintf("\n >> Magnitude: %d", EffPrd.Magnitude);
        _tprintf("\n >> Offset: %d", TwosCompInt2Int(EffPrd.Offset));
        _tprintf("\n >> Phase: %d", EffPrd.Phase * 3600 / 255);
        _tprintf("\n >> Period: %d", static_cast<int>(EffPrd.Period));
    };
#pragma endregion

#pragma region Effect Type
    if (ERROR_SUCCESS == Ffb_h_EffNew((FFB_DATA*)data, &EffectType)) {
        if (EffectType2Str(EffectType, TypeStr))
            _tprintf("\n >> Effect Type: %s", TypeStr);
        else
            _tprintf("\n >> Effect Type: Unknown");
    }

#pragma endregion

#pragma region Ramp Effect
    FFB_EFF_RAMP RampEffect;
    if (ERROR_SUCCESS == Ffb_h_Eff_Ramp((FFB_DATA*)data, &RampEffect)) {
        _tprintf("\n >> Ramp Start: %d", TwosCompInt2Int(RampEffect.Start));
        _tprintf("\n >> Ramp End: %d", TwosCompInt2Int(RampEffect.End));
    };
#pragma endregion

#pragma region Constant Effect
    FFB_EFF_CONSTANT CstEffect;
    if (ERROR_SUCCESS == Ffb_h_Eff_Constant((FFB_DATA*)data, &CstEffect)) {
        _tprintf("\n >> BlockIndex: %d", CstEffect.EffectBlockIndex);
        _tprintf("\n >> Magnitude: %d", TwosCompInt2Int(CstEffect.Magnitude));
    };
#pragma endregion

    FfbFunction(data);
    _tprintf("\n ====================================================\n");

}


// Convert Packet type to String
BOOL PacketType2Str(FFBPType Type, LPTSTR OutStr)
{
    BOOL stat = TRUE;
    LPTSTR Str = "";

    switch (Type) {
        case PT_EFFREP:
            Str = "Effect Report";
            break;
        case PT_ENVREP:
            Str = "Envelope Report";
            break;
        case PT_CONDREP:
            Str = "Condition Report";
            break;
        case PT_PRIDREP:
            Str = "Periodic Report";
            break;
        case PT_CONSTREP:
            Str = "Constant Force Report";
            break;
        case PT_RAMPREP:
            Str = "Ramp Force Report";
            break;
        case PT_CSTMREP:
            Str = "Custom Force Data Report";
            break;
        case PT_SMPLREP:
            Str = "Download Force Sample";
            break;
        case PT_EFOPREP:
            Str = "Effect Operation Report";
            break;
        case PT_BLKFRREP:
            Str = "PID Block Free Report";
            break;
        case PT_CTRLREP:
            Str = "PID Device Control";
            break;
        case PT_GAINREP:
            Str = "Device Gain Report";
            break;
        case PT_SETCREP:
            Str = "Set Custom Force Report";
            break;
        case PT_NEWEFREP:
            Str = "Create New Effect Report";
            break;
        case PT_BLKLDREP:
            Str = "Block Load Report";
            break;
        case PT_POOLREP:
            Str = "PID Pool Report";
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
    LPTSTR Str = "";

    switch (Type) {
        case ET_NONE:
            stat = FALSE;
            break;
        case ET_CONST:
            Str = "Constant Force";
            break;
        case ET_RAMP:
            Str = "Ramp";
            break;
        case ET_SQR:
            Str = "Square";
            break;
        case ET_SINE:
            Str = "Sine";
            break;
        case ET_TRNGL:
            Str = "Triangle";
            break;
        case ET_STUP:
            Str = "Sawtooth Up";
            break;
        case ET_STDN:
            Str = "Sawtooth Down";
            break;
        case ET_SPRNG:
            Str = "Spring";
            break;
        case ET_DMPR:
            Str = "Damper";
            break;
        case ET_INRT:
            Str = "Inertia";
            break;
        case ET_FRCTN:
            Str = "Friction";
            break;
        case ET_CSTM:
            Str = "Custom Force";
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
    LPTSTR Str = "";

    switch (Ctrl) {
        case CTRL_ENACT:
            Str = "Enable Actuators";
            break;
        case CTRL_DISACT:
            Str = "Disable Actuators";
            break;
        case CTRL_STOPALL:
            Str = "Stop All Effects";
            break;
        case CTRL_DEVRST:
            Str = "Device Reset";
            break;
        case CTRL_DEVPAUSE:
            Str = "Device Pause";
            break;
        case CTRL_DEVCONT:
            Str = "Device Continue";
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
    LPTSTR Str = "";

    switch (Op) {
        case EFF_START:
            Str = "Effect Start";
            break;
        case EFF_SOLO:
            Str = "Effect Solo Start";
            break;
        case EFF_STOP:
            Str = "Effect Stop";
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
    return ((UINT)Polar*360)/32767;
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
    BOOL isNeg = in>>7;
    if (isNeg) {
        tmp = (int)(inv);
        tmp = -1*tmp;
        return tmp;
    } else
        return (int)in;
}


// Convert One-Byte 2's complement input to integer
int TwosCompInt2Int(USHORT in)
{
    int tmp;
    SHORT inv = ~in + 1;
    BOOL isNeg = in>>15;
    if (isNeg) {
        tmp = (int)(inv);
        tmp = -1*tmp;
        return tmp;
    } else
        return (int)in;
}
