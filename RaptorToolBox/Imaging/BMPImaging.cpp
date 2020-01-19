// BMPImaging.cpp: implementation of the CBMPImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_BMPIMAGING_H__C9C42555_D3BF_42BE_8CC2_FA35410B79AD__INCLUDED_)
	#include "Imaging/BMPImaging.h"
#endif

#include <algorithm>


CBMPImaging::CBMPImaging(void)
{
}

CBMPImaging::~CBMPImaging(void)
{
}

bool CBMPImaging::isOfKind(const std::string &kind) const 
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return ("BMP" == ext); 
}

vector<std::string> CBMPImaging::getImageKind(void) const
{
	vector<string> result;
	result.push_back("BMP");
	return result;
}

bool CBMPImaging::storeImageFile(const std::string&, CImage* const I) const
{
	return false;
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
	GetDIBits(hdc, bmp, 0, 0, NULL, &bi, DIB_RGB_COLORS);

	int size = bi.bmiHeader.biBitCount / 8;
	int dim = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight;

	I->allocatePixels(bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, CImage::CGL_COLOR24_ALPHA);
	unsigned char *texturedata = (unsigned char*)I->getPixels();

	unsigned char *buffer = new unsigned char[dim*size];
	if (bi.bmiHeader.biHeight != GetDIBits(hdc, bmp, 0, bi.bmiHeader.biHeight, buffer, &bi, DIB_RGB_COLORS))
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

	ReleaseDC(NULL, hdc);
	DeleteObject(h);
	delete[] buffer;
	return true;
#else
	return false;
#endif
}

