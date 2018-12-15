#pragma once
#include "MyBitmap.h"
#include <vector>

// CHistogramDialog dialog

class CHistogramDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CHistogramDialog)

public:
	CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* a);
	CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* a, int threshold);
	CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* a, int threshold, std::vector<int>* b);
	CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* v, std::vector<int>* h);
	CHistogramDialog(CWnd* pParent, int width, int height, std::vector<int>* r, std::vector<int>* g, std::vector<int>* b);   // standard constructor
	virtual ~CHistogramDialog();

	int m_Width;
	int m_Height;
	CMyBitmap m_Histogram1Img;
	CMyBitmap m_Histogram2Img;
	CMyBitmap m_Histogram3Img;

	void createHistogramFromVector(int startpoint, CMyBitmap* bitmap, std::vector<int>* a);
	void createHistogramFromVector(int startpoint, CMyBitmap* bitmap, std::vector<int>* a, int threshold);
	void createRotatedHistogramFromVector(int startpoint, CMyBitmap* bitmap, std::vector<int>* a);
	void createRotatedHistogramFromVector2(CMyBitmap* bitmap, std::vector<int>* a);

	void xxx(int width, int height, std::vector<int>* v, std::vector<int>* h);
	void yyy(int width, int height, std::vector<int>* v, std::vector<int>* h);

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
