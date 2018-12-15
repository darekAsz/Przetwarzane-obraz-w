
// PO1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "PO1Dlg.h"
#include "ParamsDlg.h"
#include "afxdialogex.h"
#include <vector>
#include "HistogramDialog.h"
#include <cmath>
#include <algorithm>
#include "Pixel.h"
#include "AccumulatorDialog.h"
#include "HoughPoint.h"

#define _USE_MATH_DEFINES
#include "math.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_WND_ID_IN	100
#define IMG_WND_ID_OUT	101


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPODlg dialog



CPODlg::CPODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPODlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//Usredniajaca
	m_masks[0][0][0] = 1;
	m_masks[0][0][1] = 1;
	m_masks[0][0][2] = 1;
	m_masks[0][1][0] = 1;
	m_masks[0][1][1] = 1;
	m_masks[0][1][2] = 1;
	m_masks[0][2][0] = 1;
	m_masks[0][2][1] = 1;
	m_masks[0][2][2] = 1;

	//Gauss
	m_masks[1][0][0] = 1;
	m_masks[1][0][1] = 4;
	m_masks[1][0][2] = 1;
	m_masks[1][1][0] = 4;
	m_masks[1][1][1] = 12;
	m_masks[1][1][2] = 4;
	m_masks[1][2][0] = 1;
	m_masks[1][2][1] = 4;
	m_masks[1][2][2] = 1;

	//Sobel1
	m_masks[2][0][0] = 1;
	m_masks[2][0][1] = 2;
	m_masks[2][0][2] = 1;
	m_masks[2][1][0] = 0;
	m_masks[2][1][1] = 0;
	m_masks[2][1][2] = 0;
	m_masks[2][2][0] = -1;
	m_masks[2][2][1] = -2;
	m_masks[2][2][2] = -1;

	//Sobel2
	m_masks[3][0][0] = -1;
	m_masks[3][0][1] = 0;
	m_masks[3][0][2] = 1;
	m_masks[3][1][0] = -2;
	m_masks[3][1][1] = 0;
	m_masks[3][1][2] = 2;
	m_masks[3][2][0] = -1;
	m_masks[3][2][1] = 0;
	m_masks[3][2][2] = 1;

	//Laplasjanu
	m_masks[4][0][0] = -2;
	m_masks[4][0][1] = 1;
	m_masks[4][0][2] = -2;
	m_masks[4][1][0] = 1;
	m_masks[4][1][1] = 4;
	m_masks[4][1][2] = 1;
	m_masks[4][2][0] = -2;
	m_masks[4][2][1] = 1;
	m_masks[4][2][2] = -2;

	//Wyostrzajaca
	m_masks[5][0][0] = 0;
	m_masks[5][0][1] = -1;
	m_masks[5][0][2] = 0;
	m_masks[5][1][0] = -1;
	m_masks[5][1][1] = 5;
	m_masks[5][1][2] = -1;
	m_masks[5][2][0] = 0;
	m_masks[5][2][1] = -1;
	m_masks[5][2][2] = 0;

	normalize[0] = true;
	normalize[1] = true;
	normalize[2] = false;
	normalize[3] = false;
	normalize[4] = false;
	normalize[5] = false;

    distanceMapped = false;
}

void CPODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_SLIDER1, m_slider1);
}

BEGIN_MESSAGE_MAP(CPODlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CPODlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CPODlg::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CPODlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_PARAMS, &CPODlg::OnBnClickedButtonParams)
	ON_CBN_EDITCHANGE(IDC_COMBO1, &CPODlg::OnCbnEditchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CPODlg::OnCbnSelchangeCombo1)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CPODlg::OnNMReleasedcaptureSlider1)
END_MESSAGE_MAP()


// CPODlg message handlers

BOOL CPODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rDlg(7,7,407,407);
	MapDialogRect(rDlg);

	m_imgIN.Create(rDlg, this, IMG_WND_ID_IN);
		
	rDlg = CRect(530, 7, 930, 407);
	MapDialogRect(rDlg);

	m_imgOUT.Create(rDlg, this, IMG_WND_ID_OUT);
	
	m_combo1.AddString(L"1convert to greyscale");
	m_combo1.AddString(L"1change brightness");
	m_combo1.AddString(L"1change contrast");
	m_combo1.AddString(L"1powering :D");
	m_combo1.AddString(L"1negative");
	m_combo1.AddString(L"1show histogram");
	m_combo1.AddString(L"1align histogram");
	m_combo1.AddString(L"2binarize - simple");
	m_combo1.AddString(L"2binarize - iterative");
	m_combo1.AddString(L"2binarize - gradient");
	m_combo1.AddString(L"2binarize - otsu");
	m_combo1.AddString(L"2binarize - local otsu");

	m_combo1.AddString(L"3Linear Filter - Averaging");
	m_combo1.AddString(L"3Linear Filter - Gauss");
	m_combo1.AddString(L"3Linear Filter - Sobel1");
	m_combo1.AddString(L"3Linear Filter - Sobel2");
	m_combo1.AddString(L"3Linear Filter - Laplace");
	m_combo1.AddString(L"3Linear Filter - Sharpen");
	m_combo1.AddString(L"3Median Filter 3x3");
	m_combo1.AddString(L"3Median Filter 5x5");
	m_combo1.AddString(L"3Median Filter Cross");
	m_combo1.AddString(L"3LoG");
	m_combo1.AddString(L"4Pavlidis");
	m_combo1.AddString(L"4DistanceMap");

	m_combo1.AddString(L"5dylatacja");
	m_combo1.AddString(L"5erozja");
	m_combo1.AddString(L"5zamkniecie");
	m_combo1.AddString(L"5otwarcie");
	m_combo1.AddString(L"5kontur-wewn");
	m_combo1.AddString(L"5kontur-zewn");
	m_combo1.AddString(L"5xtreme-white");
	m_combo1.AddString(L"5xtreme-black");

	m_combo1.AddString(L"6Hough");

	m_combo1.SelectString(0, L"1convert to greyscale");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

		
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPODlg::OnBnClickedButtonLoad()
{
	WCHAR strFilter[] = { L"Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(TRUE, NULL, NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		CString path = FileDlg.GetFolderPath();
		path.Append(L"\\");
		path.Append(FileDlg.GetFileName());
		m_imgIN.m_MyBitmap.LoadDIB(path);
	    distanceMapped = false;

		InvalidateRect(NULL, 1);
	}	
}


