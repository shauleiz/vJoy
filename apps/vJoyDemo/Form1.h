
#pragma once
#define MAX_AXIS	33000

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
	using namespace LBSoft::IndustrialCtrls::Utils;

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

			// Now make all the arrangements to start working:
			// 1 Test that at least one vJoy device is ready
			// 2 Create a list of available vJoy devices
			// 3 Select the first vJoy Device and acquire it
			// 4 Update GUI according to the featurs of the selected device
			// 5 Send the first packet of data
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
			ValidControls();

			/*5*/
			js_update();

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
	private: System::Int32 max_axis;

    // Buttons
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::CheckBox^  checkBox2;
	private: System::Windows::Forms::CheckBox^  checkBox4;

	private: System::Windows::Forms::CheckBox^  checkBox3;
	private: System::Windows::Forms::CheckBox^  checkBox8;
	private: System::Windows::Forms::CheckBox^  checkBox7;
	private: System::Windows::Forms::CheckBox^  checkBox6;
	private: System::Windows::Forms::CheckBox^  checkBox5;
	private: System::Windows::Forms::CheckBox^  checkBox9;
	private: System::Windows::Forms::CheckBox^  checkBox10;
	private: System::Windows::Forms::CheckBox^  checkBox11;
	private: System::Windows::Forms::CheckBox^  checkBox12;
	private: System::Windows::Forms::CheckBox^  checkBox13;
	private: System::Windows::Forms::CheckBox^  checkBox14;
	private: System::Windows::Forms::CheckBox^  checkBox15;
	private: System::Windows::Forms::CheckBox^  checkBox16;

	private: System::Windows::Forms::RadioButton^  radioButtonPov1N;
	private: System::Windows::Forms::RadioButton^  radioButtonPov1E;
	private: System::Windows::Forms::RadioButton^  radioButtonPov1W;
	private: System::Windows::Forms::RadioButton^  radioButtonPov1S;
	private: System::Windows::Forms::RadioButton^  radioButtonPov1X;
	private: System::Windows::Forms::GroupBox^  groupBoxPov1;

	private: System::Windows::Forms::CheckBox^  checkBox17;
	private: System::Windows::Forms::CheckBox^  checkBox18;
	private: System::Windows::Forms::CheckBox^  checkBox19;
	private: System::Windows::Forms::CheckBox^  checkBox20;
	private: System::Windows::Forms::CheckBox^  checkBox21;
	private: System::Windows::Forms::CheckBox^  checkBox22;
	private: System::Windows::Forms::CheckBox^  checkBox23;
	private: System::Windows::Forms::CheckBox^  checkBox24;
	private: System::Windows::Forms::CheckBox^  checkBox25;
	private: System::Windows::Forms::CheckBox^  checkBox26;
	private: System::Windows::Forms::CheckBox^  checkBox27;
	private: System::Windows::Forms::CheckBox^  checkBox28;
	private: System::Windows::Forms::CheckBox^  checkBox29;
	private: System::Windows::Forms::CheckBox^  checkBox30;
	private: System::Windows::Forms::CheckBox^  checkBox31;
	private: System::Windows::Forms::CheckBox^  checkBox32;

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

private:				 
bool POVDiscExists;
bool POVContExists;
private: System::Windows::Forms::Label^  labelPOVcont;
private: System::Windows::Forms::Label^  labelPOVdisc;

