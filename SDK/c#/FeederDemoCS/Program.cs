/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This project demonstrates how to write a simple vJoy feeder in C#
//
// You can compile it with either #define ROBUST OR #define EFFICIENT
// The fuctionality is similar - 
// The ROBUST section demonstrate the usage of functions that are easy and safe to use but are less efficient
// The EFFICIENT ection demonstrate the usage of functions that are more efficient
//
// Functionality:
//	The program starts with creating one joystick object. 
//	Then it petches the device id from the command-line and makes sure that it is within range
//	After testing that the driver is enabled it gets information about the driver
//	Gets information about the specified virtual device
//	This feeder uses only a few axes. It checks their existence and 
//	checks the number of buttons and POV Hat switches.
//	Then the feeder acquires the virtual device
//	Here starts and endless loop that feedes data into the virtual device
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ROBUST
//#define EFFICIENT
#define FFB
#define DUMP_FFB_FRAME

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

// Don't forget to add this
using vJoyInterfaceWrap;

namespace FeederDemoCS
{
    public class vJoyFFBReceiver
    {
        protected bool isRegistered = false;
        protected vJoy Joystick;
        protected uint Id;
        protected vJoy.FfbCbFunc wrapper;
        vJoy.FFB_DEVICE_PID PIDBlock = new vJoy.FFB_DEVICE_PID();

        // For debugging only (dump frame content)
        private enum CommandType : int
        {
            IOCTL_HID_SET_FEATURE = 0xB0191,
            IOCTL_HID_WRITE_REPORT = 0xB000F
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct InternalFfbPacket
        {
            public int DataSize;
            public CommandType Command;
            public IntPtr PtrToData;
        }

        protected enum ERROR : uint
        {
            ERROR_SUCCESS = 0,
        }




        public void RegisterBaseCallback(vJoy joystick, uint id)
        {
            this.Joystick = joystick;
            this.Id = id;
            // Read PID block
            this.Joystick.FfbReadPID(this.Id, ref this.PIDBlock);

            if (!isRegistered) {
                this.wrapper = this.FfbFunction1; //needed to keep a reference!
                joystick.FfbRegisterGenCB(this.wrapper, IntPtr.Zero);
                this.isRegistered = true;
            }
        }

        protected void LogFormat(string text, params object[] args)
        {
            Console.WriteLine(String.Format(text, args));
        }

#if DUMP_FFB_FRAME
        public void DumpFrame(IntPtr data)
        {
            unsafe {
                InternalFfbPacket* FfbData = (InternalFfbPacket*)data;
                int size = FfbData->DataSize;
                int command = (int)FfbData->Command;
                byte* bytes = (byte*)FfbData->PtrToData;
                StringBuilder line = new StringBuilder();
                line.AppendFormat(String.Format("FFB Size {0}", size));
                line.AppendFormat(" Cmd:" + String.Format("{0:X08}", (int)FfbData->Command));
                line.AppendFormat(" ID:" + String.Format("{0:X02}", command));
                line.AppendFormat(" Size:" + String.Format("{0:D02}", (int)(size - 8)));
                line.AppendFormat(" -");
                for (uint i = 0; i < size - 8; i++)
                    line.AppendFormat(String.Format(" {0:X02}", (uint)(bytes[i])));

                LogFormat(line.ToString());
            }
        }
#endif

        /// <summary>
        /// Called when vJoy has a new FFB packet.
        /// WARNING This is called from a thread pool managed by windows.
        /// The thread itself is created and managed by vJoyInterface.dll.
        /// Do not overload it, else you will me missing FFB packets from
        /// third party application.
        /// </summary>
        /// <param name="ffbDataPtr"></param>
        /// <param name="userData"></param>
        public void FfbFunction1(IntPtr data, object userdata)
        {
            // Packet Header
            //copy ffb packet to managed structure
            InternalFfbPacket packet = (InternalFfbPacket)Marshal.PtrToStructure(data, typeof(InternalFfbPacket));

            // Packet Header
            LogFormat("============= FFB Packet =============");

            /////// Packet Device ID, and Type Block Index (if exists)
            #region Packet Device ID, and Type Block Index
            

            uint DeviceID = 0, BlockIndex = 0;
            FFBPType Type = new FFBPType();

            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_DeviceID(data, ref DeviceID)) {
                LogFormat(" > Device ID: {0}", DeviceID);
            }