void CPODlg::OnBnClickedButtonProcess()
{
	CString sOption;
	m_combo1.GetLBText(m_combo1.GetCurSel(), sOption);


	if (sOption == L"1convert to greyscale")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		RGBTRIPLE actualPixel24;
		BYTE actualPixel8;
		bool actualPixel1;
		BYTE actualLuminance;

		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 24)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel24 = m_imgIN.m_MyBitmap.GetPixel24(i, j);
					actualLuminance = m_imgIN.m_MyBitmap.getRGBLuminance(actualPixel24.rgbtRed, actualPixel24.rgbtGreen, actualPixel24.rgbtBlue);

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualPixel8);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel1 = m_imgIN.m_MyBitmap.GetPixel1(i, j);

					if (actualPixel1)
					{
						actualLuminance = 255;
					}
					else
					{
						actualLuminance = 0;
					}

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
	}
	else if (sOption == L"1change brightness")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		RGBTRIPLE actualPixel24;
		BYTE actualPixel8;
		bool actualPixel1;
		BYTE actualLuminance;

		int actualBrightness = 0;
		int brightnessThreshold = m_slider1.GetPos();

		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 24)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel24 = m_imgIN.m_MyBitmap.GetPixel24(i, j);
					actualLuminance = m_imgIN.m_MyBitmap.getRGBLuminance(actualPixel24.rgbtRed, actualPixel24.rgbtGreen, actualPixel24.rgbtBlue);

					actualBrightness = actualLuminance;
					actualBrightness += brightnessThreshold;

					if (actualBrightness > 255)
					{
						actualBrightness = 255;
					}
					else if (actualBrightness < 0)
					{
						actualBrightness = 0;
					}

					actualLuminance = actualBrightness;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);

					actualBrightness = actualPixel8;
					actualBrightness += brightnessThreshold;

					if (actualBrightness > 255)
					{
						actualBrightness = 255;
					}
					else if (actualBrightness < 0)
					{
						actualBrightness = 0;
					}

					actualPixel8 = actualBrightness;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualPixel8);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel1 = m_imgIN.m_MyBitmap.GetPixel1(i, j);

					if (actualPixel1)
					{
						actualLuminance = 255;
					}
					else
					{
						actualLuminance = 0;
					}

					actualBrightness = actualLuminance;
					actualBrightness += brightnessThreshold;

					if (actualBrightness > 255)
					{
						actualBrightness = 255;
					}
					else if (actualBrightness < 0)
					{
						actualBrightness = 0;
					}

					actualLuminance = actualBrightness;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
	}
	else if (sOption == L"1change contrast")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		RGBTRIPLE actualPixel24;
		BYTE actualPixel8;
		bool actualPixel1;
		BYTE actualLuminance;

		int a = 0;

		double actualBrightness = 0.f;
		int contrastThresholdInt = m_slider1.GetPos();
		double contrastThresholdReal = (double)contrastThresholdInt / 10.0f;

		double x = (double)127 * contrastThresholdReal;

		a = 127 - x;

		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 24)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel24 = m_imgIN.m_MyBitmap.GetPixel24(i, j);
					actualLuminance = m_imgIN.m_MyBitmap.getRGBLuminance(actualPixel24.rgbtRed, actualPixel24.rgbtGreen, actualPixel24.rgbtBlue);

					x = (double)actualLuminance * contrastThresholdReal;

					x += a;

					if (x > 255)
					{
						x = 255;
					}
					else if (x < 0)
					{
						x = 0;
					}

					actualLuminance = x;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);

					x = (double)actualPixel8 * contrastThresholdReal;

					x += a;

					if (x > 255)
					{
						x = 255;
					}
					else if (x < 0)
					{
						x = 0;
					}

					actualPixel8 = x;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualPixel8);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel1 = m_imgIN.m_MyBitmap.GetPixel1(i, j);

					if (actualPixel1)
					{
						actualLuminance = 255;
					}
					else
					{
						actualLuminance = 0;
					}

					x = (double)actualLuminance * contrastThresholdReal;

					x += a;

					if (x > 255)
					{
						x = 255;
					}
					else if (x < 0)
					{
						x = 0;
					}

					actualLuminance = x;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
	}
	else if (sOption == L"1powering :D")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		RGBTRIPLE actualPixel24;
		BYTE actualPixel8;
		bool actualPixel1;
		BYTE actualLuminance;

		int a = 0;

		double actualBrightness = 0.f;
		int power = m_slider1.GetPos();
		double powerReal = (double)power / 10.0f;

		double x;

		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 24)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel24 = m_imgIN.m_MyBitmap.GetPixel24(i, j);
					actualLuminance = m_imgIN.m_MyBitmap.getRGBLuminance(actualPixel24.rgbtRed, actualPixel24.rgbtGreen, actualPixel24.rgbtBlue);

					x = (double)actualLuminance / 255.f;

					x = pow(x, powerReal);

					actualLuminance = x * 255.f;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);

					x = (double)actualPixel8 / 255.f;

					x = pow(x, powerReal);

					actualPixel8 = x * 255.f;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualPixel8);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel1 = m_imgIN.m_MyBitmap.GetPixel1(i, j);

					if (actualPixel1)
					{
						actualLuminance = 255;
					}
					else
					{
						actualLuminance = 0;
					}

					x = (double)actualLuminance / 255.f;

					x = pow(x, powerReal);

					if (x > 1)
					{
						x = 1;
					}
					else if (x < 0)
					{
						x = 0;
					}

					actualLuminance = x * 255.f;

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
	}
	else if (sOption == L"1negative")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		RGBTRIPLE actualPixel24;
		BYTE actualPixel8;
		bool actualPixel1;
		BYTE actualLuminance;

		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 24)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel24 = m_imgIN.m_MyBitmap.GetPixel24(i, j);
					actualLuminance = m_imgIN.m_MyBitmap.getRGBLuminance(255 - actualPixel24.rgbtRed, 255 - actualPixel24.rgbtGreen, 255 - actualPixel24.rgbtBlue);

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 255 - actualPixel8);
				}
			}

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel1 = m_imgIN.m_MyBitmap.GetPixel1(i, j);

					if (!actualPixel1)
					{
						actualLuminance = 255;
					}
					else
					{
						actualLuminance = 0;
					}

					m_imgOUT.m_MyBitmap.SetPixel8(i, j, actualLuminance);
				}
			}

			InvalidateRect(NULL, 1);
		}
	}
	else if (sOption == L"1show histogram")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		RGBTRIPLE actualPixel24;
		BYTE actualPixel8;
		bool actualPixel1;
		BYTE actualLuminance;

		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 24)
		{
			int rChannel[256];
			int gChannel[256];
			int bChannel[256];

			memset(rChannel, 0, sizeof(rChannel));
			memset(gChannel, 0, sizeof(gChannel));
			memset(bChannel, 0, sizeof(bChannel));

			std::vector<int> r;
			std::vector<int> g;
			std::vector<int> b;

			int rmax = 0;
			int gmax = 0;
			int bmax = 0;

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel24 = m_imgIN.m_MyBitmap.GetPixel24(i, j);

					rChannel[actualPixel24.rgbtRed]++;
					gChannel[actualPixel24.rgbtGreen]++;
					bChannel[actualPixel24.rgbtBlue]++;
				}
			}

			for (int i = 0; i < 256; i++)
				if (rChannel[i] > rmax)
					rmax = rChannel[i];

			for (int i = 0; i < 256; i++)
				if (gChannel[i] > gmax)
					gmax = gChannel[i];

			for (int i = 0; i < 256; i++)
				if (bChannel[i] > bmax)
					bmax = bChannel[i];

			r.assign(rChannel, rChannel + 256);
			g.assign(gChannel, gChannel + 256);
			b.assign(bChannel, bChannel + 256);

			if (rmax > 300)
			{
				double calibrationCoefficient = 300.f / (double)rmax;

				for (int i = 0; i < r.size(); i++)
				{
					r[i] = (double)r[i] * calibrationCoefficient;
				}
			}
			if (gmax > 300)
			{
				double calibrationCoefficient = 300.f / (double)gmax;

				for (int i = 0; i < g.size(); i++)
				{
					g[i] = (double)g[i] * calibrationCoefficient;
				}
			}
			if (bmax > 300)
			{
				double calibrationCoefficient = 300.f / (double)bmax;

				for (int i = 0; i < b.size(); i++)
				{
					b[i] = (double)b[i] * calibrationCoefficient;
				}
			}

			CHistogramDialog *dialog =  new CHistogramDialog(NULL, 256, 300, &r, &g, &b);
			dialog->DoModal();
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
		{
			int luminescenceChannel[256];

			memset(luminescenceChannel, 0, sizeof(luminescenceChannel));

			std::vector<int> histogram;

			int maxValue = 0;

			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
					luminescenceChannel[actualPixel8]++;

					if (luminescenceChannel[actualPixel8] > maxValue)
					{
						maxValue = luminescenceChannel[actualPixel8];
					}
				}
			}

			histogram.assign(luminescenceChannel, luminescenceChannel + 256);

			if (maxValue > 300)
			{
				double calibrationCoefficient = 300.f / (double)maxValue;

				for (int i = 0; i < histogram.size(); i++)
				{
					histogram[i] = (double)histogram[i] * calibrationCoefficient;
				}
			}

			CHistogramDialog *dialog = new CHistogramDialog(NULL, 256, 300, &histogram);
			dialog->DoModal();

			InvalidateRect(NULL, 1);
		}
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			int* verticalChannel = new int[actualImgHeight];
			int* horizontalChannel = new int[actualImgWidth];

			std::vector<int> v;
			std::vector<int> h;

			int vmax = 0;
			int hmax = 0;

			memset(verticalChannel, 0, actualImgHeight * sizeof(int));
			memset(horizontalChannel, 0, actualImgWidth * sizeof(int));

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel1 = m_imgIN.m_MyBitmap.GetPixel1(i, j);

					if (!actualPixel1)
					{
						horizontalChannel[i]++;
						if (horizontalChannel[i] > hmax)
							hmax = horizontalChannel[i];
						verticalChannel[j]++;
						if (verticalChannel[j] > vmax)
							vmax = verticalChannel[j];
					}
				}
			}

			v.assign(verticalChannel, verticalChannel + actualImgHeight);
			h.assign(horizontalChannel, horizontalChannel + actualImgWidth);

			if (hmax > 300)
			{
				double calibrationCoefficient = 300.f / (double)hmax;

				for (int i = 0; i < h.size(); i++)
				{
					h[i] = (double)h[i] * calibrationCoefficient;
				}
			}

			if (vmax > 300)
			{
				double calibrationCoefficient = 300.f / (double)vmax;

				for (int i = 0; i < v.size(); i++)
				{
					v[i] = (double)v[i] * calibrationCoefficient;
				}
			}

			CHistogramDialog *dialog = new CHistogramDialog(NULL, 300, 300, &v, &h);
			dialog->DoModal();
		}
	}
	else if (sOption == L"1equalize histogram")
	{
		m_slider1.SetRange(0, 0, true);
	}
	else if (sOption == L"2binarize - simple")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		BYTE actualPixel8;
		int threshold = m_slider1.GetPos();

		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
		{
			m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
					if (actualPixel8 <= threshold)
					{
						m_imgOUT.m_MyBitmap.SetPixel8(i, j, 0);
					}
					else
					{
						m_imgOUT.m_MyBitmap.SetPixel8(i, j, 255);
					}
					
				}
			}

			InvalidateRect(NULL, 1);
		}
	}
	else if (sOption == L"2binarize - iterative")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount != 8)
			return;

		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		BYTE actualPixel8;

		int luminescenceChannel[256];
		double normalizedHistogram[256];
		int pixelsInImage = actualImgHeight * actualImgWidth;

		std::vector<int> histogram;

		int maxValue = 0;

		memset(luminescenceChannel, 0, sizeof(luminescenceChannel));
		memset(normalizedHistogram, 0, sizeof(normalizedHistogram));

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
				luminescenceChannel[actualPixel8]++;

				if (luminescenceChannel[actualPixel8] > maxValue)
				{
					maxValue = luminescenceChannel[actualPixel8];
				}
			}
		}

		for (int i = 0; i < 256; i++)
		{
			normalizedHistogram[i] = ((double)(luminescenceChannel[i])) / (double)pixelsInImage;
		}

		double P0, u0, P1, u1, newThreshold = 128.0f;
		double oldThreshold;

		do
		{
			oldThreshold = newThreshold;
			P0 = 0;
			for (int i = 0; i <= oldThreshold; i++)
				P0 += normalizedHistogram[i];

			u0 = 0;
			for (int i = 0; i <= oldThreshold; i++)
				u0 += ((double)i * normalizedHistogram[i]) / P0;

			P1 = 0;
			for (int i = oldThreshold + 1; i < 256; i++)
				P1 += normalizedHistogram[i];

			u1 = 0;
			for (int i = oldThreshold + 1; i < 256; i++)
				u1 += ((double)i * normalizedHistogram[i]) / P1;

			newThreshold = (u0 + u1) / 2.f;
		} while (abs(oldThreshold - newThreshold) >= 2);

		histogram.assign(luminescenceChannel, luminescenceChannel + 256);

		if (maxValue > 300)
		{
			double calibrationCoefficient = 300.f / (double)maxValue;

			for (int i = 0; i < histogram.size(); i++)
			{
				histogram[i] = (double)histogram[i] * calibrationCoefficient;
			}
		}

		
		m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
				if (actualPixel8 <= newThreshold)
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 0);
				}
				else
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 255);
				}

			}
		}

		InvalidateRect(NULL, 1);

		CHistogramDialog *dialog = new CHistogramDialog(NULL, 256, 300, &histogram, (int)newThreshold);
		dialog->DoModal();
	}
	else if (sOption == L"2binarize - gradient")
	{
		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		BYTE actualPixel8;

		int luminescenceChannel[256];
		double normalizedHistogram[256];
		int pixelsInImage = actualImgHeight * actualImgWidth;

		memset(luminescenceChannel, 0, sizeof(luminescenceChannel));
		memset(normalizedHistogram, 0, sizeof(normalizedHistogram));

		long long int up = 0;
		long long int down = 0;

		std::vector<int> histogram;

		BYTE xj1, xj2, yj1, yj2;
		int gx, gy, g;

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);

				if (i == 0 || actualImgWidth - 1)
				{
					gx = 0;
				}
				else
				{
					xj1 = m_imgIN.m_MyBitmap.GetPixel8(i + 1, j);
					xj2 = m_imgIN.m_MyBitmap.GetPixel8(i - 1, j);
					gx = xj1 - xj2;
				}

				if (j == 0 || j == actualImgHeight - 1)
				{
					gy = 0;
				}
				else
				{
					yj1 = m_imgIN.m_MyBitmap.GetPixel8(i, j + 1);
					yj2 = m_imgIN.m_MyBitmap.GetPixel8(i, j - 1);
					gy = yj1 - yj2;
				}

				if (gy > gx)
				{
					g = gy;
				}
				else
				{
					g = gx;
				}

				up += actualPixel8 * g;
			}	
		}

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);

				if (i == 0 || actualImgWidth - 1)
				{
					gx = 0;
				}
				else
				{
					xj1 = m_imgIN.m_MyBitmap.GetPixel8(i + 1, j);
					xj2 = m_imgIN.m_MyBitmap.GetPixel8(i - 1, j);
					gx = xj1 - xj2;
				}

				if (j == 0 || j == actualImgHeight - 1)
				{
					gy = 0;
				}
				else
				{
					yj1 = m_imgIN.m_MyBitmap.GetPixel8(i, j + 1);
					yj2 = m_imgIN.m_MyBitmap.GetPixel8(i, j - 1);
					gy = yj1 - yj2;
				}

				if (gy > gx)
				{
					g = gy;
				}
				else
				{
					g = gx;
				}

				down += g;
			}
		}

		int maxValue = 0;

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
				luminescenceChannel[actualPixel8]++;

				if (luminescenceChannel[actualPixel8] > maxValue)
				{
					maxValue = luminescenceChannel[actualPixel8];
				}
			}
		}

		histogram.assign(luminescenceChannel, luminescenceChannel + 256);

		if (maxValue > 300)
		{
			double calibrationCoefficient = 300.f / (double)maxValue;

			for (int i = 0; i < histogram.size(); i++)
			{
				histogram[i] = (double)histogram[i] * calibrationCoefficient;
			}
		}

		double newThreshold = (double)up / (double)down;

		m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
				if (actualPixel8 <= newThreshold)
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 0);
				}
				else
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 255);
				}

			}
		}

		InvalidateRect(NULL, 1);

		CHistogramDialog *dialog = new CHistogramDialog(NULL, 256, 300, &histogram, (int)newThreshold);
		dialog->DoModal();
	}
	else if (sOption == L"2binarize - otsu")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount != 8)
			return;

		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
		BYTE actualPixel8;

		int luminescenceChannel[256];
		double normalizedHistogram[256];
		int pixelsInImage = actualImgHeight * actualImgWidth;

		std::vector<int> histogram;
		std::vector<int> jedenprzesyv;

		int maxValue = 0;

		memset(luminescenceChannel, 0, sizeof(luminescenceChannel));
		memset(normalizedHistogram, 0, sizeof(normalizedHistogram));

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
				luminescenceChannel[actualPixel8]++;

				if (luminescenceChannel[actualPixel8] > maxValue)
				{
					maxValue = luminescenceChannel[actualPixel8];
				}
			}
		}

		for (int i = 0; i < 256; i++)
		{
			normalizedHistogram[i] = ((double)(luminescenceChannel[i])) / (double)pixelsInImage;
		}

		// Tu bydzie otsu

		double jedenprzesy[256];

		double newThreshold = 0;
		int x = 0;

		for (int threshold = 1; threshold < 254; threshold++)
		{
			double P0, u0, P1, u1;
			
			P0 = 0;
			for (int i = 0; i <= threshold; i++)
				P0 += normalizedHistogram[i];

			u0 = 0;
			for (int i = 0; i <= threshold; i++)
				u0 += ((double)i * normalizedHistogram[i]) / P0;

			P1 = 0;
			for (int i = threshold + 1; i < 256; i++)
				P1 += normalizedHistogram[i];

			u1 = 0;
			for (int i = threshold + 1; i < 256; i++)
				u1 += ((double)i * normalizedHistogram[i]) / P1;

			double wariancja0 = 0;
			for (int i = 0; i <= threshold; i++)
				wariancja0 += (pow(((double)i - u0), 2.f) * normalizedHistogram[i]) / P0;

			double wariancja1 = 0;
			for (int i = threshold; i < 256; i++)
				wariancja1 += (pow(((double)i - u1), 2.f) * normalizedHistogram[i]) / P1;

			double wariancjaw = (P0 * wariancja0) + (P1 * wariancja1);

			double jedenprzez = 1.f / wariancjaw;

			jedenprzesy[threshold] = jedenprzez;

			if (jedenprzez > newThreshold)
			{
				newThreshold = jedenprzez;
				x = threshold;
			}			
		}

		newThreshold = x;

		double costam = 250.f / jedenprzesy[x];

		int jedenprzesyyy[256];

		for (int i = 0; i < 256; i++)
		{
			jedenprzesy[i] *= costam;
			jedenprzesyyy[i] = jedenprzesy[i];
			if (jedenprzesyyy[i] < 0)
			{
				jedenprzesyyy[i] = 0;
			}
		}

		// Tu juz nie bydzie otsu

		histogram.assign(luminescenceChannel, luminescenceChannel + 256);
		jedenprzesyv.assign(jedenprzesyyy, jedenprzesyyy + 256);

		if (maxValue > 300)
		{
			double calibrationCoefficient = 300.f / (double)maxValue;

			for (int i = 0; i < histogram.size(); i++)
			{
				histogram[i] = (double)histogram[i] * calibrationCoefficient;
			}
		}

		m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
				if (actualPixel8 <= newThreshold)
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 0);
				}
				else
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 255);
				}

			}
		}

		InvalidateRect(NULL, 1);

		CHistogramDialog *dialog = new CHistogramDialog(NULL, 256, 300, &histogram, (int)newThreshold, &jedenprzesyv);
		dialog->DoModal();
	}
	else if (sOption == L"2binarize - local otsu")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount != 8)
			return;

		int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
		int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;

		m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		int colW = actualImgWidth / 10;
		int rowH = actualImgHeight / 10;

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				zotsuj(i*colW, j*rowH, (i + 1)*colW, (j + 1)*rowH);
			}
		}
	}
	else if (sOption == L"3Linear Filter - Averaging")
	{
		LinearFilter(0);
	}
	else if (sOption == L"3Linear Filter - Gauss")
	{
		LinearFilter(1);
	}
	else if (sOption == L"3Linear Filter - Sobel1")
	{
		LinearFilter(2);
	}
	else if (sOption == L"3Linear Filter - Sobel2")
	{
		LinearFilter(3);
	}
	else if (sOption == L"3Linear Filter - Laplace")
	{
		LinearFilter(4);
	}
	else if (sOption == L"3Linear Filter - Sharpen")
	{
		LinearFilter(5);
	}
	else if (sOption == L"3Median Filter 3x3")
	{
		median(true, false);
	}
	else if (sOption == L"3Median Filter 5x5")
	{
		median(false, false);
	}
	else if (sOption == L"3Median Filter Cross")
	{
		median(true, true);
	}
	else if (sOption == L"3LoG")
	{
		log();
	}
	else if (sOption == L"4Pavlidis")
	{
		pavlidisNew();
	}
	else if (sOption == L"5dylatacja")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			dylatacja1_hack(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
		else
		{
			dylatacja8(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
		//else
			//dylatacja8();
	}
	else if (sOption == L"5erozja")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			erozja1(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
		else
		{
			erozja8(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
	}
	else if (sOption == L"5zamkniecie")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			zamkniecie1(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
		else
		{
			zamkniecie8(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
	}
	else if (sOption == L"5otwarcie")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			otwarcie1(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
		else
		{
			otwarcie8(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
	}
	else if (sOption == L"5kontur-wewn")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			kontur_wew(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
	}
	else if (sOption == L"5kontur-zewn")
	{
		if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 1)
		{
			kontur_zew(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
		}
	}
	else if (sOption == L"5xtreme-white")
	{
		whitehat(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap, 1);
	}
	else if (sOption == L"5xtreme-black")
	{
		blackhat(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap, 1);
	}
	else if (sOption == L"6Hough")
	{
		hough5(&m_imgIN.m_MyBitmap, &m_imgOUT.m_MyBitmap);
	}
}

void CPODlg::hough(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	int step = m_slider1.GetPos();

	std::vector<std::vector<int> > magicznyJelonek(360, std::vector<int>((actualImgWidth + actualImgHeight), 0));

	double maxRo = 0;
	int maxI = 0;
	int maxJ = 0;
	double minRo = (actualImgWidth + actualImgHeight) * 2;


	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			actualPix = in->GetPixel1(i, j);

			if (!actualPix)
			{
				for (int tetta = 0; tetta < 360; tetta += step)
				{
					double ro = (i - (actualImgWidth / 2)) * cos(tetta * M_PI / 180.0) + (j - (actualImgHeight / 2)) * sin(tetta * M_PI / 180.0);

					if (ro < 0)
						continue;

					magicznyJelonek[tetta][ro]++;
				}
			}
		}
	}

    std::vector<CHoughPoint> houghPoints;

	for (int i = 0; i < magicznyJelonek.size(); i++)
	{
		for (int j = 0; j < magicznyJelonek[i].size(); j++)
		{
            houghPoints.push_back(CHoughPoint(i,j, magicznyJelonek[i][j]));

			if (magicznyJelonek[i][j] > maxRo)
			{
				maxRo = magicznyJelonek[i][j];
				maxI = i;
				maxJ = j;
			}
		}
	}

    std::sort(houghPoints.begin(), houghPoints.end());

	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			out->SetPixel8(i, j, 255);
		}
	}

	for (int i = 0; i < actualImgWidth; i++)
	{
     	for (int j = 0; j < actualImgHeight; j++)
		{
			actualPix = in->GetPixel1(i, j);

			int x = (i - (actualImgWidth / 2));
			int y = (j - (actualImgHeight / 2));

			if (!actualPix)
			{
				out->SetPixel8(i, j, 0);
			}
            else
            {
                for (int test = 0; test < m_slider1.GetPos(); test++)
                {
                    int mi = houghPoints[test].i;
                    int mj = houghPoints[test].j;

                    double ro = x * cos(mi * M_PI / 180.0) + y * sin(mi * M_PI / 180.0);

                    if ((int)mj == (int)ro)
                    {
                        out->SetPixel8(i, j, 0);
                    }
                }
            }
		}
	}
	
	InvalidateRect(NULL, 1);

	CAccumulatorDialog *dialog = new CAccumulatorDialog(NULL, &magicznyJelonek, maxRo, step);
	dialog->m_AccumulatorBmp.CreateGreyscaleDIB(new CRect(0, 0, 720, 200), 0, 0);

	dialog->DoModal();
}

void CPODlg::hough5(CMyBitmap *in, CMyBitmap *out)
{
    int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
    int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
    bool actualPix;
    BYTE outPix;

    int step = m_slider1.GetPos();

    std::vector<std::vector<int> > magicznyJelonek(360, std::vector<int>((actualImgWidth + actualImgHeight), 0));

    double maxRo = 0;
    int maxI = 0;
    int maxJ = 0;
    double minRo = (actualImgWidth + actualImgHeight) * 2;


    for (int i = 0; i < actualImgWidth; i++)
    {
        for (int j = 0; j < actualImgHeight; j++)
        {
            actualPix = in->GetPixel1(i, j);

            if (!actualPix)
            {
                for (int tetta = 0; tetta < 360; tetta += step)
                {
                    double ro = (i - (actualImgWidth / 2)) * cos(tetta * M_PI / 180.0) + (j - (actualImgHeight / 2)) * sin(tetta * M_PI / 180.0);

                    if (ro < 0)
                        continue;

                    magicznyJelonek[tetta][ro]++;
                }
            }
        }
    }

    std::vector<CHoughPoint> houghPointsFinal;
    bool czarodziejka = true;

    int actualMaxIdx = 0;

    int maxMax = 0;

    while (czarodziejka)
    {
        std::vector<CHoughPoint> houghPoints;

        for (int i = 0; i < magicznyJelonek.size(); i++)
        {
            for (int j = 0; j < magicznyJelonek[i].size(); j++)
            {
                houghPoints.push_back(CHoughPoint(i, j, magicznyJelonek[i][j]));

                if (magicznyJelonek[i][j] > maxRo)
                {
                    maxRo = magicznyJelonek[i][j];
                    maxI = i;
                    maxJ = j;
                }
            }
        }

        std::sort(houghPoints.begin(), houghPoints.end());

        if (actualMaxIdx == 0)
        {
            maxMax = houghPoints[0].ro;
        }
        else
        {
            if (houghPoints[actualMaxIdx].ro <= maxMax / 10)
            {
                czarodziejka = false;
                break;
            }
        }

        int ii = houghPoints[actualMaxIdx].i;
        int jj = houghPoints[actualMaxIdx].j;

        if (ii - 1 >= 0 && jj - 1 >= 0)
        {
            magicznyJelonek[ii - 1][jj - 1] = 0;
        }
        if (jj - 1 >= 0)
        {
            magicznyJelonek[ii][jj - 1] = 0;
        }
        if (ii + 1 < magicznyJelonek.size() && jj - 1 >= 0)
        {
            magicznyJelonek[ii + 1][jj - 1] = 0;
        }
        if (ii - 1 >= 0)
        {
            magicznyJelonek[ii - 1][jj] = 0;
        }
        if (ii + 1 < magicznyJelonek.size())
        {
            magicznyJelonek[ii + 1][jj] = 0;
        }
        if (ii - 1 >= 0 && jj + 1 < magicznyJelonek[0].size())
        {
            magicznyJelonek[ii - 1][jj + 1] = 0;
        }
        if (jj + 1 < magicznyJelonek[0].size())
        {
            magicznyJelonek[ii][jj + 1] = 0;
        }
        if (ii + 1 < magicznyJelonek.size() && jj + 1 < magicznyJelonek[0].size())
        {
            magicznyJelonek[ii + 1][jj + 1] = 0;
        }

        houghPointsFinal.push_back(houghPoints[actualMaxIdx]);
        actualMaxIdx++;
    }

    out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

    for (int i = 0; i < actualImgWidth; i++)
    {
        for (int j = 0; j < actualImgHeight; j++)
        {
            out->SetPixel8(i, j, 255);
        }
    }

    for (int i = 0; i < actualImgWidth; i++)
    {
        for (int j = 0; j < actualImgHeight; j++)
        {
            actualPix = in->GetPixel1(i, j);

            int x = (i - (actualImgWidth / 2));
            int y = (j - (actualImgHeight / 2));

            if (!actualPix)
            {
                out->SetPixel8(i, j, 0);
            }
            else
            {
                for (int test = 0; test < houghPointsFinal.size(); test++)
                {
                    int mi = houghPointsFinal[test].i;
                    int mj = houghPointsFinal[test].j;

                    double ro = x * cos(mi * M_PI / 180.0) + y * sin(mi * M_PI / 180.0);

                    if ((int)mj == (int)ro)
                    {
                        out->SetPixel8(i, j, 0);
                    }
                }
            }
        }
    }

    InvalidateRect(NULL, 1);

    CAccumulatorDialog *dialog = new CAccumulatorDialog(NULL, &magicznyJelonek, maxRo, step);
    dialog->m_AccumulatorBmp.CreateGreyscaleDIB(new CRect(0, 0, 720, 200), 0, 0);

    dialog->DoModal();
}

void CPODlg::LinearFilter(int maskIdx)
{
	int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
	RGBTRIPLE actualPixel24;
	BYTE actualPixel8;
	bool actualPixel1;
	BYTE actualLuminance;

	if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
	{
		m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				double jw = 0;
				double wsum = 0;
				for (int x = 0; x < 3; x++)
				{
					for (int y = 0; y < 3; y++)
					{
						int x2 = x - 1;
						int y2 = y - 1;
						if (i - x2 < 0 ||
							i - x2 > actualImgWidth - 1 ||
							j - y2 < 0 ||
							j - y2 > actualImgWidth - 1)
						{
							continue;
						}
						actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i - x2, j - y2);

						jw += (double)actualPixel8 * (double)m_masks[maskIdx][x][y];
						wsum += (double)m_masks[maskIdx][x][y];
					}
				}

				if (normalize[maskIdx])
				{
					jw /= wsum;
				}

				if (maskIdx != 5)
					jw += 127;

				if (jw > 255)
				{
					jw = 255;
				}
				if (jw < 0)
				{
					jw = 0;
				}

				m_imgOUT.m_MyBitmap.SetPixel8(i, j, jw);
			}
		}

		InvalidateRect(NULL, 1);
	}
}

