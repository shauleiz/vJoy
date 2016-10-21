#pragma once


// CMyMFCListCtrl

class CMyMFCListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CMyMFCListCtrl)
	virtual COLORREF OnGetCellTextColor(int nRow, int nColum);
	//virtual COLORREF OnGetCellBkColor(int nRow, int nColum);

public:
	CMyMFCListCtrl();
	virtual ~CMyMFCListCtrl();
	void SetExist(int id, bool exist=true);
	void SetOwned(int id, bool exist=true);

protected:
	bool Exist[16];
	bool Owned[16];

	DECLARE_MESSAGE_MAP()
};


