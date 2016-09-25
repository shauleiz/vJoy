
#pragma once
namespace vJoyDemo {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    
    // Must add file LBIndustrialCtrls.dll to exeutable folder
    using namespace LBSoft::IndustrialCtrls;
    using namespace LBSoft::IndustrialCtrls::Knobs;
	using namespace LBSoft::IndustrialCtrls::Buttons;
	using namespace System::Diagnostics;
	using namespace vJoy;

    // from http://www.codeproject.com/KB/miscctrl/NextUIknob.aspx
    // Must add file FunkyLibrary.dll to exeutable folder
    //using namespace NextUI::Collection;


    /// <summary>
    /// Summary for Form1
    /// </summary>
    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:
        Form1(void)
        {
			
            // Form init
            ReportId = -1;

            // IDE-Generated initialization of the form
            InitializeComponent();
            InitializeComponentManual();


            // Test
            BOOL data;
            BOOL ok = vJoyFfbCap(&data);
            // Now make all the arrangements to start working:
            // 1 Test that at least one vJoy device is ready
            // 2 Create a list of available vJoy devices
            // 3 Select the first vJoy Device and acquire it
            // 4 Reset all devices
            // 5 For the selected device: Update GUI according to the featurs and current values
            // 6 Start the watchdog that polls the state of the device


            /*1*/
            if (!vJoyEnabled())
            {
                textBoxInfo->Text = L"vJoy Device driver is not installed - Cannot continue\r\n";
                //return;
            };

            /*2*/
            GetExistingDevices();

            /*3*/
            if (!AcquireVJD(GetCurrentReportId()))
            {
                textBoxInfo->Text = L"Cannot open vJoy Device - Cannot continue\r\n";
                //return;
            };

            /*4*/
            ResetAll();

            /*5*/
            ValidControls();

            /*6*/
            WatchDog->Start();

            return;
        }

    protected:
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~Form1()
        {
            if (components)
            {
                delete components;
            }
        }

    // Manually added globals
    private: HWND hWin;
    private: System::Windows::Forms::TextBox^  textBoxInfo;
    private: HDEVNOTIFY hDeviceNotifyInterFace;
    private: HDEVNOTIFY hDeviceNotifyHandle;
    private: int ReportId;
    private: String^ CbName;
    //private: System::Int32 max_axis;

    // Buttons


















    private: System::Windows::Forms::RadioButton^  radioButtonPov1N;
    private: System::Windows::Forms::RadioButton^  radioButtonPov1E;
    private: System::Windows::Forms::RadioButton^  radioButtonPov1W;
    private: System::Windows::Forms::RadioButton^  radioButtonPov1S;
    private: System::Windows::Forms::RadioButton^  radioButtonPov1X;
    private: System::Windows::Forms::GroupBox^  groupBoxPov1;


















    private: System::Windows::Forms::GroupBox^  groupBoxKnobPov1;
    private: System::Windows::Forms::GroupBox^  groupBoxKnobPov2;
    private: System::Windows::Forms::GroupBox^  groupBoxKnobPov3;
    private: System::Windows::Forms::GroupBox^  groupBoxKnobPov4;
    private: System::Windows::Forms::Label^  LableKnobPov1;
    private: System::Windows::Forms::Label^  LableKnobPov2;
    private: System::Windows::Forms::Label^  LableKnobPov3;
    private: System::Windows::Forms::Label^  LableKnobPov4;
    private: System::Windows::Forms::CheckBox^  checkBoxContPov1;
    private: LBSoft::IndustrialCtrls::Knobs::LBKnob^ lbKnob1;
    private: System::Drawing::Color origIndicatorColor;

private: 


    private: LBSoft::IndustrialCtrls::Buttons::LBButton^ Btn002;
    private: LBSoft::IndustrialCtrls::Buttons::LBButton^ Btn003;
    private: LBSoft::IndustrialCtrls::Buttons::LBButton^ Btn004;
    private: LBSoft::IndustrialCtrls::Buttons::LBButton^ Btn005;
    private: LBSoft::IndustrialCtrls::Buttons::LBButton^ Btn006;




private: System::Windows::Forms::ToolTip^  toolTipDef;
private: System::Windows::Forms::GroupBox^  groupBoxX;
private: System::Windows::Forms::TrackBar^  trackBarX;
private: System::Windows::Forms::TextBox^  textBoxX;
private: System::Windows::Forms::GroupBox^  groupBoxY;
private: System::Windows::Forms::TrackBar^  trackBarY;
private: System::Windows::Forms::TextBox^  textBoxY;
private: System::Windows::Forms::GroupBox^  groupBoxRx;
private: System::Windows::Forms::TrackBar^  trackBarRx;
private: System::Windows::Forms::TextBox^  textBoxRx;
private: System::Windows::Forms::GroupBox^  groupBoxZ;
private: System::Windows::Forms::TrackBar^  trackBarZ;
private: System::Windows::Forms::TextBox^  textBoxZ;
private: System::Windows::Forms::GroupBox^  groupBoxRy;
private: System::Windows::Forms::TrackBar^  trackBarRy;
private: System::Windows::Forms::TextBox^  textBoxRy;
private: System::Windows::Forms::GroupBox^  groupBoxRz;
private: System::Windows::Forms::TrackBar^  trackBarRz;
private: System::Windows::Forms::TextBox^  textBoxRz;
private: System::Windows::Forms::GroupBox^  groupBoxSL0;
private: System::Windows::Forms::TrackBar^  trackBarSL0;
private: System::Windows::Forms::TextBox^  textBoxSL0;
private: System::Windows::Forms::GroupBox^  groupBoxSL1;
private: System::Windows::Forms::TrackBar^  trackBarSL1;
private: System::Windows::Forms::TextBox^  textBoxSL1;
private: System::Windows::Forms::GroupBox^  groupBoxAxes;
private: System::Windows::Forms::GroupBox^  groupBoxButtons;
private: System::Windows::Forms::GroupBox^  groupBoxTarget;

private: System::Windows::Forms::ListBox^  listBoxTarget;
private: System::Windows::Forms::Timer^  WatchDog;
private: System::ComponentModel::IContainer^  components;


    protected: 

    private:
        /// <summary>
        /// Required designer variable.
        /// </summary>


#pragma region Manually generated code

private: bool POVDiscExists, POVContExists;				 
private: System::Windows::Forms::Label^  labelPOVcont;
private: System::Windows::Forms::Label^  labelPOVdisc;

private: System::Windows::Forms::ListBox^  listBoxNpov;
private: System::Windows::Forms::ListBox^  listBoxNpovDisc;

private: array<System::Windows::Forms::CheckBox^> ^ CBvJoyButton; 


// Attributes of axes and POV switches to be updated every time the current device is changed:
private:
    long AxisXmax,		AxisXmin;
    long AxisYmax,		AxisYmin;
    long AxisZmax,		AxisZmin;
    long AxisRXmax,		AxisRXmin;
    long AxisRYmax,		AxisRYmin;
    long AxisRZmax,		AxisRZmin;
    long AxisSL0max,	AxisSL0min;
    long AxisSL1max,	AxisSL1min;
    long PovMax,		PovMin;






private: System::Windows::Forms::Label^  label1;