void CPODlg::zotsuj(int startx, int starty, int endx, int endy)
{
	BYTE actualPixel8;

	int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;

	int luminescenceChannel[256];
	double normalizedHistogram[256];
	int pixelsInImage = actualImgHeight * actualImgWidth;

	std::vector<int> histogram;
	std::vector<int> jedenprzesyv;

	int maxValue = 0;

	memset(luminescenceChannel, 0, sizeof(luminescenceChannel));
	memset(normalizedHistogram, 0, sizeof(normalizedHistogram));

	int minlum = 0xFF;
	int maxlum = 0;

	for (int i = startx; i <= endx; i++)
	{
		for (int j = starty; j <= endy; j++)
		{
			actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
			luminescenceChannel[actualPixel8]++;

			if (luminescenceChannel[actualPixel8] > maxValue)
			{
				maxValue = luminescenceChannel[actualPixel8];
			}

			if (actualPixel8 < minlum)
				minlum = actualPixel8;
			if (actualPixel8 > maxlum)
				maxlum = actualPixel8;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		normalizedHistogram[i] = ((double)(luminescenceChannel[i])) / (double)pixelsInImage;
	}

	// Tu bydzie otsu

	double jedenprzesy[256];

	double newThreshold = 0;
	int x = 0;

	for (int threshold = 1; threshold < 254; threshold++)
	{
		double P0, u0, P1, u1;

		P0 = 0;
		for (int i = 0; i <= threshold; i++)
			P0 += normalizedHistogram[i];

		u0 = 0;
		for (int i = 0; i <= threshold; i++)
			u0 += ((double)i * normalizedHistogram[i]) / P0;

		P1 = 0;
		for (int i = threshold + 1; i < 256; i++)
			P1 += normalizedHistogram[i];

		u1 = 0;
		for (int i = threshold + 1; i < 256; i++)
			u1 += ((double)i * normalizedHistogram[i]) / P1;

		double wariancja0 = 0;
		for (int i = 0; i <= threshold; i++)
			wariancja0 += (pow(((double)i - u0), 2.f) * normalizedHistogram[i]) / P0;

		double wariancja1 = 0;
		for (int i = threshold; i < 256; i++)
			wariancja1 += (pow(((double)i - u1), 2.f) * normalizedHistogram[i]) / P1;

		double wariancjaw = (P0 * wariancja0) + (P1 * wariancja1);

		double jedenprzez = 1.f / wariancjaw;

		jedenprzesy[threshold] = jedenprzez;

		if (jedenprzez > newThreshold)
		{
			newThreshold = jedenprzez;
			x = threshold;
		}
	}

	newThreshold = x;

	double costam = 250.f / jedenprzesy[x];

	int jedenprzesyyy[256];

	for (int i = 0; i < 256; i++)
	{
		jedenprzesy[i] *= costam;
		jedenprzesyyy[i] = jedenprzesy[i];
		if (jedenprzesyyy[i] < 0)
		{
			jedenprzesyyy[i] = 0;
		}
	}

	// Tu juz nie bydzie otsu

	histogram.assign(luminescenceChannel, luminescenceChannel + 256);
	jedenprzesyv.assign(jedenprzesyyy, jedenprzesyyy + 256);

	if (maxValue > 300)
	{
		double calibrationCoefficient = 300.f / (double)maxValue;

		for (int i = 0; i < histogram.size(); i++)
		{
			histogram[i] = (double)histogram[i] * calibrationCoefficient;
		}
	}

	bool czarny = false;
	if (((minlum + maxlum) / 2) < 127)
		czarny = true;

	int threshold = m_slider1.GetPos();

	for (int i = startx; i <= endx; i++)
	{
		for (int j = starty; j <= endy; j++)
		{
			actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i, j);
			if (maxlum - minlum > threshold)
			{
				if (actualPixel8 <= newThreshold)
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 0);
				}
				else
				{
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 255);
				}
			}
			else
			{
				if (czarny)
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 0);
				else
					m_imgOUT.m_MyBitmap.SetPixel8(i, j, 255);
			}
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::median(bool is3, bool iscross)
{
	int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
	RGBTRIPLE actualPixel24;
	BYTE actualPixel8;
	bool actualPixel1;
	BYTE actualLuminance;

	if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
	{
		m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		int aa = 3;
		int bb = 1;

		if (!is3)
		{
			aa = 5;
			bb = 2;
		}

		for (int i =  bb; i < actualImgWidth - bb; i++)
		{
			for (int j = bb; j < actualImgHeight - bb; j++)
			{
				std::vector<BYTE> neighboursLuminance;

				for (int x = 0; x < aa; x++)
				{
					for (int y = 0; y < aa; y++)
					{
						int x2 = x - bb;
						int y2 = y - bb;

						if (iscross)
						{
							if ((x == 0 && y == 0) ||
								(x == 2 && y == 0) ||
								(x == 0 && y == 2) ||
								(x == 2 && y == 2))
							{
								continue;
							}
						}
						actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i - x2, j - y2);

						neighboursLuminance.push_back(actualPixel8);
					}
				}

				sort(neighboursLuminance.begin(), neighboursLuminance.end());

				double jw = 0;
				int middle = neighboursLuminance.size() / 2;

				if (neighboursLuminance.size() != 0)
				{
					jw = neighboursLuminance[middle];
				}

				m_imgOUT.m_MyBitmap.SetPixel8(i, j, jw);
			}
		}

		InvalidateRect(NULL, 1);
	}
}

