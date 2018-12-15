#pragma once

// CBitmapOwn command target

class CMyBitmap : public CObject
{
public:
	CMyBitmap();
	virtual ~CMyBitmap();

	bool initialized;

	BITMAPFILEHEADER Header;
	unsigned char *pData;

	BITMAPINFO* getInfo();
	BYTE* getDataStartPtr();
	long int getBytePosition(int colorBits);
	unsigned char getRGBLuminance(unsigned char r, unsigned char g, unsigned char b);
	unsigned char getRGBLuminance(int colorIdx);

	bool LoadDIB(CString filename);
	bool PaintDIB(HDC ctx, CRect dest, CRect src);
	bool CreateGreyscaleDIB(CRect size, int xPPM, int yPPM);
	bool SaveDIB(CString path);
	bool GetPixel1(int x, int y);
	BYTE GetPixel8(int x, int y);
	RGBTRIPLE GetPixel24(int x, int y);

	bool SetPixel8(int x, int y, BYTE val);
};


