
// vJoyListDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CvJoyListDlg dialog
class CvJoyListDlg : public CDialog
{
// Construction
public:
	CvJoyListDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VJOYLIST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// List of vJoy devices
	CMyMFCListCtrl  m_DevList;
protected:
	bool Dev2List(int id);
public:
};
