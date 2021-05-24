// ICOImaging.cpp: implementation of the CICOImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_)
	#include "Imaging/ICOImaging.h"
#endif

#include <algorithm>


CICOImaging::CICOImaging(void)
{
}

CICOImaging::~CICOImaging(void)
{
}

bool CICOImaging::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return (("ICO" == ext) ||
			("DLL" == ext) ||
			("EXE" == ext));
}

bool CICOImaging::isOfKind(const std::string &kind) const
{
	return _isOfKind(kind);
}

std::vector<std::string> CICOImaging::getImageKind(void) const
{
	std::vector<std::string> result;

	result.push_back("ICO");
	result.push_back("DLL");
	result.push_back("EXE");

	return result;
}

bool CICOImaging::storeImageFile(const std::string& fname, CImage* const T) const
{
	return false;
}

bool CICOImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
	if (I == NULL)
		return false;

#ifdef WIN32
	HICON hh = ::ExtractIcon(0, fname.data(), (UINT)-1);
	if (NULL == hh)
		return false;
	
	HDC hdc = GetDC(NULL);
	ICONINFO	ii;

	size_t width = 0;
	size_t height = 0;

	size_t nbIcons = (size_t)hh;
	for (size_t i = 0; i < nbIcons; i++)
	{
		if ((hh = ExtractIcon(0, fname.data(), i)) == NULL)
			return false;
		else
		{
			if (0 == GetIconInfo(hh, &ii))
			{
				::DestroyIcon(hh);
				return false;
			}

			BITMAPINFO bi;
			memset(&bi, 0, sizeof(BITMAPINFO));
			bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			int info = GetDIBits(hdc, ii.hbmColor, 0, 0, NULL, &bi, DIB_RGB_COLORS);
			::DestroyIcon(hh);
			if (ii.hbmColor != NULL)
				DeleteObject(ii.hbmColor);
			if (ii.hbmMask != NULL)
				DeleteObject(ii.hbmMask);

			if (0 != info)
			{
				width = MAX(width, (size_t)bi.bmiHeader.biWidth);
				height = MAX(height, (size_t)bi.bmiHeader.biHeight);
			}
			else
				return false;
		}
	}

	//	Allocate an image array, one layer per icon.
	I->allocatePixels(width, height, nbIcons, CImage::CGL_COLOR24_ALPHA);

	for (size_t icon = 0; icon < nbIcons; icon++)
	{
		if ((hh = ExtractIcon(0, fname.data(), icon)) == NULL)
			return false;

		if (0 == GetIconInfo(hh, &ii))
		{
			::DestroyIcon(hh);
			return false;
		}

		BITMAPINFO bi;
		memset(&bi, 0, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc, ii.hbmColor, 0, 0, NULL, &bi, DIB_RGB_COLORS);
		
		int size = bi.bmiHeader.biBitCount / 8;
		int dim = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight;
		bi.bmiHeader.biCompression = BI_RGB;	// Extract RGB.
		
		unsigned char *bits = new unsigned char[dim*size];
		memset(bits, 0, dim*size);
		int nbread = GetDIBits(hdc, ii.hbmColor, 0, bi.bmiHeader.biHeight, bits, &bi, DIB_RGB_COLORS);
		if (nbread != bi.bmiHeader.biHeight)
		{
			::DestroyIcon(hh);
			return false;
		}

		unsigned char *texturedata = (unsigned char*)I->getPixels(icon);

		if (size == 2)
		{
			for (int j = 0; j < bi.bmiHeader.biHeight; j++)
				for (int i = 0; i < bi.bmiHeader.biWidth; i++)
				{
					int pos = 4 * (bi.bmiHeader.biWidth - i - 1 + bi.bmiHeader.biWidth*j);
					int pos2 = 2 * (i + j*bi.bmiHeader.biWidth);

					texturedata[pos] = (unsigned char)((bits[pos2 + 1] >> 3) << 3);
					texturedata[pos + 1] = (unsigned char)((((bits[pos2 + 1] & 7) << 3) | (bits[pos2] >> 5)) << 2);
					texturedata[pos + 2] = (unsigned char)(bits[pos2] << 3);
					texturedata[pos + 3] = 255;	// default value to ensure image is visible
				}
		}
		else if (size == 3)
		{
			for (int j = 0; j < bi.bmiHeader.biHeight; j++)
				for (int i = 0; i < bi.bmiHeader.biWidth; i++)
				{
					int pos = 4 * (bi.bmiHeader.biWidth - i - 1 + bi.bmiHeader.biWidth*j);
					int pos2 = 3 * (i + j*bi.bmiHeader.biWidth);

					texturedata[pos + 3] = 255;	// default value to ensure image is visible
					texturedata[pos + 2] = bits[pos2];
					texturedata[pos + 1] = bits[pos2 + 1];
					texturedata[pos] = bits[pos2 + 2];
				}
		}
		else if (size == 4)
		{
			for (int j = 0; j < bi.bmiHeader.biHeight; j++)
				for (int i = 0; i < bi.bmiHeader.biWidth; i++)
				{
					int pos = 4 * (bi.bmiHeader.biWidth - i - 1 + bi.bmiHeader.biWidth*j);
					int pos2 = 4 * (i + j*bi.bmiHeader.biWidth);

					texturedata[pos + 3] = bits[pos2 + 3];
					texturedata[pos + 2] = bits[pos2];
					texturedata[pos + 1] = bits[pos2 + 1];
					texturedata[pos] = bits[pos2 + 2];
				}
		}

		::DestroyIcon(hh);

		if (ii.hbmColor != NULL)
			DeleteObject(ii.hbmColor);
		if (ii.hbmMask != NULL)
			DeleteObject(ii.hbmMask);
		delete[] bits;
	}

	return true;
#else
	return false;
#endif
}