    private: System::Void ValidControls(void) {
    /*	
        Enable/Disable and set controls according to the capabilities of the selected device
        1. The function gets the currently selected Report ID
        2. Enables/disables Axes, Buttons and POV accordingly
        3. Get the current values and set the controls accordingly
    */

                 GetCurrentReportId();

                 // Select Target device box
                 this->groupBoxTarget->Enabled = (ReportId>=0);

                 // Axes
                 groupBoxX->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_X) == TRUE;
                 groupBoxY->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_Y) == TRUE;
                 groupBoxZ->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_Z) == TRUE;
                 groupBoxRx->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_RX) == TRUE;
                 groupBoxRy->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_RY) == TRUE;
                 groupBoxRz->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_RZ) == TRUE;
                 groupBoxSL0->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_SL0) == TRUE;
                 groupBoxSL1->Enabled =  GetVJDAxisExist(ReportId, HID_USAGE_SL1) == TRUE;

                 // Set axes to default (X,Y,Z to 50%, rest to minimum)
                 long max=-1, min=-1;
                 BOOL gotit = false;
                 if (GetVJDAxisMax(ReportId, HID_USAGE_X, &max) && GetVJDAxisMin(ReportId, HID_USAGE_X, &min))
                 {
                    trackBarX->Value = ConvPercentage((max+min+1)/2, max,min);
                    textBoxX->Text = System::Convert::ToString((int)trackBarX->Value);
                    AxisXmax = max;
                    AxisXmin = min;
                 };

                  if (GetVJDAxisMax(ReportId, HID_USAGE_Y, &max) && GetVJDAxisMin(ReportId, HID_USAGE_Y, &min))
                 {
                     trackBarY->Value = ConvPercentage((max + min + 1) / 2, max, min);
                     textBoxY->Text = System::Convert::ToString((int)trackBarY->Value);
                     AxisYmax = max;
                     AxisYmin = min;
                 };

                 if (GetVJDAxisMax(ReportId, HID_USAGE_Z, &max) && GetVJDAxisMin(ReportId, HID_USAGE_Z, &min))
                 {
                     trackBarZ->Value = ConvPercentage((max + min + 1) / 2, max, min);
                     textBoxZ->Text = System::Convert::ToString((int)trackBarZ->Value);
                     AxisZmax = max;
                     AxisZmin = min;
                 };

                 if (GetVJDAxisMax(ReportId, HID_USAGE_RX, &max) && GetVJDAxisMin(ReportId, HID_USAGE_RX, &min))
                 {
                    trackBarRx->Value = ConvPercentage(min, max, min);
                    textBoxRx->Text = System::Convert::ToString((int)trackBarRx->Value);
                    AxisRXmax=max;
                    AxisRXmin=min;
                 };

                 if (GetVJDAxisMax(ReportId, HID_USAGE_RY, &max) && GetVJDAxisMin(ReportId, HID_USAGE_RY, &min))
                 {
                    trackBarRy->Value = ConvPercentage(min, max, min);
                    textBoxRy->Text = System::Convert::ToString((int)trackBarRy->Value);
                    AxisRYmax=max;
                    AxisRYmin=min;
                 };

                 if (GetVJDAxisMax(ReportId, HID_USAGE_RZ, &max) && GetVJDAxisMin(ReportId, HID_USAGE_RZ, &min))
                 {
                    trackBarRz->Value = ConvPercentage(min, max, min);
                    textBoxRz->Text = System::Convert::ToString((int)trackBarRz->Value);
                    AxisRZmax=max;
                    AxisRZmin=min;
                 };

                 if (GetVJDAxisMax(ReportId, HID_USAGE_SL0, &max) && GetVJDAxisMin(ReportId, HID_USAGE_SL0, &min))
                 {
                    trackBarSL0->Value = ConvPercentage(min, max, min);
                    textBoxSL0->Text = System::Convert::ToString((int)trackBarSL0->Value);
                    AxisSL0max=max;
                    AxisSL0min=min;
                 };

                 if (GetVJDAxisMax(ReportId, HID_USAGE_SL1, &max) && GetVJDAxisMin(ReportId, HID_USAGE_SL1, &min))
                 {
                    trackBarSL1->Value = ConvPercentage(min, max, min);
                    textBoxSL1->Text = System::Convert::ToString((int)trackBarSL1->Value);
                    AxisSL1max=max;
                    AxisSL1min=min;
                 };

                 
                 //Buttons: Enable/Disable and set to default(off)
                 int nButtons = GetVJDButtonNumber(ReportId);
                 for (int index = 0; index < 128; index++)
                 {
                     this->CBvJoyButton[index]->Enabled = index<nButtons;
                     this->CBvJoyButton[index]->Checked = false;
                 }

                 //// POV Hat switches
                 POVDiscExists = GetVJDDiscPovNumber(ReportId)>0;
                 POVContExists = GetVJDContPovNumber(ReportId)>0;
                 groupBoxPov1->Enabled = POVDiscExists;
                 groupBoxKnobPov1->Enabled = POVContExists;

                 int n;
                 listBoxNpov->Items->Clear();

                 listBoxNpovDisc->Items->Clear();
                 n=GetVJDContPovNumber(ReportId);
                 if (n>0 && n<5)
                 {
                     for (int i=1; i<=n; i++)
                         listBoxNpov->Items->Add(i);
                     listBoxNpov->SelectedIndex = 0;
                     lbKnob1->Value = 0;
                     checkBoxContPov1->Checked = true;
                 };

                 n=GetVJDDiscPovNumber(ReportId);
                 if (n>0 && n<5)
                 {
                     for (int i=1; i<=n; i++)
                         listBoxNpovDisc->Items->Add(i);
                     listBoxNpovDisc->SelectedIndex = 0;
                     radioButtonPov1X->Checked = true;
                     radioButtonPov1N->Checked = false;
                     radioButtonPov1E->Checked = false;
                     radioButtonPov1W->Checked = false;
                     radioButtonPov1S->Checked = false;
                 };

                 // Get vJoy device driver Version
                 TCHAR Msg[MSG_SIZE];
                 getDeviceAttrib(Msg);

                 // Put data in text box
                 String ^ str = gcnew System::String(Msg);
                 textBoxInfo->Text = str;
                 textBoxInfo->Select(1000,0);
                 delete str;

        }
    
// Fill the data structure with data collected from the controls
// Then send to vJoy Device
    private: System::Void js_update(void) {
                 JOYSTICK_POSITION_V2	Report;
                 static UINT pov[4]={(UINT)-1,(UINT)-1,(UINT)-1,(UINT)-1};


                 // If no target selected then NO-OP
                 if (listBoxTarget->SelectedIndex == -1)
                     return;

                 // Report Id
                 Report.bDevice = System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString());

                 // Axes
                 Report.wAxisX = (USHORT)ConvAbsolute(this->trackBarX->Value, AxisXmax, AxisXmin);
                 Report.wAxisY = (USHORT)ConvAbsolute(this->trackBarY->Value, AxisYmax, AxisYmin);
                 Report.wAxisZ = (USHORT)ConvAbsolute(this->trackBarZ->Value, AxisZmax, AxisZmin);
                 Report.wAxisXRot = (USHORT)this->trackBarRx->Value;
                 Report.wAxisYRot = (USHORT)this->trackBarRy->Value;
                 Report.wAxisZRot = (USHORT)this->trackBarRz->Value;
                 Report.wSlider = (USHORT)this->trackBarSL0->Value;
                 Report.wDial = (USHORT)this->trackBarSL1->Value;

                 // Not implemented
                 Report.wAxisVBRX = 0;
                 Report.wAxisVBRY = 0;
                 Report.wAxisVBRZ = 0;
                 Report.wAxisVX = 0;
                 Report.wAxisVY = 0;
                 Report.wAxisVZ = 0;
                 Report.wAileron = 0;
                 Report.wRudder = 0;
                 Report.wThrottle = 0;
                 Report.wWheel = 0;

                 // Buttons
                 for (int index=0; index<128; index++)
                 {
                     LONG * Target;
                     int Shift = index%32;

                     switch (index/32)
                     {
                     case 0:
                         Target = &(Report.lButtons);
                         break;
                     case 1:
                         Target = &(Report.lButtonsEx1);
                         break;
                     case 2:
                         Target = &(Report.lButtonsEx2);
                         break;
                     case 3:
                         Target = &(Report.lButtonsEx3);
                         break;
                     default:
                         Target = &(Report.lButtonsEx3);
                         break;
                     };

                     if (this->CBvJoyButton[index]->Checked)
                         *Target |= (0x00000001 << Shift);
                     else
                         *Target &= (0xFFFFFFFF ^ (0x00000001 << Shift));
                 };



#if 0
                 if (this->checkBox1->Checked ) Report.lButtons |= 0x00000001; else Report.lButtons &= 0xFFFFFFFE;
                 if (this->checkBox2->Checked ) Report.lButtons |= 0x00000002; else Report.lButtons &= 0xFFFFFFFD;
                 if (this->checkBox3->Checked ) Report.lButtons |= 0x00000004; else Report.lButtons &= 0xFFFFFFFB;
                 if (this->checkBox4->Checked ) Report.lButtons |= 0x00000008; else Report.lButtons &= 0xFFFFFFF7;
                 if (this->checkBox5->Checked ) Report.lButtons |= 0x00000010; else Report.lButtons &= 0xFFFFFFEF;
                 if (this->checkBox6->Checked ) Report.lButtons |= 0x00000020; else Report.lButtons &= 0xFFFFFFDF;
                 if (this->checkBox7->Checked ) Report.lButtons |= 0x00000040; else Report.lButtons &= 0xFFFFFFBF;
                 if (this->checkBox8->Checked ) Report.lButtons |= 0x00000080; else Report.lButtons &= 0xFFFFFF7F;
                 if (this->checkBox9->Checked ) Report.lButtons |= 0x00000100; else Report.lButtons &= 0xFFFFFEFF;
                 if (this->checkBox10->Checked) Report.lButtons |= 0x00000200; else Report.lButtons &= 0xFFFFFDFF;
                 if (this->checkBox11->Checked) Report.lButtons |= 0x00000400; else Report.lButtons &= 0xFFFFFBFF;
                 if (this->checkBox12->Checked) Report.lButtons |= 0x00000800; else Report.lButtons &= 0xFFFFF7FF;
                 if (this->checkBox13->Checked) Report.lButtons |= 0x00001000; else Report.lButtons &= 0xFFFFEFFF;
                 if (this->checkBox14->Checked) Report.lButtons |= 0x00002000; else Report.lButtons &= 0xFFFFDFFF;
                 if (this->checkBox15->Checked) Report.lButtons |= 0x00004000; else Report.lButtons &= 0xFFFFBFFF;
                 if (this->checkBox16->Checked) Report.lButtons |= 0x00008000; else Report.lButtons &= 0xFFFF7FFF;
                 if (this->checkBox17->Checked) Report.lButtons |= 0x00010000; else Report.lButtons &= 0xFFFEFFFF;
                 if (this->checkBox18->Checked) Report.lButtons |= 0x00020000; else Report.lButtons &= 0xFFFDFFFF;
                 if (this->checkBox19->Checked) Report.lButtons |= 0x00040000; else Report.lButtons &= 0xFFFBFFFF;
                 if (this->checkBox20->Checked) Report.lButtons |= 0x00080000; else Report.lButtons &= 0xFFF7FFFF;
                 if (this->checkBox21->Checked) Report.lButtons |= 0x00100000; else Report.lButtons &= 0xFFEFFFFF;
                 if (this->checkBox22->Checked) Report.lButtons |= 0x00200000; else Report.lButtons &= 0xFFDFFFFF;
                 if (this->checkBox23->Checked) Report.lButtons |= 0x00400000; else Report.lButtons &= 0xFFBFFFFF;
                 if (this->checkBox24->Checked) Report.lButtons |= 0x00800000; else Report.lButtons &= 0xFF7FFFFF;
                 if (this->checkBox25->Checked) Report.lButtons |= 0x01000000; else Report.lButtons &= 0xFEFFFFFF;
                 if (this->checkBox26->Checked) Report.lButtons |= 0x02000000; else Report.lButtons &= 0xFDFFFFFF;
                 if (this->checkBox27->Checked) Report.lButtons |= 0x04000000; else Report.lButtons &= 0xFBFFFFFF;
                 if (this->checkBox28->Checked) Report.lButtons |= 0x08000000; else Report.lButtons &= 0xF7FFFFFF;
                 if (this->checkBox29->Checked) Report.lButtons |= 0x10000000; else Report.lButtons &= 0xEFFFFFFF;
                 if (this->checkBox30->Checked) Report.lButtons |= 0x20000000; else Report.lButtons &= 0xDFFFFFFF;
                 if (this->checkBox31->Checked) Report.lButtons |= 0x40000000; else Report.lButtons &= 0xBFFFFFFF;
                 if (this->checkBox32->Checked) Report.lButtons |= 0x80000000; else Report.lButtons &= 0x7FFFFFFF;  