private: System::Windows::Forms::ListBox^  listBoxNpov;
private: System::Windows::Forms::ListBox^  listBoxNpovDisc;



	private: System::Void ValidControls(void) {
	/*	
		Enable/Disable controls according to the capabilities of the selected device
		First, the function gets the currently selected Report ID
		Then it enables/disables Axes, Buttons and POV accordingly
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

				 LONG Max,Min;
				 if (GetVJDAxisMax(ReportId, HID_USAGE_X, &Max) && GetVJDAxisMin(ReportId, HID_USAGE_X, &Min))
				 {
					trackBarX->Value = (Max+Min)/2;
					textBoxX->Text = System::Convert::ToString((int)trackBarX->Value);
				 };
				 if (GetVJDAxisMax(ReportId, HID_USAGE_Y, &Max) && GetVJDAxisMin(ReportId, HID_USAGE_Y, &Min))
				 {
					trackBarY->Value = (Max+Min)/2;
					textBoxY->Text = System::Convert::ToString((int)trackBarY->Value);
				 };
				 if (GetVJDAxisMax(ReportId, HID_USAGE_Z, &Max) && GetVJDAxisMin(ReportId, HID_USAGE_Z, &Min))
				 {
					trackBarZ->Value = (Max+Min)/2;
					textBoxZ->Text = System::Convert::ToString((int)trackBarZ->Value);
				 };

				 
				 //Buttons
				 int nButtons = GetVJDButtonNumber(ReportId);
				 checkBox1->Enabled = (nButtons>=1);
				 checkBox2->Enabled = (nButtons>=2);
				 checkBox3->Enabled = (nButtons>=3);
				 checkBox4->Enabled = (nButtons>=4);
				 checkBox5->Enabled = (nButtons>=5);
				 checkBox6->Enabled = (nButtons>=6);
				 checkBox7->Enabled = (nButtons>=7);
				 checkBox8->Enabled = (nButtons>=8);
				 checkBox9->Enabled = (nButtons>=9);
				 checkBox10->Enabled = (nButtons>=10);
				 checkBox11->Enabled = (nButtons>=11);
				 checkBox12->Enabled = (nButtons>=12);
				 checkBox13->Enabled = (nButtons>=13);
				 checkBox14->Enabled = (nButtons>=14);
				 checkBox15->Enabled = (nButtons>=15);
				 checkBox16->Enabled = (nButtons>=16);
				 checkBox17->Enabled = (nButtons>=17);
				 checkBox18->Enabled = (nButtons>=18);
				 checkBox19->Enabled = (nButtons>=19);
				 checkBox20->Enabled = (nButtons>=20);
				 checkBox21->Enabled = (nButtons>=21);
				 checkBox22->Enabled = (nButtons>=22);
				 checkBox23->Enabled = (nButtons>=23);
				 checkBox24->Enabled = (nButtons>=24);
				 checkBox25->Enabled = (nButtons>=25);
				 checkBox26->Enabled = (nButtons>=26);
				 checkBox27->Enabled = (nButtons>=27);
				 checkBox28->Enabled = (nButtons>=28);
				 checkBox29->Enabled = (nButtons>=29);
				 checkBox30->Enabled = (nButtons>=30);
				 checkBox31->Enabled = (nButtons>=31);
				 checkBox32->Enabled = (nButtons>=32);

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
				 };

				 n=GetVJDDiscPovNumber(ReportId);
				 if (n>0 && n<5)
				 {
					 for (int i=1; i<=n; i++)
						 listBoxNpovDisc->Items->Add(i);
					 listBoxNpovDisc->SelectedIndex = 0;
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
				 JOYSTICK_POSITION	Report;
				 static UINT pov[4]={-1,-1,-1,-1};


				 // If no target selected then NO-OP
				 if (listBoxTarget->SelectedIndex == -1)
					 return;

				 // Report Id
				 Report.bDevice = System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString());

				 // Axes
				 Report.wAxisX = (USHORT)this->trackBarX->Value;
				 Report.wAxisY = (USHORT)this->trackBarY->Value;
				 Report.wAxisZ = (USHORT)this->trackBarZ->Value;
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

				 // Discrete POV
				 if (POVDiscExists && listBoxNpovDisc->SelectedIndex!=-1)
				 {
					 int index = System::Convert::ToInt32(listBoxNpovDisc->Text)-1;
					 int val;
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
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox4 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox3 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox8 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox7 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox6 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox5 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox9 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox10 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox11 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox12 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox13 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox14 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox15 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox16 = (gcnew System::Windows::Forms::CheckBox());
			this->radioButtonPov1N = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonPov1E = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonPov1W = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonPov1S = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonPov1X = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxPov1 = (gcnew System::Windows::Forms::GroupBox());
			this->listBoxNpovDisc = (gcnew System::Windows::Forms::ListBox());
			this->checkBox17 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox18 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox19 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox20 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox21 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox22 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox23 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox24 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox25 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox26 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox27 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox28 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox29 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox30 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox31 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox32 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxKnobPov1 = (gcnew System::Windows::Forms::GroupBox());
			this->labelPOVcont = (gcnew System::Windows::Forms::Label());
			this->labelPOVdisc = (gcnew System::Windows::Forms::Label());
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
			this->groupBoxButtons->SuspendLayout();
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
			this->textBoxInfo->TabIndex = 0;
			// 
			// checkBox1
			// 
			this->checkBox1->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox1->AutoSize = true;
			this->checkBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox1->Location = System::Drawing::Point(6, 24);
			this->checkBox1->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(30, 30);
			this->checkBox1->TabIndex = 3;
			this->checkBox1->Text = L"1";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox2
			// 
			this->checkBox2->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox2->AutoSize = true;
			this->checkBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox2->Location = System::Drawing::Point(54, 24);
			this->checkBox2->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(30, 30);
			this->checkBox2->TabIndex = 4;
			this->checkBox2->Text = L"2";
			this->checkBox2->UseVisualStyleBackColor = true;
			this->checkBox2->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox2_CheckedChanged);
			// 
			// checkBox4
			// 
			this->checkBox4->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox4->AutoSize = true;
			this->checkBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox4->Location = System::Drawing::Point(150, 24);
			this->checkBox4->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox4->Name = L"checkBox4";
			this->checkBox4->Size = System::Drawing::Size(30, 30);
			this->checkBox4->TabIndex = 6;
			this->checkBox4->Text = L"4";
			this->checkBox4->UseVisualStyleBackColor = true;
			this->checkBox4->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox4_CheckedChanged);
			// 
			// checkBox3
			// 
			this->checkBox3->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox3->AutoSize = true;
			this->checkBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox3->Location = System::Drawing::Point(102, 24);
			this->checkBox3->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox3->Name = L"checkBox3";
			this->checkBox3->Size = System::Drawing::Size(30, 30);
			this->checkBox3->TabIndex = 5;
			this->checkBox3->Text = L"3";
			this->checkBox3->UseVisualStyleBackColor = true;
			this->checkBox3->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox3_CheckedChanged);
			// 
			// checkBox8
			// 
			this->checkBox8->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox8->AutoSize = true;
			this->checkBox8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox8->Location = System::Drawing::Point(342, 24);
			this->checkBox8->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox8->Name = L"checkBox8";
			this->checkBox8->Size = System::Drawing::Size(30, 30);
			this->checkBox8->TabIndex = 10;
			this->checkBox8->Text = L"8";
			this->checkBox8->UseVisualStyleBackColor = true;
			this->checkBox8->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox8_CheckedChanged);
			// 
			// checkBox7
			// 
			this->checkBox7->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox7->AutoSize = true;
			this->checkBox7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox7->Location = System::Drawing::Point(294, 24);
			this->checkBox7->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox7->Name = L"checkBox7";
			this->checkBox7->Size = System::Drawing::Size(30, 30);
			this->checkBox7->TabIndex = 9;
			this->checkBox7->Text = L"7";
			this->checkBox7->UseVisualStyleBackColor = true;
			this->checkBox7->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox7_CheckedChanged);
			// 
			// checkBox6
			// 
			this->checkBox6->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox6->AutoSize = true;
			this->checkBox6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox6->Location = System::Drawing::Point(246, 24);
			this->checkBox6->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox6->Name = L"checkBox6";
			this->checkBox6->Size = System::Drawing::Size(30, 30);
			this->checkBox6->TabIndex = 8;
			this->checkBox6->Text = L"6";
			this->checkBox6->UseVisualStyleBackColor = true;
			this->checkBox6->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox6_CheckedChanged);
			// 
			// checkBox5
			// 
			this->checkBox5->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox5->AutoSize = true;
			this->checkBox5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox5->Location = System::Drawing::Point(198, 24);
			this->checkBox5->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox5->Name = L"checkBox5";
			this->checkBox5->Size = System::Drawing::Size(30, 30);
			this->checkBox5->TabIndex = 7;
			this->checkBox5->Text = L"5";
			this->checkBox5->UseVisualStyleBackColor = true;
			this->checkBox5->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox5_CheckedChanged);
			// 
			// checkBox9
			// 
			this->checkBox9->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox9->AutoSize = true;
			this->checkBox9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox9->Location = System::Drawing::Point(6, 60);
			this->checkBox9->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox9->Name = L"checkBox9";
			this->checkBox9->Size = System::Drawing::Size(30, 30);
			this->checkBox9->TabIndex = 3;
			this->checkBox9->Text = L"9";
			this->checkBox9->UseVisualStyleBackColor = true;
			this->checkBox9->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox10
			// 
			this->checkBox10->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox10->AutoSize = true;
			this->checkBox10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox10->Location = System::Drawing::Point(54, 60);
			this->checkBox10->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox10->Name = L"checkBox10";
			this->checkBox10->Size = System::Drawing::Size(30, 30);
			this->checkBox10->TabIndex = 4;
			this->checkBox10->Text = L"10";
			this->checkBox10->UseVisualStyleBackColor = true;
			this->checkBox10->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox2_CheckedChanged);
			// 
			// checkBox11
			// 
			this->checkBox11->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox11->AutoSize = true;
			this->checkBox11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox11->Location = System::Drawing::Point(102, 60);
			this->checkBox11->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox11->Name = L"checkBox11";
			this->checkBox11->Size = System::Drawing::Size(30, 30);
			this->checkBox11->TabIndex = 5;
			this->checkBox11->Text = L"11";
			this->checkBox11->UseVisualStyleBackColor = true;
			this->checkBox11->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox3_CheckedChanged);
			// 
			// checkBox12
			// 
			this->checkBox12->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox12->AutoSize = true;
			this->checkBox12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox12->Location = System::Drawing::Point(150, 60);
			this->checkBox12->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox12->Name = L"checkBox12";
			this->checkBox12->Size = System::Drawing::Size(30, 30);
			this->checkBox12->TabIndex = 6;
			this->checkBox12->Text = L"12";
			this->checkBox12->UseVisualStyleBackColor = true;
			this->checkBox12->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox4_CheckedChanged);
			// 
			// checkBox13
			// 
			this->checkBox13->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox13->AutoSize = true;
			this->checkBox13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox13->Location = System::Drawing::Point(198, 60);
			this->checkBox13->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox13->Name = L"checkBox13";
			this->checkBox13->Size = System::Drawing::Size(30, 30);
			this->checkBox13->TabIndex = 7;
			this->checkBox13->Text = L"13";
			this->checkBox13->UseVisualStyleBackColor = true;
			this->checkBox13->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox5_CheckedChanged);
			// 
			// checkBox14
			// 
			this->checkBox14->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox14->AutoSize = true;
			this->checkBox14->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox14->Location = System::Drawing::Point(246, 60);
			this->checkBox14->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox14->Name = L"checkBox14";
			this->checkBox14->Size = System::Drawing::Size(30, 30);
			this->checkBox14->TabIndex = 8;
			this->checkBox14->Text = L"14";
			this->checkBox14->UseVisualStyleBackColor = true;
			this->checkBox14->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox6_CheckedChanged);
			// 
			// checkBox15
			// 
			this->checkBox15->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox15->AutoSize = true;
			this->checkBox15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox15->Location = System::Drawing::Point(294, 60);
			this->checkBox15->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox15->Name = L"checkBox15";
			this->checkBox15->Size = System::Drawing::Size(30, 30);
			this->checkBox15->TabIndex = 9;
			this->checkBox15->Text = L"15";
			this->checkBox15->UseVisualStyleBackColor = true;
			this->checkBox15->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox7_CheckedChanged);
			// 
			// checkBox16
			// 
			this->checkBox16->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox16->AutoSize = true;
			this->checkBox16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox16->Location = System::Drawing::Point(342, 60);
			this->checkBox16->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox16->Name = L"checkBox16";
			this->checkBox16->Size = System::Drawing::Size(30, 30);
			this->checkBox16->TabIndex = 10;
			this->checkBox16->Text = L"16";
			this->checkBox16->UseVisualStyleBackColor = true;
			this->checkBox16->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox8_CheckedChanged);
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
			this->groupBoxPov1->TabIndex = 16;
			this->groupBoxPov1->TabStop = false;
			this->groupBoxPov1->Text = L"Discrete POV1";
			// 
			// listBoxNpovDisc
			// 
			this->listBoxNpovDisc->FormattingEnabled = true;
			this->listBoxNpovDisc->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"1", L"2", L"3", L"4"});
			this->listBoxNpovDisc->Location = System::Drawing::Point(57, 18);
			this->listBoxNpovDisc->Name = L"listBoxNpovDisc";
			this->listBoxNpovDisc->Size = System::Drawing::Size(50, 30);
			this->listBoxNpovDisc->TabIndex = 2;
			// 
			// checkBox17
			// 
			this->checkBox17->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox17->AutoSize = true;
			this->checkBox17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox17->Location = System::Drawing::Point(6, 96);
			this->checkBox17->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox17->Name = L"checkBox17";
			this->checkBox17->Size = System::Drawing::Size(30, 30);
			this->checkBox17->TabIndex = 3;
			this->checkBox17->Text = L"17";
			this->checkBox17->UseVisualStyleBackColor = true;
			this->checkBox17->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox18
			// 
			this->checkBox18->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox18->AutoSize = true;
			this->checkBox18->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox18->Location = System::Drawing::Point(54, 96);
			this->checkBox18->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox18->Name = L"checkBox18";
			this->checkBox18->Size = System::Drawing::Size(30, 30);
			this->checkBox18->TabIndex = 4;
			this->checkBox18->Text = L"18";
			this->checkBox18->UseVisualStyleBackColor = true;
			this->checkBox18->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox19
			// 
			this->checkBox19->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox19->AutoSize = true;
			this->checkBox19->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox19->Location = System::Drawing::Point(102, 96);
			this->checkBox19->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox19->Name = L"checkBox19";
			this->checkBox19->Size = System::Drawing::Size(30, 30);
			this->checkBox19->TabIndex = 3;
			this->checkBox19->Text = L"19";
			this->checkBox19->UseVisualStyleBackColor = true;
			this->checkBox19->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox20
			// 
			this->checkBox20->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox20->AutoSize = true;
			this->checkBox20->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox20->Location = System::Drawing::Point(150, 96);
			this->checkBox20->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox20->Name = L"checkBox20";
			this->checkBox20->Size = System::Drawing::Size(30, 30);
			this->checkBox20->TabIndex = 5;
			this->checkBox20->Text = L"20";
			this->checkBox20->UseVisualStyleBackColor = true;
			this->checkBox20->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox21
			// 
			this->checkBox21->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox21->AutoSize = true;
			this->checkBox21->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox21->Location = System::Drawing::Point(198, 96);
			this->checkBox21->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox21->Name = L"checkBox21";
			this->checkBox21->Size = System::Drawing::Size(30, 30);
			this->checkBox21->TabIndex = 4;
			this->checkBox21->Text = L"21";
			this->checkBox21->UseVisualStyleBackColor = true;
			this->checkBox21->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox22
			// 
			this->checkBox22->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox22->AutoSize = true;
			this->checkBox22->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox22->Location = System::Drawing::Point(246, 96);
			this->checkBox22->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox22->Name = L"checkBox22";
			this->checkBox22->Size = System::Drawing::Size(30, 30);
			this->checkBox22->TabIndex = 6;
			this->checkBox22->Text = L"22";
			this->checkBox22->UseVisualStyleBackColor = true;
			this->checkBox22->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox23
			// 
			this->checkBox23->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox23->AutoSize = true;
			this->checkBox23->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox23->Location = System::Drawing::Point(294, 96);
			this->checkBox23->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox23->Name = L"checkBox23";
			this->checkBox23->Size = System::Drawing::Size(30, 30);
			this->checkBox23->TabIndex = 5;
			this->checkBox23->Text = L"23";
			this->checkBox23->UseVisualStyleBackColor = true;
			this->checkBox23->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox24
			// 
			this->checkBox24->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox24->AutoSize = true;
			this->checkBox24->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox24->Location = System::Drawing::Point(342, 96);
			this->checkBox24->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox24->Name = L"checkBox24";
			this->checkBox24->Size = System::Drawing::Size(30, 30);
			this->checkBox24->TabIndex = 6;
			this->checkBox24->Text = L"24";
			this->checkBox24->UseVisualStyleBackColor = true;
			this->checkBox24->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox25
			// 
			this->checkBox25->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox25->AutoSize = true;
			this->checkBox25->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox25->Location = System::Drawing::Point(6, 132);
			this->checkBox25->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox25->Name = L"checkBox25";
			this->checkBox25->Size = System::Drawing::Size(30, 30);
			this->checkBox25->TabIndex = 7;
			this->checkBox25->Text = L"25";
			this->checkBox25->UseVisualStyleBackColor = true;
			this->checkBox25->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox26
			// 
			this->checkBox26->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox26->AutoSize = true;
			this->checkBox26->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox26->Location = System::Drawing::Point(54, 132);
			this->checkBox26->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox26->Name = L"checkBox26";
			this->checkBox26->Size = System::Drawing::Size(30, 30);
			this->checkBox26->TabIndex = 7;
			this->checkBox26->Text = L"26";
			this->checkBox26->UseVisualStyleBackColor = true;
			this->checkBox26->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox27
			// 
			this->checkBox27->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox27->AutoSize = true;
			this->checkBox27->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox27->Location = System::Drawing::Point(102, 132);
			this->checkBox27->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox27->Name = L"checkBox27";
			this->checkBox27->Size = System::Drawing::Size(30, 30);
			this->checkBox27->TabIndex = 8;
			this->checkBox27->Text = L"27";
			this->checkBox27->UseVisualStyleBackColor = true;
			this->checkBox27->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox28
			// 
			this->checkBox28->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox28->AutoSize = true;
			this->checkBox28->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox28->Location = System::Drawing::Point(150, 132);
			this->checkBox28->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox28->Name = L"checkBox28";
			this->checkBox28->Size = System::Drawing::Size(30, 30);
			this->checkBox28->TabIndex = 8;
			this->checkBox28->Text = L"28";
			this->checkBox28->UseVisualStyleBackColor = true;
			this->checkBox28->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox29
			// 
			this->checkBox29->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox29->AutoSize = true;
			this->checkBox29->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox29->Location = System::Drawing::Point(198, 132);
			this->checkBox29->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox29->Name = L"checkBox29";
			this->checkBox29->Size = System::Drawing::Size(30, 30);
			this->checkBox29->TabIndex = 9;
			this->checkBox29->Text = L"29";
			this->checkBox29->UseVisualStyleBackColor = true;
			this->checkBox29->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox30
			// 
			this->checkBox30->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox30->AutoSize = true;
			this->checkBox30->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox30->Location = System::Drawing::Point(246, 132);
			this->checkBox30->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox30->Name = L"checkBox30";
			this->checkBox30->Size = System::Drawing::Size(30, 30);
			this->checkBox30->TabIndex = 9;
			this->checkBox30->Text = L"30";
			this->checkBox30->UseVisualStyleBackColor = true;
			this->checkBox30->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox31
			// 
			this->checkBox31->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox31->AutoSize = true;
			this->checkBox31->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox31->Location = System::Drawing::Point(294, 132);
			this->checkBox31->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox31->Name = L"checkBox31";
			this->checkBox31->Size = System::Drawing::Size(30, 30);
			this->checkBox31->TabIndex = 10;
			this->checkBox31->Text = L"31";
			this->checkBox31->UseVisualStyleBackColor = true;
			this->checkBox31->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// checkBox32
			// 
			this->checkBox32->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox32->AutoSize = true;
			this->checkBox32->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->checkBox32->Location = System::Drawing::Point(342, 132);
			this->checkBox32->MinimumSize = System::Drawing::Size(30, 30);
			this->checkBox32->Name = L"checkBox32";
			this->checkBox32->Size = System::Drawing::Size(30, 30);
			this->checkBox32->TabIndex = 10;
			this->checkBox32->Text = L"32";
			this->checkBox32->UseVisualStyleBackColor = true;
			this->checkBox32->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// groupBoxKnobPov1
			// 
			this->groupBoxKnobPov1->Controls->Add(this->labelPOVcont);
			this->groupBoxKnobPov1->Controls->Add(this->listBoxNpov);
			this->groupBoxKnobPov1->Controls->Add(this->checkBoxContPov1);
			this->groupBoxKnobPov1->Controls->Add(this->LableKnobPov1);
			this->groupBoxKnobPov1->Controls->Add(this->lbKnob1);
			this->groupBoxKnobPov1->Location = System::Drawing::Point(405, 407);
			this->groupBoxKnobPov1->Name = L"groupBoxKnobPov1";
			this->groupBoxKnobPov1->Size = System::Drawing::Size(117, 173);
			this->groupBoxKnobPov1->TabIndex = 17;
			this->groupBoxKnobPov1->TabStop = false;
			this->groupBoxKnobPov1->Text = L"Continuous POV1";
			// 
			// labelPOVcont
			// 
			this->labelPOVcont->AutoSize = true;
			this->labelPOVcont->Location = System::Drawing::Point(11, 21);
			this->labelPOVcont->Name = L"labelPOVcont";
			this->labelPOVcont->Size = System::Drawing::Size(40, 13);
			this->labelPOVcont->TabIndex = 3;
			this->labelPOVcont->Text = L"Select:";
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
			// listBoxNpov
			// 
			this->listBoxNpov->FormattingEnabled = true;
			this->listBoxNpov->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"1", L"2", L"3", L"4"});
			this->listBoxNpov->Location = System::Drawing::Point(57, 18);
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
			this->LableKnobPov1->Location = System::Drawing::Point(43, 153);
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
			this->groupBoxX->TabIndex = 2;
			this->groupBoxX->TabStop = false;
			this->groupBoxX->Text = L"Axis X";
			this->groupBoxX->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
			// 
			// trackBarX
			// 
			this->trackBarX->LargeChange = 500;
			this->trackBarX->Location = System::Drawing::Point(6, 45);
			this->trackBarX->Maximum = 33000;
			this->trackBarX->Name = L"trackBarX";
			this->trackBarX->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarX->Size = System::Drawing::Size(45, 260);
			this->trackBarX->SmallChange = 100;
			this->trackBarX->TabIndex = 2;
			this->trackBarX->TickFrequency = 1000;
			this->trackBarX->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarX->Value = 16500;
			this->trackBarX->Scroll += gcnew System::EventHandler(this, &Form1::trackBarX_Scroll);
			// 
			// textBoxX
			// 
			this->textBoxX->Location = System::Drawing::Point(6, 19);
			this->textBoxX->Name = L"textBoxX";
			this->textBoxX->Size = System::Drawing::Size(43, 20);
			this->textBoxX->TabIndex = 1;
			this->textBoxX->Text = L"16500";
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
			this->trackBarY->LargeChange = 500;
			this->trackBarY->Location = System::Drawing::Point(6, 45);
			this->trackBarY->Maximum = 33000;
			this->trackBarY->Name = L"trackBarY";
			this->trackBarY->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarY->Size = System::Drawing::Size(45, 260);
			this->trackBarY->SmallChange = 100;
			this->trackBarY->TabIndex = 2;
			this->trackBarY->TickFrequency = 1000;
			this->trackBarY->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarY->Value = 16500;
			this->trackBarY->Scroll += gcnew System::EventHandler(this, &Form1::trackBarY_Scroll);
			// 
			// textBoxY
			// 
			this->textBoxY->Location = System::Drawing::Point(6, 19);
			this->textBoxY->Name = L"textBoxY";
			this->textBoxY->Size = System::Drawing::Size(43, 20);
			this->textBoxY->TabIndex = 1;
			this->textBoxY->Text = L"16500";
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
			this->trackBarRx->LargeChange = 500;
			this->trackBarRx->Location = System::Drawing::Point(6, 45);
			this->trackBarRx->Maximum = 33000;
			this->trackBarRx->Name = L"trackBarRx";
			this->trackBarRx->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarRx->Size = System::Drawing::Size(45, 260);
			this->trackBarRx->SmallChange = 100;
			this->trackBarRx->TabIndex = 2;
			this->trackBarRx->TickFrequency = 1000;
			this->trackBarRx->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarRx->Value = 16500;
			this->trackBarRx->Scroll += gcnew System::EventHandler(this, &Form1::trackBarRx_Scroll);
			// 
			// textBoxRx
			// 
			this->textBoxRx->Location = System::Drawing::Point(6, 19);
			this->textBoxRx->Name = L"textBoxRx";
			this->textBoxRx->Size = System::Drawing::Size(43, 20);
			this->textBoxRx->TabIndex = 1;
			this->textBoxRx->Text = L"16500";
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
			this->trackBarZ->LargeChange = 500;
			this->trackBarZ->Location = System::Drawing::Point(6, 45);
			this->trackBarZ->Maximum = 33000;
			this->trackBarZ->Name = L"trackBarZ";
			this->trackBarZ->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarZ->Size = System::Drawing::Size(45, 260);
			this->trackBarZ->SmallChange = 100;
			this->trackBarZ->TabIndex = 2;
			this->trackBarZ->TickFrequency = 1000;
			this->trackBarZ->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarZ->Value = 16500;
			this->trackBarZ->Scroll += gcnew System::EventHandler(this, &Form1::trackBarZ_Scroll);
			// 
			// textBoxZ
			// 
			this->textBoxZ->Location = System::Drawing::Point(6, 19);
			this->textBoxZ->Name = L"textBoxZ";
			this->textBoxZ->Size = System::Drawing::Size(43, 20);
			this->textBoxZ->TabIndex = 1;
			this->textBoxZ->Text = L"16500";
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
			this->trackBarRy->LargeChange = 500;
			this->trackBarRy->Location = System::Drawing::Point(6, 45);
			this->trackBarRy->Maximum = 33000;
			this->trackBarRy->Name = L"trackBarRy";
			this->trackBarRy->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarRy->Size = System::Drawing::Size(45, 260);
			this->trackBarRy->SmallChange = 100;
			this->trackBarRy->TabIndex = 2;
			this->trackBarRy->TickFrequency = 1000;
			this->trackBarRy->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarRy->Value = 16500;
			this->trackBarRy->Scroll += gcnew System::EventHandler(this, &Form1::trackBarRy_Scroll);
			// 
			// textBoxRy
			// 
			this->textBoxRy->Location = System::Drawing::Point(6, 19);
			this->textBoxRy->Name = L"textBoxRy";
			this->textBoxRy->Size = System::Drawing::Size(43, 20);
			this->textBoxRy->TabIndex = 1;
			this->textBoxRy->Text = L"16500";
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
			this->trackBarRz->LargeChange = 500;
			this->trackBarRz->Location = System::Drawing::Point(6, 45);
			this->trackBarRz->Maximum = 33000;
			this->trackBarRz->Name = L"trackBarRz";
			this->trackBarRz->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarRz->Size = System::Drawing::Size(45, 260);
			this->trackBarRz->SmallChange = 100;
			this->trackBarRz->TabIndex = 2;
			this->trackBarRz->TickFrequency = 1000;
			this->trackBarRz->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarRz->Value = 16500;
			this->trackBarRz->Scroll += gcnew System::EventHandler(this, &Form1::trackBarRz_Scroll);
			// 
			// textBoxRz
			// 
			this->textBoxRz->Location = System::Drawing::Point(6, 19);
			this->textBoxRz->Name = L"textBoxRz";
			this->textBoxRz->Size = System::Drawing::Size(43, 20);
			this->textBoxRz->TabIndex = 1;
			this->textBoxRz->Text = L"16500";
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
			this->trackBarSL0->LargeChange = 500;
			this->trackBarSL0->Location = System::Drawing::Point(6, 45);
			this->trackBarSL0->Maximum = 33000;
			this->trackBarSL0->Name = L"trackBarSL0";
			this->trackBarSL0->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarSL0->Size = System::Drawing::Size(45, 260);
			this->trackBarSL0->SmallChange = 100;
			this->trackBarSL0->TabIndex = 2;
			this->trackBarSL0->TickFrequency = 1000;
			this->trackBarSL0->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarSL0->Value = 16500;
			this->trackBarSL0->Scroll += gcnew System::EventHandler(this, &Form1::trackBarSL0_Scroll);
			// 
			// textBoxSL0
			// 
			this->textBoxSL0->Location = System::Drawing::Point(6, 19);
			this->textBoxSL0->Name = L"textBoxSL0";
			this->textBoxSL0->Size = System::Drawing::Size(43, 20);
			this->textBoxSL0->TabIndex = 1;
			this->textBoxSL0->Text = L"16500";
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
			this->trackBarSL1->LargeChange = 500;
			this->trackBarSL1->Location = System::Drawing::Point(6, 45);
			this->trackBarSL1->Maximum = 33000;
			this->trackBarSL1->Name = L"trackBarSL1";
			this->trackBarSL1->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->trackBarSL1->Size = System::Drawing::Size(45, 260);
			this->trackBarSL1->SmallChange = 100;
			this->trackBarSL1->TabIndex = 2;
			this->trackBarSL1->TickFrequency = 1000;
			this->trackBarSL1->TickStyle = System::Windows::Forms::TickStyle::Both;
			this->trackBarSL1->Value = 16500;
			this->trackBarSL1->Scroll += gcnew System::EventHandler(this, &Form1::trackBarSL1_Scroll);
			// 
			// textBoxSL1
			// 
			this->textBoxSL1->Location = System::Drawing::Point(6, 19);
			this->textBoxSL1->Name = L"textBoxSL1";
			this->textBoxSL1->Size = System::Drawing::Size(43, 20);
			this->textBoxSL1->TabIndex = 1;
			this->textBoxSL1->Text = L"16500";
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
			this->groupBoxAxes->TabIndex = 19;
			this->groupBoxAxes->TabStop = false;
			this->groupBoxAxes->Text = L"Axes";
			// 
			// groupBoxButtons
			// 
			this->groupBoxButtons->Controls->Add(this->checkBox32);
			this->groupBoxButtons->Controls->Add(this->checkBox16);
			this->groupBoxButtons->Controls->Add(this->checkBox31);
			this->groupBoxButtons->Controls->Add(this->checkBox8);
			this->groupBoxButtons->Controls->Add(this->checkBox30);
			this->groupBoxButtons->Controls->Add(this->checkBox15);
			this->groupBoxButtons->Controls->Add(this->checkBox29);
			this->groupBoxButtons->Controls->Add(this->checkBox7);
			this->groupBoxButtons->Controls->Add(this->checkBox28);
			this->groupBoxButtons->Controls->Add(this->checkBox14);
			this->groupBoxButtons->Controls->Add(this->checkBox27);
			this->groupBoxButtons->Controls->Add(this->checkBox6);
			this->groupBoxButtons->Controls->Add(this->checkBox26);
			this->groupBoxButtons->Controls->Add(this->checkBox13);
			this->groupBoxButtons->Controls->Add(this->checkBox25);
			this->groupBoxButtons->Controls->Add(this->checkBox24);
			this->groupBoxButtons->Controls->Add(this->checkBox5);
			this->groupBoxButtons->Controls->Add(this->checkBox23);
			this->groupBoxButtons->Controls->Add(this->checkBox12);
			this->groupBoxButtons->Controls->Add(this->checkBox22);
			this->groupBoxButtons->Controls->Add(this->checkBox11);
			this->groupBoxButtons->Controls->Add(this->checkBox21);
			this->groupBoxButtons->Controls->Add(this->checkBox4);
			this->groupBoxButtons->Controls->Add(this->checkBox20);
			this->groupBoxButtons->Controls->Add(this->checkBox10);
			this->groupBoxButtons->Controls->Add(this->checkBox19);
			this->groupBoxButtons->Controls->Add(this->checkBox3);
			this->groupBoxButtons->Controls->Add(this->checkBox18);
			this->groupBoxButtons->Controls->Add(this->checkBox9);
			this->groupBoxButtons->Controls->Add(this->checkBox17);
			this->groupBoxButtons->Controls->Add(this->checkBox2);
			this->groupBoxButtons->Controls->Add(this->checkBox1);
			this->groupBoxButtons->Location = System::Drawing::Point(11, 407);
			this->groupBoxButtons->Name = L"groupBoxButtons";
			this->groupBoxButtons->Size = System::Drawing::Size(379, 173);
			this->groupBoxButtons->TabIndex = 20;
			this->groupBoxButtons->TabStop = false;
			this->groupBoxButtons->Text = L"Buttons";
			// 
			// groupBoxTarget
			// 
			this->groupBoxTarget->Controls->Add(this->listBoxTarget);
			this->groupBoxTarget->Enabled = false;
			this->groupBoxTarget->Location = System::Drawing::Point(408, 3);
			this->groupBoxTarget->Name = L"groupBoxTarget";
			this->groupBoxTarget->Size = System::Drawing::Size(131, 53);
			this->groupBoxTarget->TabIndex = 21;
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
			this->listBoxTarget->TabIndex = 22;
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
			this->ClientSize = System::Drawing::Size(545, 584);
			this->Controls->Add(this->groupBoxTarget);
			this->Controls->Add(this->groupBoxButtons);
			this->Controls->Add(this->groupBoxAxes);
			this->Controls->Add(this->groupBoxKnobPov1);
			this->Controls->Add(this->groupBoxPov1);
			this->Controls->Add(this->textBoxInfo);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MaximumSize = System::Drawing::Size(555, 616);
			this->MinimumSize = System::Drawing::Size(555, 616);
			this->Name = L"Form1";
			this->Text = L"vJoy Demo";
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
			this->groupBoxButtons->ResumeLayout(false);
			this->groupBoxButtons->PerformLayout();
			this->groupBoxTarget->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


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
				 js_update();

			 }

	// Axis Y
	private: System::Void trackBarY_Scroll(System::Object^  sender, System::EventArgs^  e) {
				 trackBar_Scroll(this->trackBarY, this->textBoxY);
			 }

	private: System::Void textBoxY_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 // Inform vJoy axis Y
				 js_update();
			 }

	// Axis Z
	private: System::Void trackBarZ_Scroll(System::Object^  sender, System::EventArgs^  e) {
				 trackBar_Scroll(this->trackBarZ, this->textBoxZ);
			 }



	private: System::Void textBoxZ_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 textbox_text_changed(this->trackBarZ, this->textBoxZ);

				 // Inform vJoy axis Z
				 js_update();

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
				 js_update();
			 }
	private: System::Void textBoxRx_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 textbox_text_changed(this->trackBarRx, this->textBoxRx);
				 js_update();
			 }
	private: System::Void textBoxRy_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 textbox_text_changed(this->trackBarRy, this->textBoxRy);
				 js_update();
			 }

	private: System::Void textBoxSL0_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 textbox_text_changed(this->trackBarSL0, this->textBoxSL0);
				 js_update();
			 }

	private: System::Void textBoxSL1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 textbox_text_changed(this->trackBarSL1, this->textBoxSL1);
				 js_update();
			 }

	// Buttons
	private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }

	private: System::Void checkBox3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void checkBox4_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void checkBox5_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void checkBox6_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void checkBox7_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void checkBox8_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }

			 //-----------------------------------------------------------------------------------
	private: System::Void radioButtonPov1X_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void radioButtonPov1N_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void radioButtonPov1E_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void radioButtonPov1S_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }
	private: System::Void radioButtonPov1W_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 js_update();
			 }


private: int GetCurrentReportId(void){
			 // Get the selected value from the target's listbox
			 if (listBoxTarget->SelectedIndex <0)
				 return -1;
			 String ^ str = listBoxTarget->SelectedItem->ToString();
			 if (!System::String::IsNullOrEmpty(str))
				ReportId = System::Convert::ToInt32(listBoxTarget->SelectedItem->ToString());
			 return ReportId;
		 }

private: System::Boolean GetExistingDevices(void) {
// Populate the list of target devices with the Report IDs
// Of all existing devices
			 Boolean exist=false;
			 listBoxTarget->Items->Clear();

			 for (int i=1; i<=16; i++)
			 {
				if(GetVJDStatus(i) == VJD_STAT_FREE)
				{
					exist=true;
					listBoxTarget->Items->Add(i);
				}
			 }
			 if (exist)
				 listBoxTarget->SelectedIndex = 0;

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
			 this->groupBoxTarget->Enabled = driverOK;


			 // Get vJoy device driver Version
			 getDeviceAttrib(Msg);

			 // Put data in text box
			 str = gcnew System::String(Msg);
			 textBoxInfo->Text = str;
			 textBoxInfo->Select(1000,0);
			 delete str;

		 }
private: System::Void controlKnob_Click(System::Object^  sender, System::EventArgs ^ e) {
		 }

private: System::Void ContPov1ChckChanged(System::Object^  sender, System::EventArgs^  e) {


			 if (this->checkBoxContPov1->Checked)
			 {
				 this->lbKnob1->Enabled = true;
				 this->LableKnobPov1->Visible = true;
				this->lbKnob1->IndicatorColor = origIndicatorColor;
			 }
			 else
			 {
				 this->lbKnob1->Enabled = false;
				 this->LableKnobPov1->Visible = false;
				 origIndicatorColor = this->lbKnob1->IndicatorColor;
				 this->lbKnob1->IndicatorColor = System::Drawing::Color::Transparent;
			 }
			 js_update();

		 }
private: System::Void lbKnob1_KnobChangeValue(System::Object^  sender, LBSoft::IndustrialCtrls::Knobs::LBKnobEventArgs^  e) {
			 
			 float val = this->lbKnob1->Value;
			 String^ strVal= System::Convert::ToString((int)val);
			 this->LableKnobPov1->Text = strVal;
			 js_update();
		 }
private: System::Void PovTypeChanged(System::Object^  sender, System::EventArgs^  e) {
			 js_update();
		 }

private: System::Void TargetChanged(System::Object^  sender, System::EventArgs^  e) {
		// Handler that is called when the user changes the target device
		// 1. Closes the current device handler
		// 2. Opens the new device
		// 3. Changes the controls to suit the new device
			 RelinquishVJD(ReportId); // if (CloseJoystickDevice())
			 {
				 if (AcquireVJD(GetCurrentReportId()))
					 ValidControls();
				 else
					 textBoxInfo->Text = L"vJoy Device cannot be accessed\r\nPerhaps some other device has alredy opened a handle to it\r\n";
			 };
			 js_update();
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


private: System::Void WatchDog_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {		

			  cli::array< System::String^ > ^FfbEffectName = gcnew cli::array< System::String^ >(13) 
			  {L"NONE", L"Constant Force", L"Ramp",  L"Square",  L"Sine",  L"Triangle",  L"Sawtooth Up",\
			  L"Sawtooth Down", L"Spring",  L"Damper",  L"Inertia",  L"Friction",  L"Custom Force"};

			 static FFBEType FfbEffect=ET_NONE;
			 FFBEType FfbCurrent;

			 FfbCurrent = FfbGetEffect();
			 if (FfbCurrent != FfbEffect)
			 {
				 FfbEffect = FfbCurrent;
				 if (!FfbEffect)
					textBoxInfo->Text = L"FFB Effect Off\r\n";
				 else
					 textBoxInfo->Text = L"FFB Effect: "  + FfbEffectName[FfbEffect] + "\r\n";
			 };
			 delete FfbEffectName;

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
				 Enable(true);
				 GetExistingDevices();
				 return;
			 };

		 }

		 };
}

