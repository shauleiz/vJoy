// vJoyConf.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Public.h"
#include "vJoyInterface.h"
#include "hidtoken.h"
#include "hidusage.h"
#include "declarations.h"
#include "vJoyDevice.h"
#include "wrapper.h"
#include "hidReportDescFfb.h"
#include "vJoyConf.h"

#pragma comment(lib, "vJoyInstall.lib")
#pragma comment(lib, "vJoyInterface.lib")
#pragma comment(lib, "Version.lib")

#define MAX_LOADSTRING	255
#define MAX_MSG_SIZE	1000
#ifdef X64
BOOL x86 = FALSE;
#else
BOOL x86 = TRUE;
#endif

// Global Variables:
HINSTANCE hInst;								// current instance
HWND  hTabs;	  // Handle to tabs control
HWND  hwndToolTip; // Handle to tooltip window
HINSTANCE g_hInstance;
std::vector<CvJoyDevice*> vJoyDevice;
std::vector<int> vControls;
HWND  hDlgTab; // Dialog box inside the tab control
HWND  hTopDlg; // Top Dialog
BOOL Changed = FALSE;
LPTSTR g_msg = new TCHAR[MAX_LOADSTRING];
LPTSTR g_ttl = new TCHAR[MAX_LOADSTRING];
BOOL AddBtn;
BOOL vJoyIsOn = TRUE;
HIMAGELIST 	hImageList = NULL;
int FfbState = -1; // 0: Prohibited; 1: Inactive; 2: Active

static const int g_Controls[] = {
    IDC_RSTALL,
    IDC_DEVTABS,
    IDC_AXES_GRP,
    IDC_CHK_X,
    IDC_CHK_Y,
    IDC_CHK_Z,
    IDC_CHK_RX,
    IDC_CHK_RY,
    IDC_CHK_RZ,
    IDC_CHK_SL0,
    IDC_CHK_SL1,
    IDC_CHK_WHEEL,
    IDC_CHK_ACCELERATOR,
    IDC_CHK_BRAKE,
    IDC_CHK_CLUTCH,
    IDC_CHK_STEERING,
    IDC_CHK_AILERON,
    IDC_CHK_RUDDER,
    IDC_CHK_THROTTLE,
    IDC_BTN_GRP,
    IDC_EDIT1,
    IDC_NBTN,
    IDC_POV_DESC,
    IDC_POV_CONT,
    IDC_POV_LST,
    IDC_APPLY_BTN,
    IDC_BUTTON3,
    IDC_REVT_BTN,
    IDC_ADDDEL_BTN,
    IDC_STATUS,
    IDC_CHK_ENVJOY,
};

// Forward declarations of functions included in this code module:
INT_PTR CALLBACK	vJoyConfCB(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgTabCB(HWND, UINT, WPARAM, LPARAM);
BOOL				TopDialogInit(HWND hDlg);
void				OnSelChanged(HWND hDlg);
BOOL				OnSelChanging(HWND hDlg);
void				OnApply(HWND hDlg);
void				OnRevert(HWND hDlg, BOOL Quiet);
void				OnResetAll(HWND hDlg);
void				OnAddDeleteDevice(HWND hDlg);
BOOL				OnClosing(void);
void				OnUserChanged(void);
BOOL				Initialize(void);
void				EnableTab(int iTab, BOOL Enable);
void				PopulateTab(int iTab);
void				InitializeTab(HWND hTab);
void				GetCtrlState(HWND hDlg, bool* AxisArr, int* nPovCont, int* nPovDisc, int* nButtons, bool* Ffb);
int 				CreateHidReportDesc(void** data, UINT nButtons, bool* axes, int nPovHatsCont, int nPovHatsDir, BYTE ReportId, bool Ffb);
void				WriteHidReportDescToReg(int target, UCHAR* Descriptor, int size);
void				DeleteHidReportDescFromReg(int target);
void DeleteOEMForceFeedbackFromReg();
BOOL				isOnlyDevice(UINT Id);
void				DeleteDevice(int Id);
void				AddDevice(int Id);
LPTSTR				Res2TStr(UINT uId);
LPTSTR				Res2TTtl(UINT uId);
HWND				CreateToolTip(HWND hDlg, const int arr[], int size);
void				UpdateToolTip(LPVOID param);
HIMAGELIST			CreateTabsImageList(void);
void				RefreshTabState(int Id);
BOOL				isBitnessCompat(BOOL Quiet);
void CALLBACK		vJoyConfChangedCB(BOOL, BOOL, PVOID);
void				vJoyDeviceRemoved(void);
void				vJoyDeviceArrived(void);
VOID CALLBACK		RefreshAll(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
int					GetCurrentTabIndex(void);
void				CreateFfbDesc(std::vector<BYTE>* buffer, BYTE ReportId);
void				ModifyFfbEffectDesc(std::vector<BYTE>* buffer, UINT16 Mask);
int					GetFfbState(void);
void				EnableFfbEffectCB(BOOL Enable);
void				UpdateFfbEffectCB(int rID);
void				ClearFfbEffectCB(void);
void				SetFfbEffectCB(void);
void				FfbCBChanged(void);
UINT16				GetFfbEffectMask(void);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Test if this code runs on the correct machine
    if (!isBitnessCompat(FALSE))
        return -1;


    // TODO: Place code here.
    if (!Initialize())
        return -1;

    HACCEL hAccel;
    hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_VJOYCONF));

    // Create top dialog box
    g_hInstance = hInstance;
    hTopDlg = CreateDialog(NULL, MAKEINTRESOURCE(IDD_VJOYCONF), NULL, vJoyConfCB);

    // Start top dialog box message loop
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0)) {
        if (TranslateAccelerator(hTopDlg, hAccel, &Msg))
            continue;
        if (!IsDialogMessage(hTopDlg, &Msg)) {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }

    return (int)Msg.wParam;
}




// Message handler for top dialog box.
INT_PTR CALLBACK vJoyConfCB(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool Checked;

    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_VJOYCHANGED:
            if (wParam && lParam) // First remove message
                vJoyDeviceRemoved();
            else if (!wParam) // Any arrival message
                vJoyDeviceArrived();
            break;

        case WM_INITDIALOG:
            hTopDlg = hDlg;
            TopDialogInit(hDlg);
            CreateToolTip(hDlg, g_Controls, sizeof(g_Controls) / sizeof(int)); // Initialize tooltip object
            RegisterRemovalCB(vJoyConfChangedCB, (PVOID)hDlg);
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                if (OnClosing()) {
                    EndDialog(hDlg, LOWORD(wParam));
                    PostQuitMessage(0);
                }

                return (INT_PTR)TRUE;
            }

            if ((wParam&0xFFFF) == IDC_RSTALL) {
                OnResetAll(hDlg);
                break;
            }

            // Enable vJoy checkbox
            if ((LOWORD(wParam) == IDC_CHK_ENVJOY) && (HIWORD(wParam) == BN_CLICKED)) {
                Checked = (IsDlgButtonChecked(hDlg, IDC_CHK_ENVJOY) == BST_CHECKED);
                if (Checked)
                    enable(GetvJoyVersion());
                else
                    disable(GetvJoyVersion());
                vJoyIsOn = vJoyEnabled();
            }
            break;


            SendMessage(hDlgTab, WM_COMMAND, wParam, lParam);
            break;


        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->idFrom == IDC_DEVTABS) {
                if (((LPNMHDR)lParam)->code == TCN_SELCHANGING) {
                    SetWindowLongPtr(hDlg, DWLP_MSGRESULT, OnSelChanging(hDlgTab));
                    return  (INT_PTR)TRUE;
                };
                if (((LPNMHDR)lParam)->code == TCN_SELCHANGE)
                    OnSelChanged(hDlgTab);

                return  (INT_PTR)TRUE;
            }

            // Tooltips
            if (((LPNMHDR)lParam)->code == TTN_GETDISPINFO) {
                UpdateToolTip((LPVOID)lParam);
                return  (INT_PTR)TRUE;
            };

            break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for top dialog box.
INT_PTR CALLBACK DlgTabCB(HWND hDlgTab, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    int nButtons = 0;

    switch (message) {
        case WM_INITDIALOG:
            // Place the tab dialog box in the tabs control and display it
            SetWindowPos(hDlgTab, HWND_TOP, 0, 45, 0, 0, SWP_NOSIZE);
            InitializeTab(hDlgTab);
            ShowWindow(hDlgTab, SW_SHOW);
            CreateToolTip(hDlgTab, g_Controls, sizeof(g_Controls) / sizeof(int)); // Initialize tooltip object
            return (INT_PTR)TRUE;

        case WM_NOTIFY:
            // Tooltips
            if (((LPNMHDR)lParam)->code == TTN_GETDISPINFO) {
                UpdateToolTip((LPVOID)lParam);
                return  (INT_PTR)TRUE;
            };

            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_POV_LST && HIWORD(wParam) == LBN_SELCHANGE)
                OnUserChanged();
            if (LOWORD(wParam) == IDC_NBTN && HIWORD(wParam) == EN_CHANGE) {
                TCHAR nBtnStr[5];
                // 	Limit the values of number of buttons
                GetWindowText(GetDlgItem(hDlgTab, IDC_NBTN), nBtnStr, 5);
                _stscanf_s(nBtnStr, TEXT("%d"), &nButtons);
                if (nButtons > 128)
                    SetWindowText(GetDlgItem(hDlgTab, IDC_NBTN), TEXT("128"));
                if (nButtons < 0)
                    SetWindowText(GetDlgItem(hDlgTab, IDC_NBTN), TEXT("0"));

                OnUserChanged();
            };

            switch (wParam&0xFFFF) {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlgTab, LOWORD(wParam));
                    return (INT_PTR)TRUE;

                case IDC_APPLY_BTN:
                    OnApply(hDlgTab);
                    break;

                case IDC_REVT_BTN:
                    OnRevert(hDlgTab, FALSE);
                    break;

                case IDC_ADDDEL_BTN:
                    OnAddDeleteDevice(hDlgTab);
                    break;

                case IDC_DEL:
                    if (!AddBtn)
                        OnAddDeleteDevice(hDlgTab);
                    break;

                case IDC_ADD:
                    if (AddBtn)
                        OnAddDeleteDevice(hDlgTab);
                    break;

                    // Change in FFB checkbox
                case IDC_CHK_FFB:
                    FfbCBChanged();
                    OnUserChanged();
                    break;

                case IDC_CHK_X:
                case IDC_CHK_Y:
                case IDC_CHK_Z:
                case IDC_CHK_RX:
                case IDC_CHK_RY:
                case IDC_CHK_RZ:
                case IDC_CHK_SL0:
                case IDC_CHK_SL1:
                case IDC_CHK_WHEEL:
                case IDC_CHK_ACCELERATOR:
                case IDC_CHK_BRAKE:
                case IDC_CHK_CLUTCH:
                case IDC_CHK_STEERING:
                case IDC_CHK_AILERON:
                case IDC_CHK_RUDDER:
                case IDC_CHK_THROTTLE:

                case IDC_POV_DESC:
                case IDC_POV_CONT:
                case IDC_CHK_FFB_CONST:
                case IDC_CHK_FFB_RAMP:
                case IDC_CHK_FFB_SQUARE:
                case IDC_CHK_FFB_SINE:
                case IDC_CHK_FFB_TRI:
                case IDC_CHK_FFB_STUP:
                case IDC_CHK_FFB_STDN:
                case IDC_CHK_FFB_SPRING:
                case IDC_CHK_FFB_DAMPER:
                case IDC_CHK_FFB_INERTIA:
                case IDC_CHK_FFB_FRICTION:
                    OnUserChanged();
                    break;



                default:
                    break;
            };
            break;
    }
    return (INT_PTR)FALSE;
}

