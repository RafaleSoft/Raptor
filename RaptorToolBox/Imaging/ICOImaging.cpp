// ICOImaging.cpp: implementation of the CICOImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_)
	#include "Imaging/ICOImaging.h"
#endif


CICOImaging::CICOImaging(void)
{
}

CICOImaging::~CICOImaging(void)
{
}

bool CICOImaging::isOfKind(const std::string &kind) const 
{ 
	return (("ICO" == kind) ||
			("DLL" == kind) ||
			("EXE" == kind));
}

vector<std::string> CICOImaging::getImageKind(void) const
{
	vector<string> result;

	result.push_back("ICO");
	result.push_back("DLL");
	result.push_back("EXE");

	return result;
}

bool CICOImaging::storeImageFile(const std::string& fname,CTextureObject* const T)
{
    return false;
}

bool CICOImaging::loadImageFile(const std::string& fname,CTextureObject* const T)
{
    if (T == NULL)
        return false;

#ifdef WIN32
    ICONINFO	ii;
	HICON		hh;

	if ((hh = ExtractIcon(0,fname.data(),T->getWidth())) == NULL)
		return false;

	GetIconInfo( hh, &ii); 

	HDC hdc = GetDC(NULL);
	BITMAPINFO bi;
	GetDIBits(  hdc, ii.hbmColor, 0, 0, NULL, &bi, DIB_RGB_COLORS);
	int size = bi.bmiHeader.biBitCount / 8;
	int dim = bi.bmiHeader.biWidth * bi.bmiHeader.biHeight;

	unsigned char *bits = new unsigned char[dim*size];
	if (bi.bmiHeader.biHeight != GetDIBits(  hdc, ii.hbmColor, 0, bi.bmiHeader.biHeight, bits, &bi, DIB_RGB_COLORS))
		return false;

    T->setSize(bi.bmiHeader.biWidth , bi.bmiHeader.biHeight);
    T->allocateTexels();
	unsigned char *texturedata = T->getTexels();

	if (size == 2)
    {
		for (int j=0 ; j<bi.bmiHeader.biHeight ; j++)
			for (int i=0 ; i<bi.bmiHeader.biWidth ; i++)
		{
			int pos = 4 * (bi.bmiHeader.biWidth - i - 1 + bi.bmiHeader.biWidth*j);
			int pos2 = 2 * (i + j*bi.bmiHeader.biWidth);

			texturedata[pos] = (unsigned char)((bits[pos2+1]>>3)<<3);
			texturedata[pos+1] = (unsigned char)((((bits[pos2+1]&7)<<3)|(bits[pos2]>>5))<<2);
			texturedata[pos+2] = (unsigned char)(bits[pos2]<<3);		

			if (T->getTransparency()>0)
				texturedata[pos+3] = (unsigned char)(T->getTransparency() & 0xFF);
			else if((texturedata[pos+2]==0) && (texturedata[pos+1]==0) && (texturedata[pos]==0))
				texturedata[pos+3]=0;
		}
    }
	else if (size == 3)
    {
		for (int j=0 ; j<bi.bmiHeader.biHeight ; j++)
		for (int i=0 ; i<bi.bmiHeader.biWidth ; i++)
		{
			int pos = 4 * (bi.bmiHeader.biWidth - i - 1 + bi.bmiHeader.biWidth*j);
			int pos2 = 3 * (i + j*bi.bmiHeader.biWidth);

			texturedata[pos+2] = bits[pos2];
			texturedata[pos+1] = bits[pos2+1];
			texturedata[pos] = bits[pos2+2];

			if (T->getTransparency()>0)
				texturedata[pos+3] = (unsigned char)(T->getTransparency() & 0xFF);
			else if((texturedata[pos+2]==0)&&(texturedata[pos+1]==0)&&(texturedata[pos]==0))
				texturedata[pos+3]=0;
		}
    }
	else if (size == 4)
    {
		for (int j=0 ; j<bi.bmiHeader.biHeight ; j++)
		for (int i=0 ; i<bi.bmiHeader.biWidth ; i++)
		{
			int pos = 4 * (bi.bmiHeader.biWidth - i - 1 + bi.bmiHeader.biWidth*j);
			int pos2 = 4 * (i + j*bi.bmiHeader.biWidth);

			texturedata[pos+2] = bits[pos2];
			texturedata[pos+1] = bits[pos2+1];
			texturedata[pos] = bits[pos2+2];

			if (T->getTransparency() > 0)
				texturedata[pos+3] = (unsigned char)(T->getTransparency() & 0xFF);
			else if((texturedata[pos+2]==0)&&(texturedata[pos+1]==0)&&(texturedata[pos]==0))
				texturedata[pos+3] = 0;
		}
    }

	DestroyIcon(hh);
	if (ii.hbmColor != NULL)
		DeleteObject(ii.hbmColor);
	if (ii.hbmMask != NULL)
		DeleteObject(ii.hbmMask);
	delete [] bits;

    return true;
#else
	return false;
#endif
}