#endif // 1


                 // Discrete POV
                 if (POVDiscExists && listBoxNpovDisc->SelectedIndex!=-1)
                 {
                     int index = System::Convert::ToInt32(listBoxNpovDisc->Text)-1;
                     int val = 0x0F;
                     // 5-state POV Hats
                     if (this->radioButtonPov1X->Checked) val = 0x0F;
                     if (this->radioButtonPov1N->Checked) val = 0x00;
                     if (this->radioButtonPov1E->Checked) val = 0x01;
                     if (this->radioButtonPov1S->Checked) val = 0x02;
                     if (this->radioButtonPov1W->Checked) val = 0x03;
                     Report.bHats = 0;
                     pov[index] = val;
                     Report.bHats = (pov[3]<<12) + (pov[2]<<8) + (pov[1]<<4) + (pov[0]<<0);
                     //if (this->radioButtonPov1X->Checked) (Report.bHats &= 0xFFF0)|= 0x0F;
                     //if (this->radioButtonPov1N->Checked) (Report.bHats &= 0xFFF0)|= 0x00;
                     //if (this->radioButtonPov1E->Checked) (Report.bHats &= 0xFFF0)|= 0x01;
                     //if (this->radioButtonPov1S->Checked) (Report.bHats &= 0xFFF0)|= 0x02;
                     //if (this->radioButtonPov1W->Checked) (Report.bHats &= 0xFFF0)|= 0x03;

                     groupBoxKnobPov1->Visible = false;
                     groupBoxPov1->Visible = true;
                 };

                 // Continuous POV
                 if (POVContExists)
                 {
                     // Continuous POV Hats
                     if (LableKnobPov1->Visible && listBoxNpov->SelectedIndex!=-1)
                     {
                         int val = System::Convert::ToInt32(LableKnobPov1->Text);
                         int index = System::Convert::ToInt32(listBoxNpov->Text)-1;
                         pov[index] = val;
                        Report.bHats = pov[0];
                        Report.bHatsEx1 = pov[1];
                        Report.bHatsEx2 = pov[2];
                        Report.bHatsEx3 = pov[3]; 
                     }
                     else
                     {
                         if (!checkBoxContPov1->Checked && listBoxNpov->SelectedIndex!=-1)
                         {
                             int index = System::Convert::ToInt32(listBoxNpov->Text)-1;
                             pov[index] = -1;
                             Report.bHats = pov[0];
                             Report.bHatsEx1 = pov[1];
                             Report.bHatsEx2 = pov[2];
                             Report.bHatsEx3 = pov[3]; 
                         }
                         else
                             Report.bHats = Report.bHatsEx1 = Report.bHatsEx2 = Report.bHatsEx3 = -1;
                     };

                     groupBoxPov1->Visible = false;
                     groupBoxKnobPov1->Visible = true;
                 }

                 // Send info to selected vJoy Device
                 UpdateVJD(ReportId, (PVOID) &Report);
             }

#pragma endregion


