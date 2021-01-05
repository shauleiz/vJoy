using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

public enum HID_USAGES : UInt32
{
    HID_USAGE_X = 0x30,
    HID_USAGE_Y = 0x31,
    HID_USAGE_Z = 0x32,
    HID_USAGE_RX = 0x33,
    HID_USAGE_RY = 0x34,
    HID_USAGE_RZ = 0x35,
    HID_USAGE_SL0 = 0x36,
    HID_USAGE_SL1 = 0x37,
    HID_USAGE_WHL = 0x38,
    HID_USAGE_POV = 0x39,
    HID_USAGE_ACCELERATOR = 0xC4,
    HID_USAGE_BRAKE = 0xC5,
    HID_USAGE_CLUTCH = 0xC6,
    HID_USAGE_STEERING = 0xC8,
    HID_USAGE_AILERON = 0xB0,
    HID_USAGE_RUDDER = 0xBA,
    HID_USAGE_THROTTLE = 0xBB,
}

public enum VjdStat : UInt32 /* Declares an enumeration data type called BOOLEAN */
{
    VJD_STAT_OWN,	// The  vJoy Device is owned by this application.
    VJD_STAT_FREE,	// The  vJoy Device is NOT owned by any application (including this one).
    VJD_STAT_BUSY,	// The  vJoy Device is owned by another application. It cannot be acquired by this application.
    VJD_STAT_MISS,	// The  vJoy Device is missing. It either does not exist or the driver is down.
    VJD_STAT_UNKN	// Unknown
};


// FFB Declarations

// HID Descriptor definitions - FFB Report IDs

public enum FFBPType : UInt32 // FFB Packet Type
{
    // Write
    PT_EFFREP = 0x01,   // Usage Set Effect Report
    PT_ENVREP = 0x02,   // Usage Set Envelope Report
    PT_CONDREP = 0x03,  // Usage Set Condition Report
    PT_PRIDREP = 0x04,  // Usage Set Periodic Report
    PT_CONSTREP = 0x05, // Usage Set Constant Force Report
    PT_RAMPREP = 0x06,  // Usage Set Ramp Force Report
    PT_CSTMREP = 0x07,  // Usage Custom Force Data Report
    PT_SMPLREP = 0x08,  // Usage Download Force Sample
    PT_EFOPREP = 0x0A,  // Usage Effect Operation Report
    PT_BLKFRREP = 0x0B, // Usage PID Block Free Report
    PT_CTRLREP = 0x0C,  // Usage PID Device Control
    PT_GAINREP = 0x0D,  // Usage Device Gain Report
    PT_SETCREP = 0x0E,  // Usage Set Custom Force Report

    // Feature
    PT_NEWEFREP = 0x01+0x10,    // Usage Create New Effect Report
    PT_BLKLDREP = 0x02+0x10,    // Usage Block Load Report
    PT_POOLREP = 0x03+0x10,     // Usage PID Pool Report
    PT_STATEREP = 0x04+0x10,    // Usage PID State Report
};

public enum FFBEType : UInt32 // FFB Effect Type
{

    // Effect Type
    ET_NONE = 0,      //    No Force
    ET_CONST = 1,    //    Constant Force
    ET_RAMP = 2,    //    Ramp
    ET_SQR = 3,    //    Square
    ET_SINE = 4,    //    Sine
    ET_TRNGL = 5,    //    Triangle
    ET_STUP = 6,    //    Sawtooth Up
    ET_STDN = 7,    //    Sawtooth Down
    ET_SPRNG = 8,    //    Spring
    ET_DMPR = 9,    //    Damper
    ET_INRT = 10,   //    Inertia
    ET_FRCTN = 11,   //    Friction
    ET_CSTM = 12,   //    Custom Force Data
};