void CPODlg::dylatacja1(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

		out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPix = in->GetPixel1(i, j);

				if (!actualPix)
				{
					out->SetPixel8(i, j, 0);

					//Top left
					if (j - 1 >= 0 && i - 1 >= 0)
						out->SetPixel8(i - 1, j - 1, 0);
					//Top mid
					if (j - 1 >= 0)
						out->SetPixel8(i, j - 1, 0);
					//Top right
					if (j - 1 >= 0 && i + 1 < actualImgWidth)
						out->SetPixel8(i + 1, j - 1, 0);
					//Left
					if (i - 1 >= 0)
						out->SetPixel8(i - 1, j, 0);
					//Right
					if (i + 1 < actualImgWidth)
						out->SetPixel8(i + 1, j, 0);
					//Bottom left
					if (j + 1 < actualImgHeight && i - 1 >= 0)
						out->SetPixel8(i - 1, j + 1, 0);
					//Bottom mid
					if (j + 1 < actualImgHeight)
						out->SetPixel8(i, j + 1, 0);
					//Bottom right
					if (j + 1 < actualImgHeight && i + 1 < actualImgWidth)
						out->SetPixel8(i + 1, j + 1, 0);
				}
				else
				{
					out->SetPixel8(i, j, 255);
				}
			}
		}

		InvalidateRect(NULL, 1);
}