#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void)
        {
            this->components = (gcnew System::ComponentModel::Container());
            System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
            this->textBoxInfo = (gcnew System::Windows::Forms::TextBox());
            this->radioButtonPov1N = (gcnew System::Windows::Forms::RadioButton());
            this->radioButtonPov1E = (gcnew System::Windows::Forms::RadioButton());
            this->radioButtonPov1W = (gcnew System::Windows::Forms::RadioButton());
            this->radioButtonPov1S = (gcnew System::Windows::Forms::RadioButton());
            this->radioButtonPov1X = (gcnew System::Windows::Forms::RadioButton());
            this->groupBoxPov1 = (gcnew System::Windows::Forms::GroupBox());
            this->labelPOVdisc = (gcnew System::Windows::Forms::Label());
            this->listBoxNpovDisc = (gcnew System::Windows::Forms::ListBox());
            this->Btn002 = (gcnew LBSoft::IndustrialCtrls::Buttons::LBButton());
            this->Btn003 = (gcnew LBSoft::IndustrialCtrls::Buttons::LBButton());
            this->Btn004 = (gcnew LBSoft::IndustrialCtrls::Buttons::LBButton());
            this->Btn005 = (gcnew LBSoft::IndustrialCtrls::Buttons::LBButton());
            this->Btn006 = (gcnew LBSoft::IndustrialCtrls::Buttons::LBButton());
            this->groupBoxKnobPov1 = (gcnew System::Windows::Forms::GroupBox());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->labelPOVcont = (gcnew System::Windows::Forms::Label());
            this->listBoxNpov = (gcnew System::Windows::Forms::ListBox());
            this->checkBoxContPov1 = (gcnew System::Windows::Forms::CheckBox());
            this->LableKnobPov1 = (gcnew System::Windows::Forms::Label());
            this->lbKnob1 = (gcnew LBSoft::IndustrialCtrls::Knobs::LBKnob());
            this->toolTipDef = (gcnew System::Windows::Forms::ToolTip(this->components));
            this->groupBoxX = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarX = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxX = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxY = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarY = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxY = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxRx = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarRx = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxRx = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxZ = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarZ = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxZ = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxRy = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarRy = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxRy = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxRz = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarRz = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxRz = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxSL0 = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarSL0 = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxSL0 = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxSL1 = (gcnew System::Windows::Forms::GroupBox());
            this->trackBarSL1 = (gcnew System::Windows::Forms::TrackBar());
            this->textBoxSL1 = (gcnew System::Windows::Forms::TextBox());
            this->groupBoxAxes = (gcnew System::Windows::Forms::GroupBox());
            this->groupBoxButtons = (gcnew System::Windows::Forms::GroupBox());
            this->groupBoxTarget = (gcnew System::Windows::Forms::GroupBox());
            this->listBoxTarget = (gcnew System::Windows::Forms::ListBox());
            this->WatchDog = (gcnew System::Windows::Forms::Timer(this->components));
            this->groupBoxPov1->SuspendLayout();
            this->groupBoxKnobPov1->SuspendLayout();
            this->groupBoxX->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarX))->BeginInit();
            this->groupBoxY->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarY))->BeginInit();
            this->groupBoxRx->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRx))->BeginInit();
            this->groupBoxZ->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarZ))->BeginInit();
            this->groupBoxRy->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRy))->BeginInit();
            this->groupBoxRz->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRz))->BeginInit();
            this->groupBoxSL0->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarSL0))->BeginInit();
            this->groupBoxSL1->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarSL1))->BeginInit();
            this->groupBoxAxes->SuspendLayout();
            this->groupBoxTarget->SuspendLayout();
            this->SuspendLayout();
            // 
            // textBoxInfo
            // 
            this->textBoxInfo->BackColor = System::Drawing::SystemColors::Info;
            this->textBoxInfo->Cursor = System::Windows::Forms::Cursors::No;
            this->textBoxInfo->Location = System::Drawing::Point(13, 13);
            this->textBoxInfo->Multiline = true;
            this->textBoxInfo->Name = L"textBoxInfo";
            this->textBoxInfo->ReadOnly = true;
            this->textBoxInfo->Size = System::Drawing::Size(389, 43);
            this->textBoxInfo->TabIndex = 1;
            // 
            // radioButtonPov1N
            // 
            this->radioButtonPov1N->Appearance = System::Windows::Forms::Appearance::Button;
            this->radioButtonPov1N->AutoSize = true;
            this->radioButtonPov1N->CheckAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1N->Location = System::Drawing::Point(46, 53);
            this->radioButtonPov1N->MinimumSize = System::Drawing::Size(28, 28);
            this->radioButtonPov1N->Name = L"radioButtonPov1N";
            this->radioButtonPov1N->Size = System::Drawing::Size(28, 28);
            this->radioButtonPov1N->TabIndex = 11;
            this->radioButtonPov1N->Text = L"N";
            this->radioButtonPov1N->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1N->UseVisualStyleBackColor = true;
            this->radioButtonPov1N->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButtonPov1N_CheckedChanged);
            // 
            // radioButtonPov1E
            // 
            this->radioButtonPov1E->Appearance = System::Windows::Forms::Appearance::Button;
            this->radioButtonPov1E->AutoSize = true;
            this->radioButtonPov1E->CheckAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1E->Location = System::Drawing::Point(80, 89);
            this->radioButtonPov1E->MinimumSize = System::Drawing::Size(28, 28);
            this->radioButtonPov1E->Name = L"radioButtonPov1E";
            this->radioButtonPov1E->Size = System::Drawing::Size(28, 28);
            this->radioButtonPov1E->TabIndex = 12;
            this->radioButtonPov1E->Text = L"E";
            this->radioButtonPov1E->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1E->UseVisualStyleBackColor = true;
            this->radioButtonPov1E->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButtonPov1E_CheckedChanged);
            // 
            // radioButtonPov1W
            // 
            this->radioButtonPov1W->Appearance = System::Windows::Forms::Appearance::Button;
            this->radioButtonPov1W->AutoSize = true;
            this->radioButtonPov1W->CheckAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1W->Location = System::Drawing::Point(12, 89);
            this->radioButtonPov1W->MinimumSize = System::Drawing::Size(28, 28);
            this->radioButtonPov1W->Name = L"radioButtonPov1W";
            this->radioButtonPov1W->Size = System::Drawing::Size(28, 28);
            this->radioButtonPov1W->TabIndex = 13;
            this->radioButtonPov1W->Text = L"W";
            this->radioButtonPov1W->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1W->UseVisualStyleBackColor = true;
            this->radioButtonPov1W->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButtonPov1W_CheckedChanged);
            // 
            // radioButtonPov1S
            // 
            this->radioButtonPov1S->Appearance = System::Windows::Forms::Appearance::Button;
            this->radioButtonPov1S->AutoSize = true;
            this->radioButtonPov1S->CheckAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1S->Location = System::Drawing::Point(46, 123);
            this->radioButtonPov1S->MinimumSize = System::Drawing::Size(28, 28);
            this->radioButtonPov1S->Name = L"radioButtonPov1S";
            this->radioButtonPov1S->Size = System::Drawing::Size(28, 28);
            this->radioButtonPov1S->TabIndex = 14;
            this->radioButtonPov1S->Text = L"S";
            this->radioButtonPov1S->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1S->UseVisualStyleBackColor = true;
            this->radioButtonPov1S->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButtonPov1S_CheckedChanged);
            // 
            // radioButtonPov1X
            // 
            this->radioButtonPov1X->Appearance = System::Windows::Forms::Appearance::Button;
            this->radioButtonPov1X->AutoSize = true;
            this->radioButtonPov1X->CheckAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1X->Checked = true;
            this->radioButtonPov1X->Location = System::Drawing::Point(46, 89);
            this->radioButtonPov1X->MinimumSize = System::Drawing::Size(28, 28);
            this->radioButtonPov1X->Name = L"radioButtonPov1X";
            this->radioButtonPov1X->Size = System::Drawing::Size(28, 28);
            this->radioButtonPov1X->TabIndex = 15;
            this->radioButtonPov1X->TabStop = true;
            this->radioButtonPov1X->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->radioButtonPov1X->UseVisualStyleBackColor = true;
            this->radioButtonPov1X->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButtonPov1X_CheckedChanged);
            // 
            // groupBoxPov1
            // 
            this->groupBoxPov1->Controls->Add(this->labelPOVdisc);
            this->groupBoxPov1->Controls->Add(this->radioButtonPov1X);
            this->groupBoxPov1->Controls->Add(this->radioButtonPov1N);
            this->groupBoxPov1->Controls->Add(this->radioButtonPov1S);
            this->groupBoxPov1->Controls->Add(this->radioButtonPov1E);
            this->groupBoxPov1->Controls->Add(this->radioButtonPov1W);
            this->groupBoxPov1->Controls->Add(this->listBoxNpovDisc);
            this->groupBoxPov1->Enabled = false;
            this->groupBoxPov1->Location = System::Drawing::Point(405, 407);
            this->groupBoxPov1->Name = L"groupBoxPov1";
            this->groupBoxPov1->Size = System::Drawing::Size(117, 173);
            this->groupBoxPov1->TabIndex = 5;
            this->groupBoxPov1->TabStop = false;
            this->groupBoxPov1->Text = L"Discrete POV";
            // 
            // labelPOVdisc
            // 
            this->labelPOVdisc->AutoSize = true;
            this->labelPOVdisc->Location = System::Drawing::Point(11, 21);
            this->labelPOVdisc->Name = L"labelPOVdisc";
            this->labelPOVdisc->Size = System::Drawing::Size(40, 13);
            this->labelPOVdisc->TabIndex = 3;
            this->labelPOVdisc->Text = L"Select:";
            // 
            // listBoxNpovDisc
            // 
            this->listBoxNpovDisc->FormattingEnabled = true;
            this->listBoxNpovDisc->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"1", L"2", L"3", L"4"});
            this->listBoxNpovDisc->Location = System::Drawing::Point(57, 18);
            this->listBoxNpovDisc->Name = L"listBoxNpovDisc";
            this->listBoxNpovDisc->Size = System::Drawing::Size(50, 30);
            this->listBoxNpovDisc->TabIndex = 1;
            // 
            // Btn002
            // 
            this->Btn002->BackColor = System::Drawing::Color::Transparent;
            this->Btn002->ButtonColor = System::Drawing::Color::Red;
            this->Btn002->Label = L"";
            this->Btn002->Location = System::Drawing::Point(0, 0);
            this->Btn002->Name = L"Btn002";
            this->Btn002->Renderer = nullptr;
            this->Btn002->Size = System::Drawing::Size(150, 150);
            this->Btn002->State = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonState::Normal;
            this->Btn002->Style = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonStyle::Circular;
            this->Btn002->TabIndex = 0;
            // 
            // Btn003
            // 
            this->Btn003->BackColor = System::Drawing::Color::Transparent;
            this->Btn003->ButtonColor = System::Drawing::Color::Red;
            this->Btn003->Label = L"";
            this->Btn003->Location = System::Drawing::Point(0, 700);
            this->Btn003->Name = L"Btn003";
            this->Btn003->Renderer = nullptr;
            this->Btn003->Size = System::Drawing::Size(150, 150);
            this->Btn003->State = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonState::Normal;
            this->Btn003->Style = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonStyle::Circular;
            this->Btn003->TabIndex = 0;
            // 
            // Btn004
            // 
            this->Btn004->BackColor = System::Drawing::Color::Transparent;
            this->Btn004->ButtonColor = System::Drawing::Color::Red;
            this->Btn004->Label = L"";
            this->Btn004->Location = System::Drawing::Point(0, 0);
            this->Btn004->Name = L"Btn004";
            this->Btn004->Renderer = nullptr;
            this->Btn004->Size = System::Drawing::Size(150, 150);
            this->Btn004->State = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonState::Normal;
            this->Btn004->Style = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonStyle::Circular;
            this->Btn004->TabIndex = 0;
            // 
            // Btn005
            // 
            this->Btn005->BackColor = System::Drawing::Color::Transparent;
            this->Btn005->ButtonColor = System::Drawing::Color::Red;
            this->Btn005->Label = L"";
            this->Btn005->Location = System::Drawing::Point(0, 0);
            this->Btn005->Name = L"Btn005";
            this->Btn005->Renderer = nullptr;
            this->Btn005->Size = System::Drawing::Size(150, 150);
            this->Btn005->State = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonState::Normal;
            this->Btn005->Style = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonStyle::Circular;
            this->Btn005->TabIndex = 0;
            // 
            // Btn006
            // 
            this->Btn006->BackColor = System::Drawing::Color::Transparent;
            this->Btn006->ButtonColor = System::Drawing::Color::Red;
            this->Btn006->Label = L"1";
            this->Btn006->Location = System::Drawing::Point(405, 600);
            this->Btn006->Name = L"Btn006";
            this->Btn006->Renderer = nullptr;
            this->Btn006->Size = System::Drawing::Size(4150, 4150);
            this->Btn006->State = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonState::Normal;
            this->Btn006->Style = LBSoft::IndustrialCtrls::Buttons::LBButton::ButtonStyle::Circular;
            this->Btn006->TabIndex = 0;
            // 
            // groupBoxKnobPov1
            // 
            this->groupBoxKnobPov1->Controls->Add(this->label1);
            this->groupBoxKnobPov1->Controls->Add(this->labelPOVcont);
            this->groupBoxKnobPov1->Controls->Add(this->listBoxNpov);
            this->groupBoxKnobPov1->Controls->Add(this->checkBoxContPov1);
            this->groupBoxKnobPov1->Controls->Add(this->LableKnobPov1);
            this->groupBoxKnobPov1->Controls->Add(this->lbKnob1);
            this->groupBoxKnobPov1->Location = System::Drawing::Point(405, 407);
            this->groupBoxKnobPov1->Name = L"groupBoxKnobPov1";
            this->groupBoxKnobPov1->Size = System::Drawing::Size(134, 217);
            this->groupBoxKnobPov1->TabIndex = 1;
            this->groupBoxKnobPov1->TabStop = false;
            this->groupBoxKnobPov1->Text = L"Continuous POV";
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Location = System::Drawing::Point(49, 154);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(74, 13);
            this->label1->TabIndex = 4;
            this->label1->Text = L"1/100 Degree";
            // 
            // labelPOVcont
            // 
            this->labelPOVcont->AutoSize = true;
            this->labelPOVcont->Location = System::Drawing::Point(33, 24);
            this->labelPOVcont->Name = L"labelPOVcont";
            this->labelPOVcont->Size = System::Drawing::Size(40, 13);
            this->labelPOVcont->TabIndex = 3;
            this->labelPOVcont->Text = L"Select:";
            // 
            // listBoxNpov
            // 
            this->listBoxNpov->FormattingEnabled = true;
            this->listBoxNpov->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"1", L"2", L"3", L"4"});
            this->listBoxNpov->Location = System::Drawing::Point(79, 19);
            this->listBoxNpov->Name = L"listBoxNpov";
            this->listBoxNpov->Size = System::Drawing::Size(50, 30);
            this->listBoxNpov->TabIndex = 2;
            // 
            // checkBoxContPov1
            // 
            this->checkBoxContPov1->Appearance = System::Windows::Forms::Appearance::Button;
            this->checkBoxContPov1->AutoSize = true;
            this->checkBoxContPov1->Checked = true;
            this->checkBoxContPov1->CheckState = System::Windows::Forms::CheckState::Checked;
            this->checkBoxContPov1->Location = System::Drawing::Point(46, 89);
            this->checkBoxContPov1->MinimumSize = System::Drawing::Size(24, 24);
            this->checkBoxContPov1->Name = L"checkBoxContPov1";
            this->checkBoxContPov1->Size = System::Drawing::Size(24, 24);
            this->checkBoxContPov1->TabIndex = 1;
            this->toolTipDef->SetToolTip(this->checkBoxContPov1, L"Press to enter/leave neutral state");
            this->checkBoxContPov1->UseVisualStyleBackColor = true;
            this->checkBoxContPov1->CheckedChanged += gcnew System::EventHandler(this, &Form1::ContPov1ChckChanged);
            // 
            // LableKnobPov1
            // 
            this->LableKnobPov1->AutoSize = true;
            this->LableKnobPov1->Location = System::Drawing::Point(6, 155);
            this->LableKnobPov1->Name = L"LableKnobPov1";
            this->LableKnobPov1->Size = System::Drawing::Size(37, 13);
            this->LableKnobPov1->TabIndex = 0;
            this->LableKnobPov1->Text = L"00000";
            // 
            // lbKnob1
            // 
            this->lbKnob1->BackColor = System::Drawing::Color::Transparent;
            this->lbKnob1->IndicatorColor = System::Drawing::SystemColors::ButtonFace;
            this->lbKnob1->IndicatorOffset = 5;
            this->lbKnob1->KnobColor = System::Drawing::Color::Maroon;
            this->lbKnob1->Location = System::Drawing::Point(12, 57);
            this->lbKnob1->MaxValue = 35999;
            this->lbKnob1->MinValue = 0;
            this->lbKnob1->Name = L"lbKnob1";
            this->lbKnob1->Renderer = nullptr;
            this->lbKnob1->ScaleColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)), 
                static_cast<System::Int32>(static_cast<System::Byte>(192)));
            this->lbKnob1->Size = System::Drawing::Size(96, 90);
            this->lbKnob1->StepValue = 10;
            this->lbKnob1->Style = LBSoft::IndustrialCtrls::Knobs::LBKnob::KnobStyle::Circular;
            this->lbKnob1->TabIndex = 0;
            this->lbKnob1->Value = 0;
            this->lbKnob1->KnobChangeValue += gcnew LBSoft::IndustrialCtrls::Knobs::KnobChangeValue(this, &Form1::lbKnob1_KnobChangeValue);
            // 
            // toolTipDef
            // 
            this->toolTipDef->IsBalloon = true;
            // 
            // groupBoxX
            // 
            this->groupBoxX->Controls->Add(this->trackBarX);
            this->groupBoxX->Controls->Add(this->textBoxX);
            this->groupBoxX->Location = System::Drawing::Point(3, 16);
            this->groupBoxX->Name = L"groupBoxX";
            this->groupBoxX->Size = System::Drawing::Size(60, 310);
            this->groupBoxX->TabIndex = 0;
            this->groupBoxX->TabStop = false;
            this->groupBoxX->Text = L"Axis X";
            this->groupBoxX->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarX
            // 
            this->trackBarX->LargeChange = 1;
            this->trackBarX->Location = System::Drawing::Point(6, 45);
            this->trackBarX->Maximum = 100;
            this->trackBarX->Name = L"trackBarX";
            this->trackBarX->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarX->Size = System::Drawing::Size(45, 260);
            this->trackBarX->TabIndex = 1;
            this->trackBarX->TickFrequency = 10;
            this->trackBarX->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarX->Value = 50;
            this->trackBarX->Scroll += gcnew System::EventHandler(this, &Form1::trackBarX_Scroll);
            // 
            // textBoxX
            // 
            this->textBoxX->Location = System::Drawing::Point(6, 19);
            this->textBoxX->Name = L"textBoxX";
            this->textBoxX->Size = System::Drawing::Size(43, 20);
            this->textBoxX->TabIndex = 0;
            this->textBoxX->Text = L"50";
            this->textBoxX->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxX_TextChanged);
            // 
            // groupBoxY
            // 
            this->groupBoxY->Controls->Add(this->trackBarY);
            this->groupBoxY->Controls->Add(this->textBoxY);
            this->groupBoxY->Location = System::Drawing::Point(68, 16);
            this->groupBoxY->Name = L"groupBoxY";
            this->groupBoxY->Size = System::Drawing::Size(60, 310);
            this->groupBoxY->TabIndex = 2;
            this->groupBoxY->TabStop = false;
            this->groupBoxY->Text = L"Axis Y";
            this->groupBoxY->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarY
            // 
            this->trackBarY->LargeChange = 1;
            this->trackBarY->Location = System::Drawing::Point(6, 45);
            this->trackBarY->Maximum = 100;
            this->trackBarY->Name = L"trackBarY";
            this->trackBarY->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarY->Size = System::Drawing::Size(45, 260);
            this->trackBarY->TabIndex = 2;
            this->trackBarY->TickFrequency = 10;
            this->trackBarY->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarY->Value = 50;
            this->trackBarY->Scroll += gcnew System::EventHandler(this, &Form1::trackBarY_Scroll);
            // 
            // textBoxY
            // 
            this->textBoxY->Location = System::Drawing::Point(6, 19);
            this->textBoxY->Name = L"textBoxY";
            this->textBoxY->Size = System::Drawing::Size(43, 20);
            this->textBoxY->TabIndex = 1;
            this->textBoxY->Text = L"50";
            this->textBoxY->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxY_TextChanged);
            // 
            // groupBoxRx
            // 
            this->groupBoxRx->Controls->Add(this->trackBarRx);
            this->groupBoxRx->Controls->Add(this->textBoxRx);
            this->groupBoxRx->Location = System::Drawing::Point(200, 16);
            this->groupBoxRx->Name = L"groupBoxRx";
            this->groupBoxRx->Size = System::Drawing::Size(60, 310);
            this->groupBoxRx->TabIndex = 2;
            this->groupBoxRx->TabStop = false;
            this->groupBoxRx->Text = L"Axis Rx";
            this->groupBoxRx->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarRx
            // 
            this->trackBarRx->LargeChange = 1;
            this->trackBarRx->Location = System::Drawing::Point(6, 45);
            this->trackBarRx->Maximum = 100;
            this->trackBarRx->Name = L"trackBarRx";
            this->trackBarRx->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarRx->Size = System::Drawing::Size(45, 260);
            this->trackBarRx->TabIndex = 2;
            this->trackBarRx->TickFrequency = 10;
            this->trackBarRx->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarRx->Scroll += gcnew System::EventHandler(this, &Form1::trackBarRx_Scroll);
            // 
            // textBoxRx
            // 
            this->textBoxRx->Location = System::Drawing::Point(6, 19);
            this->textBoxRx->Name = L"textBoxRx";
            this->textBoxRx->Size = System::Drawing::Size(43, 20);
            this->textBoxRx->TabIndex = 1;
            this->textBoxRx->Text = L"0";
            this->textBoxRx->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxRx_TextChanged);
            // 
            // groupBoxZ
            // 
            this->groupBoxZ->Controls->Add(this->trackBarZ);
            this->groupBoxZ->Controls->Add(this->textBoxZ);
            this->groupBoxZ->Location = System::Drawing::Point(134, 16);
            this->groupBoxZ->Name = L"groupBoxZ";
            this->groupBoxZ->Size = System::Drawing::Size(60, 310);
            this->groupBoxZ->TabIndex = 2;
            this->groupBoxZ->TabStop = false;
            this->groupBoxZ->Text = L"Axis Z";
            this->groupBoxZ->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarZ
            // 
            this->trackBarZ->LargeChange = 1;
            this->trackBarZ->Location = System::Drawing::Point(6, 45);
            this->trackBarZ->Maximum = 100;
            this->trackBarZ->Name = L"trackBarZ";
            this->trackBarZ->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarZ->Size = System::Drawing::Size(45, 260);
            this->trackBarZ->TabIndex = 2;
            this->trackBarZ->TickFrequency = 10;
            this->trackBarZ->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarZ->Value = 50;
            this->trackBarZ->Scroll += gcnew System::EventHandler(this, &Form1::trackBarZ_Scroll);
            // 
            // textBoxZ
            // 
            this->textBoxZ->Location = System::Drawing::Point(6, 19);
            this->textBoxZ->Name = L"textBoxZ";
            this->textBoxZ->Size = System::Drawing::Size(43, 20);
            this->textBoxZ->TabIndex = 1;
            this->textBoxZ->Text = L"50";
            this->textBoxZ->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxZ_TextChanged);
            // 
            // groupBoxRy
            // 
            this->groupBoxRy->Controls->Add(this->trackBarRy);
            this->groupBoxRy->Controls->Add(this->textBoxRy);
            this->groupBoxRy->Location = System::Drawing::Point(266, 16);
            this->groupBoxRy->Name = L"groupBoxRy";
            this->groupBoxRy->Size = System::Drawing::Size(60, 310);
            this->groupBoxRy->TabIndex = 2;
            this->groupBoxRy->TabStop = false;
            this->groupBoxRy->Text = L"Axis Ry";
            this->groupBoxRy->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarRy
            // 
            this->trackBarRy->LargeChange = 1;
            this->trackBarRy->Location = System::Drawing::Point(6, 45);
            this->trackBarRy->Maximum = 100;
            this->trackBarRy->Name = L"trackBarRy";
            this->trackBarRy->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarRy->Size = System::Drawing::Size(45, 260);
            this->trackBarRy->TabIndex = 2;
            this->trackBarRy->TickFrequency = 10;
            this->trackBarRy->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarRy->Scroll += gcnew System::EventHandler(this, &Form1::trackBarRy_Scroll);
            // 
            // textBoxRy
            // 
            this->textBoxRy->Location = System::Drawing::Point(6, 19);
            this->textBoxRy->Name = L"textBoxRy";
            this->textBoxRy->Size = System::Drawing::Size(43, 20);
            this->textBoxRy->TabIndex = 1;
            this->textBoxRy->Text = L"0";
            this->textBoxRy->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxRy_TextChanged);
            // 
            // groupBoxRz
            // 
            this->groupBoxRz->Controls->Add(this->trackBarRz);
            this->groupBoxRz->Controls->Add(this->textBoxRz);
            this->groupBoxRz->Location = System::Drawing::Point(332, 16);
            this->groupBoxRz->Name = L"groupBoxRz";
            this->groupBoxRz->Size = System::Drawing::Size(60, 310);
            this->groupBoxRz->TabIndex = 2;
            this->groupBoxRz->TabStop = false;
            this->groupBoxRz->Text = L"Axis Rz";
            this->groupBoxRz->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarRz
            // 
            this->trackBarRz->LargeChange = 1;
            this->trackBarRz->Location = System::Drawing::Point(6, 45);
            this->trackBarRz->Maximum = 100;
            this->trackBarRz->Name = L"trackBarRz";
            this->trackBarRz->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarRz->Size = System::Drawing::Size(45, 260);
            this->trackBarRz->TabIndex = 2;
            this->trackBarRz->TickFrequency = 10;
            this->trackBarRz->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarRz->Scroll += gcnew System::EventHandler(this, &Form1::trackBarRz_Scroll);
            // 
            // textBoxRz
            // 
            this->textBoxRz->Location = System::Drawing::Point(6, 19);
            this->textBoxRz->Name = L"textBoxRz";
            this->textBoxRz->Size = System::Drawing::Size(43, 20);
            this->textBoxRz->TabIndex = 1;
            this->textBoxRz->Text = L"0";
            this->textBoxRz->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxRz_TextChanged);
            // 
            // groupBoxSL0
            // 
            this->groupBoxSL0->Controls->Add(this->trackBarSL0);
            this->groupBoxSL0->Controls->Add(this->textBoxSL0);
            this->groupBoxSL0->Location = System::Drawing::Point(398, 16);
            this->groupBoxSL0->Name = L"groupBoxSL0";
            this->groupBoxSL0->Size = System::Drawing::Size(60, 310);
            this->groupBoxSL0->TabIndex = 2;
            this->groupBoxSL0->TabStop = false;
            this->groupBoxSL0->Text = L"Slider0";
            this->groupBoxSL0->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarSL0
            // 
            this->trackBarSL0->LargeChange = 1;
            this->trackBarSL0->Location = System::Drawing::Point(6, 45);
            this->trackBarSL0->Maximum = 100;
            this->trackBarSL0->Name = L"trackBarSL0";
            this->trackBarSL0->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarSL0->Size = System::Drawing::Size(45, 260);
            this->trackBarSL0->TabIndex = 2;
            this->trackBarSL0->TickFrequency = 10;
            this->trackBarSL0->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarSL0->Scroll += gcnew System::EventHandler(this, &Form1::trackBarSL0_Scroll);
            // 
            // textBoxSL0
            // 
            this->textBoxSL0->Location = System::Drawing::Point(6, 19);
            this->textBoxSL0->Name = L"textBoxSL0";
            this->textBoxSL0->Size = System::Drawing::Size(43, 20);
            this->textBoxSL0->TabIndex = 1;
            this->textBoxSL0->Text = L"0";
            this->textBoxSL0->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxSL0_TextChanged);
            // 
            // groupBoxSL1
            // 
            this->groupBoxSL1->Controls->Add(this->trackBarSL1);
            this->groupBoxSL1->Controls->Add(this->textBoxSL1);
            this->groupBoxSL1->Location = System::Drawing::Point(464, 16);
            this->groupBoxSL1->Name = L"groupBoxSL1";
            this->groupBoxSL1->Size = System::Drawing::Size(60, 310);
            this->groupBoxSL1->TabIndex = 2;
            this->groupBoxSL1->TabStop = false;
            this->groupBoxSL1->Text = L"Slider1";
            this->groupBoxSL1->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
            // 
            // trackBarSL1
            // 
            this->trackBarSL1->LargeChange = 1;
            this->trackBarSL1->Location = System::Drawing::Point(6, 45);
            this->trackBarSL1->Maximum = 100;
            this->trackBarSL1->Name = L"trackBarSL1";
            this->trackBarSL1->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->trackBarSL1->Size = System::Drawing::Size(45, 260);
            this->trackBarSL1->TabIndex = 2;
            this->trackBarSL1->TickFrequency = 10;
            this->trackBarSL1->TickStyle = System::Windows::Forms::TickStyle::Both;
            this->trackBarSL1->Scroll += gcnew System::EventHandler(this, &Form1::trackBarSL1_Scroll);
            // 
            // textBoxSL1
            // 
            this->textBoxSL1->Location = System::Drawing::Point(6, 19);
            this->textBoxSL1->Name = L"textBoxSL1";
            this->textBoxSL1->Size = System::Drawing::Size(43, 20);
            this->textBoxSL1->TabIndex = 1;
            this->textBoxSL1->Text = L"0";
            this->textBoxSL1->TextChanged += gcnew System::EventHandler(this, &Form1::textBoxSL1_TextChanged);
            // 
            // groupBoxAxes
            // 
            this->groupBoxAxes->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
            this->groupBoxAxes->Controls->Add(this->groupBoxSL1);
            this->groupBoxAxes->Controls->Add(this->groupBoxSL0);
            this->groupBoxAxes->Controls->Add(this->groupBoxRz);
            this->groupBoxAxes->Controls->Add(this->groupBoxRy);
            this->groupBoxAxes->Controls->Add(this->groupBoxZ);
            this->groupBoxAxes->Controls->Add(this->groupBoxRx);
            this->groupBoxAxes->Controls->Add(this->groupBoxY);
            this->groupBoxAxes->Controls->Add(this->groupBoxX);
            this->groupBoxAxes->Location = System::Drawing::Point(10, 60);
            this->groupBoxAxes->Name = L"groupBoxAxes";
            this->groupBoxAxes->Size = System::Drawing::Size(530, 338);
            this->groupBoxAxes->TabIndex = 0;
            this->groupBoxAxes->TabStop = false;
            this->groupBoxAxes->Text = L"Axes (Values in %)";
            // 
            // groupBoxButtons
            // 
            this->groupBoxButtons->Location = System::Drawing::Point(11, 407);
            this->groupBoxButtons->Name = L"groupBoxButtons";
            this->groupBoxButtons->Size = System::Drawing::Size(388, 217);
            this->groupBoxButtons->TabIndex = 3;
            this->groupBoxButtons->TabStop = false;
            this->groupBoxButtons->Text = L"Buttons";
            this->groupBoxButtons->Enter += gcnew System::EventHandler(this, &Form1::groupBoxButtons_Enter);
            // 
            // groupBoxTarget
            // 
            this->groupBoxTarget->Controls->Add(this->listBoxTarget);
            this->groupBoxTarget->Enabled = false;
            this->groupBoxTarget->Location = System::Drawing::Point(408, 3);
            this->groupBoxTarget->Name = L"groupBoxTarget";
            this->groupBoxTarget->Size = System::Drawing::Size(131, 53);
            this->groupBoxTarget->TabIndex = 2;
            this->groupBoxTarget->TabStop = false;
            this->groupBoxTarget->Text = L"Terget Device";
            // 
            // listBoxTarget
            // 
            this->listBoxTarget->AllowDrop = true;
            this->listBoxTarget->FormattingEnabled = true;
            this->listBoxTarget->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"-- Select --"});
            this->listBoxTarget->Location = System::Drawing::Point(6, 16);
            this->listBoxTarget->Name = L"listBoxTarget";
            this->listBoxTarget->Size = System::Drawing::Size(109, 30);
            this->listBoxTarget->TabIndex = 0;
            this->listBoxTarget->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::TargetChanged);
            // 
            // WatchDog
            // 
            this->WatchDog->Tick += gcnew System::EventHandler(this, &Form1::WatchDog_Tick);
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
            this->ClientSize = System::Drawing::Size(535, 657);
            this->Controls->Add(this->groupBoxTarget);
            this->Controls->Add(this->groupBoxButtons);
            this->Controls->Add(this->groupBoxAxes);
            this->Controls->Add(this->groupBoxKnobPov1);
            this->Controls->Add(this->groupBoxPov1);
            this->Controls->Add(this->textBoxInfo);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
            this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
            this->MaximizeBox = false;
            this->MaximumSize = System::Drawing::Size(555, 700);
            this->MinimumSize = System::Drawing::Size(555, 700);
            this->Name = L"Form1";
            this->Text = L"vJoy Feeder";
            this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
            this->groupBoxPov1->ResumeLayout(false);
            this->groupBoxPov1->PerformLayout();
            this->groupBoxKnobPov1->ResumeLayout(false);
            this->groupBoxKnobPov1->PerformLayout();
            this->groupBoxX->ResumeLayout(false);
            this->groupBoxX->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarX))->EndInit();
            this->groupBoxY->ResumeLayout(false);
            this->groupBoxY->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarY))->EndInit();
            this->groupBoxRx->ResumeLayout(false);
            this->groupBoxRx->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRx))->EndInit();
            this->groupBoxZ->ResumeLayout(false);
            this->groupBoxZ->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarZ))->EndInit();
            this->groupBoxRy->ResumeLayout(false);
            this->groupBoxRy->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRy))->EndInit();
            this->groupBoxRz->ResumeLayout(false);
            this->groupBoxRz->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarRz))->EndInit();
            this->groupBoxSL0->ResumeLayout(false);
            this->groupBoxSL0->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarSL0))->EndInit();
            this->groupBoxSL1->ResumeLayout(false);
            this->groupBoxSL1->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBarSL1))->EndInit();
            this->groupBoxAxes->ResumeLayout(false);
            this->groupBoxTarget->ResumeLayout(false);
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion

        void InitializeComponentManual(void)
        {

            // Initialize vJoy Button section 
            this->CBvJoyButton = (gcnew array<System::Windows::Forms::CheckBox^>(128));
            Drawing::Size cbSize =  Drawing::Size(24, 24);
            Drawing::Point origin = Drawing::Point(6, 20);
            Drawing::Point Location = origin;

            for (int index = 0; index<128; index++)
            {
                // Set location
                Location.X = origin.X+ (index%16 * 23);
                Location.Y = origin.Y+ (index/16 * 23);

                // Create checkbox
                this->CBvJoyButton[index] = (gcnew System::Windows::Forms::CheckBox());
                this->groupBoxButtons->Controls->Add(this->CBvJoyButton[index]);			

                this->CBvJoyButton[index]->Appearance = System::Windows::Forms::Appearance::Button;
                this->CBvJoyButton[index]->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 5.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                    static_cast<System::Byte>(0)));
                this->CBvJoyButton[index]->Location = Location;
                this->CBvJoyButton[index]->Margin = System::Windows::Forms::Padding(0);
                this->CBvJoyButton[index]->Name = L"CBvJoyButton[index]";
                this->CBvJoyButton[index]->Size = cbSize;
                //this->CBvJoyButton[index]->TabIndex = 9;
                this->CBvJoyButton[index]->Text = (index+1).ToString();
                this->CBvJoyButton[index]->UseVisualStyleBackColor = true;
                //this->CBvJoyButton[index]->Tag = index+1;
                this->CBvJoyButton[index]->Checked = false;
                this->CBvJoyButton[index]->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox_CheckedChanged);
            };
        }



    private: System::Void textbox_text_changed(System::Windows::Forms::TrackBar^ tb, System::Windows::Forms::TextBox^  txt){
                 // Axis text box changed handler
                 // Set the value of track-bar acording to the value of the corresponding text box

                 int val;

                 try
                 {
                     val = int::Parse(txt->Text);
                     if (val > tb->Maximum) val = tb->Maximum;
                     if (val < tb->Minimum) val = tb->Minimum;
                 }
                 catch (FormatException^)
                 {
                     val = tb->Value;
                 };
                 txt->Text = val.ToString();
                 tb->Value = val;

             }

    private: System::Void trackBar_Scroll(System::Windows::Forms::TrackBar^ tb, System::Windows::Forms::TextBox^  txt){
             // Axis track bar scroll handler
             // Set the value of text box according to the value of the corresponding track bar
                 txt->Text = tb->Value.ToString();
             }

    private: System::Void groupBox1_Enter(System::Object^  sender, System::EventArgs^  e) {
             }

    //// Individual axes and buttons

    // Axis X
    private: System::Void trackBarX_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarX, this->textBoxX);
             }



    private: System::Void textBoxX_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarX, this->textBoxX);

                 // Inform vJoy axis X
                 SetAxis(ConvAbsolute(this->trackBarX->Value, AxisXmax, AxisXmin), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_X);	
             }

    // Axis Y
    private: System::Void trackBarY_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarY, this->textBoxY);
             }

    private: System::Void textBoxY_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarY, this->textBoxY);
                 // Inform vJoy axis Y
                 SetAxis(ConvAbsolute(this->trackBarY->Value, AxisYmax, AxisYmin), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_Y);	
             }

    // Axis Z
    private: System::Void trackBarZ_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarZ, this->textBoxZ);
             }



    private: System::Void textBoxZ_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarZ, this->textBoxZ);

                 // Inform vJoy axis Z
                 SetAxis(ConvAbsolute(this->trackBarZ->Value, AxisZmax, AxisZmin), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_Z);	
             }

    // Axis Rx
    private: System::Void trackBarRx_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarRx, this->textBoxRx);
             }
    // Axis Ry
    private: System::Void trackBarRy_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarRy, this->textBoxRy);
             }
    // Axis Rz
    private: System::Void trackBarRz_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarRz, this->textBoxRz);
             }

    // Axis Slider0
    private: System::Void trackBarSL0_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarSL0, this->textBoxSL0);
             }

    // Axis Slider1
    private: System::Void trackBarSL1_Scroll(System::Object^  sender, System::EventArgs^  e) {
                 trackBar_Scroll(this->trackBarSL1, this->textBoxSL1);
             }

    private: System::Void textBoxRz_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarRz, this->textBoxRz);
                 SetAxis(ConvAbsolute(this->trackBarRz->Value, AxisRZmax, AxisRZmin), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_RZ);	
             }
    private: System::Void textBoxRx_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarRx, this->textBoxRx);
                 SetAxis(ConvAbsolute(this->trackBarRx->Value, AxisRXmax, AxisRXmin), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_RX);	
             }
    private: System::Void textBoxRy_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarRy, this->textBoxRy);
                 SetAxis(ConvAbsolute(this->trackBarRy->Value, AxisRYmax, AxisRYmin), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_RY);	
             }

    private: System::Void textBoxSL0_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarSL0, this->textBoxSL0);
                 SetAxis(ConvAbsolute(this->trackBarSL0->Value, AxisSL0max, AxisSL0min), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_SL0);	
             }

    private: System::Void textBoxSL1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
                 textbox_text_changed(this->trackBarSL1, this->textBoxSL1);
                 SetAxis(ConvAbsolute(this->trackBarSL1->Value, AxisSL1max, AxisSL1min), System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), HID_USAGE_SL1);	
             }

    // Buttons
    private: System::Void checkBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 
                 String ^ strCB;

                 Control^ cb = (Control^)sender;
                 strCB = cb->Text;
                 int id = Convert::ToInt32(strCB);
                 SetBtn(((CheckBox^)cb)->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), id);
             }