public enum FFB_CTRL : UInt32
{
    CTRL_ENACT = 1,	// Enable all device actuators.
    CTRL_DISACT = 2,	// Disable all the device actuators.
    CTRL_STOPALL = 3,	// Stop All Effects­ Issues a stop on every running effect.
    CTRL_DEVRST = 4,	// Device Reset– Clears any device paused condition, enables all actuators and clears all effects from memory.
    CTRL_DEVPAUSE = 5,	// Device Pause– The all effects on the device are paused at the current time step.
    CTRL_DEVCONT = 6,	// Device Continue– The all effects that running when the device was paused are restarted from their last time step.
};

public enum FFBOP : UInt32
{
    EFF_START = 1, // EFFECT START
    EFF_SOLO = 2, // EFFECT SOLO START
    EFF_STOP = 3, // EFFECT STOP
};

namespace vJoyInterfaceWrap
{
    public class vJoy
    {

        /***************************************************/
        /*********** Various declarations ******************/
        /***************************************************/
        private static RemovalCbFunc UserRemCB;
        private static WrapRemovalCbFunc wrf;
        private static GCHandle hRemUserData;


        private static FfbCbFunc UserFfbCB;
        private static WrapFfbCbFunc wf;
        private static GCHandle hFfbUserData;

        #region Mapping vJoyInterface DLL with 4-bytes aligned structs

        [StructLayout(LayoutKind.Sequential)]
        public struct JoystickState
        {
            public byte bDevice;
            public Int32 Throttle;
            public Int32 Rudder;
            public Int32 Aileron;

            public Int32 AxisX;
            public Int32 AxisY;
            public Int32 AxisZ;
            public Int32 AxisXRot;
            public Int32 AxisYRot;
            public Int32 AxisZRot;
            public Int32 Slider;
            public Int32 Dial;

            public Int32 Wheel;
            public Int32 Accelerator;
            public Int32 Brake;
            public Int32 Clutch;
            public Int32 Steering;

            public Int32 AxisVX;
            public Int32 AxisVY;

            public UInt32 Buttons;
            public UInt32 bHats;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
            public UInt32 bHatsEx1;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
            public UInt32 bHatsEx2;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch
            public UInt32 bHatsEx3;	// Lower 4 bits: HAT switch or 16-bit of continuous HAT switch

            public UInt32 ButtonsEx1;
            public UInt32 ButtonsEx2;
            public UInt32 ButtonsEx3;

            public Int32 AxisVZ;
            public Int32 AxisVBRX;
            public Int32 AxisVBRY;
            public Int32 AxisVBRZ;
        };

        [StructLayout(LayoutKind.Explicit)]
        private struct FFB_DATA
        {
            [FieldOffset(0)]
            private UInt32 size;
            [FieldOffset(4)]
            private UInt32 cmd;
            [FieldOffset(8)]
            private IntPtr data;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_CONSTANT
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public Int16 Magnitude;
        }

