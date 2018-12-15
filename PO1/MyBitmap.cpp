// MyBitmap.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "MyBitmap.h"


// CBitmapOwn

CMyBitmap::CMyBitmap()
{
	initialized = false;
}

CMyBitmap::~CMyBitmap()
{
}

bool CMyBitmap::LoadDIB(CString filename)
{
	CFile file;

	file.Open(filename, CFile::modeReadWrite);
	file.Read(&Header, sizeof(BITMAPFILEHEADER));

	int dataSize = Header.bfSize - sizeof(BITMAPFILEHEADER);

	pData = (unsigned char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dataSize);

	file.Read(pData, dataSize);

	initialized = true;

	return true;
}

bool CMyBitmap::PaintDIB(HDC ctx, CRect dest, CRect src)
{
	if (!initialized)
		return true;

	double boxWidth = dest.right;
	double boxHeight = dest.bottom;
	int picWidth = getInfo()->bmiHeader.biWidth;
	int picHeight = getInfo()->bmiHeader.biHeight;
	double scaleRatio = (double)picWidth / (double)picHeight;

	SetStretchBltMode(ctx, COLORONCOLOR);
	if (scaleRatio < 1)
	{
		boxWidth *= scaleRatio;
	}
	else
	{
		boxHeight /= scaleRatio;
	}

	StretchDIBits(ctx,
		dest.left,
		dest.top,
		boxWidth,
		boxHeight,
		0,
		0,
		picWidth,
		picHeight,
		getDataStartPtr(),
		getInfo(),
		DIB_RGB_COLORS,
		SRCCOPY);

	return true;
}

bool CMyBitmap::SaveDIB(CString path)
{
	CFile file;

	if (!initialized)
		return true;

	file.Open(path, CFile::modeCreate | CFile::modeWrite);
	file.Write(&Header, sizeof(BITMAPFILEHEADER));
	file.Write(pData, Header.bfSize - sizeof(BITMAPFILEHEADER));
	file.Close();

	return true;
}

bool CMyBitmap::CreateGreyscaleDIB(CRect size, int xPPM, int yPPM)
{
	Header.bfReserved1 = 0;
	Header.bfReserved2 = 0;
	((char*)&Header.bfType)[0] = 'B';
	((char*)&Header.bfType)[1] = 'M';

	long int bitmapLineSize = ((long int)(8 * size.right + 31 / 32)) * 4;
	long int bitmapSizeBytes = size.bottom * bitmapLineSize;
	pData = (BYTE*)malloc(sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+bitmapSizeBytes);
	memset(pData, 0, sizeof(BITMAPINFOHEADER)+256 * sizeof(RGBQUAD)+bitmapSizeBytes);

	getInfo()->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	getInfo()->bmiHeader.biWidth = size.right;
	getInfo()->bmiHeader.biHeight = size.bottom;
	getInfo()->bmiHeader.biPlanes = 1;
	getInfo()->bmiHeader.biBitCount = 8;
	getInfo()->bmiHeader.biCompression = BI_RGB;
	getInfo()->bmiHeader.biSizeImage = 0;
	getInfo()->bmiHeader.biXPelsPerMeter = xPPM;
	getInfo()->bmiHeader.biYPelsPerMeter = yPPM;
	getInfo()->bmiHeader.biClrUsed = 0;
	getInfo()->bmiHeader.biClrImportant = 0;
	for (int i = 0; i < 256; i++)
	{
		getInfo()->bmiColors[i].rgbBlue = i;
		getInfo()->bmiColors[i].rgbGreen = i;
		getInfo()->bmiColors[i].rgbRed = i;
		getInfo()->bmiColors[i].rgbReserved = 0;
	}

	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	Header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + bitmapSizeBytes;

	initialized = true;

	return true;
}

bool CMyBitmap::GetPixel1(int x, int y)
{
	if (getInfo()->bmiHeader.biBitCount == 1)
	{
		long int bitPos = y * getBytePosition(1) + (x / 8);
		BYTE byte = (getDataStartPtr())[bitPos];
		bool bit = (bool)(byte & (0x80 >> (x % 8)));

		return bit;
	}
	if (getInfo()->bmiHeader.biBitCount == 8)
	{
		long int bytePos = y * getBytePosition(8) + x;
		BYTE byte = (getDataStartPtr())[bytePos];

		if (byte == 255)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

BYTE CMyBitmap::GetPixel8(int x, int y)
{
	long int bytePos = y * getBytePosition(8) + x;
	BYTE byte = (getDataStartPtr())[bytePos];

	return byte;
}

RGBTRIPLE CMyBitmap::GetPixel24(int x, int y) {
	long int bytePos = y * getBytePosition(24) + (x * 3);
	BYTE *pFirstByte = &(getDataStartPtr())[bytePos];
	RGBTRIPLE *pRetval = (RGBTRIPLE*)pFirstByte;

	return *pRetval;
}

bool CMyBitmap::SetPixel8(int x, int y, BYTE val)
{
	long int bytePos = y * getBytePosition(8) + x;

	(getDataStartPtr())[bytePos] = val;

	return true;
}

long int CMyBitmap::getBytePosition(int colorBits) {
	return (((long int)(colorBits * getInfo()->bmiHeader.biWidth + 31)) / 32) * 4;
}

unsigned char CMyBitmap::getRGBLuminance(unsigned char r, unsigned char g, unsigned char b) {
	double retval = (((double)r) * 0.299f) + (((double)g) * 0.587f) + (((double)b) * 0.114f);

	if (retval > 255)
		retval = 255;

	return (unsigned char)retval;
}

unsigned char CMyBitmap::getRGBLuminance(int colorIdx) {
	double r = getInfo()->bmiColors[colorIdx].rgbRed;
	double g = getInfo()->bmiColors[colorIdx].rgbGreen;
	double b = getInfo()->bmiColors[colorIdx].rgbBlue;

	double retval = (r * 0.299f) + (g * 0.587f) + (b * 0.114f);

	return (unsigned char)retval;
}

BITMAPINFO* CMyBitmap::getInfo()
{
	return (BITMAPINFO*)pData;
}

BYTE* CMyBitmap::getDataStartPtr()
{
	return ((BYTE*)pData + Header.bfOffBits - sizeof(Header));
}

// CBitmapOwn member functions