#if 0
    private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                                 SetBtn(checkBox1->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 1);
                             }
    private: System::Void checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox2->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 2);
             }

    private: System::Void checkBox3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox3->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 3);
             }
    private: System::Void checkBox4_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox4->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 4);
             }
    private: System::Void checkBox5_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox5->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 5);
             }
    private: System::Void checkBox6_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox6->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 6);
             }
    private: System::Void checkBox7_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox7->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 7);
             }
    private: System::Void checkBox8_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox8->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 8);
             }

    private: System::Void checkBox9_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox9->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 9);
             }

    private: System::Void checkBox10_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox10->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 10);
             }

    private: System::Void checkBox11_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox11->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 11);
             }
    private: System::Void checkBox12_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox12->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 12);
             }

    private: System::Void checkBox13_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox13->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 13);
             }
    private: System::Void checkBox14_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox14->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 14);
             }
    private: System::Void checkBox15_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox15->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 15);
             }
    private: System::Void checkBox16_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox16->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 16);
             }
    private: System::Void checkBox17_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox17->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 17);
             }
    private: System::Void checkBox18_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox18->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 18);
             }
    private: System::Void checkBox19_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox19->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 19);
             }

    private: System::Void checkBox20_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox20->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 20);
             }

    private: System::Void checkBox21_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox21->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 21);
             }
    private: System::Void checkBox22_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox22->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 22);
             }

    private: System::Void checkBox23_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox23->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 23);
             }
    private: System::Void checkBox24_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox24->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 24);
             }
    private: System::Void checkBox25_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox25->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 25);
             }
    private: System::Void checkBox26_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox16->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 26);
             }
    private: System::Void checkBox27_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox27->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 27);
             }
    private: System::Void checkBox28_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox28->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 28);
             }
    private: System::Void checkBox29_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox29->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 29);
             }
    private: System::Void checkBox30_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox30->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 30);
             }

    private: System::Void checkBox31_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox31->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 31);
             }
    private: System::Void checkBox32_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetBtn(checkBox32->Checked, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), 32);
             }