            // Effect block index only used when simultaneous effects should be done by
            // underlying hardware, which is not the case for a single motor driving wheel
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_EffectBlockIndex(data, ref BlockIndex)) {
                LogFormat(" > Effect Block Index: {0}", BlockIndex);
            }

            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_Type(data, ref Type)) {
                if (!PacketType2Str(Type, out var TypeStr))
                    LogFormat(" > Packet Type: {0}", Type);
                else
                    LogFormat(" > Packet Type: {0}", TypeStr);
                switch (Type) {
                    case FFBPType.PT_POOLREP:
                        LogFormat(" > Pool report handled by driver side");
                        break;
                    case FFBPType.PT_BLKLDREP:
                        LogFormat(" > Block Load report handled by driver side");
                        break;
                    case FFBPType.PT_BLKFRREP:
                        //FFBManager.FreeEffect(BlockIndex);
                        // Update PID
                        Joystick.FfbReadPID(DeviceID, ref PIDBlock);
                        LogFormat(" > Block Free effect id {0}", PIDBlock.NextFreeEID);
                        break;
                }
            }



            #endregion

            #region PID Device Control
            FFB_CTRL Control = new FFB_CTRL();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_DevCtrl(data, ref Control) && DevCtrl2Str(Control, out var CtrlStr)) {
                LogFormat(" >> PID Device Control: {0}", CtrlStr);
                switch (Control) {
                    case FFB_CTRL.CTRL_DEVRST:
                        // Update PID data to get the resetted values from driver side
                        Joystick.FfbReadPID(DeviceID, ref PIDBlock);
                        // device reset
                        break;
                    case FFB_CTRL.CTRL_ENACT:
                        break;
                    case FFB_CTRL.CTRL_DISACT:
                        break;
                    case FFB_CTRL.CTRL_STOPALL:
                        break;
                }
            }

            #endregion


            #region Create new effect
            FFBEType EffectType = new FFBEType();
            uint NewBlockIndex = 0;
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_CreateNewEffect(data, ref EffectType, ref NewBlockIndex)) {
                // Create new effect

                // Update PID
                Joystick.FfbReadPID(Id, ref PIDBlock);

                if (EffectType2Str(EffectType, out var TypeStr))
                    LogFormat(" >> Effect Type: {0}", TypeStr);
                else
                    LogFormat(" >> Effect Type: Unknown({0})", EffectType);
                LogFormat(" >> New Effect ID: {0}", NewBlockIndex);
                if (NewBlockIndex != PIDBlock.PIDBlockLoad.EffectBlockIndex) {
                    LogFormat("!!! BUG NewBlockIndex=" + NewBlockIndex + " <> pid=" + ((int)PIDBlock.PIDBlockLoad.EffectBlockIndex));
                }
                LogFormat(" >> LoadStatus {0}", PIDBlock.PIDBlockLoad.LoadStatus);
            }
            #endregion

            #region Condition
            vJoy.FFB_EFF_COND Condition = new vJoy.FFB_EFF_COND();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_Eff_Cond(data, ref Condition)) {

                if (Condition.isY)
                    LogFormat(" >> Y Axis");
                else
                    LogFormat(" >> X Axis");
                LogFormat(" >> Center Point Offset: {0}", TwosCompWord2Int(Condition.CenterPointOffset));
                LogFormat(" >> Positive Coefficient: {0}", TwosCompWord2Int(Condition.PosCoeff));
                LogFormat(" >> Negative Coefficient: {0}", TwosCompWord2Int(Condition.NegCoeff));
                LogFormat(" >> Positive Saturation: {0}", Condition.PosSatur);
                LogFormat(" >> Negative Saturation: {0}", Condition.NegSatur);
                LogFormat(" >> Dead Band: {0}", Condition.DeadBand);

            }
            #endregion

            #region Effect Report
            vJoy.FFB_EFF_REPORT Effect = new vJoy.FFB_EFF_REPORT();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_Eff_Report(data, ref Effect)) {
                if (!EffectType2Str(Effect.EffectType, out var TypeStr))
                    LogFormat(" >> Effect Report: {0} {1}", (int)Effect.EffectType, Effect.EffectType.ToString());
                else
                    LogFormat(" >> Effect Report: {0}", TypeStr);
                LogFormat(" >> AxisEnabledDirection: {0}", (ushort)Effect.AxesEnabledDirection);
                if (Effect.Polar) {
                    LogFormat(" >> Direction: {0} deg ({1})", Polar2Deg(Effect.Direction), Effect.Direction);
                } else {
                    LogFormat(" >> X Direction: {0}", Effect.DirX);
                    LogFormat(" >> Y Direction: {0}", Effect.DirY);
                }

                if (Effect.Duration == 0xFFFF)
                    LogFormat(" >> Duration: Infinit");
                else
                    LogFormat(" >> Duration: {0} MilliSec", (int)(Effect.Duration));

                if (Effect.TrigerRpt == 0xFFFF)
                    LogFormat(" >> Trigger Repeat: Infinit");
                else
                    LogFormat(" >> Trigger Repeat: {0}", (int)(Effect.TrigerRpt));

                if (Effect.SamplePrd == 0xFFFF)
                    LogFormat(" >> Sample Period: Infinit");
                else
                    LogFormat(" >> Sample Period: {0}", (int)(Effect.SamplePrd));

                if (Effect.StartDelay == 0xFFFF)
                    LogFormat(" >> Start Delay: max ");
                else
                    LogFormat(" >> Start Delay: {0}", (int)(Effect.StartDelay));


                LogFormat(" >> Gain: {0}%%", Byte2Percent(Effect.Gain));

            }
            #endregion

            #region Effect Operation
            vJoy.FFB_EFF_OP Operation = new vJoy.FFB_EFF_OP();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_EffOp(data, ref Operation) && EffectOpStr(Operation.EffectOp, out var EffOpStr)) {

                LogFormat(" >> Effect Operation: {0}", EffOpStr);
                if (Operation.LoopCount == 0xFF)
                    LogFormat(" >> Loop until stopped");
                else
                    LogFormat(" >> Loop {0} times", (int)(Operation.LoopCount));

                switch (Operation.EffectOp) {
                    case FFBOP.EFF_START:
                        // Start the effect identified by the Effect Handle.
                        break;
                    case FFBOP.EFF_STOP:
                        // Stop the effect identified by the Effect Handle.
                        break;
                    case FFBOP.EFF_SOLO:
                        // Start the effect identified by the Effect Handle and stop all other effects.
                        break;
                }

            }
            #endregion

            #region Global Device Gain
            byte Gain = 0;
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_DevGain(data, ref Gain)) {

                LogFormat(" >> Global Device Gain: {0}", Byte2Percent(Gain));
            }

            #endregion

            #region Envelope
            vJoy.FFB_EFF_ENVLP Envelope = new vJoy.FFB_EFF_ENVLP();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_Eff_Envlp(data, ref Envelope)) {

                LogFormat(" >> Attack Level: {0}", Envelope.AttackLevel);
                LogFormat(" >> Fade Level: {0}", Envelope.FadeLevel);
                LogFormat(" >> Attack Time: {0}", (int)(Envelope.AttackTime));
                LogFormat(" >> Fade Time: {0}", (int)(Envelope.FadeTime));
            }

            #endregion

            #region Periodic
            vJoy.FFB_EFF_PERIOD EffPrd = new vJoy.FFB_EFF_PERIOD();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_Eff_Period(data, ref EffPrd)) {

                LogFormat(" >> Magnitude: {0}", EffPrd.Magnitude);
                LogFormat(" >> Offset: {0}", TwosCompWord2Int(EffPrd.Offset));
                LogFormat(" >> Phase: {0}", EffPrd.Phase * 3600 / 255);
                LogFormat(" >> Period: {0}", (int)(EffPrd.Period));
            }
            #endregion

            #region Ramp Effect
            vJoy.FFB_EFF_RAMP RampEffect = new vJoy.FFB_EFF_RAMP();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_Eff_Ramp(data, ref RampEffect)) {
                LogFormat(" >> Ramp Start: {0}", TwosCompWord2Int(RampEffect.Start));
                LogFormat(" >> Ramp End: {0}", TwosCompWord2Int(RampEffect.End));
            }

            #endregion

            #region Constant Effect
            vJoy.FFB_EFF_CONSTANT CstEffect = new vJoy.FFB_EFF_CONSTANT();
            if ((uint)ERROR.ERROR_SUCCESS == Joystick.Ffb_h_Eff_Constant(data, ref CstEffect)) {
                LogFormat(" >> Block Index: {0}", TwosCompWord2Int(CstEffect.EffectBlockIndex));
                LogFormat(" >> Magnitude: {0}", TwosCompWord2Int(CstEffect.Magnitude));
            }

            #endregion


            DumpFrame(data);
            LogFormat("======================================");

        }



        // Convert Packet type to String
        public static bool PacketType2Str(FFBPType Type, out string Str)
        {
            bool stat = true;
            Str = "";

            switch (Type) {
                case FFBPType.PT_EFFREP:
                    Str = "Effect Report";
                    break;
                case FFBPType.PT_ENVREP:
                    Str = "Envelope Report";
                    break;
                case FFBPType.PT_CONDREP:
                    Str = "Condition Report";
                    break;
                case FFBPType.PT_PRIDREP:
                    Str = "Periodic Report";
                    break;
                case FFBPType.PT_CONSTREP:
                    Str = "Constant Force Report";
                    break;
                case FFBPType.PT_RAMPREP:
                    Str = "Ramp Force Report";
                    break;
                case FFBPType.PT_CSTMREP:
                    Str = "Custom Force Data Report";
                    break;
                case FFBPType.PT_SMPLREP:
                    Str = "Download Force Sample";
                    break;
                case FFBPType.PT_EFOPREP:
                    Str = "Effect Operation Report";
                    break;
                case FFBPType.PT_BLKFRREP:
                    Str = "PID Block Free Report";
                    break;
                case FFBPType.PT_CTRLREP:
                    Str = "PID Device Control";
                    break;
                case FFBPType.PT_GAINREP:
                    Str = "Device Gain Report";
                    break;
                case FFBPType.PT_SETCREP:
                    Str = "Set Custom Force Report";
                    break;
                case FFBPType.PT_NEWEFREP:
                    Str = "Create New Effect Report";
                    break;
                case FFBPType.PT_BLKLDREP:
                    Str = "Block Load Report";
                    break;
                case FFBPType.PT_POOLREP:
                    Str = "PID Pool Report";
                    break;
                default:
                    stat = false;
                    break;
            }

            return stat;
        }

        // Convert Effect type to String
        public static bool EffectType2Str(FFBEType Type, out string Str)
        {
            bool stat = true;
            Str = "";

            switch (Type) {
                case FFBEType.ET_NONE:
                    stat = false;
                    break;
                case FFBEType.ET_CONST:
                    Str = "Constant Force";
                    break;
                case FFBEType.ET_RAMP:
                    Str = "Ramp";
                    break;
                case FFBEType.ET_SQR:
                    Str = "Square";
                    break;
                case FFBEType.ET_SINE:
                    Str = "Sine";
                    break;
                case FFBEType.ET_TRNGL:
                    Str = "Triangle";
                    break;
                case FFBEType.ET_STUP:
                    Str = "Sawtooth Up";
                    break;
                case FFBEType.ET_STDN:
                    Str = "Sawtooth Down";
                    break;
                case FFBEType.ET_SPRNG:
                    Str = "Spring";
                    break;
                case FFBEType.ET_DMPR:
                    Str = "Damper";
                    break;
                case FFBEType.ET_INRT:
                    Str = "Inertia";
                    break;
                case FFBEType.ET_FRCTN:
                    Str = "Friction";
                    break;
                case FFBEType.ET_CSTM:
                    Str = "Custom Force";
                    break;
                default:
                    stat = false;
                    break;
            }

            return stat;
        }

        // Convert PID Device Control to String
        public static bool DevCtrl2Str(FFB_CTRL Ctrl, out string Str)
        {
            bool stat = true;
            Str = "";

            switch (Ctrl) {
                case FFB_CTRL.CTRL_ENACT:
                    Str = "Enable Actuators";
                    break;
                case FFB_CTRL.CTRL_DISACT:
                    Str = "Disable Actuators";
                    break;
                case FFB_CTRL.CTRL_STOPALL:
                    Str = "Stop All Effects";
                    break;
                case FFB_CTRL.CTRL_DEVRST:
                    Str = "Device Reset";
                    break;
                case FFB_CTRL.CTRL_DEVPAUSE:
                    Str = "Device Pause";
                    break;
                case FFB_CTRL.CTRL_DEVCONT:
                    Str = "Device Continue";
                    break;
                default:
                    stat = false;
                    break;
            }

            return stat;
        }

        // Convert Effect operation to string
        public static bool EffectOpStr(FFBOP Op, out string Str)
        {
            bool stat = true;
            Str = "";

            switch (Op) {
                case FFBOP.EFF_START:
                    Str = "Effect Start";
                    break;
                case FFBOP.EFF_SOLO:
                    Str = "Effect Solo Start";
                    break;
                case FFBOP.EFF_STOP:
                    Str = "Effect Stop";
                    break;
                default:
                    stat = false;
                    break;
            }

            return stat;
        }

        // Polar values (0x00-0xFF) to Degrees (0-360)
        public static int Polar2Deg(UInt16 Polar)
        {
            return (int)((long)Polar * 360) / 32767;
        }

        // Convert range 0x00-0xFF to 0%-100%
        public static int Byte2Percent(byte InByte)
        {
            return ((byte)InByte * 100) / 255;
        }

        // Convert One-Byte 2's complement input to integer
        public static int TwosCompByte2Int(byte inb)
        {
            int tmp;
            byte inv = (byte)~inb;
            bool isNeg = ((inb >> 7) != 0 ? true : false);
            if (isNeg) {
                tmp = (int)(inv);
                tmp = -1 * tmp;
                return tmp;
            } else
                return (int)inb;
        }

        // Convert One-Byte 2's complement input to integer
        public static int TwosCompWord2Int(short inb)
        {
            int tmp;
            int inv = (int)~inb + 1;
            bool isNeg = ((inb >> 15) != 0 ? true : false);
            if (isNeg) {
                tmp = (int)(inv);
                tmp = -1 * tmp;
                return tmp;
            } else
                return (int)inb;
        }

    }

    class Program
    {
        // Declaring one joystick (Device id 1) and a position structure. 
        static public vJoy joystick;
        static public vJoy.JoystickState iReport;
        static public vJoyFFBReceiver FFBReceiver;
        static public uint id = 1;

        static public int StartAndRegisterFFB()
        {
            // Start FFB
#if FFB
            if (joystick.IsDeviceFfb(id)) {

                // Register Generic callback function
                // At this point you instruct the Receptor which callback function to call with every FFB packet it receives
                // It is the role of the designer to register the right FFB callback function

                // Note from me:
                // Warning: the callback is called in the context of a thread started by vJoyInterface.dll
                // when opening the joystick. This thread blocks upon a new system event from the driver.
                // It is perfectly ok to do some work in it, but do not overload it to avoid
                // loosing/desynchronizing FFB packets from the third party application.
                FFBReceiver.RegisterBaseCallback(joystick, id);
            }
#endif // FFB
            return 0;
        }

        static void Main(string[] args)
        {
            // Create one joystick object and a position structure.
            joystick = new vJoy();
            iReport = new vJoy.JoystickState();
            FFBReceiver = new vJoyFFBReceiver();

            // Device ID can only be in the range 1-16
            if (args.Length>0 && !String.IsNullOrEmpty(args[0]))
                id = Convert.ToUInt32(args[0]);
            if (id <= 0 || id > 16) {
                Console.WriteLine("Illegal device ID {0}\nExit!", id);
                return;
            }

            // Get the driver attributes (Vendor ID, Product ID, Version Number)
            if (!joystick.vJoyEnabled()) {
                Console.WriteLine("vJoy driver not enabled: Failed Getting vJoy attributes.");
                return;
            } else
                Console.WriteLine("Vendor: {0}\nProduct :{1}\nVersion Number:{2}", joystick.GetvJoyManufacturerString(), joystick.GetvJoyProductString(), joystick.GetvJoySerialNumberString());

            // Get the state of the requested device
            VjdStat status = joystick.GetVJDStatus(id);
            switch (status) {
                case VjdStat.VJD_STAT_OWN:
                    Console.WriteLine("vJoy Device {0} is already owned by this feeder", id);
                    break;
                case VjdStat.VJD_STAT_FREE:
                    Console.WriteLine("vJoy Device {0} is free", id);
                    break;
                case VjdStat.VJD_STAT_BUSY:
                    Console.WriteLine("vJoy Device {0} is already owned by another feeder\nCannot continue", id);
                    return;
                case VjdStat.VJD_STAT_MISS:
                    Console.WriteLine("vJoy Device {0} is not installed or disabled\nCannot continue", id);
                    return;
                default:
                    Console.WriteLine("vJoy Device {0} general error\nCannot continue", id);
                    return;
            };

            // Check which axes are supported
            bool AxisX = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_X);
            bool AxisY = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_Y);
            bool AxisZ = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_Z);
            bool AxisRX = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_RX);
            bool AxisRZ = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_RZ);
            // Get the number of buttons and POV Hat switchessupported by this vJoy device
            int nButtons = joystick.GetVJDButtonNumber(id);
            int ContPovNumber = joystick.GetVJDContPovNumber(id);
            int DiscPovNumber = joystick.GetVJDDiscPovNumber(id);

            // Print results
            Console.WriteLine("\nvJoy Device {0} capabilities:", id);
            Console.WriteLine("Numner of buttons\t\t{0}", nButtons);
            Console.WriteLine("Numner of Continuous POVs\t{0}", ContPovNumber);
            Console.WriteLine("Numner of Descrete POVs\t\t{0}", DiscPovNumber);
            Console.WriteLine("Axis X\t\t{0}", AxisX ? "Yes" : "No");
            Console.WriteLine("Axis Y\t\t{0}", AxisX ? "Yes" : "No");
            Console.WriteLine("Axis Z\t\t{0}", AxisX ? "Yes" : "No");
            Console.WriteLine("Axis Rx\t\t{0}", AxisRX ? "Yes" : "No");
            Console.WriteLine("Axis Rz\t\t{0}", AxisRZ ? "Yes" : "No");

            // Test if DLL matches the driver
            UInt32 DllVer = 0, DrvVer = 0;
            bool match = joystick.DriverMatch(ref DllVer, ref DrvVer);
            if (match)
                Console.WriteLine("Version of Driver Matches DLL Version ({0:X})", DllVer);
            else
                Console.WriteLine("Version of Driver ({0:X}) does NOT match DLL Version ({1:X})", DrvVer, DllVer);


            // Acquire the target
            if ((status == VjdStat.VJD_STAT_OWN) || ((status == VjdStat.VJD_STAT_FREE) && (!joystick.AcquireVJD(id)))) {
                Console.WriteLine("Failed to acquire vJoy device number {0}.", id);
                return;
            } else
                Console.WriteLine("Acquired: vJoy device number {0}.", id);

            StartAndRegisterFFB();

            Console.WriteLine("\npress enter to stat feeding");
            Console.ReadKey(true);

            int X, Y, Z, ZR, XR;
            uint count = 0;
            long maxval = 0;

            X = 20;
            Y = 30;
            Z = 40;
            XR = 60;
            ZR = 80;

            joystick.GetVJDAxisMax(id, HID_USAGES.HID_USAGE_X, ref maxval);