// Initializing the top level dialog box
BOOL TopDialogInit(HWND hDlg)
{
    // Initialize common controls
    INITCOMMONCONTROLSEX InitCtrlEx;
    InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCtrlEx.dwICC = 0 /*ICC_PROGRESS_CLASS | ICC_TAB_CLASSES | ICC_STANDARD_CLASSES*/;
    InitCommonControlsEx(&InitCtrlEx);

    // Locate dialog in the center
    // Collect information about vJoy installation
    // Create Tabs

    TCITEM tie;

    hTabs = GetDlgItem(hDlg, IDC_DEVTABS);
    if (!hTabs)
        return FALSE;

    // Fix white background around tab icon
    // SetWindowTheme(hTabs, L" ", L" ");

    // Create child dialogs and tabs
    tie.mask = /*TCIF_TEXT | TCIF_IMAGE |*/ TCIF_PARAM;
    tie.iImage = -1;

    // Place the tabs
    // Unimplemented devices marked with '[]'
    std::wstring s;
    for (UINT i = VJOY_MAX_N_DEVICES; i; i--) {
#if 0
        if ((vJoyDevice.size() >= i) && (vJoyDevice[i - 1]->Exists())) {
            s = std::to_wstring(i);
            tie.iImage = (i - 1) * 2; // TODO
        } else {
            s = L"[" + std::to_wstring(i) + L"]";
            tie.iImage = (1 + (i - 1) * 2); // TODO
        };

#endif // 0

        tie.pszText = (LPTSTR)s.c_str();
        tie.lParam = i;
        TabCtrl_InsertItem(hTabs, 0, &tie);
    }

    // Add icons
    //TabCtrl_SetPadding(hTabs, 10, 5);
    hImageList = CreateTabsImageList();
    if (hImageList)
        TabCtrl_SetImageList(hTabs, hImageList);


    GetFfbState();
    for (UINT i = VJOY_MAX_N_DEVICES; i; i--)
        RefreshTabState(i);

    // Create the dialog box that is imbedded in the tab control
    hDlgTab = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DEVICE), hTabs, DlgTabCB);

    // Select the first tab (by default)
    // Select the tab (Tab 0)
    TabCtrl_SetCurSel(hTabs, 0);

    GetFfbState();
    OnSelChanged(hDlg);

    // Set "vJoy Enable" checkbox
    if (vJoyIsOn)
        CheckDlgButton(hDlg, IDC_CHK_ENVJOY, BST_CHECKED);  // vJoy Enable checkbox checked
    else
        CheckDlgButton(hDlg, IDC_CHK_ENVJOY, BST_UNCHECKED);  // vJoy Enable checkbox unchecked

    return TRUE;
}

// Currently selected tab is about to change (TCN_SELCHANGING notification)
BOOL OnSelChanging(HWND hDlg)
{
    // If the tab was not changed by the user then it may be left
    if (!Changed)
        return FALSE;

    // The tab was changed by the user
    // Ask the user if to Apply (Yes/No/Cancel)
    int mb_res = MessageBox(NULL, Res2TStr(IDS_I_CHNGNG), Res2TTtl(IDS_T_CHNGNG), MB_YESNOCANCEL | MB_ICONQUESTION);
    if (mb_res == IDYES) {
        OnApply(hDlg);
        return FALSE;
    } else if (mb_res == IDNO) {
        OnRevert(hDlg, TRUE);
        return FALSE;
    } else
        return TRUE;
}

// Called when a tab is selected
void OnSelChanged(HWND hDlg)
{
    WCHAR buf1[MAX_MSG_SIZE] = L"";
    WCHAR buf2[MAX_MSG_SIZE] = L"";
    LPWSTR SerialNunberStr = (LPWSTR)GetvJoySerialNumberString();

    // Get the current selection - extract the ID 
    int iTab = GetCurrentTabIndex();

    if (SerialNunberStr) {
        // Display the device number and version
        LoadStringW(hInst, IDS_DEVNUM, buf1, MAX_MSG_SIZE);
        swprintf_s(buf2, MAX_MSG_SIZE, buf1, iTab + 1, SerialNunberStr);
    } else {
        // Display the device number
        LoadStringW(hInst, IDS_DEVNUM2, buf1, MAX_MSG_SIZE);
        swprintf_s(buf2, MAX_MSG_SIZE, buf1, iTab + 1);
    };

    //DevIdStr = L"vJoy Device: " + std::to_wstring(iTab + 1) + L"\t\t(v" + (LPWSTR)GetvJoySerialNumberString() +L")";
    SetWindowTextW(GetDlgItem(hDlgTab, IDC_DEVNUMBER), buf2);

    // This is a slow function - show a wait cursor
    HCURSOR WaitCursor = LoadCursor(NULL, IDC_WAIT);
    HCURSOR PrevCursor = SetCursor(WaitCursor);


    //ShowWindow(hDlgTab, SW_HIDE);
    //EnableWindow(hDlgTab, FALSE);

    PopulateTab(iTab);
    if (vJoyDevice[iTab]->Exists()) {
        SetWindowText(GetDlgItem(hDlgTab, IDC_STATUS), TEXT(""));
        SetWindowText(GetDlgItem(hDlgTab, IDC_ADDDEL_BTN), Res2TStr(IDS_DELDEV));
        AddBtn = FALSE;
        EnableTab(iTab, TRUE);
        swprintf_s(buf1, MAX_MSG_SIZE, L"");
    } else {
        SetWindowText(GetDlgItem(hDlgTab, IDC_STATUS), Res2TStr(IDS_STAT_NOT_IMP));
        SetWindowText(GetDlgItem(hDlgTab, IDC_ADDDEL_BTN), Res2TStr(IDS_ADDDEV));
        AddBtn = TRUE;
        EnableTab(iTab, FALSE);
        LoadStringW(hInst, IDS_STAT_ADDDEV, buf1, MAX_MSG_SIZE);
    };
    SetWindowTextW(GetDlgItem(hDlgTab, IDC_STATLINE), buf1);



    // If this is the only device then you cannot delete it
    if (isOnlyDevice(iTab+1))
        EnableWindow(GetDlgItem(hDlgTab, IDC_ADDDEL_BTN), FALSE);
    else
        EnableWindow(GetDlgItem(hDlgTab, IDC_ADDDEL_BTN), TRUE);

    // If this is Device 16 and FFB State is Active then you cannot add it
    if ((FfbState == 2) && (iTab == 15))
        EnableWindow(GetDlgItem(hDlgTab, IDC_ADDDEL_BTN), FALSE);

    // Apply button is disabled before startting to make changes
    Changed = FALSE;
    EnableWindow(GetDlgItem(hDlgTab, IDC_APPLY_BTN), Changed);
    EnableWindow(GetDlgItem(hDlgTab, IDC_REVT_BTN), Changed);

    SetCursor(PrevCursor);
    ShowWindow(hDlgTab, SW_SHOW);
}

