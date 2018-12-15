#pragma once

#include "MyBitmap.h"
#include <vector>

// CAccumulatorDialog dialog

class CAccumulatorDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAccumulatorDialog)

public:
	std::vector < std::vector <int> > *MyVec;
	int maxxx;
	int stepp;

	CAccumulatorDialog(CWnd* pParent, std::vector < std::vector <int> > *vec, int max, int step);   // standard constructor
	virtual ~CAccumulatorDialog();

	CMyBitmap m_AccumulatorBmp;

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
};
