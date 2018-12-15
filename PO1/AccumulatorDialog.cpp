// AccumulatorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "AccumulatorDialog.h"
#include "afxdialogex.h"
#include <vector>


// CAccumulatorDialog dialog

IMPLEMENT_DYNAMIC(CAccumulatorDialog, CDialogEx)

CAccumulatorDialog::CAccumulatorDialog(CWnd* pParent, std::vector < std::vector <int> > *vec, int max, int step)
	: CDialogEx(CAccumulatorDialog::IDD, pParent)
{
	MyVec = vec;
	maxxx = max;
	stepp = step;
}

CAccumulatorDialog::~CAccumulatorDialog()
{
}

void CAccumulatorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAccumulatorDialog, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CAccumulatorDialog message handlers


void CAccumulatorDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
	CPen pen;
	pen.CreatePen(PS_SOLID, stepp, RGB(255, 255, 255));
	dc.SelectObject(pen);



	CRect rect;
	GetClientRect(&rect);

	int yplus = -100;
	int xplus = 50;
	
	double factor = 255 / (double)maxxx;

	for (int i = 0; i < MyVec->size(); i++)
	{
		for (int j = 0; j < (*MyVec)[i].size(); j++)
		{
			dc.Rectangle(i * 2 + xplus, (*MyVec)[i].size() - j + yplus, i * 2 + 2 + xplus, (*MyVec)[i].size() - j + 2 + yplus);
		}
	}

	for (int i = 0; i < MyVec->size(); i++)
	{
		for (int j = 0; j < (*MyVec)[i].size(); j++)
		{
			int ffff = (*MyVec)[i][j];

			if (ffff <= 0)
				continue;

			int color = (double)ffff * factor;
			CPen pen2;
			pen2.CreatePen(PS_SOLID, stepp, RGB(255 - color, 255 - color, 255 - color));
			dc.SelectObject(pen2);

			CBrush brush;
			brush.CreateSolidBrush(RGB(255 - color, 255 - color, 255 - color));
			dc.SelectObject(brush);

			dc.Rectangle(i * 2 + xplus, (*MyVec)[i].size() - j + yplus, i * 2 + 2 + xplus, (*MyVec)[i].size() - j + 2 + yplus);
		}
	}
}


BOOL CAccumulatorDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
