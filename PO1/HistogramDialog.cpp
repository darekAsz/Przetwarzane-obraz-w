// HistogramDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "HistogramDialog.h"
#include "afxdialogex.h"
#include <vector>


// CHistogramDialog dialog

IMPLEMENT_DYNAMIC(CHistogramDialog, CDialogEx)

CHistogramDialog::CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* a)
: CDialogEx(CHistogramDialog::IDD, pParent)
{
	m_Height = height;
	m_Width = width;

	if (m_Height > 300)
	{
		m_Height = 300;
	}

	m_Histogram1Img.CreateGreyscaleDIB(new CRect(0, 0, a->size(), m_Height), 0, 0);

	createHistogramFromVector(0, &m_Histogram1Img, a);
}

CHistogramDialog::CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* a, int threshold)
: CDialogEx(CHistogramDialog::IDD, pParent)
{
	m_Height = height;
	m_Width = width;

	if (m_Height > 300)
	{
		m_Height = 300;
	}

	m_Histogram1Img.CreateGreyscaleDIB(new CRect(0, 0, a->size(), m_Height), 0, 0);

	createHistogramFromVector(0, &m_Histogram1Img, a, threshold);
}

CHistogramDialog::CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* a, int threshold, std::vector<int>* b)
: CDialogEx(CHistogramDialog::IDD, pParent)
{
	m_Height = height;
	m_Width = 600;

	m_Height = 600;

	m_Histogram1Img.CreateGreyscaleDIB(new CRect(0, 0, 256, 605), 0, 0);

	createHistogramFromVector(0, &m_Histogram1Img, a, threshold);
	createHistogramFromVector(300, &m_Histogram1Img, b);
}

void CHistogramDialog::xxx(int width, int height, std::vector<int>* v, std::vector<int>* h) {
	m_Histogram1Img.CreateGreyscaleDIB(new CRect(0, 0, 600, 600), 0, 0);

	createHistogramFromVector(0, &m_Histogram1Img, h);
	createRotatedHistogramFromVector(300, &m_Histogram1Img, v);
}

CHistogramDialog::CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* v, std::vector<int>* h)
: CDialogEx(CHistogramDialog::IDD, pParent)
{
	m_Height = height * 2;
	m_Width = width * 2;

	xxx(width, height, v, h);
}

CHistogramDialog::CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* a, std::vector<int>* b, std::vector<int>* c)
	: CDialogEx(CHistogramDialog::IDD, pParent)
{
	m_Height = height;
	m_Width = 256 * 2;

	m_Height = (300 + 3) * 3;

	m_Histogram1Img.CreateGreyscaleDIB(new CRect(0, 0, 256, m_Height), 0, 0);

	createHistogramFromVector(0, &m_Histogram1Img, a);
	createHistogramFromVector(303, &m_Histogram1Img, b);
	createHistogramFromVector(606, &m_Histogram1Img, c);
}

void CHistogramDialog::createHistogramFromVector(int startpoint, CMyBitmap* bitmap, std::vector<int>* a)
{
	int actualValue = 0;
	int bitmapheight = bitmap->getInfo()->bmiHeader.biHeight;

	for (int i = 0; i < a->size(); i++)
	{
		for (int j = 0; j < (*a)[i]; j++)
		{
			bitmap->SetPixel8(i, j + startpoint, 0);
		}
		for (int j = (*a)[i]; j < 300 + 1; j++)
		{
			bitmap->SetPixel8(i, j + startpoint, 255);
		}
	}
}

void CHistogramDialog::createHistogramFromVector(int startpoint, CMyBitmap* bitmap, std::vector<int>* a, int threshold)
{
	int actualValue = 0;
	int bitmapheight = bitmap->getInfo()->bmiHeader.biHeight;

	bitmap->getInfo()->bmiColors[6].rgbRed = 255;
	bitmap->getInfo()->bmiColors[6].rgbGreen = 0;
	bitmap->getInfo()->bmiColors[6].rgbBlue = 0;

	for (int i = 0; i < a->size(); i++)
	{
		for (int j = 0; j < (*a)[i]; j++)
		{
			bitmap->SetPixel8(i, j + startpoint, 0);
		}
		for (int j = (*a)[i]; j < 300 + 1; j++)
		{
			bitmap->SetPixel8(i, j + startpoint, 255);
		}
	}

	for (int j = 0; j < 300 + 1; j++)
	{
		bitmap->SetPixel8(threshold, j, 6);
	}
	for (int j = 0; j < 300 + 1; j++)
	{
		bitmap->SetPixel8(threshold + 1, j, 6);
	}
}

void CHistogramDialog::createRotatedHistogramFromVector(int startpoint, CMyBitmap* bitmap, std::vector<int>* a)
{
	int actualValue = 0;
	//int bitmapheight = bitmap->getInfo()->bmiHeader.biHeight;

	for (int i = 0; i < a->size(); i++)
	{
		for (int j = 0; j < (*a)[i]; j++)
		{
			bitmap->SetPixel8(j, i + startpoint, 0);
		}
		for (int j = (*a)[i]; j < bitmap->getInfo()->bmiHeader.biWidth + 1; j++)
		{
			bitmap->SetPixel8(j, i + startpoint, 255);
		}
	}
}

void CHistogramDialog::createRotatedHistogramFromVector2(CMyBitmap* bitmap, std::vector<int>* a)
{
	int actualValue = 0;
	int bitmapheight = bitmap->getInfo()->bmiHeader.biHeight;

	for (int i = 0; i < a->size(); i++)
	{
		for (int j = 0; j < (*a)[i]; j++)
		{
			bitmap->SetPixel8(j, i, 0);
		}
		for (int j = (*a)[i]; j < bitmap->getInfo()->bmiHeader.biWidth + 1; j++)
		{
			bitmap->SetPixel8(j, i, 255);
		}
	}
}


CHistogramDialog::~CHistogramDialog()
{
}

void CHistogramDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHistogramDialog, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CHistogramDialog message handlers


BOOL CHistogramDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  Add extra initialization here

	SetWindowPos(NULL, 0, 0, m_Width, m_Height, SWP_NOMOVE | SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CHistogramDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages

	CRect rect;
	GetClientRect(&rect);

	CRect rect2 = rect;
	rect2.top = rect.bottom;
	rect2.bottom = rect2.top + rect.bottom;

	m_Histogram1Img.PaintDIB(dc, rect, CRect(0, 0, m_Histogram1Img.getInfo()->bmiHeader.biWidth, m_Histogram1Img.getInfo()->bmiHeader.biHeight));

	//m_Histogram2Img.PaintDIB(dc, rect, CRect(0, 0, m_Histogram2Img.getInfo()->bmiHeader.biWidth, m_Histogram2Img.getInfo()->bmiHeader.biHeight));
}
