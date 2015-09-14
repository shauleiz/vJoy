using System;
using System.Collections.Generic;
using System.ComponentModel;
//using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using vJoyInterfaceWrap;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace WrapperTest
{

    public partial class TesterForm : Form
    {
        public vJoy joystick;
        public vJoy.JoystickState position;
        public uint id = 1;


        public TesterForm()
        {
            InitializeComponent();
            joystick = new vJoy();
            position = new vJoy.JoystickState();


            /////	General driver data
            short iVer = joystick.GetvJoyVersion();
            bool enabled = joystick.vJoyEnabled();
            string Prd = joystick.GetvJoyProductString();
            string Mnf = joystick.GetvJoyManufacturerString();
            string Srl = joystick.GetvJoySerialNumberString();
            string prt = String.Format("Product: {0}; Version {1:X}; Manuf: {2}; Serial:{3}", Prd, iVer, Mnf, Srl);
            label1.Text = prt;
            Enbld.Checked = enabled;

            /////	vJoy Device properties
            int nBtn = joystick.GetVJDButtonNumber(id);
            int nDPov = joystick.GetVJDDiscPovNumber(id);
            int nCPov = joystick.GetVJDContPovNumber(id);
            bool X_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_X);
            bool Y_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_Y);
            bool Z_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_Z);
            bool RX_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_RX);
            prt = String.Format("Device[{0}]: Buttons={1}; DiscPOVs:{2}; ContPOVs:{3}", id, nBtn, nDPov, nCPov);
            label2.Text = prt;

            UInt32 DllVer = 0, DrvVer = 0;
            bool match = joystick.DriverMatch(ref DllVer, ref DrvVer);
            if (match)
                prt = String.Format("Version of Driver Matches DLL Version {0:X}", DllVer);
            else
                prt = String.Format("Version of Driver ({0:X}) does NOT match DLL Version ({1:X})", DrvVer, DllVer);
            label7.Text = prt;

            long max = 10, min = 10;
            bool ok;
            ok = joystick.GetVJDAxisMax(id, HID_USAGES.HID_USAGE_X, ref max);
            ok = joystick.GetVJDAxisMin(id, HID_USAGES.HID_USAGE_X, ref min);

            /////	Write access to vJoy Device - Basic
            VjdStat status;
            status = joystick.GetVJDStatus(id);
            bool acq = joystick.AcquireVJD(id);
            status = joystick.GetVJDStatus(id);

            position.AxisX = 1000;
            position.AxisY = 5000;
            position.AxisZ = 10000;
            position.AxisXRot = 20000;
            position.Buttons = 0xA5A5A5A5;
            position.ButtonsEx1 = 0;
            bool upd = joystick.UpdateVJD(id, ref position);
            status = joystick.GetVJDStatus(id);

            //// Reset functions
            joystick.ResetButtons(id);

            // Register callback function
            // Function to register:     Removal()
            // User data to pass:        label2
            joystick.RegisterRemovalCB(Removal, label2);
            
        }

        // User-defined Removal/Arival callback Function
        // When registered, label2 was passed as user-data 
        private void Removal(bool removal, bool first, object userData)
        {

            string prt;
            Label l = userData as Label;

            int nBtn = joystick.GetVJDButtonNumber(id);
            int nDPov = joystick.GetVJDDiscPovNumber(id);
            int nCPov = joystick.GetVJDContPovNumber(id);
            bool X_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_X);
            bool Y_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_Y);
            bool Z_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_Z);
            bool RX_Exist = joystick.GetVJDAxisExist(id, HID_USAGES.HID_USAGE_RX);

            // Final values after the last arival
            if (!removal && !first)
            {
                prt = String.Format("Device[{0}]: Buttons={1}; DiscPOVs:{2}; ContPOVs:{3}", id, nBtn, nDPov, nCPov);
                label2.Text = prt;
            }

            // Temporary message during intermediate states
            else 
            {
                prt = String.Format("Device[{0}]: Wait ...", id);
                label2.Text = prt;
            }
        }
        // Demo callback function
        // Called with every FFB packet that is sent to a vJoy device
        // Must be registered using function FfbRegisterGenCB()

        private void TesterForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            VjdStat status;

            // Don't use (Too slow) -  joystick.ResetAll();
            joystick.ResetVJD(id);
            status = joystick.GetVJDStatus(id);
            joystick.RelinquishVJD(id);
            status = joystick.GetVJDStatus(id);
 
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void trackBar_X_ValueChanged(object sender, EventArgs e)
        {
            joystick.SetAxis(trackBar_X.Value, id, HID_USAGES.HID_USAGE_X);
        }

        private void trackBar_Y_ValueChanged(object sender, EventArgs e)
        {
            joystick.SetAxis(trackBar_Y.Value, id, HID_USAGES.HID_USAGE_Y);
        }

        private void trackBarZ_ValueChanged(object sender, EventArgs e)
        {
            joystick.SetAxis(trackBar_Z.Value, id, HID_USAGES.HID_USAGE_Z);
        }

        private void trackBarRX_ValueChanged(object sender, EventArgs e)
        {
            joystick.SetAxis(trackBar_RX.Value, id, HID_USAGES.HID_USAGE_RX);
        }

        private void Btn1_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn1.Checked, id, 1);
        }

        private void Btn2_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn2.Checked, id, 2);
        }

        private void Btn3_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn3.Checked, id, 3);
        }

        private void Btn4_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn4.Checked, id, 4);
        }

        private void Btn5_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn5.Checked, id, 5);
        }

        private void Btn6_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn6.Checked, id, 6);
        }

        private void Btn7_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn7.Checked, id, 7);
        }

        private void Btn8_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn8.Checked, id, 8);
        }

        private void Btn81_CheckedChanged(object sender, EventArgs e)
        {
            joystick.SetBtn(Btn81.Checked, id, 81);
        }

        private void groupBox2_Enter(object sender, EventArgs e)
        {

        }

        private void FfbTextBox_MouseDown(object sender, MouseEventArgs e)
        {

        }

        #region Write to FFB Text box
        // This region is based on
        // http://stackoverflow.com/questions/10775367/cross-thread-operation-not-valid-control-textbox1-accessed-from-a-thread-othe
        public void FfbTextBox_Write(string text)
        {
            text = FfbTextBox.Text + text;
            SetText(text);
        }

        delegate void SetTextCallback(string text);
        private void SetText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.FfbTextBox.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.FfbTextBox.Text = text;
            }
        }


        private void FfbTextBox_TextChanged(object sender, EventArgs e)
        {
            FfbTextBox.SelectionStart = FfbTextBox.TextLength;
            FfbTextBox.ScrollToCaret();
        }

        #endregion

        public Int32 trackBarValue(string axis)
        {
            switch(axis)
            {
                case "x":
                case "X":
                    return trackBar_X.Value;

                case "y":
                case "Y":
                    return trackBar_Y.Value;

                case "z":
                case "Z":
                    return trackBar_Z.Value;

                case "RX":
                case "rx":
                case "Rx":
                    return trackBar_RX.Value;

                default:
                    return 0;

            }
        }

    }

    public class FfbInterface
    {
        public static vJoy joystick;
        public static TesterForm dlg;
        public uint id = 1;

        public FfbInterface(TesterForm dialog)
        {
            dlg = dialog;
            joystick = dialog.joystick;
            // FFB
#pragma warning disable 0618
            if (!joystick.FfbStart(id))
                throw new Exception(String.Format("Failed to start Forcefeedback on device {0}", id));
#pragma warning restore 0618

            // Convert Form to pointer and pass it as user data to the callback function
            GCHandle h = GCHandle.Alloc(dialog);
            IntPtr parameter = (IntPtr)h;
            // joystick.FfbRegisterGenCB(OnEffect, parameter);
            joystick.FfbRegisterGenCB(OnEffectObj, dialog);
        }

        public FfbInterface()
        {
            // TODO: Complete member initialization
        }

        private static void OnEffect(IntPtr data, IntPtr userData)
        {
            int id = 0;
            FFBPType type = 0;
            int DSize = 0;
            Byte[] arr = new Byte[20];
            UInt32 wType = 0;
            IntPtr bytes = IntPtr.Zero;
            string TypeStr = "";
            object obj = null;
            TesterForm form = null;

            // Converting user data from pointer to object
            if (userData != IntPtr.Zero)
            {
                // Convert userData from pointer to object
                GCHandle handle2 = (GCHandle)userData;
                obj = handle2.Target as object;
                form = obj as TesterForm;
            }


            // Get FFB packet raw data
            var result = joystick.Ffb_h_Packet(data, ref wType, ref DSize, ref arr);
            dlg.FfbTextBox_Write(String.Format("\r\n\r\n ============= FFB Packet size Size {0} =============", DSize + 8));

            // Get the ID of the device that this DDB packet refers to
            result = joystick.Ffb_h_DeviceID(data, ref id);
            dlg.FfbTextBox_Write(String.Format("\r\n > Device ID: {0}", id));

            // Get the type of the packet
            result = joystick.Ffb_h_Type(data, ref type);
            if (result == 0)
            {
                bool ok = PacketType2Str(type, ref TypeStr);
                if (!ok)
                    dlg.FfbTextBox_Write(String.Format("\r\n > Packet Type: {0}", type));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n > Packet Type: {0}", TypeStr));
            };

            // Get the index of the effect block (Always 1)
            int iBlock = 0;
            result = joystick.Ffb_h_EBI(data, ref iBlock);
            if (result == 0)
                dlg.FfbTextBox_Write(String.Format("\r\n > Effect Block Index: {0}", iBlock));

            // TODO: Add constant magnitude

            #region Effect Report
            // Effect Report
            vJoy.FFB_EFF_REPORT Effect = new vJoy.FFB_EFF_REPORT();
            result = joystick.Ffb_h_Eff_Report(data, ref Effect);
            if (result == 0)
            {
                ///// This is an Effect Block
                // Effect type
                if (!EffectType2Str(Effect.EffectType, ref TypeStr))
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Report: {0}", Effect.EffectType));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Report: {0}", TypeStr));

                // Effect Direction 
                if (Effect.Polar)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Direction: {0} deg ({1:D2})", Polar2Deg(Effect.Direction), Effect.Direction));
                else
                {
                    dlg.FfbTextBox_Write(String.Format("\r\n >> X Direction: {0:X2}", Effect.DirX));
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Y Direction: {0:X2}", Effect.DirY));
                };

                // Duration of the effect
                if (Effect.Duration == 0xFFFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Duration: Infinit"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Duration: {0} MilliSec", (int)(Effect.Duration)));

                // Trigger Repeat
                if (Effect.TrigerRpt == 0xFFFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Trigger Repeat: Infinit"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Trigger Repeat: {0}", (int)(Effect.TrigerRpt)));

                // Sample Period
                if (Effect.SamplePrd == 0xFFFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Sample Period: Infinit"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Sample Period: {0}", (int)(Effect.SamplePrd)));


                // Gain
                dlg.FfbTextBox_Write(String.Format("\r\n >> Gain: {0}%", Byte2Percent(Effect.Gain)));
            };

            #endregion   Effect Report

            #region PID Device Control
            // Device Control (Global controls such as Device Reset, Device Pause .....
            FFB_CTRL Control = FFB_CTRL.CTRL_STOPALL;
            string CtrlStr = "";
            if ((0 == joystick.Ffb_h_DevCtrl(data, ref Control)) && DevCtrl2Str(Control, ref CtrlStr))
                dlg.FfbTextBox_Write(String.Format("\r\n >> PID Device Control: {0}", CtrlStr));

            #endregion

            #region Effect Operation
            vJoy.FFB_EFF_OP Operation = new vJoy.FFB_EFF_OP();
            string EffOpStr = "";
            if (0 == joystick.Ffb_h_EffOp(data, ref Operation) && EffectOpStr(Operation.EffectOp, ref EffOpStr))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Operation: {0}", EffOpStr));
                if (Operation.LoopCount == 0xFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Loop until stopped"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Loop {0} times", (int)(Operation.LoopCount)));

            };
            #endregion

            #region Global Device Gain
            Byte Gain = 0;
            if (0 == joystick.Ffb_h_DevGain(data, ref Gain))
                dlg.FfbTextBox_Write(String.Format("\r\n >> Global Device Gain: {0}", Byte2Percent(Gain)));

            #endregion

            #region Condition
            vJoy.FFB_EFF_COND Condition = new vJoy.FFB_EFF_COND();
            if (0 == joystick.Ffb_h_Eff_Cond(data, ref Condition))
            {
                if (Condition.isY)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Y Axis"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> X Axis"));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Center Point Offset: {0}", Condition.CenterPointOffset));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Positive Coefficient: {0}", Condition.PosCoeff));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Negative Coefficient: {0}", Condition.NegCoeff));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Positive Saturation: {0}", Condition.PosSatur));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Negative Saturation: {0}", Condition.NegSatur));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Dead Band: {0}", Condition.DeadBand ));
            }
            #endregion

            #region Envelope
            vJoy.FFB_EFF_ENVLP Envelope = new vJoy.FFB_EFF_ENVLP();
            if (0 == joystick.Ffb_h_Eff_Envlp(data, ref Envelope))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Attack Level: {0}", Envelope.AttackLevel));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Fade Level: {0}", Envelope.FadeLevel));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Attack Time: {0}", (int)(Envelope.AttackTime)));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Fade Time: {0}", (int)(Envelope.FadeTime)));
            };
            #endregion

            #region Periodic
            vJoy.FFB_EFF_PERIOD EffPrd = new vJoy.FFB_EFF_PERIOD();
            if (0 == joystick.Ffb_h_Eff_Period(data, ref EffPrd))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Magnitude: {0}", EffPrd.Magnitude ));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Offset: {0}", EffPrd.Offset));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Phase: {0}", EffPrd.Phase));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Period: {0}", (int)(EffPrd.Period)));
            };
            #endregion

            #region Effect Type
            FFBEType EffectType = FFBEType.ET_NONE;
            if (0 == joystick.Ffb_h_EffNew(data, ref EffectType))
            {
                if (EffectType2Str(EffectType, ref TypeStr))
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Type: {0}", TypeStr));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Type: Unknown"));
            }
            #endregion

            #region Ramp Effect
            vJoy.FFB_EFF_RAMP RampEffect = new vJoy.FFB_EFF_RAMP();
            if (0 == joystick.Ffb_h_Eff_Ramp(data, ref RampEffect))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Ramp Start: {0}", RampEffect.Start));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Ramp End: {0}", RampEffect.End));
            };
            #endregion

            #region Constant Effect
            vJoy.FFB_EFF_CONSTANT ConstantEffect = new vJoy.FFB_EFF_CONSTANT();
            if (0 == joystick.Ffb_h_Eff_Constant(data, ref ConstantEffect))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Constant Magnitude: {0}", ConstantEffect.Magnitude));
            };

            #endregion

            dlg.FfbTextBox_Write(String.Format("\r\n ==============================================\r\n"));

        }

        private static void OnEffectObj(IntPtr data, object userData)
        {

            int id = 0;
            FFBPType type = 0;
            int DSize = 0;
            Byte[] arr = new Byte[20];
            UInt32 wType = 0;
            IntPtr bytes = IntPtr.Zero;
            string TypeStr = "";


            // Get FFB packet raw data
            var result = joystick.Ffb_h_Packet(data, ref wType, ref DSize, ref arr);
            dlg.FfbTextBox_Write(String.Format("\r\n\r\n ============= FFB Packet size Size {0} =============", DSize + 8));

            // Get the ID of the device that this DDB packet refers to
            result = joystick.Ffb_h_DeviceID(data, ref id);
            dlg.FfbTextBox_Write(String.Format("\r\n > Device ID: {0}", id));

            // Get the type of the packet
            result = joystick.Ffb_h_Type(data, ref type);
            if (result == 0)
            {
                bool ok = PacketType2Str(type, ref TypeStr);
                if (!ok)
                    dlg.FfbTextBox_Write(String.Format("\r\n > Packet Type: {0}", type));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n > Packet Type: {0}", TypeStr));
            };

            // Get the index of the effect block (Always 1)
            int iBlock = 0;
            result = joystick.Ffb_h_EBI(data, ref iBlock);
            if (result == 0)
                dlg.FfbTextBox_Write(String.Format("\r\n > Effect Block Index: {0}", iBlock));

            // TODO: Add constant magnitude

            #region Effect Report
            // Effect Report
            vJoy.FFB_EFF_REPORT Effect = new vJoy.FFB_EFF_REPORT();
            result = joystick.Ffb_h_Eff_Report(data, ref Effect);
            if (result == 0)
            {
                ///// This is an Effect Block
                // Effect type
                if (!EffectType2Str(Effect.EffectType, ref TypeStr))
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Report: {0}", Effect.EffectType));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Report: {0}", TypeStr));

                // Effect Direction 
                if (Effect.Polar)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Direction: {0} deg ({1:D2})", Polar2Deg(Effect.Direction), Effect.Direction));
                else
                {
                    dlg.FfbTextBox_Write(String.Format("\r\n >> X Direction: {0:X2}", Effect.DirX));
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Y Direction: {0:X2}", Effect.DirY));
                };

                // Duration of the effect
                if (Effect.Duration == 0xFFFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Duration: Infinit"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Duration: {0} MilliSec", (int)(Effect.Duration)));

                // Trigger Repeat
                if (Effect.TrigerRpt == 0xFFFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Trigger Repeat: Infinit"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Trigger Repeat: {0}", (int)(Effect.TrigerRpt)));

                // Sample Period
                if (Effect.SamplePrd == 0xFFFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Sample Period: Infinit"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Sample Period: {0}", (int)(Effect.SamplePrd)));


                // Gain
                dlg.FfbTextBox_Write(String.Format("\r\n >> Gain: {0}%", Byte2Percent(Effect.Gain)));
            };

            #endregion   Effect Report

            #region PID Device Control
            // Device Control (Global controls such as Device Reset, Device Pause .....
            FFB_CTRL Control = FFB_CTRL.CTRL_STOPALL;
            string CtrlStr = "";
            if ((0 == joystick.Ffb_h_DevCtrl(data, ref Control)) && DevCtrl2Str(Control, ref CtrlStr))
                dlg.FfbTextBox_Write(String.Format("\r\n >> PID Device Control: {0}", CtrlStr));

            #endregion

            #region Effect Operation
            vJoy.FFB_EFF_OP Operation = new vJoy.FFB_EFF_OP();
            string EffOpStr = "";
            if (0 == joystick.Ffb_h_EffOp(data, ref Operation) && EffectOpStr(Operation.EffectOp, ref EffOpStr))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Operation: {0}", EffOpStr));
                if (Operation.LoopCount == 0xFF)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Loop until stopped"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Loop {0} times", (int)(Operation.LoopCount)));

            };
            #endregion

            #region Global Device Gain
            Byte Gain = 0;
            if (0 == joystick.Ffb_h_DevGain(data, ref Gain))
                dlg.FfbTextBox_Write(String.Format("\r\n >> Global Device Gain: {0}", Byte2Percent(Gain)));

            #endregion

            #region Condition
            vJoy.FFB_EFF_COND Condition = new vJoy.FFB_EFF_COND();
            if (0 == joystick.Ffb_h_Eff_Cond(data, ref Condition))
            {
                if (Condition.isY)
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Y Axis"));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> X Axis"));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Center Point Offset: {0}", Condition.CenterPointOffset));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Positive Coefficient: {0}", Condition.PosCoeff));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Negative Coefficient: {0}", Condition.NegCoeff));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Positive Saturation: {0}", Condition.PosSatur ));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Negative Saturation: {0}", Condition.NegSatur));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Dead Band: {0}", Condition.DeadBand));
            }
            #endregion

            #region Envelope
            vJoy.FFB_EFF_ENVLP Envelope = new vJoy.FFB_EFF_ENVLP();
            if (0 == joystick.Ffb_h_Eff_Envlp(data, ref Envelope))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Attack Level: {0}", Envelope.AttackLevel));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Fade Level: {0}", Envelope.FadeLevel ));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Attack Time: {0}", (int)(Envelope.AttackTime)));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Fade Time: {0}", (int)(Envelope.FadeTime)));
            };
            #endregion

            #region Periodic
            vJoy.FFB_EFF_PERIOD EffPrd = new vJoy.FFB_EFF_PERIOD();
            if (0 == joystick.Ffb_h_Eff_Period(data, ref EffPrd))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Magnitude: {0}", EffPrd.Magnitude));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Offset: {0}", EffPrd.Offset));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Phase: {0}", EffPrd.Phase));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Period: {0}", (int)(EffPrd.Period)));
            };
            #endregion

            #region Effect Type
            FFBEType EffectType = FFBEType.ET_NONE;
            if (0 == joystick.Ffb_h_EffNew(data, ref EffectType))
            {
                if (EffectType2Str(EffectType, ref TypeStr))
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Type: {0}", TypeStr));
                else
                    dlg.FfbTextBox_Write(String.Format("\r\n >> Effect Type: Unknown"));
            }
            #endregion

            #region Ramp Effect
            vJoy.FFB_EFF_RAMP RampEffect = new vJoy.FFB_EFF_RAMP();
            if (0 == joystick.Ffb_h_Eff_Ramp(data, ref RampEffect))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Ramp Start: {0}", RampEffect.Start));
                dlg.FfbTextBox_Write(String.Format("\r\n >> Ramp End: {0}", RampEffect.End));
            };
            #endregion

            #region Constant Effect
            vJoy.FFB_EFF_CONSTANT ConstantEffect = new vJoy.FFB_EFF_CONSTANT();
            if (0 == joystick.Ffb_h_Eff_Constant(data, ref ConstantEffect))
            {
                dlg.FfbTextBox_Write(String.Format("\r\n >> Constant Magnitude: {0}", ConstantEffect.Magnitude));
            };

            #endregion
            dlg.FfbTextBox_Write(String.Format("\r\n ==============================================\r\n"));

        }

        // Convert One-Byte 2's complement input to integer
        private static int TwosCompByte2Int(Byte inVal)
        {
            int tmp;
            bool isNeg;

            Byte inv = (Byte)~inVal;
            if ((inVal >> 7) == 0)
                isNeg = false;
            else
                 isNeg = true;

            if (isNeg)
            {
                tmp = (int)(inv);
                tmp = -1 * tmp;
                return tmp;
            }
            else
                return (int)inVal;
        }

        // Polar values (0x00-0xFF) to Degrees (0-360)
        private static int Polar2Deg(Byte Polar)
        {
            return (Polar * 360) / 255;
        }

        // Convert PID Device Control to String
        private static bool DevCtrl2Str(FFB_CTRL Ctrl, ref string Str)
        {

            bool stat = true;

            switch (Ctrl)
            {
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

        // Convert Effect type to String
        private static bool EffectType2Str(FFBEType Type, ref string Str)
        {
            bool stat = true;

            switch (Type)
            {
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
            };

            return stat;
        }

        // Convert Effect operation to string
        private static bool EffectOpStr(FFBOP Op, ref string Str)
        {
            bool stat = true;

            switch (Op)
            {
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

        // Convert Packet type to String
        private static bool PacketType2Str(FFBPType Type, ref string Str)
        {
            bool stat = true;

            switch (Type)
            {
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

        // Convert range 0x00-0xFF to 0%-100%
        private static int Byte2Percent(Byte InByte)
        {
            return (InByte * 100) / 255;
        }

    }
}