void CPODlg::dylatacja1_hack(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			out->SetPixel8(i, j, 255);
		}
	}

	for (int i = 1; i < actualImgWidth - 1; i++)
	{
		for (int j = 1; j < actualImgHeight - 1; j++)
		{
			actualPix = in->GetPixel1(i, j);

			if (!actualPix)
			{
				
				out->SetPixel8(i - 1, j - 1, 0);
				out->SetPixel8(i    , j - 1, 0);
				out->SetPixel8(i + 1, j - 1, 0);
				out->SetPixel8(i - 1, j    , 0);
				out->SetPixel8(i + 1, j    , 0);
				out->SetPixel8(i, j, 0);
				out->SetPixel8(i - 1, j + 1, 0);
				out->SetPixel8(i    , j + 1, 0);
				out->SetPixel8(i + 1, j + 1, 0);
			}
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::dylatacja8(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;

	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			BYTE mask[3][3] = { 255, 255, 255, 255, 255, 255, 255, 255, 255 };

			mask[1][1] = in->GetPixel8(i, j);

			//Top left
			if (j - 1 >= 0 && i - 1 >= 0)
				mask[0][0] = in->GetPixel8(i - 1, j - 1);
			//Top mid
			if (j - 1 >= 0)
				mask[1][0] = in->GetPixel8(i, j - 1);
			//Top right
			if (j - 1 >= 0 && i + 1 < actualImgWidth)
				mask[2][0] = in->GetPixel8(i + 1, j - 1);
			//Left
			if (i - 1 >= 0)
				mask[0][1] = in->GetPixel8(i - 1, j);
			//Right
			if (i + 1 < actualImgWidth)
				mask[2][1] = in->GetPixel8(i + 1, j);
			//Bottom left
			if (j + 1 < actualImgHeight && i - 1 >= 0)
				mask[0][2] = in->GetPixel8(i - 1, j + 1);
			//Bottom mid
			if (j + 1 < actualImgHeight)
				mask[1][2] = in->GetPixel8(i, j + 1);
			//Bottom right
			if (j + 1 < actualImgHeight && i + 1 < actualImgWidth)
				mask[2][2] = in->GetPixel8(i + 1, j + 1);

			BYTE min = 255;
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					if (mask[x][y] < min)
						min = mask[x][y];
				}
			}

			out->SetPixel8(i, j, min);
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::erozja8(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;

	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			BYTE mask[3][3] = { 255, 255, 255, 255, 255, 255, 255, 255, 255 };

			mask[1][1] = in->GetPixel8(i, j);

			//Top left
			if (j - 1 >= 0 && i - 1 >= 0)
				mask[0][0] = in->GetPixel8(i - 1, j - 1);
			//Top mid
			if (j - 1 >= 0)
				mask[1][0] = in->GetPixel8(i, j - 1);
			//Top right
			if (j - 1 >= 0 && i + 1 < actualImgWidth)
				mask[2][0] = in->GetPixel8(i + 1, j - 1);
			//Left
			if (i - 1 >= 0)
				mask[0][1] = in->GetPixel8(i - 1, j);
			//Right
			if (i + 1 < actualImgWidth)
				mask[2][1] = in->GetPixel8(i + 1, j);
			//Bottom left
			if (j + 1 < actualImgHeight && i - 1 >= 0)
				mask[0][2] = in->GetPixel8(i - 1, j + 1);
			//Bottom mid
			if (j + 1 < actualImgHeight)
				mask[1][2] = in->GetPixel8(i, j + 1);
			//Bottom right
			if (j + 1 < actualImgHeight && i + 1 < actualImgWidth)
				mask[2][2] = in->GetPixel8(i + 1, j + 1);

			BYTE max = 0;
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					if (mask[x][y] > max)
						max = mask[x][y];
				}
			}

			out->SetPixel8(i, j, max);
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::erozja1(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

		out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				actualPix = in->GetPixel1(i, j);

				if (!actualPix)
				{
					bool mask[3][3] = { true, true, true, true, false, true, true, true, true };

					out->SetPixel8(i, j, 0);

					//Top left
					if (j - 1 >= 0 && i - 1 >= 0)
						mask[0][0] = in->GetPixel1(i - 1, j - 1);
					//Top mid
					if (j - 1 >= 0)
						mask[1][0] = in->GetPixel1(i, j - 1);
					//Top right
					if (j - 1 >= 0 && i + 1 < actualImgWidth)
						mask[2][0] = in->GetPixel1(i + 1, j - 1);
					//Left
					if (i - 1 >= 0)
						mask[0][1] = in->GetPixel1(i - 1, j);
					//Right
					if (i + 1 < actualImgWidth)
						mask[2][1] = in->GetPixel1(i + 1, j);
					//Bottom left
					if (j + 1 < actualImgHeight && i - 1 >= 0)
						mask[0][2] = in->GetPixel1(i - 1, j + 1);
					//Bottom mid
					if (j + 1 < actualImgHeight)
						mask[1][2] = in->GetPixel1(i, j + 1);
					//Bottom right
					if (j + 1 < actualImgHeight && i + 1 < actualImgWidth)
						mask[2][2] = in->GetPixel1(i + 1, j + 1);

					if ((!mask[0][0] && !mask[0][1] && !mask[0][2]) &&
						(!mask[1][0] && !mask[1][1] && !mask[1][2]) &&
						(!mask[2][0] && !mask[2][1] && !mask[2][2]))
					{
						out->SetPixel8(i, j, 0);
					}
					else
					{
						out->SetPixel8(i, j, 255);
					}
				}
				else
				{
					out->SetPixel8(i, j, 255);
				}
			}
		}

		InvalidateRect(NULL, 1);
}

void CPODlg::zamkniecie1(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	dylatacja1_hack(in, &tmp);
	erozja1(&tmp, out);
}