#endif // 0


             //-----------------------------------------------------------------------------------
    private: System::Void radioButtonPov1X_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetDiscPov(-1, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), System::Convert::ToInt32(listBoxNpovDisc->Text));
             }
    private: System::Void radioButtonPov1N_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetDiscPov(0x00, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), System::Convert::ToInt32(listBoxNpovDisc->Text));
             }
    private: System::Void radioButtonPov1E_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetDiscPov(0x01, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), System::Convert::ToInt32(listBoxNpovDisc->Text));
             }
    private: System::Void radioButtonPov1S_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetDiscPov(0x02, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), System::Convert::ToInt32(listBoxNpovDisc->Text));
             }
    private: System::Void radioButtonPov1W_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
                 SetDiscPov(0x03, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), System::Convert::ToInt32(listBoxNpovDisc->Text));
             }


private: int GetCurrentReportId(void){
             // Get the selected value from the target's listbox
             if (listBoxTarget->SelectedIndex <0)
                 ReportId =  -1;
             else
             {
                 String ^ str = listBoxTarget->SelectedItem->ToString();
                 if (!System::String::IsNullOrEmpty(str))
                     ReportId = System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString());
             }
             return ReportId;
         }

private: System::Boolean GetExistingDevices(void) {
    // Populate the list of target devices with the Report IDs
    // Of all existing devices
    VjdStat s = VJD_STAT_UNKN;
    Boolean exist = false;
    int first_free = -1;
    listBoxTarget->Items->Clear();

    for (int i = 1; i <= 16; i++)
    {
//		s = GetVJDStatus(i);
//		if (s == VJD_STAT_FREE || s == VJD_STAT_OWN || s == VJD_STAT_BUSY)
        if (isVJDExists(i))
        {
            exist = true;
            listBoxTarget->Items->Add(i);
            if (GetVJDStatus(i) == VJD_STAT_FREE && first_free < 0)
                first_free = listBoxTarget->Items->Count - 1;
        }
    }
    if (exist)
        listBoxTarget->SelectedIndex = first_free;

    return exist;
}