        [System.Obsolete("use FFB_EFF_REPORT")]
        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_CONST
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public FFBEType EffectType;
            [FieldOffset(8)]
            public UInt16 Duration;// Value in milliseconds. 0xFFFF means infinite
            [FieldOffset(10)]
            public UInt16 TrigerRpt;
            [FieldOffset(12)]
            public UInt16 SamplePrd;
            [FieldOffset(14)]
            public UInt16 StartDelay;
            [FieldOffset(16)]
            public Byte Gain;
            [FieldOffset(17)]
            public Byte TrigerBtn;
            [FieldOffset(18)]
            public Byte AxesEnabledDirection;
            [FieldOffset(20)]
            public bool Polar; // How to interpret force direction Polar (0-360°) or Cartesian (X,Y)
            [FieldOffset(24)]
            public UInt16 Direction; // Polar direction: (0x00-0x7FFF correspond to 0-360°)
            [FieldOffset(24)]
            public UInt16 DirX; // X direction: Positive values are To the right of the center (X); Negative are Two's complement
            [FieldOffset(26)]
            public UInt16 DirY; // Y direction: Positive values are below the center (Y); Negative are Two's complement
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_REPORT
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public FFBEType EffectType;
            [FieldOffset(8)]
            public UInt16 Duration;// Value in milliseconds. 0xFFFF means infinite
            [FieldOffset(10)]
            public UInt16 TrigerRpt;
            [FieldOffset(12)]
            public UInt16 SamplePrd;
            [FieldOffset(14)]
            public UInt16 StartDelay;
            [FieldOffset(16)]
            public Byte Gain;
            [FieldOffset(17)]
            public Byte TrigerBtn;
            [FieldOffset(18)]
            public Byte AxesEnabledDirection;
            [FieldOffset(20)]
            public bool Polar; // How to interpret force direction Polar (0-360°) or Cartesian (X,Y)
            [FieldOffset(24)]
            public UInt16 Direction; // Polar direction: (0x00-0x7FFF correspond to 0-360°)
            [FieldOffset(24)]
            public UInt16 DirX; // X direction: Positive values are To the right of the center (X); Negative are Two's complement
            [FieldOffset(26)]
            public UInt16 DirY; // Y direction: Positive values are below the center (Y); Negative are Two's complement
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_OP
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public FFBOP EffectOp;
            [FieldOffset(8)]
            public Byte LoopCount;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_COND
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public bool isY;
            [FieldOffset(8)]
            public Int16 CenterPointOffset; // CP Offset: Range 0x80­0x7F (­10000 ­ 10000)
            [FieldOffset(12)]
            public Int16 PosCoeff; // Positive Coefficient: Range 0x80­0x7F (­10000 ­ 10000)
            [FieldOffset(16)]
            public Int16 NegCoeff; // Negative Coefficient: Range 0x80­0x7F (­10000 ­ 10000)
            [FieldOffset(20)]
            public UInt32 PosSatur; // Positive Saturation: Range 0x00­0xFF (0 – 10000)
            [FieldOffset(24)]
            public UInt32 NegSatur; // Negative Saturation: Range 0x00­0xFF (0 – 10000)
            [FieldOffset(28)]
            public Int32 DeadBand; // Dead Band: : Range 0x00­0xFF (0 – 10000)
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_ENVLP
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public UInt16 AttackLevel;
            [FieldOffset(8)]
            public UInt16 FadeLevel;
            [FieldOffset(12)]
            public UInt32 AttackTime;
            [FieldOffset(16)]
            public UInt32 FadeTime;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_PERIOD
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public UInt32 Magnitude;
            [FieldOffset(8)]
            public Int16 Offset;
            [FieldOffset(12)]
            public UInt32 Phase;
            [FieldOffset(16)]
            public UInt32 Period;
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct FFB_EFF_RAMP
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;
            [FieldOffset(4)]
            public Int16 Start;             // The Normalized magnitude at the start of the effect
            [FieldOffset(8)]
            public Int16 End;               // The Normalized magnitude at the end of the effect
        }
        #endregion

        #region Mapping to vJoy's driver with 1-byte packed structs - must match driver version!
        public const int VJOY_FFB_FIRST_EFFECT_ID = (1);
        // See VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX in public.h
        public const int VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX = (100);
        public const int VJOY_FFB_MAX_SIMULTANEOUS_EFFECTS = (10);
        public const int VJOY_FFB_EFFECT_FREE = (0x00);
        public const int VJOY_FFB_EFFECT_ALLOCATED = (0x01);

        // All structures are packed with 1 byte alignment
        [StructLayout(LayoutKind.Explicit, Size = 4)]
        public struct FFB_PID_BLOCK_LOAD_REPORT
        {
            [FieldOffset(0)]
            public Byte EffectBlockIndex;	// 1..100 (max is VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX)
            [FieldOffset(1)]
            public Byte LoadStatus;	        // 0 ongoing, 1=Success,2=Full,3=Error
            [FieldOffset(2)]
            public UInt16 RAMPoolAvailable;	// =0 if full, or sizeof(FFB_PID_EFFECT_STATE_REPORT) * (100 - created)
        }