// Apply button pressed
void OnApply(HWND hDlg)
{
    // Get the current selection - extract the ID
    int iTab = TabCtrl_GetCurSel(hTabs);
    if (iTab<0)
        return;

    TCITEM tie;
    tie.mask = TCIF_PARAM;
    BOOL sTab = TabCtrl_GetItem(hTabs, iTab, &tie);
    if (!sTab)
        return;

    bool AxisChecked[VJOY_NUMBER_OF_AXES] = { 0 };
    int nPovHatsCont = 0, nPovHatsDir = 0, nButtons = 0;
    bool FfbEn = false;

    // Get changed (current) state
    GetCtrlState(hDlg, &AxisChecked[0], &nPovHatsCont, &nPovHatsDir, &nButtons, &FfbEn);

    // Filter out disabled FFB in driver
    BOOL FfbSupported = FALSE;
    vJoyFfbCap(&FfbSupported);
    if (!FfbSupported) {
        FfbEn = FALSE;
    }

    ///////// Create HID Report Descriptor and load it to the registry                  /////////////////
    ///////// We do it twice - once without the FFB section for backwords compatibility ///////////////// 
    ///////// Second time (If FFB enabled) with the FFB section

    // Call external C-function that creats an array of bytes that holds
    // the HID Report Descriptor

    UCHAR** out2 = (UCHAR**)malloc(sizeof(UCHAR*));
    if (!out2)
        return;
    int desc_size = CreateHidReportDesc((void**)out2, nButtons, AxisChecked, nPovHatsCont, nPovHatsDir, (int)(iTab + 1), FfbEn);
    if ((desc_size <= 0)) {
        free(out2);
        MessageBox(NULL, Res2TStr(IDC_FAILED_CREATE), Res2TTtl(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        return;
    };

    // Write descriptor to registry
    WriteHidReportDescToReg((int)(iTab + 1), *out2, desc_size);
    free(out2);

    if (!FfbEn)
        DeleteOEMForceFeedbackFromReg();

    // Refresh
    refresh_vjoy();

    // Disable Apply button
    Changed = FALSE;
    EnableWindow(GetDlgItem(hDlgTab, IDC_APPLY_BTN), Changed);
    EnableWindow(GetDlgItem(hDlgTab, IDC_REVT_BTN), Changed);

    // Refresh the state of the affected tab
    RefreshTabState(iTab + 1);

}

// Revert button pressed
void OnRevert(HWND hDlg, BOOL Quiet)
{
    if (!Quiet) {
        if (MessageBox(NULL, Res2TStr(IDS_RUSURE_REVT), Res2TTtl(IDS_T_RUSURE_REVT), MB_YESNO | MB_ICONQUESTION) == IDNO)
            return;
    };

    // Get the current selection - extract the ID
    int iTab = TabCtrl_GetCurSel(hTabs);
    if (iTab<0)
        return;

    TCITEM tie;
    tie.mask = TCIF_PARAM;
    BOOL sTab = TabCtrl_GetItem(hTabs, iTab, &tie);
    if (!sTab)
        return;

    TabCtrl_SetCurSel(hTabs, iTab);
    OnSelChanged(hDlg);
}

// Button Reset All pressed
// Delete all registry entries
void OnResetAll(HWND hDlg)
{
    if (MessageBox(NULL, Res2TStr(IDS_RST_ALL), Res2TTtl(IDS_RST_T_ALL), MB_YESNO | MB_ICONWARNING) == IDNO)
        return;

    DeleteHidReportDescFromReg(0);

    // Refresh
    is_vjoy_installed_specific(GetvJoyVersion());

    // Select First device tab
    TabCtrl_SetCurSel(hTabs, 0);
    OnSelChanged(hTabs);

    // Disable Apply button
    Changed = FALSE;
    EnableWindow(GetDlgItem(hDlgTab, IDC_APPLY_BTN), Changed);
    EnableWindow(GetDlgItem(hDlgTab, IDC_REVT_BTN), Changed);

    // Refresh statuses of all tabs
    for (int i = VJOY_MAX_N_DEVICES; i; i--)
        RefreshTabState(i);
}


// Called when closing application
// Return TRUE to allow
BOOL OnClosing(void)
{
    // If the tab was not changed by the user then it may be left
    if (!Changed)
        return TRUE;

    // The tab was changed by the user
    // Ask the user if really want to exit (Yes/No)
    int mb_res = MessageBox(NULL, Res2TStr(IDS_I_EXIT), Res2TTtl(IDS_T_EXIT), MB_YESNO | MB_ICONQUESTION);
    if (mb_res == IDYES)
        return TRUE;
    else
        return FALSE;
}

// User made changes to the selected tab
// Need to enable Buttons Apply and Undo
// Need to warn user
void OnUserChanged(void)
{
    Changed = TRUE;
    EnableWindow(GetDlgItem(hDlgTab, IDC_APPLY_BTN), Changed);
    EnableWindow(GetDlgItem(hDlgTab, IDC_REVT_BTN), Changed);

    WCHAR ChangedStr[MAX_MSG_SIZE] = (L"");
    LoadStringW(hInst, IDS_STAT_CHNGD, ChangedStr, MAX_MSG_SIZE);
    SetWindowTextW(GetDlgItem(hDlgTab, IDC_STATLINE), ChangedStr);
}

// Button Add/Delete device pressed
// If device exists then the action is Delete
// If device does not exist then the action is Add
void OnAddDeleteDevice(HWND hDlg)
{
    // Get current device
    // Get the current selection - extract the ID
    int iTab = TabCtrl_GetCurSel(hTabs);
    if (iTab<0)
        return;

    TCITEM tie;
    tie.mask = TCIF_PARAM;
    BOOL sTab = TabCtrl_GetItem(hTabs, iTab, &tie);
    if (!sTab)
        return;

    // Test if it exists and act
    if (vJoyDevice[iTab]->Exists())
        DeleteDevice(iTab + 1);
    else
        AddDevice(iTab + 1);
}


/// Initialize application:
/// Test that vJoy installed and that there is at least one device
/// For every device (Implemented and Unimplemented)
/// + Create device object and reset it
///	+ If device NOT implemented mark as unimplemented
/// + If device Implemented get the implemented controls (Buttons etc)
BOOL Initialize()
{

    // Is vJoy installed?
    if (!vJoyEnabled()) {
        //	MessageBox(NULL, Res2TStr(IDC_NOT_EXIST), Res2TTtl(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        //	return FALSE;
        vJoyIsOn = FALSE;  // vJoy Enable checkbox unchecked
    }

    else {
        // Does DLL match vJoy driver
        WORD DllVer, DrvVer;
        if (!DriverMatch(&DllVer, &DrvVer)) {
            TCHAR msg[250] = { 0 };
            _stprintf_s(msg, Res2TStr(IDC_VER_MISMATCH), DllVer, DrvVer);
            if (IDYES != MessageBox(NULL, msg, Res2TStr(IDS_APP_TITLE), MB_YESNO | MB_ICONWARNING))
                return FALSE;
        };
    }

    // Create a vector device objects
    // Vector item n points at vJoy object with id=n+1
    vJoyDevice.clear();
    for (auto i = 1; i <= VJOY_MAX_N_DEVICES; i++) {
        CvJoyDevice* d = new CvJoyDevice(i);
        vJoyDevice.push_back(d);
    };


    return TRUE;

}

// Enable/Disable tab by tab index (0-based)
void EnableTab(int iTab, BOOL Enable)
{
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_X), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_Y), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_Z), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_RX), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_RY), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_RZ), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_SL0), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_SL1), Enable);

    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_WHEEL), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_ACCELERATOR), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_BRAKE), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_CLUTCH), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_STEERING), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_AILERON), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_RUDDER), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_THROTTLE), Enable);

    EnableWindow(GetDlgItem(hDlgTab, IDC_NBTN), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_POV_DESC), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_POV_CONT), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_POV_LST), Enable);
    EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_FFB), Enable);
}

