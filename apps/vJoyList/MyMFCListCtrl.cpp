// MyMFCListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "vJoyList.h"
#include "MyMFCListCtrl.h"


// CMyMFCListCtrl

IMPLEMENT_DYNAMIC(CMyMFCListCtrl, CMFCListCtrl)

COLORREF CMyMFCListCtrl::OnGetCellTextColor(int nRow, int nColum)
{
	if (!Exist[nRow])
		return RGB(200, 200, 200);
	else
	{
		if (!Owned[nRow])
			return RGB(0, 0, 0);
		else
			return RGB(250, 50, 0);
	}
}

CMyMFCListCtrl::CMyMFCListCtrl()
{

}

CMyMFCListCtrl::~CMyMFCListCtrl()
{
}

void CMyMFCListCtrl::SetExist(int id, bool exist)
{
	if (id < 1 || id>16)
		return;

	Exist[id - 1] = exist;
}

void CMyMFCListCtrl::SetOwned(int id, bool exist)
{
	if (id < 1 || id>16)
		return;

	Owned[id - 1] = exist;
}


BEGIN_MESSAGE_MAP(CMyMFCListCtrl, CMFCListCtrl)
END_MESSAGE_MAP()



// CMyMFCListCtrl message handlers