void CPODlg::kontur_wew(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);
	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	erozja1(in, &tmp);

	BYTE pix1;
	BYTE pix2;

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			pix1 = in->GetPixel1(i, j);
			pix2 = tmp.GetPixel1(i, j);
			if (pix1 != pix2)
			{
				out->SetPixel8(i, j, 0);
			}
			else
			{
				out->SetPixel8(i, j, 255);
			}
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::kontur_zew(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);
	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	dylatacja1_hack(in, &tmp);

	BYTE pix1;
	BYTE pix2;

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			pix1 = in->GetPixel1(i, j);
			pix2 = tmp.GetPixel1(i, j);
			if (pix2 == 0 && pix1 != pix2)
			{
				out->SetPixel8(i, j, 0);
			}
			else
			{
				out->SetPixel8(i, j, 255);
			}
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::otwarcie1(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	erozja1(in, &tmp);
	dylatacja1_hack(&tmp, out);
}

void CPODlg::otwarcie8(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	erozja8(in, &tmp);
	dylatacja8(&tmp, out);
}

void CPODlg::zamkniecie8(CMyBitmap *in, CMyBitmap *out)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	dylatacja8(in, &tmp);
	erozja8(&tmp, out);
}

void CPODlg::blackhat(CMyBitmap *in, CMyBitmap *out, float a)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);
	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	otwarcie8(in, &tmp);

	BYTE pix1;
	BYTE pix2;
	BYTE pixout;

	BYTE max = 0;

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			pix1 = in->GetPixel8(i, j);
			pix2 = tmp.GetPixel8(i, j);
			if (pix1 > pix2)
			{
				pixout = pix1 - pix2;
			}
			else
			{
				pixout = pix2 - pix1;
			}

			if (pixout > max)
				max = pixout;
		}
	}

	double factor = 255.f / (double)max;

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			pix1 = in->GetPixel8(i, j);
			pix2 = tmp.GetPixel8(i, j);
			if (pix1 > pix2)
			{
				pixout = pix1 - pix2;
			}
			else
			{
				pixout = pix2 - pix1;
			}

			double pix = (double)pixout * factor;

			pixout = pix;

			out->SetPixel8(i, j, pixout);
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::whitehat(CMyBitmap *in, CMyBitmap *out, float a)
{
	int actualImgWidth = in->getInfo()->bmiHeader.biWidth;
	int actualImgHeight = in->getInfo()->bmiHeader.biHeight;
	bool actualPix;
	BYTE outPix;

	CMyBitmap tmp;

	tmp.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);
	out->CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	zamkniecie8(in, &tmp);

	BYTE pix1;
	BYTE pix2;
	BYTE pixout;
	BYTE max = 0;

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			pix1 = in->GetPixel8(i, j);
			pix2 = tmp.GetPixel8(i, j);
			if (pix1 > pix2)
			{
				pixout = pix1 - pix2;
			}
			else
			{
				pixout = pix2 - pix1;
			}

			if (pixout > max)
				max = pixout;
		}
	}

	double factor = 255.f / (double)max;

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			pix1 = in->GetPixel8(i, j);
			pix2 = tmp.GetPixel8(i, j);
			if (pix1 > pix2)
			{
				pixout = pix1 - pix2;
			}
			else
			{
				pixout = pix2 - pix1;
			}

			double pix = (double)pixout * factor;

			pixout = pix;

			out->SetPixel8(i, j, pixout);
		}
	}

	InvalidateRect(NULL, 1);
}

bool CPODlg::pavlidisMask0Rot0(int x, int y)
{
	bool topRowOk = false;
	bool leftPixelOk = false;
	bool rightPixelOk = false;
	bool bottomRowOk = false;

	int actualImgWidth = m_imgOUT.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgOUT.m_MyBitmap.getInfo()->bmiHeader.biHeight;
	BYTE actualPixel8;

	//Top row
	if (y - 1 >= 0)
	{
		if (x - 1 >= 0)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y - 1);
			if (actualPixel8 == 0)
			{
				topRowOk = true;
			}
		}
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x, y - 1);
		if (actualPixel8 == 0)
		{
			topRowOk = true;
		}
		if (x + 1 < actualImgWidth)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y - 1);
			if (actualPixel8 == 0)
			{
				topRowOk = true;
			}
		}
	}
	//Bottom row
	if (y + 1 < actualImgHeight)
	{
		if (x - 1 >= 0)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y + 1);
			if (actualPixel8 == 0)
			{
				bottomRowOk = true;
			}
		}
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x, y + 1);
		if (actualPixel8 == 0)
		{
			bottomRowOk = true;
		}
		if (x + 1 < actualImgWidth)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y + 1);
			if (actualPixel8 == 0)
			{
				bottomRowOk = true;
			}
		}
	}

	//Left neighbour
	if (x - 1 >= 0)
	{
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y);
		if (actualPixel8 == 255)
		{
			leftPixelOk = true;
		}
	}
	else
	{
		leftPixelOk = true;
	}

	//Right neighbour
	if (x + 1 < actualImgWidth)
	{
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y);
		if (actualPixel8 == 255)
		{
			rightPixelOk = true;
		}
	}
	else
	{
		rightPixelOk = true;
	}

	return (topRowOk || bottomRowOk) && leftPixelOk && rightPixelOk;
}
bool CPODlg::pavlidisMask0Rot1(int x, int y)
{
	bool leftColumnOk = false;
	bool rightColumnOk = false;
	bool topPixelOk = false;
	bool bottomPixelOk = false;

	int actualImgWidth = m_imgOUT.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgOUT.m_MyBitmap.getInfo()->bmiHeader.biHeight;
	BYTE actualPixel8;

	//Left column
	if (x - 1 >= 0)
	{
		if (y - 1 >= 0)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y - 1);
			if (actualPixel8 == 0)
			{
				leftColumnOk = true;
			}
		}
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y);
		if (actualPixel8 == 0)
		{
			leftColumnOk = true;
		}
		if (y + 1 < actualImgHeight)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y + 1);
			if (actualPixel8 == 0)
			{
				leftColumnOk = true;
			}
		}
	}
	//Right column
	if (x + 1 < actualImgWidth)
	{
		if (y - 1 >= 0)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y - 1);
			if (actualPixel8 == 0)
			{
				rightColumnOk = true;
			}
		}
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y);
		if (actualPixel8 == 0)
		{
			rightColumnOk = true;
		}
		if (y + 1 < actualImgHeight)
		{
			actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y + 1);
			if (actualPixel8 == 0)
			{
				rightColumnOk = true;
			}
		}
	}

	//Top neighbour
	if (y - 1 >= 0)
	{
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x, y - 1);
		if (actualPixel8 == 255)
		{
			topPixelOk = true;
		}
	}
	else
	{
		topPixelOk = true;
	}

	//Bottom neighbour
	if (y + 1 < actualImgHeight)
	{
		actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(x, y + 1);
		if (actualPixel8 == 255)
		{
			bottomPixelOk = true;
		}
	}
	else
	{
		bottomPixelOk = true;
	}

	return (rightColumnOk || leftColumnOk) && topPixelOk && bottomPixelOk;
}
bool CPODlg::pavlidisMask1(int x, int y)
{
	int actualImgWidth = m_imgOUT.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgOUT.m_MyBitmap.getInfo()->bmiHeader.biHeight;
	BYTE actualPixel8;

	BYTE mask[3][3] = { 255, 255, 255, 255, 255, 255, 255, 255, 255 };

	//Top left
	if (y - 1 >= 0 && x - 1 >= 0)
	{
		mask[0][0] = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y - 1);
	}
	else
	{
		mask[0][0] = 255;
	}
	//Top mid
	if (y - 1 >= 0)
	{
		mask[0][1] = m_imgOUT.m_MyBitmap.GetPixel8(x, y - 1);
	}
	else
	{
		mask[0][1] = 255;
	}
	//Top right
	if (y - 1 >= 0 && x + 1 < actualImgWidth)
	{
		mask[0][2] = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y - 1);
	}
	else
	{
		mask[0][2] = 255;
	}

	//Left
	if (x - 1 >= 0)
	{
		mask[1][0] = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y);
	}
	else
	{
		mask[1][0] = 255;
	}

	//Right
	if (x + 1 < actualImgWidth)
	{
		mask[1][2] = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y);
	}
	else
	{
		mask[1][2] = 255;
	}

	//Bottom left
	if (y + 1 < actualImgHeight && x - 1 >= 0)
	{
		mask[2][0] = m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y + 1);
	}
	else
	{
		mask[2][0] = 255;
	}
	//Bottom mid
	if (y + 1 < actualImgHeight)
	{
		mask[2][1] = m_imgOUT.m_MyBitmap.GetPixel8(x, y + 1);
	}
	else
	{
		mask[2][1] = 255;
	}
	//Bottom right
	if (y + 1 < actualImgHeight && x + 1 < actualImgWidth)
	{
		mask[2][2] = m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y + 1);
	}
	else
	{
		mask[2][2] = 255;
	}

	bool pavlidis0 = false;
	bool xxx0 = false;
	bool zeros0 = false;
	bool ones0 = false;

	if (mask[0][0] == 0 ||
		mask[0][1] == 0 ||
		mask[0][2] == 0 ||
		mask[1][2] == 0 ||
		mask[2][2] == 0)
	{
		xxx0 = true;
	}

	if (mask[1][0] == 255 && mask[2][1] == 255)
	{
		zeros0 = true;
	}

	if (mask[2][0] == 0)
	{
		ones0 = true;
	}

	pavlidis0 = xxx0 && zeros0 && ones0;

	bool pavlidis1 = false;
	bool xxx1 = false;
	bool zeros1 = false;
	bool ones1 = false;

	if (mask[0][2] == 0 ||
		mask[1][2] == 0 ||
		mask[2][2] == 0 ||
		mask[2][0] == 0 ||
		mask[2][1] == 0)
	{
		xxx1 = true;
	}

	if (mask[0][1] == 255 && mask[1][0] == 255)
	{
		zeros1 = true;
	}

	if (mask[0][0] == 0)
	{
		ones1 = true;
	}

	pavlidis1 = xxx1 && zeros1 && ones1;

	bool pavlidis2 = false;
	bool xxx2 = false;
	bool zeros2 = false;
	bool ones2 = false;

	if (mask[0][0] == 0 ||
		mask[1][0] == 0 ||
		mask[2][0] == 0 ||
		mask[2][1] == 0 ||
		mask[2][2] == 0)
	{
		xxx2 = true;
	}

	if (mask[0][1] == 255 && mask[1][2] == 255)
	{
		zeros2 = true;
	}

	if (mask[0][2] == 0)
	{
		ones2 = true;
	}

	pavlidis2 = xxx2 && zeros2 && ones2;

	bool pavlidis3 = false;
	bool xxx3 = false;
	bool zeros3 = false;
	bool ones3 = false;

	if (mask[0][0] == 0 ||
		mask[0][1] == 0 ||
		mask[0][2] == 0 ||
		mask[1][0] == 0 ||
		mask[2][0] == 0)
	{
		xxx3 = true;
	}

	if (mask[1][2] == 255 && mask[2][1] == 255)
	{
		zeros3 = true;
	}

	if (mask[2][2] == 0)
	{
		ones3 = true;
	}

	pavlidis3 = xxx3 && zeros3 && ones3;

	return pavlidis0 || pavlidis1 || pavlidis2 || pavlidis3;
}

