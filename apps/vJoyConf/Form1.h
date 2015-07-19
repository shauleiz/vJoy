#pragma once
#include "declarations.h"

#include <public.h>
#include <wrapper.h> // wrapper for vJoyInstall
#include <vJoyInterface.h>
#ifdef _DEBUG
// #include "vld.h" // Memory leak detection
#endif


// General prototypes	
int CreateHidReportDesc(void ** data, UINT nButtons, bool * axes, int nPovHatsCont, int nPovHatsDir, BYTE ReportId);
void WriteHidReportDescToReg(int target, UCHAR * Descriptor, int size);
void DeleteHidReportDescFromReg(int target);


namespace vJoyConf {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Runtime::InteropServices;


	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			Array::Resize(AxisChecked, 32);
			Array::Resize(AxisList, 32);
			array<String^> ^Axes = AxesIDs;
			Axes->CopyTo(AxisList, 0);			
			Array::Resize(ButtonChecked, 128);

			// Tooltips
			//this->toolTip1->SetToolTip(this->radioButtonPovDir, TT_POVDIR);
			//this->toolTip1->SetToolTip(this->tabPageAxes, TT_TABAXES);
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

	protected: 


















































	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::Button^  buttonApply;
	private: System::Windows::Forms::Button^  buttonCancel;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


