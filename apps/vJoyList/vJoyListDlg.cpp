
// vJoyListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "./MyMFCListCtrl.h"
#include "vJoyList.h"
#include "vJoyListDlg.h"
#include "afxdialogex.h"
#include "..\..\inc\public.h"
#include "../../inc/vjoyinterface.h"

#pragma comment(lib, "vJoyInterface.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CvJoyListDlg dialog



CvJoyListDlg::CvJoyListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_VJOYLIST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvJoyListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEV, m_DevList);
}

BEGIN_MESSAGE_MAP(CvJoyListDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CvJoyListDlg message handlers

BOOL CvJoyListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize list
	m_DevList.InsertColumn(0, _T("Owner PID"), LVCFMT_LEFT, 80);
	m_DevList.InsertColumn(0, _T("#"), LVCFMT_LEFT, 30);

	// Populate list for the first time
	for (int id = 16; id >0; id--)
		Dev2List(id);

	// Start timer that will check devices every  500mSec
	SetTimer(1, 500, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CvJoyListDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CvJoyListDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CvJoyListDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CvJoyListDlg::OnTimer(UINT_PTR nIDEvent)
{

	// Populate list for the first time
	for (int id = 16; id >0; id--)
		Dev2List(id);

	CDialog::OnTimer(nIDEvent);
}


// Add/modify a device to the list
bool CvJoyListDlg::Dev2List(int id)
{
	// Sanity check
	if (id < 1 || id>16)
		return FALSE;

	// Does this device exist?
	m_DevList.SetExist(id, false);
	if (isVJDExists(id))
		m_DevList.SetExist(id);

	// Get the PID of the owner
	int OwnerPid = GetOwnerPid(id);

	// Search for existing entry
	CString str;
	LVFINDINFO FindInfo;
	FindInfo.flags = LVFI_STRING;
	str.Format(_T("%d"), id);
	FindInfo.psz = (LPCTSTR)str;
	int index = m_DevList.FindItem(&FindInfo);

	// Entry does not exist
	if (index ==-1)
	{ 
		int nIndex = m_DevList.InsertItem(0, FindInfo.psz);
		if (OwnerPid>0)
		{ 
			str.Format(_T("%d"), OwnerPid);
			m_DevList.SetItemText(nIndex, 1, (LPCTSTR)str);
		}
	}

	// Entry already exists - replace PID
	else
	{		
		if (OwnerPid > 0)
		{ 
			str.Format(_T("%d"), OwnerPid);
			m_DevList.SetOwned(id);
		}
		else
		{ 
			str = _T("");
			m_DevList.SetOwned(id, false);
		}

		m_DevList.SetItemText(index, 1, (LPCTSTR)str);
		m_DevList.Update(index);
	}

	return false;
}

