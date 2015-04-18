// BMPImaging.cpp: implementation of the CBMPImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_BMPIMAGING_H__C9C42555_D3BF_42BE_8CC2_FA35410B79AD__INCLUDED_)
	#include "Imaging/BMPImaging.h"
#endif


CBMPImaging::CBMPImaging(void)
{
}

CBMPImaging::~CBMPImaging(void)
{
}

bool CBMPImaging::isOfKind(const std::string &kind) const 
{ 
	return ("BMP" == kind); 
}

vector<std::string> CBMPImaging::getImageKind(void) const
{
	vector<string> result;
	result.push_back("BMP");
	return result;
}

bool CBMPImaging::storeImageFile(const std::string&,CTextureObject* const T)
{
    return false;
}

bool CBMPImaging::loadImageFile(const std::string& fname,CTextureObject* const T)
{
#ifdef WIN32
   	HANDLE h = LoadImage(NULL,fname.data(),IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

	if (h == NULL)
		return false;

	HBITMAP bmp = (HBITMAP)h;


	HDC hdc = GetDC(NULL);
	BITMAPINFO bi;
	GetDIBits(  hdc, bmp, 0, 0, NULL, &bi, DIB_RGB_COLORS);

	int size = bi.bmiHeader.biBitCount / 8;
	int dim = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight;
	
    T->setSize(bi.bmiHeader.biWidth , bi.bmiHeader.biHeight);
    T->allocateTexels();
	unsigned char *texturedata = T->getTexels();

	unsigned char *buffer = new unsigned char[dim*size];
	if (bi.bmiHeader.biHeight != GetDIBits(  hdc, bmp, 0, bi.bmiHeader.biHeight, buffer, &bi, DIB_RGB_COLORS))
		return false;

	int t_pos = 0;
	int b_pos = 0;

	for ( int i=0; i < dim; i++ )
	{
		switch(size)
		{
			case 3:
			case 4:	// most 32 bits TGA images are defined with alpha as 0 !!!, so manage like 24bits
				texturedata[t_pos] = buffer[b_pos];
				texturedata[t_pos+1] = buffer[b_pos+1];
				texturedata[t_pos+2] = buffer[b_pos+2];
				
				if (T->getTransparency()>255)
				{
					if ((texturedata[t_pos]==0)&&(texturedata[t_pos+1]==0)&&(texturedata[t_pos+2]==0))
						texturedata[t_pos+3]=0;
					else 
						texturedata[t_pos+3]=255;
				}
				else if (T->getTransparency()>0)
					texturedata[t_pos+3] = (unsigned char)(T->getTransparency() & 0xFF);
				else 
				{	
					texturedata[t_pos+3] =
					(unsigned char)((texturedata[t_pos] + texturedata[t_pos+1] + texturedata[t_pos+2]) / 3);
				}

				break;
			default:
				texturedata[t_pos] = 0xFF;
				texturedata[t_pos+1] = 0xFF;
				texturedata[t_pos+2] = 0xFF;
				if (T->getTransparency()>0)
					texturedata[t_pos+3] = (unsigned char)(T->getTransparency() & 0xFF);
		}

		b_pos += size;
		t_pos += 4;
	}

	ReleaseDC(NULL,hdc);
	DeleteObject(h);
	delete [] buffer;
	return true;
#else
    return false;
#endif
}

