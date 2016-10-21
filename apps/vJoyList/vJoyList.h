
// vJoyList.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CvJoyListApp:
// See vJoyList.cpp for the implementation of this class
//

class CvJoyListApp : public CWinApp
{
public:
	CvJoyListApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	// Add/modify a device to the list
//	BOOL Dev2List(int id);
};

extern CvJoyListApp theApp;