	private: array<bool> ^AxisChecked;
	private: array<String^> ^AxisList;
	private: array<bool> ^ButtonChecked;
	private: unsigned int nButtons;
	private: int nPovHats;
	private: System::Windows::Forms::ToolTip^  toolTip1;









private: System::Windows::Forms::SaveFileDialog^  saveHeaderFileDialog;
private: System::Windows::Forms::TabPage^  tabPageAxes;
private: System::Windows::Forms::GroupBox^  groupBoxAxV;
private: System::Windows::Forms::CheckBox^  checkBoxVRy;
private: System::Windows::Forms::CheckBox^  checkBoxVZ;
private: System::Windows::Forms::CheckBox^  checkBoxVY;
private: System::Windows::Forms::CheckBox^  checkBoxVS2;
private: System::Windows::Forms::CheckBox^  checkBoxVRx;
private: System::Windows::Forms::CheckBox^  checkBoxVX;
private: System::Windows::Forms::CheckBox^  checkBoxVRz;
private: System::Windows::Forms::CheckBox^  checkBoxVS1;
private: System::Windows::Forms::GroupBox^  groupBoxButtons;
private: System::Windows::Forms::GroupBox^  groupBoxPov;
private: System::Windows::Forms::RadioButton^  radioButtonPovCont;
private: System::Windows::Forms::RadioButton^  radioButtonPovDir;

private: System::Windows::Forms::GroupBox^  groupBoxAxF;
private: System::Windows::Forms::CheckBox^  checkBoxFRy;
private: System::Windows::Forms::CheckBox^  checkBoxFZ;
private: System::Windows::Forms::CheckBox^  checkBoxFY;
private: System::Windows::Forms::CheckBox^  checkBoxFS2;
private: System::Windows::Forms::CheckBox^  checkBoxFRx;
private: System::Windows::Forms::CheckBox^  checkBoxFX;
private: System::Windows::Forms::CheckBox^  checkBoxFRz;
private: System::Windows::Forms::CheckBox^  checkBoxFS1;












private: System::Windows::Forms::GroupBox^  groupBoxAxBasic;
private: System::Windows::Forms::CheckBox^  checkBoxX;
private: System::Windows::Forms::CheckBox^  checkBoxY;
private: System::Windows::Forms::CheckBox^  checkBoxU;
private: System::Windows::Forms::CheckBox^  checkBoxZ;
private: System::Windows::Forms::CheckBox^  checkBoxR;
private: System::Windows::Forms::CheckBox^  checkBoxV;
private: System::Windows::Forms::TabControl^  tabControTop;
private: System::Windows::Forms::CheckBox^  checkBoxHeaderFile;
private: System::Windows::Forms::GroupBox^  groupBoxTarget;
private: System::Windows::Forms::NumericUpDown^  numericUpDownTarget;
private: System::Windows::Forms::GroupBox^  groupBoxAction;
private: System::Windows::Forms::RadioButton^  radioButtonDelete;
private: System::Windows::Forms::RadioButton^  radioButtonDeleteAll;

private: System::Windows::Forms::RadioButton^  radioButtonConf;
private: System::Windows::Forms::ListBox^  listBoxNpov;

private: System::Windows::Forms::TextBox^  textBoxButtons;
private: System::Windows::Forms::CheckBox^  checkBoxRy;
private: System::Windows::Forms::CheckBox^  checkBoxRx;
private: System::Windows::Forms::Label^  labelNpov;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonApply = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->radioButtonPovDir = (gcnew System::Windows::Forms::RadioButton());
			this->checkBoxHeaderFile = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDownTarget = (gcnew System::Windows::Forms::NumericUpDown());
			this->radioButtonDelete = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonDeleteAll = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonConf = (gcnew System::Windows::Forms::RadioButton());
			this->listBoxNpov = (gcnew System::Windows::Forms::ListBox());
			this->textBoxButtons = (gcnew System::Windows::Forms::TextBox());
			this->saveHeaderFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->tabPageAxes = (gcnew System::Windows::Forms::TabPage());
			this->groupBoxAction = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxTarget = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxAxV = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxVRy = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVZ = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVY = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVS2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVRx = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVX = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVRz = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxVS1 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxButtons = (gcnew System::Windows::Forms::GroupBox());
			this->groupBoxPov = (gcnew System::Windows::Forms::GroupBox());
			this->labelNpov = (gcnew System::Windows::Forms::Label());
			this->radioButtonPovCont = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxAxF = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxFRy = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFZ = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFY = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFS2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFRx = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFX = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFRz = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFS1 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxAxBasic = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxRy = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxX = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxRx = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxY = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxU = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxZ = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxR = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxV = (gcnew System::Windows::Forms::CheckBox());
			this->tabControTop = (gcnew System::Windows::Forms::TabControl());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownTarget))->BeginInit();
			this->tabPageAxes->SuspendLayout();
			this->groupBoxAction->SuspendLayout();
			this->groupBoxTarget->SuspendLayout();
			this->groupBoxAxV->SuspendLayout();
			this->groupBoxButtons->SuspendLayout();
			this->groupBoxPov->SuspendLayout();
			this->groupBoxAxF->SuspendLayout();
			this->groupBoxAxBasic->SuspendLayout();
			this->tabControTop->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonOK
			// 
			this->buttonOK->Location = System::Drawing::Point(7, 306);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(60, 23);
			this->buttonOK->TabIndex = 1;
			this->buttonOK->Text = L"OK";
			this->buttonOK->UseVisualStyleBackColor = true;
			this->buttonOK->Click += gcnew System::EventHandler(this, &Form1::buttonOK_Click);
			// 
			// buttonApply
			// 
			this->buttonApply->Location = System::Drawing::Point(73, 307);
			this->buttonApply->Name = L"buttonApply";
			this->buttonApply->Size = System::Drawing::Size(60, 23);
			this->buttonApply->TabIndex = 2;
			this->buttonApply->Text = L"Apply";
			this->toolTip1->SetToolTip(this->buttonApply, L"Write configuration to registry\r\nOptionally: Create header file");
			this->buttonApply->UseVisualStyleBackColor = true;
			this->buttonApply->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::BtnApplyClicked);
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(139, 307);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(60, 23);
			this->buttonCancel->TabIndex = 3;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->UseVisualStyleBackColor = true;
			this->buttonCancel->Click += gcnew System::EventHandler(this, &Form1::buttonCancel_Click);
			// 
			// toolTip1
			// 
			this->toolTip1->IsBalloon = true;
			this->toolTip1->ToolTipIcon = System::Windows::Forms::ToolTipIcon::Info;
			// 
			// radioButtonPovDir
			// 
			this->radioButtonPovDir->AutoSize = true;
			this->radioButtonPovDir->Checked = true;
			this->radioButtonPovDir->Location = System::Drawing::Point(6, 19);
			this->radioButtonPovDir->Name = L"radioButtonPovDir";
			this->radioButtonPovDir->Size = System::Drawing::Size(81, 17);
			this->radioButtonPovDir->TabIndex = 12;
			this->radioButtonPovDir->TabStop = true;
			this->radioButtonPovDir->Text = L"4 Directions";
			this->toolTip1->SetToolTip(this->radioButtonPovDir, L"Four Directions:\r\nNorth, East, South, West\r\nand a neutral state");
			this->radioButtonPovDir->UseVisualStyleBackColor = true;
			// 
			// checkBoxHeaderFile
			// 
			this->checkBoxHeaderFile->AutoSize = true;
			this->checkBoxHeaderFile->Location = System::Drawing::Point(209, 313);
			this->checkBoxHeaderFile->Name = L"checkBoxHeaderFile";
			this->checkBoxHeaderFile->Size = System::Drawing::Size(88, 17);
			this->checkBoxHeaderFile->TabIndex = 4;
			this->checkBoxHeaderFile->Text = L"Create .h File";
			this->toolTip1->SetToolTip(this->checkBoxHeaderFile, L"Create output header file\r\nwhen pressing of Apply button\r\nor on OK button");
			this->checkBoxHeaderFile->UseVisualStyleBackColor = true;
			// 
			// numericUpDownTarget
			// 
			this->numericUpDownTarget->Location = System::Drawing::Point(6, 19);
			this->numericUpDownTarget->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {16, 0, 0, 0});
			this->numericUpDownTarget->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownTarget->Name = L"numericUpDownTarget";
			this->numericUpDownTarget->ReadOnly = true;
			this->numericUpDownTarget->Size = System::Drawing::Size(90, 20);
			this->numericUpDownTarget->TabIndex = 0;
			this->toolTip1->SetToolTip(this->numericUpDownTarget, L"Select the target device to configure or to delete");
			this->numericUpDownTarget->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// radioButtonDelete
			// 
			this->radioButtonDelete->AutoSize = true;
			this->radioButtonDelete->Location = System::Drawing::Point(6, 42);
			this->radioButtonDelete->Name = L"radioButtonDelete";
			this->radioButtonDelete->Size = System::Drawing::Size(93, 17);
			this->radioButtonDelete->TabIndex = 0;
			this->radioButtonDelete->Text = L"Delete Device";
			this->toolTip1->SetToolTip(this->radioButtonDelete, L"Delete device - Impossible to Undo\r\nSelect target device before pressing Apply or" 
				L" OK");
			this->radioButtonDelete->UseVisualStyleBackColor = true;
			// 
			// radioButtonDeleteAll
			// 
			this->radioButtonDeleteAll->AutoSize = true;
			this->radioButtonDeleteAll->Location = System::Drawing::Point(6, 65);
			this->radioButtonDeleteAll->Name = L"radioButtonDeleteAll";
			this->radioButtonDeleteAll->Size = System::Drawing::Size(53, 17);
			this->radioButtonDeleteAll->TabIndex = 0;
			this->radioButtonDeleteAll->Text = L"Reset";
			this->toolTip1->SetToolTip(this->radioButtonDeleteAll, L"Reset to default device\r\nDelete ALL configured devices - Impossible to Undo\r\n");
			this->radioButtonDeleteAll->UseVisualStyleBackColor = true;
			// 
			// radioButtonConf
			// 
			this->radioButtonConf->AutoSize = true;
			this->radioButtonConf->Checked = true;
			this->radioButtonConf->Location = System::Drawing::Point(6, 19);
			this->radioButtonConf->Name = L"radioButtonConf";
			this->radioButtonConf->Size = System::Drawing::Size(70, 17);
			this->radioButtonConf->TabIndex = 0;
			this->radioButtonConf->TabStop = true;
			this->radioButtonConf->Text = L"Configure";
			this->toolTip1->SetToolTip(this->radioButtonConf, L"Create/Configure device\r\nSelect target device before pressing Apply or OK");
			this->radioButtonConf->UseVisualStyleBackColor = true;
			// 
			// listBoxNpov
			// 
			this->listBoxNpov->AllowDrop = true;
			this->listBoxNpov->FormatString = L"N0";
			this->listBoxNpov->FormattingEnabled = true;
			this->listBoxNpov->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"0", L"1", L"2", L"3", L"4"});
			this->listBoxNpov->Location = System::Drawing::Point(46, 65);
			this->listBoxNpov->Name = L"listBoxNpov";
			this->listBoxNpov->Size = System::Drawing::Size(50, 30);
			this->listBoxNpov->TabIndex = 13;
			this->toolTip1->SetToolTip(this->listBoxNpov, L"Number of POV Hat switches");
			// 
			// textBoxButtons
			// 
			this->textBoxButtons->Location = System::Drawing::Point(6, 19);
			this->textBoxButtons->MaxLength = 3;
			this->textBoxButtons->Name = L"textBoxButtons";
			this->textBoxButtons->Size = System::Drawing::Size(90, 20);
			this->textBoxButtons->TabIndex = 1;
			this->textBoxButtons->Text = L"8";
			this->toolTip1->SetToolTip(this->textBoxButtons, L"Enter the number of buttons\r\nRange: 0-128");
			// 
			// saveHeaderFileDialog
			// 
			this->saveHeaderFileDialog->DefaultExt = L"h";
			this->saveHeaderFileDialog->FileName = L"HidRepDesc";
			this->saveHeaderFileDialog->Filter = L"\"header files (*.h)|*.h|All files (*.*)|*.*\"";
			this->saveHeaderFileDialog->InitialDirectory = L".";
			this->saveHeaderFileDialog->Title = L"Save HID Report Descriptor in file";
			// 
			// tabPageAxes
			// 
			this->tabPageAxes->Controls->Add(this->groupBoxAction);
			this->tabPageAxes->Controls->Add(this->groupBoxTarget);
			this->tabPageAxes->Controls->Add(this->groupBoxAxV);
			this->tabPageAxes->Controls->Add(this->groupBoxButtons);
			this->tabPageAxes->Controls->Add(this->groupBoxPov);
			this->tabPageAxes->Controls->Add(this->groupBoxAxF);
			this->tabPageAxes->Controls->Add(this->groupBoxAxBasic);
			this->tabPageAxes->Location = System::Drawing::Point(4, 22);
			this->tabPageAxes->Name = L"tabPageAxes";
			this->tabPageAxes->Padding = System::Windows::Forms::Padding(3);
			this->tabPageAxes->Size = System::Drawing::Size(245, 274);
			this->tabPageAxes->TabIndex = 0;
			this->tabPageAxes->Text = L"Axes, Buttons & POV";
			this->tabPageAxes->UseVisualStyleBackColor = true;
			// 
			// groupBoxAction
			// 
			this->groupBoxAction->Controls->Add(this->radioButtonDelete);
			this->groupBoxAction->Controls->Add(this->radioButtonDeleteAll);
			this->groupBoxAction->Controls->Add(this->radioButtonConf);
			this->groupBoxAction->Location = System::Drawing::Point(6, 172);
			this->groupBoxAction->Name = L"groupBoxAction";
			this->groupBoxAction->Size = System::Drawing::Size(117, 94);
			this->groupBoxAction->TabIndex = 15;
			this->groupBoxAction->TabStop = false;
			this->groupBoxAction->Text = L"Action";
			// 
			// groupBoxTarget
			// 
			this->groupBoxTarget->Controls->Add(this->numericUpDownTarget);
			this->groupBoxTarget->Location = System::Drawing::Point(129, 172);
			this->groupBoxTarget->Name = L"groupBoxTarget";
			this->groupBoxTarget->Size = System::Drawing::Size(111, 94);
			this->groupBoxTarget->TabIndex = 14;
			this->groupBoxTarget->TabStop = false;
			this->groupBoxTarget->Text = L"Target Device";
			// 
			// groupBoxAxV
			// 
			this->groupBoxAxV->Controls->Add(this->checkBoxVRy);
			this->groupBoxAxV->Controls->Add(this->checkBoxVZ);
			this->groupBoxAxV->Controls->Add(this->checkBoxVY);
			this->groupBoxAxV->Controls->Add(this->checkBoxVS2);
			this->groupBoxAxV->Controls->Add(this->checkBoxVRx);
			this->groupBoxAxV->Controls->Add(this->checkBoxVX);
			this->groupBoxAxV->Controls->Add(this->checkBoxVRz);
			this->groupBoxAxV->Controls->Add(this->checkBoxVS1);
			this->groupBoxAxV->Location = System::Drawing::Point(367, 122);
			this->groupBoxAxV->Name = L"groupBoxAxV";
			this->groupBoxAxV->Size = System::Drawing::Size(200, 100);
			this->groupBoxAxV->TabIndex = 13;
			this->groupBoxAxV->TabStop = false;
			this->groupBoxAxV->Text = L"Velocity";
			this->groupBoxAxV->Visible = false;
			// 
			// checkBoxVRy
			// 
			this->checkBoxVRy->AutoSize = true;
			this->checkBoxVRy->Location = System::Drawing::Point(52, 50);
			this->checkBoxVRy->Name = L"checkBoxVRy";
			this->checkBoxVRy->Size = System::Drawing::Size(46, 17);
			this->checkBoxVRy->TabIndex = 16;
			this->checkBoxVRy->Tag = L"VRy";
			this->checkBoxVRy->Text = L"VRy";
			this->checkBoxVRy->UseVisualStyleBackColor = true;
			// 
			// checkBoxVZ
			// 
			this->checkBoxVZ->AutoSize = true;
			this->checkBoxVZ->Location = System::Drawing::Point(98, 27);
			this->checkBoxVZ->Name = L"checkBoxVZ";
			this->checkBoxVZ->Size = System::Drawing::Size(40, 17);
			this->checkBoxVZ->TabIndex = 14;
			this->checkBoxVZ->Tag = L"VZ";
			this->checkBoxVZ->Text = L"VZ";
			this->checkBoxVZ->UseVisualStyleBackColor = true;
			// 
			// checkBoxVY
			// 
			this->checkBoxVY->AutoSize = true;
			this->checkBoxVY->Location = System::Drawing::Point(52, 27);
			this->checkBoxVY->Name = L"checkBoxVY";
			this->checkBoxVY->Size = System::Drawing::Size(40, 17);
			this->checkBoxVY->TabIndex = 13;
			this->checkBoxVY->Tag = L"VY";
			this->checkBoxVY->Text = L"VY";
			this->checkBoxVY->UseVisualStyleBackColor = true;
			// 
			// checkBoxVS2
			// 
			this->checkBoxVS2->AutoSize = true;
			this->checkBoxVS2->Location = System::Drawing::Point(98, 73);
			this->checkBoxVS2->Name = L"checkBoxVS2";
			this->checkBoxVS2->Size = System::Drawing::Size(65, 17);
			this->checkBoxVS2->TabIndex = 5;
			this->checkBoxVS2->Tag = L"VS2";
			this->checkBoxVS2->Text = L"VSlider2";
			this->checkBoxVS2->UseVisualStyleBackColor = true;
			// 
			// checkBoxVRx
			// 
			this->checkBoxVRx->AutoSize = true;
			this->checkBoxVRx->Location = System::Drawing::Point(6, 50);
			this->checkBoxVRx->Name = L"checkBoxVRx";
			this->checkBoxVRx->Size = System::Drawing::Size(46, 17);
			this->checkBoxVRx->TabIndex = 15;
			this->checkBoxVRx->Tag = L"VRx";
			this->checkBoxVRx->Text = L"VRx";
			this->checkBoxVRx->UseVisualStyleBackColor = true;
			// 
			// checkBoxVX
			// 
			this->checkBoxVX->AutoSize = true;
			this->checkBoxVX->Location = System::Drawing::Point(6, 27);
			this->checkBoxVX->Name = L"checkBoxVX";
			this->checkBoxVX->Size = System::Drawing::Size(40, 17);
			this->checkBoxVX->TabIndex = 12;
			this->checkBoxVX->Tag = L"VX";
			this->checkBoxVX->Text = L"VX";
			this->checkBoxVX->UseVisualStyleBackColor = true;
			// 
			// checkBoxVRz
			// 
			this->checkBoxVRz->AutoSize = true;
			this->checkBoxVRz->Location = System::Drawing::Point(98, 50);
			this->checkBoxVRz->Name = L"checkBoxVRz";
			this->checkBoxVRz->Size = System::Drawing::Size(46, 17);
			this->checkBoxVRz->TabIndex = 17;
			this->checkBoxVRz->Tag = L"VRz";
			this->checkBoxVRz->Text = L"VRz";
			this->checkBoxVRz->UseVisualStyleBackColor = true;
			// 
			// checkBoxVS1
			// 
			this->checkBoxVS1->AutoSize = true;
			this->checkBoxVS1->Location = System::Drawing::Point(6, 73);
			this->checkBoxVS1->Name = L"checkBoxVS1";
			this->checkBoxVS1->Size = System::Drawing::Size(65, 17);
			this->checkBoxVS1->TabIndex = 4;
			this->checkBoxVS1->Tag = L"VS1";
			this->checkBoxVS1->Text = L"VSlider1";
			this->checkBoxVS1->UseVisualStyleBackColor = true;
			// 
			// groupBoxButtons
			// 
			this->groupBoxButtons->Controls->Add(this->textBoxButtons);
			this->groupBoxButtons->Location = System::Drawing::Point(129, 116);
			this->groupBoxButtons->Name = L"groupBoxButtons";
			this->groupBoxButtons->Size = System::Drawing::Size(109, 50);
			this->groupBoxButtons->TabIndex = 13;
			this->groupBoxButtons->TabStop = false;
			this->groupBoxButtons->Text = L"number of Buttons";
			// 
			// groupBoxPov
			// 
			this->groupBoxPov->Controls->Add(this->labelNpov);
			this->groupBoxPov->Controls->Add(this->listBoxNpov);
			this->groupBoxPov->Controls->Add(this->radioButtonPovCont);
			this->groupBoxPov->Controls->Add(this->radioButtonPovDir);
			this->groupBoxPov->Location = System::Drawing::Point(129, 10);
			this->groupBoxPov->Name = L"groupBoxPov";
			this->groupBoxPov->Size = System::Drawing::Size(109, 100);
			this->groupBoxPov->TabIndex = 13;
			this->groupBoxPov->TabStop = false;
			this->groupBoxPov->Text = L"POV Hat Switch";
			// 
			// labelNpov
			// 
			this->labelNpov->AutoSize = true;
			this->labelNpov->Location = System::Drawing::Point(3, 62);
			this->labelNpov->Name = L"labelNpov";
			this->labelNpov->Size = System::Drawing::Size(37, 13);
			this->labelNpov->TabIndex = 14;
			this->labelNpov->Text = L"POVs:";
			// 
			// radioButtonPovCont
			// 
			this->radioButtonPovCont->AutoSize = true;
			this->radioButtonPovCont->Location = System::Drawing::Point(6, 42);
			this->radioButtonPovCont->Name = L"radioButtonPovCont";
			this->radioButtonPovCont->Size = System::Drawing::Size(78, 17);
			this->radioButtonPovCont->TabIndex = 12;
			this->radioButtonPovCont->Text = L"Continuous";
			this->radioButtonPovCont->UseVisualStyleBackColor = true;
			// 
			// groupBoxAxF
			// 
			this->groupBoxAxF->Controls->Add(this->checkBoxFRy);
			this->groupBoxAxF->Controls->Add(this->checkBoxFZ);
			this->groupBoxAxF->Controls->Add(this->checkBoxFY);
			this->groupBoxAxF->Controls->Add(this->checkBoxFS2);
			this->groupBoxAxF->Controls->Add(this->checkBoxFRx);
			this->groupBoxAxF->Controls->Add(this->checkBoxFX);
			this->groupBoxAxF->Controls->Add(this->checkBoxFRz);
			this->groupBoxAxF->Controls->Add(this->checkBoxFS1);
			this->groupBoxAxF->Location = System::Drawing::Point(367, 6);
			this->groupBoxAxF->Name = L"groupBoxAxF";
			this->groupBoxAxF->Size = System::Drawing::Size(200, 100);
			this->groupBoxAxF->TabIndex = 9;
			this->groupBoxAxF->TabStop = false;
			this->groupBoxAxF->Text = L"Force";
			this->groupBoxAxF->Visible = false;
			// 
			// checkBoxFRy
			// 
			this->checkBoxFRy->AutoSize = true;
			this->checkBoxFRy->Location = System::Drawing::Point(52, 50);
			this->checkBoxFRy->Name = L"checkBoxFRy";
			this->checkBoxFRy->Size = System::Drawing::Size(45, 17);
			this->checkBoxFRy->TabIndex = 16;
			this->checkBoxFRy->Tag = L"FRy";
			this->checkBoxFRy->Text = L"FRy";
			this->checkBoxFRy->UseVisualStyleBackColor = true;
			// 
			// checkBoxFZ
			// 
			this->checkBoxFZ->AutoSize = true;
			this->checkBoxFZ->Location = System::Drawing::Point(98, 27);
			this->checkBoxFZ->Name = L"checkBoxFZ";
			this->checkBoxFZ->Size = System::Drawing::Size(39, 17);
			this->checkBoxFZ->TabIndex = 14;
			this->checkBoxFZ->Tag = L"FZ";
			this->checkBoxFZ->Text = L"FZ";
			this->checkBoxFZ->UseVisualStyleBackColor = true;
			// 
			// checkBoxFY
			// 
			this->checkBoxFY->AutoSize = true;
			this->checkBoxFY->Location = System::Drawing::Point(52, 27);
			this->checkBoxFY->Name = L"checkBoxFY";
			this->checkBoxFY->Size = System::Drawing::Size(39, 17);
			this->checkBoxFY->TabIndex = 13;
			this->checkBoxFY->Tag = L"FY";
			this->checkBoxFY->Text = L"FY";
			this->checkBoxFY->UseVisualStyleBackColor = true;
			// 
			// checkBoxFS2
			// 
			this->checkBoxFS2->AutoSize = true;
			this->checkBoxFS2->Location = System::Drawing::Point(98, 73);
			this->checkBoxFS2->Name = L"checkBoxFS2";
			this->checkBoxFS2->Size = System::Drawing::Size(64, 17);
			this->checkBoxFS2->TabIndex = 5;
			this->checkBoxFS2->Tag = L"FS2";
			this->checkBoxFS2->Text = L"FSlider2";
			this->checkBoxFS2->UseVisualStyleBackColor = true;
			// 
			// checkBoxFRx
			// 
			this->checkBoxFRx->AutoSize = true;
			this->checkBoxFRx->Location = System::Drawing::Point(6, 50);
			this->checkBoxFRx->Name = L"checkBoxFRx";
			this->checkBoxFRx->Size = System::Drawing::Size(45, 17);
			this->checkBoxFRx->TabIndex = 15;
			this->checkBoxFRx->Tag = L"FRx";
			this->checkBoxFRx->Text = L"FRx";
			this->checkBoxFRx->UseVisualStyleBackColor = true;
			this->checkBoxFRx->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkedListBoxBtn1_SelectedIndexChanged);
			// 
			// checkBoxFX
			// 
			this->checkBoxFX->AutoSize = true;
			this->checkBoxFX->Location = System::Drawing::Point(6, 27);
			this->checkBoxFX->Name = L"checkBoxFX";
			this->checkBoxFX->Size = System::Drawing::Size(39, 17);
			this->checkBoxFX->TabIndex = 12;
			this->checkBoxFX->Tag = L"FX";
			this->checkBoxFX->Text = L"FX";
			this->checkBoxFX->UseVisualStyleBackColor = true;
			this->checkBoxFX->CheckStateChanged += gcnew System::EventHandler(this, &Form1::checkedListBoxBtn1_SelectedIndexChanged);
			// 
			// checkBoxFRz
			// 
			this->checkBoxFRz->AutoSize = true;
			this->checkBoxFRz->Location = System::Drawing::Point(98, 50);
			this->checkBoxFRz->Name = L"checkBoxFRz";
			this->checkBoxFRz->Size = System::Drawing::Size(45, 17);
			this->checkBoxFRz->TabIndex = 17;
			this->checkBoxFRz->Tag = L"FRz";
			this->checkBoxFRz->Text = L"FRz";
			this->checkBoxFRz->UseVisualStyleBackColor = true;
			// 
			// checkBoxFS1
			// 
			this->checkBoxFS1->AutoSize = true;
			this->checkBoxFS1->Location = System::Drawing::Point(6, 73);
			this->checkBoxFS1->Name = L"checkBoxFS1";
			this->checkBoxFS1->Size = System::Drawing::Size(64, 17);
			this->checkBoxFS1->TabIndex = 4;
			this->checkBoxFS1->Tag = L"FS1";
			this->checkBoxFS1->Text = L"FSlider1";
			this->checkBoxFS1->UseVisualStyleBackColor = true;
			// 
			// groupBoxAxBasic
			// 
			this->groupBoxAxBasic->BackColor = System::Drawing::Color::LavenderBlush;
			this->groupBoxAxBasic->Controls->Add(this->checkBoxRy);
			this->groupBoxAxBasic->Controls->Add(this->checkBoxX);
			this->groupBoxAxBasic->Controls->Add(this->checkBoxRx);
			this->groupBoxAxBasic->Controls->Add(this->checkBoxY);
			this->groupBoxAxBasic->Controls->Add(this->checkBoxU);
			this->groupBoxAxBasic->Controls->Add(this->checkBoxZ);
			this->groupBoxAxBasic->Controls->Add(this->checkBoxR);
			this->groupBoxAxBasic->Controls->Add(this->checkBoxV);
			this->groupBoxAxBasic->Location = System::Drawing::Point(6, 10);
			this->groupBoxAxBasic->Name = L"groupBoxAxBasic";
			this->groupBoxAxBasic->Size = System::Drawing::Size(117, 156);
			this->groupBoxAxBasic->TabIndex = 8;
			this->groupBoxAxBasic->TabStop = false;
			this->groupBoxAxBasic->Text = L"Axes";
			// 
			// checkBoxRy
			// 
			this->checkBoxRy->AutoSize = true;
			this->checkBoxRy->Checked = true;
			this->checkBoxRy->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxRy->Location = System::Drawing::Point(6, 67);
			this->checkBoxRy->Name = L"checkBoxRy";
			this->checkBoxRy->Size = System::Drawing::Size(39, 17);
			this->checkBoxRy->TabIndex = 9;
			this->checkBoxRy->Tag = L"Ry";
			this->checkBoxRy->Text = L"Ry";
			this->checkBoxRy->UseVisualStyleBackColor = true;
			// 
			// checkBoxX
			// 
			this->checkBoxX->AutoSize = true;
			this->checkBoxX->Checked = true;
			this->checkBoxX->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxX->Location = System::Drawing::Point(6, 23);
			this->checkBoxX->Name = L"checkBoxX";
			this->checkBoxX->Size = System::Drawing::Size(33, 17);
			this->checkBoxX->TabIndex = 0;
			this->checkBoxX->Tag = L"X";
			this->checkBoxX->Text = L"X";
			this->checkBoxX->UseVisualStyleBackColor = true;
			// 
			// checkBoxRx
			// 
			this->checkBoxRx->AutoSize = true;
			this->checkBoxRx->Checked = true;
			this->checkBoxRx->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxRx->Location = System::Drawing::Point(6, 45);
			this->checkBoxRx->Name = L"checkBoxRx";
			this->checkBoxRx->Size = System::Drawing::Size(39, 17);
			this->checkBoxRx->TabIndex = 10;
			this->checkBoxRx->Tag = L"Rx";
			this->checkBoxRx->Text = L"Rx";
			this->checkBoxRx->UseVisualStyleBackColor = true;
			// 
			// checkBoxY
			// 
			this->checkBoxY->AutoSize = true;
			this->checkBoxY->Checked = true;
			this->checkBoxY->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxY->Location = System::Drawing::Point(45, 23);
			this->checkBoxY->Name = L"checkBoxY";
			this->checkBoxY->Size = System::Drawing::Size(33, 17);
			this->checkBoxY->TabIndex = 1;
			this->checkBoxY->Tag = L"Y";
			this->checkBoxY->Text = L"Y";
			this->checkBoxY->UseVisualStyleBackColor = true;
			// 
			// checkBoxU
			// 
			this->checkBoxU->AutoSize = true;
			this->checkBoxU->Checked = true;
			this->checkBoxU->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxU->Location = System::Drawing::Point(6, 111);
			this->checkBoxU->Name = L"checkBoxU";
			this->checkBoxU->Size = System::Drawing::Size(65, 17);
			this->checkBoxU->TabIndex = 4;
			this->checkBoxU->Tag = L"S1";
			this->checkBoxU->Text = L"U/Slider";
			this->checkBoxU->UseVisualStyleBackColor = true;
			// 
			// checkBoxZ
			// 
			this->checkBoxZ->AutoSize = true;
			this->checkBoxZ->Checked = true;
			this->checkBoxZ->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxZ->Location = System::Drawing::Point(84, 23);
			this->checkBoxZ->Name = L"checkBoxZ";
			this->checkBoxZ->Size = System::Drawing::Size(33, 17);
			this->checkBoxZ->TabIndex = 2;
			this->checkBoxZ->Tag = L"Z";
			this->checkBoxZ->Text = L"Z";
			this->checkBoxZ->UseVisualStyleBackColor = true;
			// 
			// checkBoxR
			// 
			this->checkBoxR->AutoSize = true;
			this->checkBoxR->Checked = true;
			this->checkBoxR->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxR->Location = System::Drawing::Point(6, 89);
			this->checkBoxR->Name = L"checkBoxR";
			this->checkBoxR->Size = System::Drawing::Size(92, 17);
			this->checkBoxR->TabIndex = 3;
			this->checkBoxR->Tag = L"Rz";
			this->checkBoxR->Text = L"R/Rz/Rudder";
			this->checkBoxR->UseVisualStyleBackColor = true;
			// 
			// checkBoxV
			// 
			this->checkBoxV->AutoSize = true;
			this->checkBoxV->Checked = true;
			this->checkBoxV->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxV->Location = System::Drawing::Point(6, 133);
			this->checkBoxV->Name = L"checkBoxV";
			this->checkBoxV->Size = System::Drawing::Size(93, 17);
			this->checkBoxV->TabIndex = 5;
			this->checkBoxV->Tag = L"S2";
			this->checkBoxV->Text = L"V/Dial/Slider2";
			this->checkBoxV->UseVisualStyleBackColor = true;
			// 
			// tabControTop
			// 
			this->tabControTop->Controls->Add(this->tabPageAxes);
			this->tabControTop->Location = System::Drawing::Point(6, 1);
			this->tabControTop->Name = L"tabControTop";
			this->tabControTop->SelectedIndex = 0;
			this->tabControTop->Size = System::Drawing::Size(253, 300);
			this->tabControTop->TabIndex = 0;
			// 
			// Form1
			// 
			this->AcceptButton = this->buttonOK;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->buttonCancel;
			this->ClientSize = System::Drawing::Size(263, 336);
			this->Controls->Add(this->checkBoxHeaderFile);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonApply);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->tabControTop);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Text = L"Configure vJoy";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownTarget))->EndInit();
			this->tabPageAxes->ResumeLayout(false);
			this->groupBoxAction->ResumeLayout(false);
			this->groupBoxAction->PerformLayout();
			this->groupBoxTarget->ResumeLayout(false);
			this->groupBoxAxV->ResumeLayout(false);
			this->groupBoxAxV->PerformLayout();
			this->groupBoxButtons->ResumeLayout(false);
			this->groupBoxButtons->PerformLayout();
			this->groupBoxPov->ResumeLayout(false);
			this->groupBoxPov->PerformLayout();
			this->groupBoxAxF->ResumeLayout(false);
			this->groupBoxAxF->PerformLayout();
			this->groupBoxAxBasic->ResumeLayout(false);
			this->groupBoxAxBasic->PerformLayout();
			this->tabControTop->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();