#if ROBUST
            bool res;
            // Reset this device to default values
            joystick.ResetVJD(id);

            // Feed the device in endless loop
            while (true) {
                // Set position of 4 axes
                res = joystick.SetAxis(X, id, HID_USAGES.HID_USAGE_X);
                res = joystick.SetAxis(Y, id, HID_USAGES.HID_USAGE_Y);
                res = joystick.SetAxis(Z, id, HID_USAGES.HID_USAGE_Z);
                res = joystick.SetAxis(XR, id, HID_USAGES.HID_USAGE_RX);
                res = joystick.SetAxis(ZR, id, HID_USAGES.HID_USAGE_RZ);

                // Press/Release Buttons
                res = joystick.SetBtn(true, id, count / 50);
                res = joystick.SetBtn(false, id, 1 + count / 50);

                // If Continuous POV hat switches installed - make them go round
                // For high values - put the switches in neutral state
                if (ContPovNumber>0) {
                    if ((count * 70) < 30000) {
                        res = joystick.SetContPov(((int)count * 70), id, 1);
                        res = joystick.SetContPov(((int)count * 70) + 2000, id, 2);
                        res = joystick.SetContPov(((int)count * 70) + 4000, id, 3);
                        res = joystick.SetContPov(((int)count * 70) + 6000, id, 4);
                    } else {
                        res = joystick.SetContPov(-1, id, 1);
                        res = joystick.SetContPov(-1, id, 2);
                        res = joystick.SetContPov(-1, id, 3);
                        res = joystick.SetContPov(-1, id, 4);
                    };
                };

                // If Discrete POV hat switches installed - make them go round
                // From time to time - put the switches in neutral state
                if (DiscPovNumber>0) {
                    if (count < 550) {
                        joystick.SetDiscPov((((int)count / 20) + 0) % 4, id, 1);
                        joystick.SetDiscPov((((int)count / 20) + 1) % 4, id, 2);
                        joystick.SetDiscPov((((int)count / 20) + 2) % 4, id, 3);
                        joystick.SetDiscPov((((int)count / 20) + 3) % 4, id, 4);
                    } else {
                        joystick.SetDiscPov(-1, id, 1);
                        joystick.SetDiscPov(-1, id, 2);
                        joystick.SetDiscPov(-1, id, 3);
                        joystick.SetDiscPov(-1, id, 4);
                    };
                };

                System.Threading.Thread.Sleep(20);
                X += 150; if (X > maxval) X = 0;
                Y += 250; if (Y > maxval) Y = 0;
                Z += 350; if (Z > maxval) Z = 0;
                XR += 220; if (XR > maxval) XR = 0;
                ZR += 200; if (ZR > maxval) ZR = 0;
                count++;

                if (count > 640)
                    count = 0;

            } // While (Robust)