void CPODlg::pavlidis()
{
	int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
	RGBTRIPLE actualPixel24;
	BYTE actualPixel8;
	bool actualPixel1;
	BYTE actualLuminance;

	if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount != 8)
	{
		return;
	}
	m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

	for (int i = 0; i < actualImgWidth; i++)
	{
		for (int j = 0; j < actualImgHeight; j++)
		{
			m_imgOUT.m_MyBitmap.SetPixel8(i, j, m_imgIN.m_MyBitmap.GetPixel8(i, j));
		}
	}

	std::vector<CPixel> repeatableGlobal;

	bool dupa = true;
	int kkkkk = 0;
	while (dupa)
	{
		for (int kjsdfkjsdffs = 0; kjsdfkjsdffs < 4; kjsdfkjsdffs++)
		{
			std::vector<CPixel> border;
			//Get border
			for (int i = 0; i < actualImgWidth; i++)
			{
				for (int j = 0; j < actualImgHeight; j++)
				{
					actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(i, j);
					if (actualPixel8 == 0)
					{
						if (kjsdfkjsdffs == 2)
						{
							if (i - 1 >= 0)
							{
								actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(i - 1, j);
								if (actualPixel8 == 255)
								{
									CPixel pix(i, j);
									border.push_back(pix);
								}
							}
							else
							{
								CPixel pix(i, j);
								border.push_back(pix);
							}
						}
						if (kjsdfkjsdffs == 0)
						{
							if (i + 1 < actualImgWidth)
							{
								actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(i + 1, j);
								if (actualPixel8 == 255)
								{
									CPixel pix(i, j);
									border.push_back(pix);
								}
							}
							else
							{
								CPixel pix(i, j);
								border.push_back(pix);
							}
						}
						if (kjsdfkjsdffs == 1)
						{
							if (j - 1 >= 0)
							{
								actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(i, j - 1);
								if (actualPixel8 == 255)
								{
									CPixel pix(i, j);
									border.push_back(pix);
								}
							}
							else
							{
								CPixel pix(i, j);
								border.push_back(pix);
							}
						}
						if (kjsdfkjsdffs == 3)
						{
							if (j + 1 < actualImgHeight)
							{
								actualPixel8 = m_imgOUT.m_MyBitmap.GetPixel8(i, j + 1);
								if (actualPixel8 == 255)
								{
									CPixel pix(i, j);
									border.push_back(pix);
								}
							}
							else
							{
								CPixel pix(i, j);
								border.push_back(pix);
							}
						}
					}
				}
			}

			std::vector<CPixel> repeatable;

			for (int i = 0; i < border.size(); i++)
			{
				if (pavlidisMask0Rot0(border[i].x, border[i].y) ||
					pavlidisMask0Rot1(border[i].x, border[i].y) ||
					pavlidisMask1(border[i].x, border[i].y))
				{
					repeatable.push_back(border[i]);
				}
			}

			repeatableGlobal.insert(repeatableGlobal.end(), repeatable.begin(), repeatable.end());

			std::vector<CPixel> output;
			for (int i = 0; i < border.size(); i++)
			{
				bool exist = false;

				for (int j = 0; j < repeatableGlobal.size(); j++)
				{
					if (border[i].x == repeatableGlobal[j].x &&
						border[i].y == repeatableGlobal[j].y)
					{
						exist = true;
						break;
					}
				}

				if (!exist)
				{
					output.push_back(border[i]);
				}
			}

			if (output.size() == 0)
			{
				dupa = false;
				break;
			}
			else
			{
				for (int i = 0; i < output.size(); i++)
				{
					m_imgOUT.m_MyBitmap.SetPixel8(output[i].x, output[i].y, 255);
				}
			}
			kkkkk++;

			if (kkkkk == m_slider1.GetPos())
			{
				dupa = false;
				break;
			}
				
		}
	}

	InvalidateRect(NULL, 1);
}

void CPODlg::pavlidisNew()
{
    int imgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
    int imgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;

    m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, imgWidth, imgHeight), 0, 0);
        
    std::vector<std::pair<int, int>> kontur;
    std::vector<std::vector<float>> maska;
    std::vector<std::vector<std::pair<int, int>>::iterator> wywal;
    std::vector<std::vector<bool>> powtarzalne;

    powtarzalne.resize(imgWidth + 1);
    for (int i = 0; i < imgWidth; i++)
    {
        powtarzalne[i].resize(imgHeight + 1, false);
    }

    //copy image
    for (int x = 0; x < imgWidth; x++)
    {
        for (int y = 0; y <= imgHeight; y++)
        {
            m_imgOUT.m_MyBitmap.SetPixel8(x, y, m_imgIN.m_MyBitmap.GetPixel8(x, y));
        }
    }

    bool dzialaj = true;
    while (dzialaj)
    {
        bool neighbourhood[3][3];
        bool currRedundant = false;

        for (int sasiad = 0; sasiad < 4; sasiad++)
        {
            for (int x = 0; x < imgWidth; x++)
            {
                for (int y = 0; y < imgHeight; y++)
                {
                    if (!powtarzalne[x][y] && m_imgOUT.m_MyBitmap.GetPixel8(x, y) == 0 &&
                        ((m_imgOUT.m_MyBitmap.GetPixel8(x - 1, y) != 0 && sasiad == 0) ||
                        (m_imgOUT.m_MyBitmap.GetPixel8(x + 1, y) != 0 && sasiad == 1) ||
                        (m_imgOUT.m_MyBitmap.GetPixel8(x, y - 1) != 0 && sasiad == 2) ||
                        (m_imgOUT.m_MyBitmap.GetPixel8(x, y + 1) != 0 && sasiad == 3)))
                    {
                        std::pair<int, int>* pair = new std::pair<int, int>(x, y);

                        neighbourhood[0][0] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first - 1, pair->second - 1) == 0;
                        neighbourhood[1][0] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first, pair->second - 1) == 0;
                        neighbourhood[2][0] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first + 1, pair->second - 1) == 0;
                        neighbourhood[0][1] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first - 1, pair->second) == 0;
                        neighbourhood[2][1] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first + 1, pair->second) == 0;
                        neighbourhood[0][2] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first - 1, pair->second + 1) == 0;
                        neighbourhood[1][2] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first, pair->second + 1) == 0;
                        neighbourhood[2][2] = m_imgOUT.m_MyBitmap.GetPixel8(pair->first + 1, pair->second + 1) == 0;
                        currRedundant = false;
                        if (((neighbourhood[0][0] || neighbourhood[1][0] || neighbourhood[2][0]) &&
                            (neighbourhood[0][2] || neighbourhood[1][2] || neighbourhood[2][2])) &&
                            (!neighbourhood[0][1] && !neighbourhood[2][1]))
                            currRedundant = true;
                        else if (((neighbourhood[0][0] || neighbourhood[0][1] || neighbourhood[0][2]) &&
                            (neighbourhood[2][0] || neighbourhood[2][1] || neighbourhood[2][2])) &&
                            (!neighbourhood[1][0] && !neighbourhood[1][2]))
                            currRedundant = true;
                        else if (neighbourhood[0][0] && (!neighbourhood[1][0] && !neighbourhood[0][1]) &&
                            (neighbourhood[2][0] || neighbourhood[2][1] || neighbourhood[2][2] || neighbourhood[0][2] || neighbourhood[1][2]))
                            currRedundant = true;
                        else if (neighbourhood[2][0] && (!neighbourhood[1][0] && !neighbourhood[2][1]) &&
                            (neighbourhood[0][0] || neighbourhood[0][1] || neighbourhood[0][2] || neighbourhood[1][2] || neighbourhood[2][2]))
                            currRedundant = true;
                        else if (neighbourhood[2][2] && (!neighbourhood[2][1] && !neighbourhood[1][2]) &&
                            (neighbourhood[0][0] || neighbourhood[1][0] || neighbourhood[2][0] || neighbourhood[0][1] || neighbourhood[0][2]))
                            currRedundant = true;
                        else if (neighbourhood[0][2] && (!neighbourhood[0][1] && !neighbourhood[1][2]) &&
                            (neighbourhood[0][0] || neighbourhood[1][0] || neighbourhood[2][0] || neighbourhood[2][1] || neighbourhood[2][2]))
                            currRedundant = true;

                        if (!currRedundant)
                        {
                            kontur.push_back(std::pair<int, int>(x, y));
                        }
                        else
                        {
                            powtarzalne[x][y] = true;
                        }

                        delete pair;
                    }
                }
            }

            if (kontur.empty())
            {
                dzialaj = false;
                break;
            }

            for (auto pair : kontur)
            {
                m_imgOUT.m_MyBitmap.SetPixel8(pair.first, pair.second, 255);
            }

            kontur.clear();
            wywal.clear();
        }

		Sleep(30);
		CPaintDC dc(&m_imgOUT);
		CRect rect;
		m_imgOUT.GetClientRect(&rect);
		m_imgOUT.m_MyBitmap.PaintDIB(dc, rect, CRect(0, 0, 100, 100));
		m_imgOUT.OnPaint();
		InvalidateRect(NULL, 1);
    }

    InvalidateRect(NULL, 1);
}