        [StructLayout(LayoutKind.Explicit, Size = 4)]
        public struct FFB_PID_POOL_REPORT
        {
            [FieldOffset(0)]
            public UInt16 RAMPoolSize;	// 0xFFFF
            [FieldOffset(2)]
            public Byte MaxSimultaneousEffects;	// 10
            [FieldOffset(3)]
            public Byte MemoryManagement;	// Bits: 0=DeviceManagedPool, 1=SharedParameterBlocks
        }

        [StructLayout(LayoutKind.Explicit, Size = 2)]
        public struct FFB_PID_EFFECT_STATE_REPORT
        {
            [FieldOffset(0)]
            public Byte PIDEffectStateReport;
            [FieldOffset(1)]
            public Byte State;
        }

        [StructLayout(LayoutKind.Explicit, Size = 10+(2*VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX))]
        public struct FFB_DEVICE_PID
        {
            [FieldOffset(0)]
            public FFB_PID_BLOCK_LOAD_REPORT PIDBlockLoad;
            [FieldOffset(4)]
            public FFB_PID_POOL_REPORT PIDPool;
            [FieldOffset(8)]
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX)]
            public FFB_PID_EFFECT_STATE_REPORT[] EffectStates;
            [FieldOffset(8+2*VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX)]
            public Byte NextFreeEID;
            [FieldOffset(8+2*VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX+1)]
            public Byte LastEID;
        }
        #endregion


        /***************************************************/
        /***** Import from file vJoyInterface.dll (C) ******/
        /***************************************************/

        /////	General driver data
        [DllImport("vJoyInterface.dll", EntryPoint = "GetvJoyVersion", CallingConvention = CallingConvention.Cdecl)]
        private static extern short _GetvJoyVersion();

        [DllImport("vJoyInterface.dll", EntryPoint = "vJoyEnabled", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _vJoyEnabled();

        [DllImport("vJoyInterface.dll", EntryPoint = "GetvJoyProductString", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr _GetvJoyProductString();

        [DllImport("vJoyInterface.dll", EntryPoint = "GetvJoyManufacturerString", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr _GetvJoyManufacturerString();

        [DllImport("vJoyInterface.dll", EntryPoint = "GetvJoySerialNumberString", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr _GetvJoySerialNumberString();

        [DllImport("vJoyInterface.dll", EntryPoint = "DriverMatch", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _DriverMatch(ref UInt32 DllVer, ref UInt32 DrvVer);

        /////	vJoy Device properties
        [DllImport("vJoyInterface.dll", EntryPoint = "GetVJDButtonNumber", CallingConvention = CallingConvention.Cdecl)]
        private static extern int _GetVJDButtonNumber(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetVJDDiscPovNumber", CallingConvention = CallingConvention.Cdecl)]
        private static extern int _GetVJDDiscPovNumber(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetVJDContPovNumber", CallingConvention = CallingConvention.Cdecl)]
        private static extern int _GetVJDContPovNumber(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetVJDAxisExist", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _GetVJDAxisExist(UInt32 rID, UInt32 Axis);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetVJDAxisMax", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _GetVJDAxisMax(UInt32 rID, UInt32 Axis, ref long Max);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetVJDAxisMin", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _GetVJDAxisMin(UInt32 rID, UInt32 Axis, ref long Min);

        [DllImport("vJoyInterface.dll", EntryPoint = "isVJDExists", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _isVJDExists(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetOwnerPid", CallingConvention = CallingConvention.Cdecl)]
        private static extern int _GetOwnerPid(UInt32 rID);

        /////	Write access to vJoy Device - Basic
        [DllImport("vJoyInterface.dll", EntryPoint = "AcquireVJD", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _AcquireVJD(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "RelinquishVJD", CallingConvention = CallingConvention.Cdecl)]
        private static extern void _RelinquishVJD(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "UpdateVJD", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _UpdateVJD(UInt32 rID, ref JoystickState pData);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetVJDStatus", CallingConvention = CallingConvention.Cdecl)]
        private static extern int _GetVJDStatus(UInt32 rID);


        //// Reset functions
        [DllImport("vJoyInterface.dll", EntryPoint = "ResetVJD", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _ResetVJD(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "ResetAll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _ResetAll();

        [DllImport("vJoyInterface.dll", EntryPoint = "ResetButtons", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _ResetButtons(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "ResetPovs", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _ResetPovs(UInt32 rID);

        ////// Write data
        [DllImport("vJoyInterface.dll", EntryPoint = "SetAxis", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _SetAxis(Int32 Value, UInt32 rID, HID_USAGES Axis);

        [DllImport("vJoyInterface.dll", EntryPoint = "SetBtn", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _SetBtn(bool Value, UInt32 rID, Byte nBtn);

        [DllImport("vJoyInterface.dll", EntryPoint = "SetDiscPov", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _SetDiscPov(Int32 Value, UInt32 rID, uint nPov);

        [DllImport("vJoyInterface.dll", EntryPoint = "SetContPov", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _SetContPov(Int32 Value, UInt32 rID, uint nPov);

        [DllImport("vJoyInterface.dll", EntryPoint = "RegisterRemovalCB", CallingConvention = CallingConvention.Cdecl)]
        private extern static void _RegisterRemovalCB(WrapRemovalCbFunc cb, IntPtr data);

        public delegate void RemovalCbFunc(bool complete, bool First, object userData);
        public delegate void WrapRemovalCbFunc(bool complete, bool First, IntPtr userData);

        public static void WrapperRemCB(bool complete, bool First, IntPtr userData)
        {

            object obj = null;

            if (userData != IntPtr.Zero) {
                // Convert userData from pointer to object
                GCHandle handle2 = (GCHandle)userData;
                obj = handle2.Target as object;
            }

            // Call user-defined CB function
            UserRemCB(complete, First, obj);
        }

        // Force Feedback (FFB)
        [DllImport("vJoyInterface.dll", EntryPoint = "FfbRegisterGenCB", CallingConvention = CallingConvention.Cdecl)]
        private extern static void _FfbRegisterGenCB(WrapFfbCbFunc cb, IntPtr data);

        public delegate void FfbCbFunc(IntPtr data, object userData);
        public delegate void WrapFfbCbFunc(IntPtr data, IntPtr userData);

        public static void WrapperFfbCB(IntPtr data, IntPtr userData)
        {

            object obj = null;

            if (userData != IntPtr.Zero) {
                // Convert userData from pointer to object
                GCHandle handle2 = (GCHandle)userData;
                obj = handle2.Target as object;
            }

            // Call user-defined CB function
            UserFfbCB(data, obj);
        }

        [DllImport("vJoyInterface.dll", EntryPoint = "FfbStart", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _FfbStart(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "FfbStop", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _FfbStop(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "IsDeviceFfb", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _IsDeviceFfb(UInt32 rID);

        [DllImport("vJoyInterface.dll", EntryPoint = "IsDeviceFfbEffect", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _IsDeviceFfbEffect(UInt32 rID, UInt32 Effect);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_DeviceID", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_DeviceID(IntPtr Packet, ref UInt32 DeviceID);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Type", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Type(IntPtr Packet, ref FFBPType Type);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Packet", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Packet(IntPtr Packet, ref UInt32 Type, ref UInt32 DataSize, ref IntPtr Data);


        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_EBI", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_EBI(IntPtr Packet, ref UInt32 Index);

#pragma warning disable 618
        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Eff_Const", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Eff_Const(IntPtr Packet, ref FFB_EFF_CONST Effect);
#pragma warning restore 618

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Eff_Report", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Eff_Report(IntPtr Packet, ref FFB_EFF_REPORT Effect);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_DevCtrl", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_DevCtrl(IntPtr Packet, ref FFB_CTRL Control);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_EffOp", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_EffOp(IntPtr Packet, ref FFB_EFF_OP Operation);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_DevGain", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_DevGain(IntPtr Packet, ref Byte Gain);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Eff_Cond", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Eff_Cond(IntPtr Packet, ref FFB_EFF_COND Condition);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Eff_Envlp", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Eff_Envlp(IntPtr Packet, ref FFB_EFF_ENVLP Envelope);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Eff_Period", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Eff_Period(IntPtr Packet, ref FFB_EFF_PERIOD Effect);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_EffNew", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_EffNew(IntPtr Packet, ref FFBEType Effect);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Eff_Ramp", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Eff_Ramp(IntPtr Packet, ref FFB_EFF_RAMP RampEffect);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_Eff_Constant", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_Eff_Constant(IntPtr Packet, ref FFB_EFF_CONSTANT ConstantEffect);

        // New API added with 2.2.0
        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_EffectBlockIndex", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_EffectBlockIndex(IntPtr packet, ref UInt32 effectId);

        [DllImport("vJoyInterface.dll", EntryPoint = "Ffb_h_CreateNewEffect", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_CreateNewEffect(IntPtr packet, ref FFBEType effect, ref UInt32 newEffectId);

        [DllImport("vJoyInterface.dll", EntryPoint = "FfbWritePID", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_WritePID(UInt32 rID, ref FFB_DEVICE_PID PIDData);

        [DllImport("vJoyInterface.dll", EntryPoint = "FfbReadPID", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _Ffb_h_ReadPID(UInt32 rID, ref FFB_DEVICE_PID PIDData);

        [DllImport("vJoyInterface.dll", EntryPoint = "FfbUpdateEffectState", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _FfbUpdateEffectState(UInt32 rID, UInt32 EffectID, UInt32 effectState);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetPosition", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 _GetPosition(UInt32 rID, ref JoystickState pPosition);

        [DllImport("vJoyInterface.dll", EntryPoint = "GetvJoyMaxDevices", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool _GetvJoyMaxDevices(ref UInt32 nDevices);




        /***************************************************/
        /********** Export functions (C#) ******************/
        /***************************************************/

        /////	General driver data
        public short GetvJoyVersion() { return _GetvJoyVersion(); }
        public bool vJoyEnabled() { return _vJoyEnabled(); }
        public string GetvJoyProductString() { return Marshal.PtrToStringAuto(_GetvJoyProductString()); }
        public string GetvJoyManufacturerString() { return Marshal.PtrToStringAuto(_GetvJoyManufacturerString()); }
        public string GetvJoySerialNumberString() { return Marshal.PtrToStringAuto(_GetvJoySerialNumberString()); }
        public bool DriverMatch(ref UInt32 DllVer, ref UInt32 DrvVer) { return _DriverMatch(ref DllVer, ref DrvVer); }

        /////	vJoy Device properties
        public int GetVJDButtonNumber(uint rID) { return _GetVJDButtonNumber(rID); }
        public int GetVJDDiscPovNumber(uint rID) { return _GetVJDDiscPovNumber(rID); }
        public int GetVJDContPovNumber(uint rID) { return _GetVJDContPovNumber(rID); }
        public bool GetVJDAxisExist(UInt32 rID, HID_USAGES Axis)
        {
            UInt32 res = _GetVJDAxisExist(rID, (uint)Axis);
            if (res == 1)
                return true;
            else
                return false;
        }
        public bool GetVJDAxisMax(UInt32 rID, HID_USAGES Axis, ref long Max) { return _GetVJDAxisMax(rID, (uint)Axis, ref Max); }
        public bool GetVJDAxisMin(UInt32 rID, HID_USAGES Axis, ref long Min) { return _GetVJDAxisMin(rID, (uint)Axis, ref Min); }
        public bool isVJDExists(UInt32 rID) { return _isVJDExists(rID); }
        public int GetOwnerPid(UInt32 rID) { return _GetOwnerPid(rID); }

        /////	Write access to vJoy Device - Basic
        public bool AcquireVJD(UInt32 rID) { return _AcquireVJD(rID); }
        public void RelinquishVJD(uint rID) { _RelinquishVJD(rID); }
        public bool UpdateVJD(UInt32 rID, ref JoystickState pData) { return _UpdateVJD(rID, ref pData); }
        public VjdStat GetVJDStatus(UInt32 rID) { return (VjdStat)_GetVJDStatus(rID); }

        //// Reset functions
        public bool ResetVJD(UInt32 rID) { return _ResetVJD(rID); }
        public bool ResetAll() { return _ResetAll(); }
        public bool ResetButtons(UInt32 rID) { return _ResetButtons(rID); }
        public bool ResetPovs(UInt32 rID) { return _ResetPovs(rID); }

        ////// Write data
        public bool SetAxis(Int32 Value, UInt32 rID, HID_USAGES Axis) { return _SetAxis(Value, rID, Axis); }
        public bool SetBtn(bool Value, UInt32 rID, uint nBtn) { return _SetBtn(Value, rID, (Byte)nBtn); }
        public bool SetDiscPov(Int32 Value, UInt32 rID, uint nPov) { return _SetDiscPov(Value, rID, nPov); }
        public bool SetContPov(Int32 Value, UInt32 rID, uint nPov) { return _SetContPov(Value, rID, nPov); }

        // Register CB function that takes a C# object as userdata
        public void RegisterRemovalCB(RemovalCbFunc cb, object data)
        {
            // Free existing GCHandle (if exists)
            if (hRemUserData.IsAllocated && hRemUserData.Target != null)
                hRemUserData.Free();

            // Convert object to pointer
            hRemUserData = GCHandle.Alloc(data);

            // Apply the user-defined CB function          
            UserRemCB = new RemovalCbFunc(cb);
            wrf = new WrapRemovalCbFunc(WrapperRemCB);

            _RegisterRemovalCB(wrf, (IntPtr)hRemUserData);
        }

        // Register CB function that takes a pointer as userdata
        public void RegisterRemovalCB(WrapRemovalCbFunc cb, IntPtr data)
        {
            wrf = new WrapRemovalCbFunc(cb);
            _RegisterRemovalCB(wrf, data);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////
        //// Force Feedback (FFB)

        // Register CB function that takes a C# object as userdata
        public void FfbRegisterGenCB(FfbCbFunc cb, object data)
        {
            // Free existing GCHandle (if exists)
            if (hFfbUserData.IsAllocated && hFfbUserData.Target != null)
                hFfbUserData.Free();

            // Convert object to pointer
            hFfbUserData = GCHandle.Alloc(data);

            // Apply the user-defined CB function          
            UserFfbCB = new FfbCbFunc(cb);
            wf = new WrapFfbCbFunc(WrapperFfbCB);

            _FfbRegisterGenCB(wf, (IntPtr)hFfbUserData);
        }

        // Register CB function that takes a pointer as userdata
        public void FfbRegisterGenCB(WrapFfbCbFunc cb, IntPtr data)
        {
            wf = new WrapFfbCbFunc(cb);
            _FfbRegisterGenCB(wf, data);
        }

        [Obsolete("you can remove the function from your code")]
        public bool FfbStart(UInt32 rID) { return _FfbStart(rID); }
        [Obsolete("you can remove the function from your code")]
        public bool FfbStop(UInt32 rID) { return _FfbStop(rID); }
        public bool IsDeviceFfb(UInt32 rID) { return _IsDeviceFfb(rID); }
        public bool IsDeviceFfbEffect(UInt32 rID, UInt32 Effect) { return _IsDeviceFfbEffect(rID, Effect); }
        public UInt32 Ffb_h_DeviceID(IntPtr Packet, ref UInt32 DeviceID) { return _Ffb_h_DeviceID(Packet, ref DeviceID); }
        public UInt32 Ffb_h_Type(IntPtr Packet, ref FFBPType Type) { return _Ffb_h_Type(Packet, ref Type); }
        public UInt32 Ffb_h_Packet(IntPtr Packet, ref UInt32 Type, ref UInt32 DataSize, ref Byte[] Data)
        {
            IntPtr buf = IntPtr.Zero;
            UInt32 res = _Ffb_h_Packet(Packet, ref Type, ref DataSize, ref buf);
            if (res != 0)
                return res;

            DataSize -= 8;
            Data = new byte[DataSize];
            Marshal.Copy(buf, Data, 0, (Int32)DataSize);
            return res;
        }
        public UInt32 Ffb_h_EBI(IntPtr Packet, ref UInt32 Index) { return _Ffb_h_EBI(Packet, ref Index); }
        [Obsolete("use Ffb_h_Eff_Report instead")]
        public UInt32 Ffb_h_Eff_Const(IntPtr Packet, ref FFB_EFF_CONST Effect) { return _Ffb_h_Eff_Const(Packet, ref Effect); }
        public UInt32 Ffb_h_Eff_Report(IntPtr Packet, ref FFB_EFF_REPORT Effect) { return _Ffb_h_Eff_Report(Packet, ref Effect); }
        public UInt32 Ffb_h_DevCtrl(IntPtr Packet, ref FFB_CTRL Control) { return _Ffb_h_DevCtrl(Packet, ref Control); }
        public UInt32 Ffb_h_EffOp(IntPtr Packet, ref FFB_EFF_OP Operation) { return _Ffb_h_EffOp(Packet, ref Operation); }
        public UInt32 Ffb_h_DevGain(IntPtr Packet, ref Byte Gain) { return _Ffb_h_DevGain(Packet, ref Gain); }
        public UInt32 Ffb_h_Eff_Cond(IntPtr Packet, ref FFB_EFF_COND Condition) { return _Ffb_h_Eff_Cond(Packet, ref Condition); }
        public UInt32 Ffb_h_Eff_Envlp(IntPtr Packet, ref FFB_EFF_ENVLP Envelope) { return _Ffb_h_Eff_Envlp(Packet, ref Envelope); }
        public UInt32 Ffb_h_Eff_Period(IntPtr Packet, ref FFB_EFF_PERIOD Effect) { return _Ffb_h_Eff_Period(Packet, ref Effect); }
        public UInt32 Ffb_h_EffNew(IntPtr Packet, ref FFBEType Effect) { return _Ffb_h_EffNew(Packet, ref Effect); }
        public UInt32 Ffb_h_Eff_Ramp(IntPtr Packet, ref FFB_EFF_RAMP RampEffect) { return _Ffb_h_Eff_Ramp(Packet, ref RampEffect); }
        public UInt32 Ffb_h_Eff_Constant(IntPtr Packet, ref FFB_EFF_CONSTANT ConstantEffect) { return _Ffb_h_Eff_Constant(Packet, ref ConstantEffect); }
        public UInt32 Ffb_h_EffectBlockIndex(IntPtr Packet, ref UInt32 effectId) { return _Ffb_h_EffectBlockIndex(Packet, ref effectId); }
        public UInt32 Ffb_h_CreateNewEffect(IntPtr Packet, ref FFBEType effectType, ref UInt32 newEffectId) { return _Ffb_h_CreateNewEffect(Packet, ref effectType, ref newEffectId); }
        public UInt32 FfbWritePID(UInt32 rID, ref FFB_DEVICE_PID PID) { return _Ffb_h_WritePID(rID, ref PID); }
        public UInt32 FfbReadPID(UInt32 rID, ref FFB_DEVICE_PID PID) { return _Ffb_h_ReadPID(rID, ref PID); }
        public UInt32 FfbUpdateEffectState(UInt32 rID, UInt32 effectId, UInt32 effectState) { return _FfbUpdateEffectState(rID, effectId, effectState); }
        public UInt32 GetPosition(UInt32 rID, ref JoystickState pPosition) { return _GetPosition(rID, ref pPosition); }
        public bool GetvJoyMaxDevices(ref UInt32 nDevices) { return _GetvJoyMaxDevices(ref nDevices); }

    }
}