#endif // ROBUST
#if EFFICIENT

            byte[] pov = new byte[4];

      while (true)
            {
            iReport.bDevice = (byte)id;
            iReport.AxisX = X;
            iReport.AxisY = Y;
            iReport.AxisZ = Z;
            iReport.AxisZRot = ZR;
            iReport.AxisXRot = XR;

            // Set buttons one by one
            iReport.Buttons = (uint)(0x1 <<  (int)(count / 20));

        if (ContPovNumber>0)
        {
            // Make Continuous POV Hat spin
            iReport.bHats		= (count*70);
            iReport.bHatsEx1	= (count*70)+3000;
            iReport.bHatsEx2	= (count*70)+5000;
            iReport.bHatsEx3	= 15000 - (count*70);
            if ((count*70) > 36000)
            {
                iReport.bHats =    0xFFFFFFFF; // Neutral state
                iReport.bHatsEx1 = 0xFFFFFFFF; // Neutral state
                iReport.bHatsEx2 = 0xFFFFFFFF; // Neutral state
                iReport.bHatsEx3 = 0xFFFFFFFF; // Neutral state
            };
        }
        else
        {
            // Make 5-position POV Hat spin
            
            pov[0] = (byte)(((count / 20) + 0)%4);
            pov[1] = (byte)(((count / 20) + 1) % 4);
            pov[2] = (byte)(((count / 20) + 2) % 4);
            pov[3] = (byte)(((count / 20) + 3) % 4);

            iReport.bHats		= (uint)(pov[3]<<12) | (uint)(pov[2]<<8) | (uint)(pov[1]<<4) | (uint)pov[0];
            if ((count) > 550)
                iReport.bHats = 0xFFFFFFFF; // Neutral state
        };

        /*** Feed the driver with the position packet - is fails then wait for input then try to re-acquire device ***/
        if (!joystick.UpdateVJD(id, ref iReport))
        {
            Console.WriteLine("Feeding vJoy device number {0} failed - try to enable device then press enter\n", id);
            Console.ReadKey(true);
            joystick.AcquireVJD(id);
        }

        System.Threading.Thread.Sleep(20);
        count++;
        if (count > 640) count = 0;

        X += 150; if (X > maxval) X = 0;
        Y += 250; if (Y > maxval) Y = 0;
        Z += 350; if (Z > maxval) Z = 0;
        XR += 220; if (XR > maxval) XR = 0;
        ZR += 200; if (ZR > maxval) ZR = 0;  
         
      }; // While

#endif // EFFICIENT

        } // Main
    } // class Program
} // namespace FeederDemoCS
