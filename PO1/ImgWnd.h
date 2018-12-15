#pragma once
#include "MyBitmap.h"

// CImgWnd

class CImgWnd : public CWnd
{
	DECLARE_DYNAMIC(CImgWnd)

public:
	CImgWnd();
	virtual ~CImgWnd();
	CMyBitmap m_MyBitmap;

	BOOL Create(const RECT& rect, CWnd*  pParentWnd, UINT nID);

protected:
	

	

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