// Populate tab control according to current set-up
void PopulateTab(int iTab)
{

    BOOL Exist_X = TRUE;
    BOOL Exist_Y = TRUE;
    BOOL Exist_Z = TRUE;
    BOOL Exist_RX = TRUE;
    BOOL Exist_RY = TRUE;
    BOOL Exist_RZ = TRUE;
    BOOL Exist_SL0 = TRUE;
    BOOL Exist_SL1 = TRUE;

    BOOL Exist_Wheel = TRUE;
    BOOL Exist_Accelerator = TRUE;
    BOOL Exist_Brake = TRUE;
    BOOL Exist_Clutch = TRUE;
    BOOL Exist_Steering = TRUE;
    BOOL Exist_Aileron = TRUE;
    BOOL Exist_Rudder = TRUE;
    BOOL Exist_Throttle = TRUE;

    int nBtn = 32;
    int nDiscPov = 0;
    int nContPov = 0;

    if (vJoyDevice[iTab]->Exists()) {
        //// Get data from driver
        // Axes
        Exist_X = vJoyDevice[iTab]->AxisExists(HID_USAGE_X);
        Exist_Y = vJoyDevice[iTab]->AxisExists(HID_USAGE_Y);
        Exist_Z = vJoyDevice[iTab]->AxisExists(HID_USAGE_Z);
        Exist_RX = vJoyDevice[iTab]->AxisExists(HID_USAGE_RX);
        Exist_RY = vJoyDevice[iTab]->AxisExists(HID_USAGE_RY);
        Exist_RZ = vJoyDevice[iTab]->AxisExists(HID_USAGE_RZ);
        Exist_SL0 = vJoyDevice[iTab]->AxisExists(HID_USAGE_SL0);
        Exist_SL1 = vJoyDevice[iTab]->AxisExists(HID_USAGE_SL1);

        Exist_Wheel = vJoyDevice[iTab]->AxisExists(HID_USAGE_WHL);
        Exist_Accelerator = vJoyDevice[iTab]->AxisExists(HID_USAGE_ACCELERATOR);
        Exist_Brake = vJoyDevice[iTab]->AxisExists(HID_USAGE_BRAKE);
        Exist_Clutch = vJoyDevice[iTab]->AxisExists(HID_USAGE_CLUTCH);
        Exist_Steering = vJoyDevice[iTab]->AxisExists(HID_USAGE_STEERING);
        Exist_Aileron = vJoyDevice[iTab]->AxisExists(HID_USAGE_AILERON);
        Exist_Rudder = vJoyDevice[iTab]->AxisExists(HID_USAGE_RUDDER);
        Exist_Throttle = vJoyDevice[iTab]->AxisExists(HID_USAGE_THROTTLE);

        // Buttons
        nBtn = vJoyDevice[iTab]->GetButtonNumber();

        // POV
        nDiscPov = vJoyDevice[iTab]->GetDiscPovNumber();
        nContPov = vJoyDevice[iTab]->GetContPovNumber();
    };

    //// Write data to User Interface
    // Axes
    CheckDlgButton(hDlgTab, IDC_CHK_X, Exist_X);
    CheckDlgButton(hDlgTab, IDC_CHK_Y, Exist_Y);
    CheckDlgButton(hDlgTab, IDC_CHK_Z, Exist_Z);
    CheckDlgButton(hDlgTab, IDC_CHK_RX, Exist_RX);
    CheckDlgButton(hDlgTab, IDC_CHK_RY, Exist_RY);
    CheckDlgButton(hDlgTab, IDC_CHK_RZ, Exist_RZ);
    CheckDlgButton(hDlgTab, IDC_CHK_SL0, Exist_SL0);
    CheckDlgButton(hDlgTab, IDC_CHK_SL1, Exist_SL1);

    CheckDlgButton(hDlgTab, IDC_CHK_WHEEL, Exist_Wheel);
    CheckDlgButton(hDlgTab, IDC_CHK_ACCELERATOR, Exist_Accelerator);
    CheckDlgButton(hDlgTab, IDC_CHK_BRAKE, Exist_Brake);
    CheckDlgButton(hDlgTab, IDC_CHK_CLUTCH, Exist_Clutch);
    CheckDlgButton(hDlgTab, IDC_CHK_STEERING, Exist_Steering);
    CheckDlgButton(hDlgTab, IDC_CHK_AILERON, Exist_Aileron);
    CheckDlgButton(hDlgTab, IDC_CHK_RUDDER, Exist_Rudder);
    CheckDlgButton(hDlgTab, IDC_CHK_THROTTLE, Exist_Throttle);

    // Buttons
    TCHAR nBtnStr[4];
    _stprintf_s(nBtnStr, 4, TEXT("%d"), nBtn);
    SetWindowText(GetDlgItem(hDlgTab, IDC_NBTN), nBtnStr);

    // POV
    if (nDiscPov) {
        SendMessage(GetDlgItem(hDlgTab, IDC_POV_DESC), BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(GetDlgItem(hDlgTab, IDC_POV_CONT), BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hDlgTab, IDC_POV_LST), LB_SETCURSEL, nDiscPov, 0);
    } else {
        SendMessage(GetDlgItem(hDlgTab, IDC_POV_DESC), BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hDlgTab, IDC_POV_CONT), BM_SETCHECK, BST_CHECKED, 0);
        SendMessage(GetDlgItem(hDlgTab, IDC_POV_LST), LB_SETCURSEL, nContPov, 0);
    };

    // FFB
    // Show only for if the device supports FFB 
    // and only for the first 15 tabs
    BOOL FfbSupported = FALSE;
    vJoyFfbCap(&FfbSupported);
    if ((iTab+1) >= MAX_N_FFB || !FfbSupported) {
        //ShowWindow(GetDlgItem(hDlgTab, IDC_FRAME_FFB), SW_HIDE);
        //ShowWindow(GetDlgItem(hDlgTab, IDC_CHK_FFB), SW_HIDE); 
        EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_FFB), FALSE);
        SendMessage(GetDlgItem(hDlgTab, IDC_CHK_FFB), BM_SETCHECK, BST_UNCHECKED, 0);
        ClearFfbEffectCB();
    } else {
        //ShowWindow(GetDlgItem(hDlgTab, IDC_CHK_FFB), SW_SHOW);

        // Show if FFB supported for this device
        if (IsDeviceFfb(iTab + 1))
            SendMessage(GetDlgItem(hDlgTab, IDC_CHK_FFB), BM_SETCHECK, BST_CHECKED, 0);
        else
            SendMessage(GetDlgItem(hDlgTab, IDC_CHK_FFB), BM_SETCHECK, BST_UNCHECKED, 0);

        if (FfbState == 0) // Prohibited
            EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_FFB), FALSE);
        else
            EnableWindow(GetDlgItem(hDlgTab, IDC_CHK_FFB), TRUE);
    };

    // FFB -  Effects
    // If FFB checkbox is checked - Effect checkboxes are enabled
    // If FFB checkbox is unchecked - Effect checkboxes are disabled
    // If Effect Checkboz is enabled then check if effect is configured
    if (vJoyDevice[iTab]->Exists())
        UpdateFfbEffectCB(iTab + 1);
    if (Button_GetCheck(GetDlgItem(hDlgTab, IDC_CHK_FFB)) == BST_CHECKED)
        EnableFfbEffectCB(TRUE);
    else {
        EnableFfbEffectCB(FALSE);
        ClearFfbEffectCB();
    }


    if (((iTab+1) >= MAX_N_FFB) && (FfbState == 2)) // This is Device 16 + FFB State: Active
        EnableTab(iTab, FALSE);

}

void InitializeTab(HWND hTab)
{
    // Initialize number-of-POVs list box
    SendMessage(GetDlgItem(hTab, IDC_POV_LST), LB_INSERTSTRING, -1, (LPARAM)TEXT("0"));
    SendMessage(GetDlgItem(hTab, IDC_POV_LST), LB_INSERTSTRING, -1, (LPARAM)TEXT("1"));
    SendMessage(GetDlgItem(hTab, IDC_POV_LST), LB_INSERTSTRING, -1, (LPARAM)TEXT("2"));
    SendMessage(GetDlgItem(hTab, IDC_POV_LST), LB_INSERTSTRING, -1, (LPARAM)TEXT("3"));
    SendMessage(GetDlgItem(hTab, IDC_POV_LST), LB_INSERTSTRING, -1, (LPARAM)TEXT("4"));
    SendMessage(GetDlgItem(hTab, IDC_POV_LST), LB_SETCURSEL, 0, 0);

    // Select default POV type (IDC_POV_CONT)
    SendMessage(GetDlgItem(hTab, IDC_POV_CONT), BM_SETCHECK, BST_CHECKED, 0);


}

// Read the state of Tab dialog box controls
void GetCtrlState(HWND hDlg, bool* AxisArr, int* nPovCont, int* nPovDisc, int* nButtons, bool* Ffb)
{

    // Axes
    AxisArr[0] = (IsDlgButtonChecked(hDlg, IDC_CHK_X) == BST_CHECKED);
    AxisArr[1] = (IsDlgButtonChecked(hDlg, IDC_CHK_Y) == BST_CHECKED);
    AxisArr[2] = (IsDlgButtonChecked(hDlg, IDC_CHK_Z) == BST_CHECKED);
    AxisArr[3] = (IsDlgButtonChecked(hDlg, IDC_CHK_RX) == BST_CHECKED);
    AxisArr[4] = (IsDlgButtonChecked(hDlg, IDC_CHK_RY) == BST_CHECKED);
    AxisArr[5] = (IsDlgButtonChecked(hDlg, IDC_CHK_RZ) == BST_CHECKED);
    AxisArr[6] = (IsDlgButtonChecked(hDlg, IDC_CHK_SL0) == BST_CHECKED);
    AxisArr[7] = (IsDlgButtonChecked(hDlg, IDC_CHK_SL1) == BST_CHECKED);

    AxisArr[8] = (IsDlgButtonChecked(hDlg, IDC_CHK_WHEEL) == BST_CHECKED);
    AxisArr[9] = (IsDlgButtonChecked(hDlg, IDC_CHK_ACCELERATOR) == BST_CHECKED);
    AxisArr[10] = (IsDlgButtonChecked(hDlg, IDC_CHK_BRAKE) == BST_CHECKED);
    AxisArr[11] = (IsDlgButtonChecked(hDlg, IDC_CHK_CLUTCH) == BST_CHECKED);
    AxisArr[12] = (IsDlgButtonChecked(hDlg, IDC_CHK_STEERING) == BST_CHECKED);
    AxisArr[13] = (IsDlgButtonChecked(hDlg, IDC_CHK_AILERON) == BST_CHECKED);
    AxisArr[14] = (IsDlgButtonChecked(hDlg, IDC_CHK_RUDDER) == BST_CHECKED);
    AxisArr[15] = (IsDlgButtonChecked(hDlg, IDC_CHK_THROTTLE) == BST_CHECKED);

    // Number of buttons
    TCHAR nBtnStr[4];
    GetWindowText(GetDlgItem(hDlg, IDC_NBTN), nBtnStr, 4);
    _stscanf_s(nBtnStr, TEXT("%d"), nButtons);

    // Number of Continuous POVs
    if (IsDlgButtonChecked(hDlg, IDC_POV_CONT) == BST_CHECKED)
        *nPovCont = (int)SendMessage(GetDlgItem(hDlg, IDC_POV_LST), LB_GETCURSEL, 0, 0);
    else
        *nPovCont = 0;

    if (IsDlgButtonChecked(hDlg, IDC_POV_DESC) == BST_CHECKED)
        *nPovDisc = (int)SendMessage(GetDlgItem(hDlg, IDC_POV_LST), LB_GETCURSEL, 0, 0);
    else
        *nPovDisc = 0;

    // Ffb
    *Ffb = (IsDlgButtonChecked(hDlg, IDC_CHK_FFB) == BST_CHECKED);
}

