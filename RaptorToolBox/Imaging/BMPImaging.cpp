// BMPImaging.cpp: implementation of the CBMPImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_BMPIMAGING_H__C9C42555_D3BF_42BE_8CC2_FA35410B79AD__INCLUDED_)
	#include "Imaging/BMPImaging.h"
#endif

CBMPImaging::CBMPImaging(void)
{
}

CBMPImaging::~CBMPImaging(void)
{
}

bool CBMPImaging::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return ("BMP" == ext);
}

bool CBMPImaging::isOfKind(const std::string &kind) const 
{
	return _isOfKind(kind); 
}

std::vector<std::string> CBMPImaging::getImageKind(void) const
{
	std::vector<std::string> result;
	result.push_back("BMP");
	return result;
}

bool CBMPImaging::storeImageFile(const std::string&, CImage* const I) const
{
	return false;
}

DWORD ColorTableLength(BITMAPINFOHEADER& h)
{
	DWORD result = 0;

	DWORD biClrUsed = h.biClrUsed;
	WORD  biBitCount = h.biBitCount;
	DWORD biCompression = h.biCompression;

	switch (biBitCount)
	{
		case 24:
			result = biClrUsed;
			break;
		case 16:
		case 32:
			if (biCompression == BI_RGB)
				result = biClrUsed;
			else if (biCompression == BI_BITFIELDS)
				result = 3;
			break;
		default: // for 0, 1, 2, 4, and 8
			if (biClrUsed == 0)
				result = (1 << biBitCount); // 2^biBitCount
			else
				result = biClrUsed;
			break;
	}

	return result;
}

bool CBMPImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
#ifdef WIN32
	HANDLE h = LoadImage(NULL, fname.data(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if ((NULL == h) || (NULL == I))
		return false;

	HBITMAP bmp = (HBITMAP)h;

	HDC hdc = GetDC(NULL);
	BITMAPINFO bi;
	memset(&bi, 0, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(hdc, bmp, 0, 0, NULL, &bi, DIB_RGB_COLORS);

	DWORD ctsize = ColorTableLength(bi.bmiHeader);
	BITMAPINFO *lpbi = (BITMAPINFO *)new uint8_t[bi.bmiHeader.biSize + ctsize * sizeof(RGBQUAD)];
	lpbi->bmiHeader = bi.bmiHeader;
	
	int size = bi.bmiHeader.biBitCount / 8;
	int dim = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight;

	I->allocatePixels(bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, CImage::CGL_COLOR24_ALPHA);
	unsigned char *texturedata = (unsigned char*)I->getPixels();
	unsigned char *buffer = new unsigned char[dim*size];
	
	if (bi.bmiHeader.biHeight != GetDIBits(hdc, bmp, 0, bi.bmiHeader.biHeight, buffer, lpbi, DIB_RGB_COLORS))
		return false;

	int t_pos = 0;
	int b_pos = 0;
	
	for (int i = 0; i < dim; i++)
	{
		switch (size)
		{
			case 3:
			case 4:	// most 32 bits TGA images are defined with alpha as 0 !!!, so manage like 24bits
			{
				texturedata[t_pos] = buffer[b_pos];
				texturedata[t_pos + 1] = buffer[b_pos + 1];
				texturedata[t_pos + 2] = buffer[b_pos + 2];
				texturedata[t_pos + 3] = 0xFF;	// default value to ensure image is visible
				break;
			}
			default:
			{
				texturedata[t_pos] = 0xFF;
				texturedata[t_pos + 1] = 0xFF;
				texturedata[t_pos + 2] = 0xFF;
				texturedata[t_pos + 3] = 0xFF;	// default value to ensure image is visible
			}
		}

		b_pos += size;
		t_pos += 4;
	}

	delete[] buffer;
	ReleaseDC(NULL, hdc);
	DeleteObject(h);
	delete[] lpbi;

	return true;
#else
	return false;
#endif
}