#pragma endregion
			this->listBoxNpov->SelectedIndex = 0;
		}

	// Not really needed
	private: System::Void checkedListBoxBtn1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 System::String ^ AxisName = ((System::Windows::Forms::CheckBox ^)sender)->Text;
				 bool Checked = ((System::Windows::Forms::CheckBox ^)sender)->Checked;
			 }

/*
	Apply Button clicked
	1. Collect data
	2. Create DB
	3. Print to File
	4. Refresh driver (Disable/enable)
*/
private: System::Void BtnApplyClicked(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {

			 if (!UpdateButtonState(sender, e))
				 return;
			 ACTION action = UpdateActionState(sender, e);
			 UpdateAxesState(sender, e);
			 UpdatePovHatsState(sender, e);

			 if (action == config)
				 WriteHidReportDescriptorToFile(this->numericUpDownTarget->Value, sender, e);
			 else if (action == del_all)
				 DeleteHidReportDescFromReg(0);
			 else
				 DeleteHidReportDescFromReg((int)numericUpDownTarget->Value);

			 // Test if any of the interfaces is used.
			 // If used - issue a message box
Test:
			 System::Windows::Forms::DialogResult res;
			 int count=0;
			 for (UINT i=1 ; i<=16;i++)
			 {
				 if (GetVJDStatus(i) == VJD_STAT_OWN)
				 {
					 count++;
				 };
				 if (count)
				 {
					 res = MessageBox::Show("One or more vJoy device interfaces are open",  "Close then then continue", MessageBoxButtons::RetryCancel, MessageBoxIcon::Warning);
					 if (res == System::Windows::Forms::DialogResult::Cancel)
						 return;
					 else
						 goto Test;
				 };
			 }


			 // Refresh
			refresh_vjoy_specific(GetvJoyVersion());

		 }

private: System::Void UpdatePovHatsState(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){
			 nPovHats = listBoxNpov->SelectedIndex;
		 }

private: ACTION UpdateActionState(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 if (this->radioButtonDelete->Checked)
				 return del;
			 else if (this->radioButtonDeleteAll->Checked)
				 return del_all;
			 else //if (this->radioButtonConf->Checked)
				 return config;
		 }

private: System::Void UpdateAxesState(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 Control^ ctrl;
			 String ^ sTag;
			 int index;

			 array<GroupBox^> ^AxisGroupList = {groupBoxAxBasic,  groupBoxAxF, /*groupBoxAxA,*/ groupBoxAxV};

			 // loop on all axis groupBoxs
			 for each (GroupBox ^ gb in AxisGroupList)
			 {
				 // loop on all controls in groupBox
				 ctrl = gb;
				 int nControls = gb->Controls->Count;
				 for (int i=0; i<nControls; i++)
				 {
					 ctrl = gb->GetNextControl( ctrl, false);
					 sTag = ctrl->Tag->ToString();
					 index = Array::IndexOf( AxisList,  sTag );
					 AxisChecked[index] = ((CheckBox ^)ctrl)->Checked;
				 };
			 };
		 }

private: bool UpdateButtonState(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 try
			 {
				 //nButtons = System::Convert::ToInt32(comboBoxButtons->Text);
				 nButtons = System::Convert::ToInt32(textBoxButtons->Text);
				 if (nButtons>128 || nButtons<0)
				 {				
					textBoxButtons->Text = L"8";
					 MessageBox::Show("Range: 0 to 128",  "Form Closing", MessageBoxButtons::OK, MessageBoxIcon::Error);
					 return false;
				 };

			 }
			 catch (FormatException^ e )
			 {
				 e->ToString();				
				 textBoxButtons->Text = L"8";
				 MessageBox::Show("Only numeric value allowed",  "Form Closing", MessageBoxButtons::OK, MessageBoxIcon::Error);
				 return false;
			 }

			return true;
			 //array<CheckedListBox^> ^ButtonGroupList = {checkedListBoxBtn1, checkedListBoxBtn2, checkedListBoxBtn3, checkedListBoxBtn4};
			 //Object^ itemChecked;
			 //String ^ strButton;

			 //// Clean array of buttons
			 //System::Array::Clear(ButtonChecked, 0, ButtonChecked->Length);

			 //// loop on all button groups
			 //for each(CheckedListBox ^ clb in ButtonGroupList)
			 //{
				// // Create enumerator for all checked items for this group
				// IEnumerator^ EnumChecked = clb->CheckedItems->GetEnumerator();
				// while ( EnumChecked->MoveNext() )
				// {
				//	 itemChecked = safe_cast<Object^>(EnumChecked->Current);
				//	 strButton = itemChecked->ToString();
				//	 ButtonChecked[System::Convert::ToInt32(strButton)-1] = true;
				// };

			 //};
		 }

private: bool WriteHidReportDescriptorToFile(System::Decimal target, System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){

			 // Axes: Creat C-language array
			 int nAxes = AxisChecked->Length;
			 bool * Axes = new bool[nAxes];
			 for (int i=0; i<nAxes; i++)
				 Axes[i] = AxisChecked[i];


			 // POV Hats
			 int nPovHatsCont=0, nPovHatsDir=0;
			 int nPovs = listBoxNpov->SelectedIndex;
			 if (nPovs)
			 {
				 if (radioButtonPovDir->Checked)
					 nPovHatsDir = nPovs;
				 if (radioButtonPovCont->Checked)
					 nPovHatsCont = nPovs;
			 }; // if (nPovs)


			 // Call external C-function that creats an array of bytes that holds
			 // the HID Report Descriptor
			 UCHAR **out = (UCHAR **)malloc(sizeof(UCHAR *));
			 int desc_size = CreateHidReportDesc((void **)out, nButtons, Axes, nPovHatsCont, nPovHatsDir, (int)target);

			 if ((desc_size<=0))
			 {
				 MessageBox::Show("Failed to create HID Report Descriptor",  "Form Closing", MessageBoxButtons::OK, MessageBoxIcon::Error);
				 return false;
			 };

			 if (this->checkBoxHeaderFile->Checked)
			 {
				 // Get destination file name
				 if (saveHeaderFileDialog->ShowDialog() != System::Windows::Forms::DialogResult::OK) return false;
				 String ^ HeaderFileName = saveHeaderFileDialog->FileName;

				 // Print
				 String ^ line;
				 StringBuilder^ sb = gcnew StringBuilder();
				 sb->AppendLine(L"// Automatically generated HID Report Descriptor");
				 sb->AppendLine(L"// File: " + HeaderFileName);
				 sb->AppendLine(L"//");
				 sb->Append(L"char G_DefaultReportDescriptor[");
				 sb->Append(desc_size);
				 sb->AppendLine("] = {");
				 for (int i=0; i<desc_size; i++)
				 {
					 line = String::Format(L"\t0x{0:X2},", (unsigned int)((*out)[i]));
					 sb->AppendLine(line);
				 };
				 sb->Remove((sb->Length)-3,1);
				 sb->AppendLine("};");
				 StreamWriter^ outfile = gcnew StreamWriter(HeaderFileName);
				 outfile->Write(sb->ToString());
				 outfile->Close();
			 };

			 WriteHidReportDescToReg((int)target, *out, desc_size);
			 return true;
		 }
private: System::Void buttonCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
			 Application::Exit();
		 }
private: System::Void buttonOK_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->BtnApplyClicked(sender, (System::Windows::Forms::MouseEventArgs^)e);
			 //if (!UpdateButtonState(sender, nullptr))
				// return;
			 //UpdateAxesState(sender, nullptr);
			 //UpdatePovHatsState(sender, nullptr);
			 //if (!WriteHidReportDescriptorToFile(sender, nullptr))
				// return;
			 buttonCancel_Click(sender, nullptr);
		 }
};


}