/*
Create an HID Report Descriptor
Inputs:
data:	Pointer to the pointer to array of bytes that will hold the descriptor
buttons: Array of buttons. Every entry represents a button. True = Implemented button.
axes: Array of axes. Every entry represents an axis. True = Implemented axis.
nPovHatsCont: Number of implemented Continuous POV Hat switches
nPovHatsDir: Number of implemented 5-state POV Hat switches

Return:
Positive value: Size of HID Report Descriptor (output buffer) in bytes.

*/
int CreateHidReportDesc(void** data, UINT nButtons, bool* axes, int nPovHatsCont, int nPovHatsDir, BYTE ReportId, bool Ffb)
{
    /* normalize if illegal parameters are passed */
    // Buttons: 0-128
    if (nButtons>VJOY_NUMBER_OF_BUTTONS)
        nButtons = VJOY_NUMBER_OF_BUTTONS;

    /* Special case of zero buttons - Local Min Usage is 0 rather than 1 */
    UINT localminusage_buttons = nButtons ? 0x01 : 0x00;

    // POV (Cont): 0-4
    if (nPovHatsCont<0)
        nPovHatsCont = 0;
    if (nPovHatsCont>VJOY_NUMBER_OF_HAT)
        nPovHatsCont = VJOY_NUMBER_OF_HAT;
    // POV (Dir): 0-4
    if (nPovHatsDir<0)
        nPovHatsDir = 0;
    if (nPovHatsDir>VJOY_NUMBER_OF_HAT)
        nPovHatsDir = VJOY_NUMBER_OF_HAT;

    BYTE AxesHID[] {
        HID_USAGE_X,
        HID_USAGE_Y,
        HID_USAGE_Z,
        HID_USAGE_RX,
        HID_USAGE_RY,
        HID_USAGE_RZ,
        HID_USAGE_SL0,
        HID_USAGE_SL1,
        HID_USAGE_WHL,
        HID_USAGE_ACCELERATOR,
        HID_USAGE_BRAKE,
        HID_USAGE_CLUTCH,
        HID_USAGE_STEERING,
        HID_USAGE_AILERON,
        HID_USAGE_RUDDER,
        HID_USAGE_THROTTLE,
    };

    /* Create a data buffer large enough to hold the resulting HID Report Descriptor */
    //int buffer_size = 1023;
    static std::vector<BYTE> buffer;
    buffer.clear();

    /* Create standard header */
    NEXT_BYTE(buffer, HIDP_GLOBAL_USAGE_PAGE_1);     // USAGE_PAGE(Generic Desktop):		05 01
    NEXT_BYTE(buffer, HID_USAGE_PAGE_GENERIC);
    NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1);			// LOGICAL_MINIMUM(0):				15 00
    NEXT_BYTE(buffer, 0);
    NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1);			// USAGE (Joystick):				09 04
    NEXT_BYTE(buffer, HID_USAGE_GENERIC_JOYSTICK);
    NEXT_BYTE(buffer, HIDP_MAIN_COLLECTION);			// COLLECTION( Application):		A1 01
    NEXT_BYTE(buffer, HIDP_MAIN_COLLECTION_APP);

    /* Collection 1 */
    NEXT_BYTE(buffer, HIDP_GLOBAL_USAGE_PAGE_1);		// USAGE_PAGE(Generic Desktop):		05 01
    NEXT_BYTE(buffer, HID_USAGE_PAGE_GENERIC);
    NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_ID);			//	REPORT_ID (x)					85 ID
    NEXT_BYTE(buffer, ReportId);
    NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1);			// USAGE(Pointer):					09 01
    NEXT_BYTE(buffer, HID_USAGE_GENERIC_POINTER);
    NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1);			// LOGICAL_MINIMUM(0):				15 00
    NEXT_BYTE(buffer, 0);
    NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_2);			// LOGICAL_MAXIMUM(32767):			26 FF 7F
    NEXT_SHORT(buffer, VJOY_AXIS_MAX_VALUE);
    NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE);		// REPORT_SIZE(32):					75 20
    NEXT_BYTE(buffer, 0x20);
    NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(1):					95 01
    NEXT_BYTE(buffer, 0x01);
    NEXT_BYTE(buffer, HIDP_MAIN_COLLECTION);			// COLLECTION(Physical):			A1 00
    NEXT_BYTE(buffer, 0x00);

    /** Collection 2 **/
    /* Loop on fitst Axes */
    for (int i = 0; i<VJOY_NUMBER_OF_AXES; i++) {
        if (axes[i]) {
            // Insert Axis
            NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1);	// USAGE(X+offset):					0x09 0x30+i
            NEXT_BYTE(buffer, AxesHID[i]);
            NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);		// INPUT (Data,Var,Abs):			0x81 0x02
            NEXT_BYTE(buffer, 0x02);
        } else {
            // Insert place holder
            NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);		// INPUT (Cnst,Ary,Abs):			0x81 0x01
            NEXT_BYTE(buffer, 0x01);
        }
    }
    NEXT_BYTE(buffer, HIDP_MAIN_ENDCOLLECTION);		// END_COLLECTION:					0xC0

    if (nPovHatsDir) {
        // POV - supported: One switch at most, 5-state only
        NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1);			// LOGICAL_MINIMUM(0):		15 00
        NEXT_BYTE(buffer, 0x00);
        NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_1);			// LOGICAL_MAXIMUM(3):		25 03
        NEXT_BYTE(buffer, 0x03);
        NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MIN_1);			// PHYSICAL_MINIMUM (0):	35 00
        NEXT_BYTE(buffer, 0x00);
        NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MAX_2);			// PHYSICAL_MAXIMUM (270):	46 0e 01
        NEXT_SHORT(buffer, 0x010e);
        NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_1);			// UNIT (Eng Rot:Angular Pos):	65 14
        NEXT_BYTE(buffer, 0x14);
        // One 4-bit data  + 31 4-bit padding
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE);		// REPORT_SIZE(4):			75 04
        NEXT_BYTE(buffer, 0x04);
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(1):			95 01
        NEXT_BYTE(buffer, 0x01);

        // Insert 1-4 5-state POVs
        for (int i = 1; i <= nPovHatsDir; i++) {
            NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1);			// USAGE(Hat switch):		0x09 0x39
            NEXT_BYTE(buffer, HID_USAGE_GENERIC_HATSWITCH);
            NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);				// INPUT (Data,Var,Abs):	0x81 0x02
            NEXT_BYTE(buffer, 0x02);
        }

        // Insert 5-state POV place holders
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(31):		95 1F
        NEXT_BYTE(buffer, 0x20 - nPovHatsDir);
        NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);				// INPUT (Cnst,Ary,Abs):	0x81 0x01
        NEXT_BYTE(buffer, 0x01);

    } else if (nPovHatsCont) {
        // Continuous POV
        NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1);			// LOGICAL_MINIMUM(0):		15 00
        NEXT_BYTE(buffer, 0x00);
        NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_4);			// LOGICAL_MAXIMUM(35900):	27 3c 8c 00 00
        NEXT_SHORT(buffer, 0x8C3C);
        NEXT_SHORT(buffer, 0x0000);
        NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MIN_1);			// PHYSICAL_MINIMUM (0):	35 00
        NEXT_BYTE(buffer, 0x00);
        NEXT_BYTE(buffer, HIDP_GLOBAL_PHY_MAX_4);			// PHYSICAL_MAXIMUM (35900):	47 3c 8c 00 00
        NEXT_SHORT(buffer, 0x8C3C);
        NEXT_SHORT(buffer, 0x0000);
        NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_1);			// UNIT (Eng Rot:Angular Pos):	65 14
        NEXT_BYTE(buffer, 0x14);
        //
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE);		// REPORT_SIZE(32):			75 20
        NEXT_BYTE(buffer, 0x20);
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(1):			95 01
        NEXT_BYTE(buffer, 0x01);

        // Insert 1-4 continuous POVs
        for (int i = 1; i <= nPovHatsCont; i++) {
            NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_1);			// USAGE(Hat switch):		0x09 0x39
            NEXT_BYTE(buffer, HID_USAGE_GENERIC_HATSWITCH);
            NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);				// INPUT (Data,Var,Abs):	0x81 0x02
            NEXT_BYTE(buffer, 0x02);
        }

        // Insert 1-3 continuous POV place holders
        if (nPovHatsCont<4) {
            NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(3):		95 03
            NEXT_BYTE(buffer, 0x04 - nPovHatsCont);
            NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);				// INPUT (Cnst,Ary,Abs):	0x81 0x01
            NEXT_BYTE(buffer, 0x01);
        }
    } else {
        // Sixteen 4-bit padding
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE);		// REPORT_SIZE(32):			75 20
        NEXT_BYTE(buffer, 0x20);
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(4):			95 04
        NEXT_BYTE(buffer, 0x04);
        NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);				// INPUT (Cnst,Ary,Abs):	0x81 0x01
        NEXT_BYTE(buffer, 0x01);
    }


    // Buttons - up to 32 buttons supported. Only the NUMBER of buttons can be set
    //int nButtons = 0;
    //for (int i=0; i<MAX_BUTTONS; i++)
    //	if (buttons[i]) 
    //		nButtons++;

    // There are buttons
    NEXT_BYTE(buffer, HIDP_GLOBAL_USAGE_PAGE_1);		// USAGE_PAGE(Buttons):		05 09
    NEXT_BYTE(buffer, HID_USAGE_PAGE_BUTTON);
    NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MIN_1); 		// LOGICAL_MINIMUM(0):		15 00
    NEXT_BYTE(buffer, 0x00);
    NEXT_BYTE(buffer, HIDP_GLOBAL_LOG_MAX_1);			// LOGICAL_MAXIMUM(0):		25 01
    NEXT_BYTE(buffer, 0x01);
    NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_EXP_1);		// UNIT_EXPONENT(0):		55 00
    NEXT_BYTE(buffer, 0x00);
    NEXT_BYTE(buffer, HIDP_GLOBAL_UNIT_1);			// UNIT (None):				65 00
    NEXT_BYTE(buffer, 0x00);
    NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_MIN_1);		// USAGE_MINIMUM(1):		19 01/00
    NEXT_BYTE(buffer, localminusage_buttons);
    NEXT_BYTE(buffer, HIDP_LOCAL_USAGE_MAX_1);		// USAGE_MAXIMUM(nButtons):	29 nButtons
    NEXT_BYTE(buffer, nButtons);
    NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE);		// REPORT_SIZE(1):			75 01
    NEXT_BYTE(buffer, 0x01);
    NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(nButtons):	95 nButtons
    NEXT_BYTE(buffer, nButtons);
    NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);				// INPUT (Data,Var,Abs):	0x81 0x02
    NEXT_BYTE(buffer, 0x02);

    // Padding, if there are less than 32 buttons
    if (nButtons < VJOY_NUMBER_OF_BUTTONS) {
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_SIZE);	// REPORT_SIZE(x):		75 32-nButtons
        NEXT_BYTE(buffer, VJOY_NUMBER_OF_BUTTONS - nButtons);
        NEXT_BYTE(buffer, HIDP_GLOBAL_REPORT_COUNT_1);	// REPORT_COUNT(1):	95 nButtons
        NEXT_BYTE(buffer, 0x01);
        NEXT_BYTE(buffer, HIDP_MAIN_INPUT_1);			// INPUT (Cnst,Ary,Abs):0x81 0x01
        NEXT_BYTE(buffer, 0x01);
    }


    // Insert FFB section to the descriptor if the user chose to
    if (Ffb) {
        CreateFfbDesc(&buffer, ReportId);
        UINT16 mask = GetFfbEffectMask();
        ModifyFfbEffectDesc(&buffer, mask);
    }

    NEXT_BYTE(buffer, HIDP_MAIN_ENDCOLLECTION);			// END_COLLECTION:					0xC0

    UCHAR* orig = &buffer[0];
    *data = (void**)orig;

    return (int)buffer.size();

}