int CPODlg::getNearestPixel(int x, int y)
{
    int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
    int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
    BYTE actualPixel = m_imgIN.m_MyBitmap.GetPixel8(x, y);

    if (actualPixel == 255)
    {
        return 0;
    }

    int findX = x;
    int findY = y;
    bool found = false;
    int actualSpiralSize = 3;
    int direction = 0;
    int step = 0;

    while ((findX >= 0 && findX < actualImgWidth) &&
           (findY >= 0 && findY < actualImgHeight) &&
           !found)
    {
        if (direction == 0)
        {
            if (step == 0)
            {
                findX += 1;
            }
            else if (step < actualSpiralSize - 1)
            {
                findY += 1;
            }
            else
            {
                step = 0;
                direction += 1;
                findX -= 1;
            }
        }
        else if (direction == 1)
        {
            if (step < actualSpiralSize - 1)
            {
                findX -= 1;
            }
            else
            {
                step = 0;
                direction += 1;
                findY -= 1;
            }
        }
        else if (direction == 2)
        {
            if (step < actualSpiralSize - 1)
            {
                findY -= 1;
            }
            else
            {
                step = 0;
                direction += 1;
                findX += 1;
            }
        }
        else if (direction == 3)
        {
            if (step < actualSpiralSize - 1)
            {
                findX += 1;
            }
            else
            {
                step = 0;
                direction = 0;
                findX += 1;
                actualSpiralSize += 2;
            }
        }

        step++;

        actualPixel = m_imgIN.m_MyBitmap.GetPixel8(findX, findY);
        if (actualPixel == 255)
        {
            found = true;
            break;
        }
    }

    if (found)
    {
        double width = abs(x - findX);
        double height = abs(y - findY);
        double dist = sqrt(pow(width, 2.f) + pow(height, 2.f));

        return (int)dist;
    }
    else
    {
        return 0;
    }
}

void CPODlg::distanceMap()
{
    int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
    int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
    BYTE actualPixel8;

    if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount != 8)
    {
        return;
    }

    this->distanceMapping = std::vector<std::vector<int> >(actualImgWidth);

    for (int i = 0; i < this->distanceMapping.size(); i++)
    {
        this->distanceMapping[i] = std::vector<int>(actualImgHeight, 0);
    }

    maxDistance = 0;

    for (int i = 0; i < actualImgWidth; i++)
    {
        for (int j = 0; j < actualImgHeight; j++)
        {
            this->distanceMapping[i][j] = getNearestPixel(i, j);
            if(maxDistance < this->distanceMapping[i][j])
                maxDistance = this->distanceMapping[i][j];
        }
    }

    m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

    double factor = 255.f / (double)maxDistance;

    for (int i = 0; i < actualImgWidth; i++)
    {
        for (int j = 0; j < actualImgHeight; j++)
        {
            BYTE dupsko = 0;
            int distance = (int)((double)this->distanceMapping[i][j] * factor);
            if (distance >= 255)
                distance = 255;
            
            dupsko = 255 - distance;

            m_imgOUT.m_MyBitmap.SetPixel8(i, j, dupsko);
        }
    }
    
    m_slider1.SetRange(0, maxDistance, true);

    InvalidateRect(NULL, 1);

    distanceMapped = true;
}

void CPODlg::drawDistanceMap(int threshold, bool binary)
{
    if(!distanceMapped)
        return;

    int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
    int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;

    m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

    double factor = 255.f / (double)maxDistance;

    for (int i = 0; i < actualImgWidth; i++)
    {
        for (int j = 0; j < actualImgHeight; j++)
        {
            BYTE dupsko = 0;
            int distance = this->distanceMapping[i][j];
            int distanceNormalized = (int)((double)distance * factor);
            if (distanceNormalized >= 255)
                distanceNormalized = 255;

            dupsko = 255 - distanceNormalized;

            if (binary)
            {
                if (distance >= threshold)
                {
                    dupsko = 0;
                }
                else
                {
                    dupsko = 255;
                }
            }
            else
            {
                if (distance < threshold)
                {
                    dupsko = 255;
                }
            }

            m_imgOUT.m_MyBitmap.SetPixel8(i, j, dupsko);
        }
    }

    CPaintDC dc(&m_imgOUT);
    CRect rect;
    m_imgOUT.GetClientRect(&rect);
    m_imgOUT.m_MyBitmap.PaintDIB(dc, rect, CRect(0, 0, 100, 100));

    InvalidateRect(NULL, 1);
}

void CPODlg::log()
{
	int actualImgWidth = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biWidth;
	int actualImgHeight = m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biHeight;
	RGBTRIPLE actualPixel24;
	BYTE actualPixel8;
	bool actualPixel1;
	BYTE actualLuminance;

	double factor =  (double)m_slider1.GetPos() / 10.0f;

	if (m_imgIN.m_MyBitmap.getInfo()->bmiHeader.biBitCount == 8)
	{
		m_imgOUT.m_MyBitmap.CreateGreyscaleDIB(new CRect(0, 0, actualImgWidth, actualImgHeight), 0, 0);

		int maskWidth = 1 + (2 * ceil(2.5f * factor));

		std::vector<std::vector<double> > sombrero;

		double sum = 0;
		double absSum = 0;

		for (int i = -(maskWidth / 2); i < (maskWidth / 2) + 1; i++)
		{
			std::vector<double> xvec;
			for (int j = -(maskWidth / 2); j < (maskWidth / 2) + 1; j++)
			{
				double expPower = -((pow(i, 2.0f) + pow(j, 2.0f)) / (2.f * pow(factor, 2.f)));
				double eee = exp(expPower);
				double yyy = -((pow(i, 2.f) + pow(j, 2.f) - (2.f * pow(factor, 2.f))) / (pow(factor, 4.f)));
				double uuu = yyy * eee;

				sum += uuu;
				absSum += abs(uuu);

				xvec.push_back(uuu);
			}
			sombrero.push_back(xvec);
		}

		double multiplier = sombrero.operator[](0).operator[](maskWidth / 2) / (-1.f);

		sum = 0;

		for (int i = 0; i < maskWidth; i++)
		{
			for (int j = 0; j < maskWidth; j++)
			{
				(sombrero.operator[](i)).operator[](j) /= (multiplier);
				sum += (sombrero.operator[](i)).operator[](j);
			}
		}

		for (int i = 0; i < actualImgWidth; i++)
		{
			for (int j = 0; j < actualImgHeight; j++)
			{
				double jw = 0;
				for (int x = 0 - (maskWidth / 2); x < (maskWidth / 2); x++)
				{
					for (int y = 0 - (maskWidth / 2); y < (maskWidth / 2); y++)
					{
						if (i - x < 0 ||
							i - x > actualImgWidth - 1 ||
							j - y < 0 ||
							j - y > actualImgWidth - 1)
						{
							continue;
						}
						actualPixel8 = m_imgIN.m_MyBitmap.GetPixel8(i - x, j - y);

						jw += (double)actualPixel8 * (sombrero.operator[](((-x) + (maskWidth / 2)) - 1)).operator[]((-y) + (maskWidth / 2) - 1);
					}
				}

				jw += 127;

				if (jw > 255)
				{
					jw = 255;
				}
				if (jw < 0)
				{
					jw = 0;
				}

				m_imgOUT.m_MyBitmap.SetPixel8(i, j, jw);
			}
		}

		InvalidateRect(NULL, 1);
	}
}


void CPODlg::OnBnClickedButtonSave()
{
	WCHAR strFilter[] = { L"Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(FALSE, NULL, NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		CString path = FileDlg.GetFolderPath();
		path.Append(L"\\");
		path.Append(FileDlg.GetFileName());

		m_imgOUT.m_MyBitmap.SaveDIB(path);
	}
}


void CPODlg::OnBnClickedButtonParams()
{
	CParamsDlg paramsDlg;
	CString s;
	
	if (paramsDlg.DoModal() == IDOK)
	{
		s = paramsDlg.m_sParams;
	}
}


void CPODlg::OnCbnEditchangeCombo1()
{

}


void CPODlg::OnCbnSelchangeCombo1()
{
	CString sOption;
	m_combo1.GetLBText(m_combo1.GetCurSel(), sOption);


	if (sOption == L"1convert to greyscale")
	{
		m_slider1.SetRange(0, 0, true);
	}
	else if (sOption == L"1change brightness")
	{
		m_slider1.SetRange(-100, 100, true);
	}
	else if (sOption == L"1change contrast")
	{
		m_slider1.SetRange(1, 50, true);
	}
	else if (sOption == L"1powering :D")
	{
		m_slider1.SetRange(1, 30, true);
	}
	else if (sOption == L"1negative")
	{
		m_slider1.SetRange(0, 0, true);
	}
	else if (sOption == L"1show histogram")
	{
		m_slider1.SetRange(0, 0, true);
	}
	else if (sOption == L"1align histogram")
	{
		m_slider1.SetRange(0, 0, true);
	}
	else if (sOption == L"2binarize - simple"    ||
			 sOption == L"2binarize - iterative" ||
			 sOption == L"2binarize - gradient"  ||
			 sOption == L"2binarize - otsu")
	{
		m_slider1.SetRange(0, 255, true);
	}
	else if (sOption == L"3LoG")
	{
		m_slider1.SetRange(6, 24, true);
	}
	else if (sOption == L"6Hough")
	{
		m_slider1.SetRange(1, 20, true);
	}
}


void CPODlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    drawDistanceMap(m_slider1.GetPos(), true);

    *pResult = 0;
}