private: System::Void Enable(bool driverOK) {
/*
    Enable/Disable controls
    Put device attribute data in the text box
*/

             TCHAR Msg[MSG_SIZE];
             System::String ^ str;


             radioButtonPov1X->Enabled = driverOK;
             radioButtonPov1N->Enabled = driverOK;
             radioButtonPov1S->Enabled = driverOK;
             radioButtonPov1E->Enabled = driverOK;
             radioButtonPov1W->Enabled = driverOK;

             groupBoxAxes->Enabled = driverOK;
             groupBoxButtons->Enabled = driverOK;
             // groupBoxTarget->Enabled = driverOK;


             if (driverOK)
             {
                 // Get vJoy device driver Version
                 getDeviceAttrib(Msg);

                 // Put data in text box
                 str = gcnew System::String(Msg);
                 textBoxInfo->Text = str;
                 textBoxInfo->Select(1000, 0);
                 delete str;
             };

         }

private: System::Void controlKnob_Click(System::Object^  sender, System::EventArgs ^ e) {
         }

private: System::Void ContPov1ChckChanged(System::Object^  sender, System::EventArgs^  e) {

             int val;
             if (this->checkBoxContPov1->Checked)
             {
                 this->lbKnob1->Enabled = true;
                 this->LableKnobPov1->Visible = true;
                this->lbKnob1->IndicatorColor = origIndicatorColor;
                val = System::Convert::ToInt32(LableKnobPov1->Text);
             }
             else
             {
                 this->lbKnob1->Enabled = false;
                 this->LableKnobPov1->Visible = false;
                 origIndicatorColor = this->lbKnob1->IndicatorColor;
                 this->lbKnob1->IndicatorColor = System::Drawing::Color::Transparent;
                 val = -1;
             }

            
            int index = System::Convert::ToInt32(listBoxNpov->Text);
            SetContPov(val, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), index);


         }