/*
Write the HID Report Descriptor to the registry
Key:	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\vjoy\Parameters\Devicexx
Name:	HidReportDescriptor
Type:	REG_BINARY
*/
void WriteHidReportDescToReg(int target, UCHAR* Descriptor, int size)
{
    /* Open registry - Most of the path should exist */
    HKEY hParams, hDevDef;
    LONG RegRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PARAM, 0, KEY_ALL_ACCESS, &hParams);
    if (RegRes != ERROR_SUCCESS)
        return;	// Error

    LPCWSTR lpRegParam;
    // Create target key string from target number
    WCHAR RegParam[sizeof(REG_PARAM_DEV) + 3] = { 0 };
    swprintf_s(RegParam, sizeof(REG_PARAM_DEV) + 3, REG_PARAM_DEV  L"%02d", target);
    lpRegParam = RegParam;

    // Create Key
    RegRes = RegCreateKey(HKEY_LOCAL_MACHINE, lpRegParam, &hDevDef);
    if (RegRes != ERROR_SUCCESS) {
        RegCloseKey(hParams);
        return;	// Error
    };

    /* Write the data */
    RegRes = RegSetValueEx(hDevDef, DESC_NAME, 0, REG_BINARY, Descriptor, size);
    if (RegRes != ERROR_SUCCESS) {
        RegCloseKey(hParams);
        RegCloseKey(hDevDef);
        return;	// Error
    };

    /* Write the data Size*/
    DWORD dSize = (DWORD)size;
    RegSetValueEx(hDevDef, DESC_SIZE, 0, REG_DWORD, (const BYTE*)(&dSize), 4);
    RegCloseKey(hParams);
    RegCloseKey(hDevDef);
}

/*
    Delete the HID Report Descriptor from the registry Key:
    HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\vjoy\Parameters\Device0
*/
void DeleteHidReportDescFromReg(int target)
{
    /* Open registry - Most of the path should exist */
    HKEY hParams;
    LONG RegRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_PARAM, 0, KEY_ALL_ACCESS, &hParams);
    if (RegRes != ERROR_SUCCESS)
        return;	// Error

    // Determine the key(s) to delete
    int top = 16, bottom = 1;
    if (target<0 || target>16)
        return;
    if (target)
        top = bottom = target;

    for (int i = bottom; i <= top; i++) {
        // Create target key string from target number
        WCHAR RegParam[sizeof(REG_PARAM_DEV) + 3] = { 0 };
        swprintf_s(RegParam, sizeof(REG_PARAM_DEV) + 3, REG_PARAM_DEV  L"%02d", i);
        LPCWSTR lpRegParam = RegParam;

        // Delete
        if (lpRegParam) {
            auto stt = RegDeleteTree(HKEY_LOCAL_MACHINE, lpRegParam);
            if (stt != ERROR_SUCCESS) {
                auto msg = "Failed with error" + std::to_string(stt);
                perror(msg.c_str());
            }
        }
    }

    RegCloseKey(hParams);
    return;
}

/*
    Delete OEMForceFeedback key from the registry
    Key:	HKEY_CURRENT_USER\\SYSTEM\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Joystick\\OEM\\VID_1234&PID_0FFB"
    Other interesting places to clean-up:
            HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\MediaProperties\PrivateProperties\Joystick\OEM\VID_1234&PID_0FFB
            HKEY_LOCAL_MACHINE\SYSTEM\Setup\Upgrade\PnP\CurrentControlSet\Control\DeviceMigration\Devices\HID\HIDCLASS
*/
void DeleteOEMForceFeedbackFromReg()
{
    /* Open registry - Most of the path should exist */
    HKEY hParams;
    LONG RegRes = RegOpenKeyEx(HKEY_CURRENT_USER, REG_OEMFORCEFEEDBACK_0, 0, DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE, &hParams);
    if (RegRes == ERROR_SUCCESS) {
        // Delete Tree
        RegRes = RegDeleteTree(hParams, NULL);
        RegCloseKey(hParams);
    }

    RegRes = RegOpenKeyEx(HKEY_CURRENT_USER, REG_OEMFORCEFEEDBACK_1, 0, DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE, &hParams);
    if (RegRes == ERROR_SUCCESS) {
        // Delete Tree
        RegRes = RegDeleteTree(hParams, NULL);
        RegCloseKey(hParams);
    }

    RegRes = RegOpenKeyEx(HKEY_CURRENT_USER, REG_OEMFORCEFEEDBACK_2, 0, DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE, &hParams);
    if (RegRes == ERROR_SUCCESS) {
        // Delete Tree
        RegRes = RegDeleteTree(hParams, NULL);
        RegCloseKey(hParams);
    }
}

// Tests if Id is the only vJoy device implemented
// Retuns TRUE if:
//  1. Device Id is implemented
//	2. No other device is implemented
BOOL isOnlyDevice(UINT Id)
{
    if (!vJoyDevice[Id - 1]->Exists())
        return FALSE;

    UINT i = 0;
    while (i < vJoyDevice.size()) {
        i++;
        if (i == Id)
            continue;
        if (vJoyDevice[i-1]->Exists())
            return FALSE;
    };
    return TRUE;
}

// Delete existing device
void DeleteDevice(int Id)
{
    if (MessageBox(NULL, Res2TStr(IDS_DEL_DEV), Res2TTtl(IDS_DEL_T_DEV), MB_YESNO | MB_ICONWARNING) == IDNO)
        return;

    // Delete from registry
    DeleteHidReportDescFromReg(Id);

    // Refresh
    is_vjoy_installed_specific(GetvJoyVersion());

    // Refresh the state of the affected tab
    RefreshTabState(Id);

    // Refresh Tab
    OnSelChanged(hDlgTab);

}

// Add a device
void AddDevice(int Id)
{
    // Apply default configuration
    OnApply(hDlgTab);

    // Refresh all tab icons
    for (int i = VJOY_MAX_N_DEVICES; i; i--)
        RefreshTabState(i);

    // Refresh Tab
    OnSelChanged(hDlgTab);
}

// Return the string related to the resource string ID
LPTSTR Res2TStr(UINT uId)
{
    int res = LoadString(hInst, uId, g_msg, MAX_LOADSTRING);
    if (!res)
        return TEXT("");
    return g_msg;
}

// Return the string related to the resource string ID (Use it for titles)
LPTSTR Res2TTtl(UINT uId)
{
    int res = LoadString(hInst, uId, g_ttl, MAX_LOADSTRING);
    if (!res)
        return TEXT("");
    return g_ttl;
}

// Create one central  Tooltip object
HWND CreateToolTip(HWND hDlg, const int arr[], int size)
{
    if (!hDlg || !g_hInstance)
        return (HWND)NULL;

    // Create the tooltip.
    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                                  WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON | WS_EX_TOOLWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  hDlg, NULL,
                                  g_hInstance, NULL);

    if (!hwndTip)
        hwndToolTip = (HWND)NULL;
    else
        hwndToolTip = hwndTip;


    // Initializing Tooltip per control
    if (hwndToolTip) {
        // General initialization
        TOOLINFO toolInfo = { 0 };
        toolInfo.cbSize = TTTOOLINFO_V1_SIZE;
        toolInfo.hwnd = hDlg;
        toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        toolInfo.lpszText = LPSTR_TEXTCALLBACK;

        // Loop on all controls that require tooltip
        vControls.assign(arr, arr + size);
        for (auto ctrl : vControls) {
            HWND hwndTool = GetDlgItem(hDlg, ctrl);
            toolInfo.uId = (UINT_PTR)hwndTool;
            SendMessage(hwndToolTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
            SendMessage(hwndToolTip, TTM_ACTIVATE, TRUE, (LPARAM)&toolInfo);
        };
    }

    return hwndToolTip;
}


// Display callback-type tooltip
// lpttt:	Pointer to tooltip structure
// TxtID:	The resource ID of the text to display
// TitleID:	The resource ID of the title - default is "no title"
// Icon:	Icon to display. Possible values are TTI_NONE (default), TTI_INFO, TTI_WARNING, TTI_ERROR
void DisplayToolTip(LPNMTTDISPINFO lpttt, int TxtID, int TitleID, int Icon)
{
    TCHAR ControlText[MAX_MSG_SIZE] = { 0 };
    TCHAR TitleText[MAX_MSG_SIZE] = { 0 };

    LoadString(g_hInstance, TxtID, ControlText, MAX_MSG_SIZE);
    lpttt->lpszText = ControlText;

    if (TitleID<0)
        SendMessage(hwndToolTip, TTM_SETTITLE, TTI_NONE, (LPARAM)TEXT(""));
    else {
        LoadString(g_hInstance, TitleID, TitleText, MAX_MSG_SIZE);
        SendMessage(hwndToolTip, TTM_SETTITLE, Icon, (LPARAM)TitleText);
    };
}

