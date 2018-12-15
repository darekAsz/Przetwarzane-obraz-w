
// PO1Dlg.h : header file
//

#pragma once
#include "ImgWnd.h"
#include "afxwin.h"
#include "MyBitmap.h"
#include <vector>



// CPODlg dialog
class CPODlg : public CDialogEx
{
// Construction
public:
	CPODlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PO1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CImgWnd m_imgIN;
	CImgWnd m_imgOUT;

	CComboBox m_combo1;

	CSliderCtrl m_slider1;

	int m_masks[6][3][3];

	bool normalize[6];

    std::vector<std::vector<int> > distanceMapping;
    int maxDistance;

    bool distanceMapped;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonProcess();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonParams();
	afx_msg void OnCbnEditchangeCombo1();
	afx_msg void OnCbnSelchangeCombo1();
	void zotsuj(int startx, int starty, int endx, int endy);
	void LinearFilter(int maskIdx);
	void median(bool is3, bool iscross);
	void log();
	void pavlidis();
	bool pavlidisMask0Rot0(int x, int y);
	bool pavlidisMask0Rot1(int x, int y);
	bool pavlidisMask1(int x, int y);
    void distanceMap();
    int getNearestPixel(int x, int y);
    void pavlidisNew();
    void drawDistanceMap(int threshold, bool binary);
    afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	//
	void CPODlg::erozja1(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::dylatacja1(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::dylatacja1_hack(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::otwarcie1(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::zamkniecie1(CMyBitmap *in, CMyBitmap *out);
	//
	void CPODlg::kontur_wew(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::kontur_zew(CMyBitmap *in, CMyBitmap *out);
	//
	void CPODlg::erozja8(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::dylatacja8(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::otwarcie8(CMyBitmap *in, CMyBitmap *out);
	void CPODlg::zamkniecie8(CMyBitmap *in, CMyBitmap *out);
	//
	void CPODlg::whitehat(CMyBitmap *in, CMyBitmap *out, float a);
	void CPODlg::blackhat(CMyBitmap *in, CMyBitmap *out, float a);
	void CPODlg::hough(CMyBitmap *in, CMyBitmap *out);
    void CPODlg::hough5(CMyBitmap *in, CMyBitmap *out);
};