private: System::Void lbKnob1_KnobChangeValue(System::Object^  sender, LBSoft::IndustrialCtrls::Knobs::LBKnobEventArgs^  e) {
             
            UINT val = (UINT)this->lbKnob1->Value;
            String^ strVal= System::Convert::ToString((int)val);
            this->LableKnobPov1->Text = strVal;
            //int val = System::Convert::ToInt32(LableKnobPov1->Text);
            int index = System::Convert::ToInt32(listBoxNpov->Text)-1;
            SetContPov(val, System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString()), index+1);
         }

private: System::Void TargetChanged(System::Object^  sender, System::EventArgs^  e) 
         {
             // Handler that is called when the user changes the target device
             // 1. Closes the current device handler
             // 2. Opens the new device
             // 3. Changes the controls to suit the new device

             static int sindex = -1;
             int index = sindex;

             if (ReportId>0)
                 RelinquishVJD(ReportId); // if (CloseJoystickDevice())

             if (AcquireVJD(GetCurrentReportId()))
             {
                 index = listBoxTarget->SelectedIndex;
                 ValidControls();
                 js_update();
             }
             else /*if (index >0)*/
             {
                 //listBoxTarget->SelectedIndex = index;
                 String^ message;
                 if (ReportId>0)
                    message = L"vJoy Device " + ReportId + " cannot be accessed\r\nPerhaps some other device has already opened a handle to it\r\n";
                 else
                    message = L"vJoy Device cannot be accessed\r\nPerhaps some other device has already opened a handle to it\r\n";
                 String^ title = L"vJoyFeeder";
                 textBoxInfo->Text = message;
                 MessageBox::Show(this, textBoxInfo->Text, title, MessageBoxButtons::OK, MessageBoxIcon::Warning);				
             }		 
         }


private: BOOL getDeviceAttrib(TCHAR * Msg)
// ---------------------------------------------------------------------------------------------------------- \\
// Get the driver attributes (Vendor ID, Product ID, Version Number)
{

    int res = 1;
    if (!Msg)
        return FALSE;

    PWSTR Product = (PWSTR)GetvJoyProductString();
    PWSTR Manufacturer = (PWSTR)GetvJoyManufacturerString();
    PWSTR SerialNumber = (PWSTR)GetvJoySerialNumberString();
    USHORT version = GetvJoyVersion();

    WORD VerDll, VerDrv;
    if (!DriverMatch(&VerDll, &VerDrv))
    {
        _stprintf_s(Msg, MSG_SIZE, _T("Failed\r\nvJoy Driver (version %04x) does not match vJoyInterface DLL (version %04x)"), VerDrv ,VerDll);
        return FALSE;
    }

    if (!version)
    {
        _stprintf_s(Msg, MSG_SIZE, _T("Failed\r\nvJoy is probably disabled or uninstalled"));
        return FALSE;
    }
    else
    {
        if (!Product)
            _stprintf_s(Msg, MSG_SIZE, _T("VendorID:0x%04X ProductID:0x%04X VersionNumber:0x%04X\r\n"), VENDOR_N_ID,  PRODUCT_N_ID, version);
        else
            _stprintf_s(Msg, MSG_SIZE, _T("Product ID: %S, Manufacturer ID: %S, Serial Number: %S\r\nVendorID:0x%04X ProductID:0x%04X VersionNumber:0x%04X"),Product, Manufacturer, SerialNumber, VENDOR_N_ID,  PRODUCT_N_ID, version);

        return TRUE;
    };

}

private: long ConvPercentage(long val, long max, long min)
{
    // Covert absolute value to a number in the range 0-100
    if (max==min)
        return 0;
    return 100*(val-min)/(max-min);
}

private: long ConvAbsolute(long val, long max, long min)
{
    // Covert to absolute value from a number in the range 0-100
    if (max==min)
        return 0;
#ifdef XBOX
	max = 32767;
	min = 0/*-32768*/;
	return val*(max - min) / 100 + min;
#endif
	return val*(max - min) / 100 + min + 1;
}

private: System::Void WatchDog_Tick(System::Object^  sender, System::EventArgs^  e) {		

             VjdStat stat;
             /* 
             If device was acquired then test if it is still enabled 
             If it is then we're done, if it is DISabled then disable the GUI and await re-enabling
             */
             if (ReportId>0)
             {
                 stat  = GetVJDStatus(ReportId);
                 if (stat == VJD_STAT_OWN)
                     return;
                 else
                 {
                     ReportId = -1;
                     Enable(false);
                     return;
                 };
             }
             /* 
             If device is not acquired then test if it could be enabled 
             If it is then re-enable GUI. If it is not, do nothing 
             */
             else if (vJoyEnabled())
             {
                     GetExistingDevices();
                     GetCurrentReportId();
                     if (ReportId>0)
                        Enable(true);
                     return;
             };

         }

         private: System::Void groupBoxButtons_Enter(System::Object^  sender, System::EventArgs^  e) {
                  }
private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
         }
};
}