/*
Called every time mouse hovers over a control that was previously registered for tool tip
Registration was done in CreateToolTip()
The Control ID (CtrlId) of the control is extracted from the input 'param'
The correct text is displayed according to the Control ID
*/
void UpdateToolTip(LPVOID param)
{
    LPNMTTDISPINFO lpttt = (LPNMTTDISPINFO)param;
    //TCHAR ControlText[MAX_MSG_SIZE] = { 0 };
    //TCHAR TitleText[MAX_MSG_SIZE] = { 0 };
    //int ControlTextSize = 0;

    // Since the id field of the control in the tooltip was defined as a handle - it has to be converted back
    int CtrlId = GetDlgCtrlID((HWND)lpttt->hdr.idFrom);

    // Handle to the tooltip window
    lpttt->hdr.hwndFrom;

    switch (CtrlId) // Per-control tooltips
    {
        case IDC_CHK_X:
            DisplayToolTip(lpttt, IDS_I_CHK_X, NULL, TTI_NONE);
            break;

        case IDC_CHK_Y:
            DisplayToolTip(lpttt, IDS_I_CHK_Y, NULL, TTI_NONE);
            break;

        case IDC_CHK_Z:
            DisplayToolTip(lpttt, IDS_I_CHK_Z, NULL, TTI_NONE);
            break;

        case IDC_CHK_RX:
            DisplayToolTip(lpttt, IDS_I_CHK_RX, NULL, TTI_NONE);
            break;

        case IDC_CHK_RY:
            DisplayToolTip(lpttt, IDS_I_CHK_RY, NULL, TTI_NONE);
            break;

        case IDC_CHK_RZ:
            DisplayToolTip(lpttt, IDS_I_CHK_RZ, NULL, TTI_NONE);
            break;

        case IDC_CHK_SL0:
            DisplayToolTip(lpttt, IDS_I_CHK_SL0, NULL, TTI_NONE);
            break;

        case IDC_CHK_SL1:
            DisplayToolTip(lpttt, IDS_I_CHK_SL1, NULL, TTI_NONE);
            break;

        case IDC_NBTN:
            DisplayToolTip(lpttt, IDS_I_NBTN, IDS_T_NBTN, TTI_NONE);
            break;

        case IDC_POV_DESC:
            DisplayToolTip(lpttt, IDS_I_POV_DESC, IDS_T_POV, TTI_NONE);
            break;

        case IDC_POV_CONT:
            DisplayToolTip(lpttt, IDS_I_POV_CONT, IDS_T_POV, TTI_NONE);
            break;

        case IDC_POV_LST:
            DisplayToolTip(lpttt, IDS_I_POV_LST, NULL, TTI_NONE);
            break;

        case IDC_ADDDEL_BTN:
            if (AddBtn)
                DisplayToolTip(lpttt, IDS_I_ADD_BTN, NULL, TTI_NONE);
            else
                DisplayToolTip(lpttt, IDS_I_DEL_BTN, NULL, TTI_NONE);
            break;

        case IDC_APPLY_BTN:
            DisplayToolTip(lpttt, IDS_I_APPLY_BTN, IDS_T_APPLY_BTN, TTI_NONE);
            break;

        case IDC_REVT_BTN:
            DisplayToolTip(lpttt, IDS_I_REVT_BTN, IDS_T_REVT_BTN, TTI_NONE);
            break;

        case IDC_RSTALL:
            DisplayToolTip(lpttt, IDS_I_RSTALL, IDS_T_RSTALL, TTI_NONE);
            break;

        case IDC_CHK_ENVJOY: // TODO: Implement with condition (vJoyIsOn)
            if (vJoyIsOn)
                DisplayToolTip(lpttt, IDS_I_CHK_DISVJOY, IDS_T_CHK_ENVJOY, TTI_NONE);
            else
                DisplayToolTip(lpttt, IDS_I_CHK_ENVJOY, IDS_T_CHK_ENVJOY, TTI_NONE);


        default:
            //DisplayToolTip(lpttt, IDS_W_NOT_IMP, L"OOOPS", TTI_WARNING);
            break;
    }

}

// Create a list of immages to be used by tab control
HIMAGELIST CreateTabsImageList(void)
{
    HICON icon;
    ICONINFO iconinf;
    HIMAGELIST hList;
    HBITMAP bitmap;

    int icons[] =
    {
        IDI_1_ON, IDI_1_OFF, IDI_2_ON, IDI_2_OFF, IDI_3_ON, IDI_3_OFF, IDI_4_ON, IDI_4_OFF,
        IDI_5_ON, IDI_5_OFF, IDI_6_ON, IDI_6_OFF, IDI_7_ON, IDI_7_OFF, IDI_8_ON, IDI_8_OFF,
        IDI_9_ON, IDI_9_OFF, IDI_10_ON, IDI_10_OFF, IDI_11_ON, IDI_11_OFF, IDI_12_ON, IDI_12_OFF,
        IDI_13_ON, IDI_13_OFF, IDI_14_ON, IDI_14_OFF, IDI_15_ON, IDI_15_OFF, IDI_16_ON, IDI_16_OFF, IDI_16_DEL
    };
    int iLast = sizeof(icons) / sizeof(int);

    // Creat a list of 16x16 images that may contain up to 4 icons
    hList = ImageList_Create(16, 16, ILC_COLOR32, iLast, 0); // 
    if (!hList)
        return hList;

    // Load icons to the limmage list
    for (int i = 0; i<iLast; ++i) {
        icon = reinterpret_cast<HICON>(LoadImage(g_hInstance, MAKEINTRESOURCE(icons[i]), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT));
        if (!icon)
            continue;
        GetIconInfo(icon, &iconinf);
        bitmap = iconinf.hbmColor;
        ImageList_Add(hList, bitmap, NULL);
        DestroyIcon(icon);
    };

    return hList;
}

// Refresh state of tab
// - Change Tab Icon
void RefreshTabState(int Id)
{
    TCITEM tie;

    // Create child dialogs and tabs
    tie.mask = TCIF_IMAGE;
    tie.iImage = -1;

    // Special case - Tab #16  +  Active FFB state
    if ((Id == 16) &&   (FfbState == 2))
        tie.iImage = 32;

    // All other tabs
    else {

        if (vJoyDevice[Id - 1]->Exists()) {
            tie.iImage = (Id - 1) * 2; // TODO
        } else {
            tie.iImage = (1 + (Id - 1) * 2); // TODO
        };
    };

    TabCtrl_SetItem(hTabs, Id - 1, &tie);
}

// Test if this application can run on this platform
BOOL isBitnessCompat(BOOL Quiet)
{
    SYSTEM_INFO info;
    GetNativeSystemInfo(&info);

    if ((info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) && x86) {
        if (!Quiet)
            MessageBox(NULL, Res2TStr(IDS_I_FOR32), Res2TTtl(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        return FALSE;
    };

    if ((info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) && !x86) {
        if (!Quiet)
            MessageBox(NULL, Res2TStr(IDS_I_FOR64), Res2TTtl(IDS_APP_TITLE), MB_OK | MB_ICONERROR);
        return FALSE;
    };

    return TRUE;
}

void CALLBACK vJoyConfChangedCB(BOOL Removed, BOOL First, PVOID data)
{
    HWND hDlg = (HWND)data;
    PostMessage(hDlg, WM_VJOYCHANGED, (WPARAM)Removed, (LPARAM)First);
}

// This function is called when the first vJoy device is removed
// Start an hourglass wait period
// It should invalidate all tabs
// Disable cantrolls in the selected tab
void vJoyDeviceRemoved(void)
{
    TCITEM tie;

    // TODO - Start an hourglass wait period

    // Change all tab icons to inactive
    tie.mask = TCIF_IMAGE;
    tie.iImage = -1;
    for (int Id = VJOY_MAX_N_DEVICES; Id; Id--) {
        tie.iImage = (1 + (Id - 1) * 2);
        TabCtrl_SetItem(hTabs, Id - 1, &tie);
    };

    // Disable controls of currently selected tab and of top dialog box
    EnableTab(1, FALSE);
    EnableWindow(GetDlgItem(hDlgTab, IDC_ADDDEL_BTN), FALSE);
    EnableWindow(GetDlgItem(hTopDlg, IDC_RSTALL), FALSE);

}

// This function is called for every arriving device
// It resets the refresh-timer to 1000mSec
// When the timer finally expires - a refresh function is called and the hourglass wait period expires
void vJoyDeviceArrived(void)
{
    SetTimer(hTopDlg, TIMER_ID, 1000, RefreshAll);
}

// Callback function for arrival timer - will be called only once
// Recalculate the state of each device - updating the tab icons.
// Recalculate the state of the controls for the selected tab
// Enable all controls (If device enabled)
VOID CALLBACK RefreshAll(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    // Ignore all other timers
    if (idEvent != TIMER_ID)
        return;

    // Calculate the FFB State
    GetFfbState();

    // Enable  controls of the top dialog box
    EnableWindow(GetDlgItem(hTopDlg, IDC_RSTALL), TRUE);

    // Update the selected tab
    OnSelChanged(hTabs);


    // Update tab icons
    for (int i = VJOY_MAX_N_DEVICES; i; i--)
        RefreshTabState(i);

    KillTimer(hwnd, idEvent);
    return;
}

// Returns the index (0-15) of the current selected Tab
int GetCurrentTabIndex(void)
{
    // Get the current selection
    int iTab = TabCtrl_GetCurSel(hTabs);
    if (iTab<0)
        return 0;

    //TCITEM tie;
    //tie.mask = TCIF_PARAM;
    //BOOL sTab = TabCtrl_GetItem(hTabs, iTab, &tie);
    //if (!sTab)
    //	return 0;

    return iTab;
}


// Appends the FFB section of the descriptor to a given buff
// The FFB section is given as a global array of bytes
void CreateFfbDesc(std::vector<BYTE>* buffer, BYTE ReportId)
{

    // Vector of bytes depending on the ID
    std::vector<BYTE> vars
    {
        //static_cast<BYTE>(HID_ID_STATE + 0x10 * ReportId),      //    Report ID 2
        static_cast<BYTE>(HID_ID_EFFREP + 0x10 * ReportId),     //    Report ID 1
        static_cast<BYTE>(HID_ID_ENVREP + 0x10 * ReportId),     //    Report ID 2
        static_cast<BYTE>(HID_ID_CONDREP + 0x10 * ReportId),    //    Report ID 3
        static_cast<BYTE>(HID_ID_PRIDREP + 0x10 * ReportId),    //    Report ID 4
        static_cast<BYTE>(HID_ID_CONSTREP + 0x10 * ReportId),   //    Report ID 5
        static_cast<BYTE>(HID_ID_RAMPREP + 0x10 * ReportId),    //    Report ID 6
        static_cast<BYTE>(HID_ID_CSTMREP + 0x10 * ReportId),    //    Report ID 7
        static_cast<BYTE>(HID_ID_SMPLREP + 0x10 * ReportId),    //    Report ID 8
        static_cast<BYTE>(HID_ID_EFOPREP + 0x10 * ReportId),    //    Report ID Ah (10d)
        static_cast<BYTE>(HID_ID_BLKFRREP + 0x10 * ReportId),   //    Report ID Bh (11d)
        static_cast<BYTE>(HID_ID_CTRLREP + 0x10 * ReportId),    //    Report ID Ch (12d)
        static_cast<BYTE>(HID_ID_GAINREP + 0x10 * ReportId),    //    Report ID Dh (13d)
        static_cast<BYTE>(HID_ID_SETCREP + 0x10 * ReportId),    //    Report ID Eh (14d)
        static_cast<BYTE>(HID_ID_NEWEFREP + 0x10 * ReportId),   //    Report ID 1
        static_cast<BYTE>(HID_ID_BLKLDREP + 0x10 * ReportId),   //    Report ID 2
        static_cast<BYTE>(HID_ID_POOLREP + 0x10 * ReportId),    //    Report ID 3
        static_cast<BYTE>(HID_ID_STATEREP + 0x10 * ReportId),   //    Report ID 4
    };

    // Replace the first byte of each sub vector with the corresponding varible (exclude first sub vector)
    // Append modified sub vector to buffer
    buffer->insert(buffer->end(), FfbDescriptor[0].begin(), FfbDescriptor[0].end());
    for (UINT i = 1; i < FfbDescriptor.size(); i++) {
        FfbDescriptor[i][0] = vars[i - 1];
        buffer->insert(buffer->end(), FfbDescriptor[i].begin(), FfbDescriptor[i].end());
    }
}

// Modify the Descriptor according to the FFB Effects the user selects
// Parameters:
// Buffer: Descriptor to be modified
// Mask: Bit-mask representing the effects required
void ModifyFfbEffectDesc(std::vector<BYTE>* buffer, UINT16 Mask)
{
    int Effect[] { HID_USAGE_CONST,
        HID_USAGE_RAMP,
        HID_USAGE_SQUR,
        HID_USAGE_SINE,
        HID_USAGE_TRNG,
        HID_USAGE_STUP,
        HID_USAGE_STDN,
        HID_USAGE_SPRNG,
        HID_USAGE_DMPR,
        HID_USAGE_INRT,
        HID_USAGE_FRIC
    };
    BYTE nEff = sizeof(Effect) / sizeof(int);

    // Search for sequence(0x09, 0x25, 0xA1, 0x02)
    for (auto& i : *buffer) {
        if ((i == 0x09) && (*std::next(&i, 1) == 0x25) && (*std::next(&i, 2) == 0xA1) && (*std::next(&i, 3) == 0x02)) {
            // Sequence found - now replace by going over the effects 
            for (BYTE e = 0; e < nEff; e++) {
                if ((Mask >> (nEff-e-1)) & 0x01) {
                    // Fill all effects types
                    *std::next(&i, 5 + e * 2) = Effect[e];
                } else {
                    // Fill unused effect (reserved)
                    *std::next(&i, 5 + e * 2) = HID_USAGE_RESERVD;
                }
            }
        }
    }
}

//// Calculate FFB State
//
// Updates Global Variable FfbState
// int FfbState = -1; // 0: Prohibited; 1: Inactive; 2: Active
//
// If device #16 exists (or FFB not supported) - FFB State == Prohibited
// If device #16 does not exist and none of the other devices is FFB - FFB State == Inactive
// If device #16 does not exist and one or more of the other devices is FFB - FFB State == Active
int GetFfbState(void)
{
    BOOL FfbSupported = FALSE;
    vJoyFfbCap(&FfbSupported);
    if (!FfbSupported) {
        FfbState = 0;
        return 	FfbState;
    };

    // Test if device 16 exists
    int Id = 16;
    if (vJoyDevice[Id - 1]->Exists()) {
        FfbState = 0;
        return 	FfbState;
    };

    // Test if any of the lower 15 devices supports FFB
    for (int id = 1; id <= MAX_N_FFB; id++) {
        if (IsDeviceFfb(id)) {
            FfbState = 2;
            return 	FfbState;
        };
    }
    FfbState = 1;
    return 	FfbState;
}

// Enable/Disable all Checkboxes related to FFB Effects
void EnableFfbEffectCB(BOOL Enable)
{
    int cb[]
    {
        IDC_CHK_FFB_CONST,  IDC_CHK_FFB_RAMP, IDC_CHK_FFB_SQUARE,  IDC_CHK_FFB_SINE,
        IDC_CHK_FFB_TRI, IDC_CHK_FFB_STUP,  IDC_CHK_FFB_STDN,  IDC_CHK_FFB_SPRING,
        IDC_CHK_FFB_DAMPER,  IDC_CHK_FFB_INERTIA, IDC_CHK_FFB_FRICTION
    };

    for (auto effect : cb)
        EnableWindow(GetDlgItem(hDlgTab, effect), Enable);
}

// Get all active FFB effects
// Check/Uncheck the respective checkboxes accordingly
void UpdateFfbEffectCB(int rID)
{
    BOOL isEffect;
    HWND h;
    int pairs[][2]
    {
        { IDC_CHK_FFB_CONST, 0x26 }, { IDC_CHK_FFB_RAMP, 0x27 }, { IDC_CHK_FFB_SQUARE, 0x30 }, { IDC_CHK_FFB_SINE, 0x31 },
        { IDC_CHK_FFB_TRI, 0x32 }, { IDC_CHK_FFB_STUP, 0x33 }, { IDC_CHK_FFB_STDN, 0x34 }, { IDC_CHK_FFB_SPRING, 0x40 },
        { IDC_CHK_FFB_DAMPER, 0x41 }, { IDC_CHK_FFB_INERTIA, 0x42 }, { IDC_CHK_FFB_FRICTION, 0x43 }
    };

    for (auto effect : pairs) {
        if (!vJoyDevice[rID - 1]->Exists())
            isEffect = FALSE;
        else
            isEffect = IsDeviceFfbEffect(rID, effect[1]);
        h = GetDlgItem(hDlgTab, effect[0]);
        SendMessage(h, BM_SETCHECK, isEffect, 0);
    }
}

// Clear all all Checkboxes related to FFB Effects
void ClearFfbEffectCB(void)
{
    HWND h;
    int cb[]
    {
        IDC_CHK_FFB_CONST, IDC_CHK_FFB_RAMP, IDC_CHK_FFB_SQUARE, IDC_CHK_FFB_SINE,
            IDC_CHK_FFB_TRI, IDC_CHK_FFB_STUP, IDC_CHK_FFB_STDN, IDC_CHK_FFB_SPRING,
            IDC_CHK_FFB_DAMPER, IDC_CHK_FFB_INERTIA, IDC_CHK_FFB_FRICTION
    };

    for (auto effect : cb) {
        h = GetDlgItem(hDlgTab, effect);
        SendMessage(h, BM_SETCHECK, FALSE, 0);
    };

}
// Clear all all Checkboxes related to FFB Effects
void SetFfbEffectCB(void)
{
    HWND h;
    int cb[]
    {
        IDC_CHK_FFB_CONST, IDC_CHK_FFB_RAMP, IDC_CHK_FFB_SQUARE, IDC_CHK_FFB_SINE,
            IDC_CHK_FFB_TRI, IDC_CHK_FFB_STUP, IDC_CHK_FFB_STDN, IDC_CHK_FFB_SPRING,
            IDC_CHK_FFB_DAMPER, IDC_CHK_FFB_INERTIA, IDC_CHK_FFB_FRICTION
    };

    for (auto effect : cb) {
        h = GetDlgItem(hDlgTab, effect);
        SendMessage(h, BM_SETCHECK, TRUE, 0);
    };

}

// React to user changing the FFB enable checkbox
// If Checked then enable all  Checkboxes related to FFB Effects and check/uncheck them
// If Unchecked then disable all Checkboxes related to FFB Effects
void FfbCBChanged(void)
{
    // Get current Tab
    int id = TabCtrl_GetCurSel(hTabs) +1;
    if (id<1)
        return;


    // Get state of FFB checkbox
    BOOL Checked = Button_GetCheck(GetDlgItem(hDlgTab, IDC_CHK_FFB)) == BST_CHECKED;
    EnableFfbEffectCB(Checked);


    if (Checked) {
        if (IsDeviceFfb(id))
            UpdateFfbEffectCB(id);
        else
            SetFfbEffectCB();
    } else {
        if (!IsDeviceFfb(id))
            ClearFfbEffectCB();
    };
}

// Read the Checkboxes related to FFB Effects
// Every Enabled+Checked checkbox sets a corresponding bit in the mask
UINT16 GetFfbEffectMask(void)
{
    HWND h;
    UINT16 Mask = 0;

    int cb[]
    {
        IDC_CHK_FFB_CONST, IDC_CHK_FFB_RAMP, IDC_CHK_FFB_SQUARE, IDC_CHK_FFB_SINE,
            IDC_CHK_FFB_TRI, IDC_CHK_FFB_STUP, IDC_CHK_FFB_STDN, IDC_CHK_FFB_SPRING,
            IDC_CHK_FFB_DAMPER, IDC_CHK_FFB_INERTIA, IDC_CHK_FFB_FRICTION
    };

    for (auto effect : cb) {
        BOOL Checked = FALSE, Enabled = FALSE;
        h = GetDlgItem(hDlgTab, effect);
        if (Button_GetCheck(h) == BST_CHECKED)
            Checked = TRUE;
        Enabled = IsWindowEnabled(h);
        if (Checked && Enabled)
            Mask |= 0x01;
        Mask = Mask << 1;
    };

    Mask = Mask >> 1;
    return Mask;
}