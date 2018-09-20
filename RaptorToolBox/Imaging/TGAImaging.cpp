// TGAImaging.cpp: implementation of the CTGAImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_)
	#include "Imaging/TGAImaging.h"
#endif


#if defined(WIN32)
	#include <share.h>
	static FILE *msdn_fopen(const char *filename, const char *mode)
	{
		if ((NULL == filename) || (NULL == mode))
			return NULL;
		FILE* pFile = NULL;
		errno_t err = fopen_s(&pFile, filename, mode);
		if (0 == err)
			return pFile;
		else
			return NULL;
	}
	#define FOPEN(a,b) msdn_fopen(a,b)
#elif LINUX
	#include <stdio.h>
	#include <string.h>
	#define FOPEN(a,b) fopen(a,b)
#endif


CTGAImaging::CTGAImaging(void)
{
}

CTGAImaging::~CTGAImaging(void)
{
}


bool CTGAImaging::isOfKind(const std::string &kind) const 
{ 
	return ("TGA" == kind); 
}

vector<std::string> CTGAImaging::getImageKind(void) const
{
	vector<string> result;
	result.push_back("TGA");
	return result;
}

bool CTGAImaging::storeImageFile(const std::string& fname, CImage* const I) const
{
	if (I == NULL)
		return false;

	unsigned char	*outcolors = I->getPixels();
	if (outcolors == NULL)
		return false;

	FILE *out_rgb = NULL;
	if (NULL == (out_rgb = fopen(fname.data(), "wb")))
		return false;

	unsigned char	head[18];
	memset(head, 0, 18);

	head[2] = 2;
	head[12] = I->getWidth() % 256;
	head[13] = I->getWidth() / 256;
	head[14] = I->getHeight() % 256;
	head[15] = I->getHeight() / 256;

	//  pixel format is forced to RGBA 32. Maybe future enhancements will support others...
	head[16] = 32;

	if (18 != fwrite(head, sizeof(char), 18, out_rgb))
	{
		fclose(out_rgb);
		return false;
	}

	unsigned int size = I->getWidth() * 4;
	unsigned char *buffer = new unsigned char[size];

	for (unsigned int row = 0; row < I->getHeight(); row++)
	{
		unsigned int texel = 0;
		while (texel < size)
		{
			buffer[texel] = outcolors[texel + 2];
			buffer[texel + 1] = outcolors[texel + 1];
			buffer[texel + 2] = outcolors[texel];
			buffer[texel + 3] = 255;
			texel += 4;
		}
		if (1 != fwrite(buffer, size, 1, out_rgb))
		{
			fclose(out_rgb);
			return false;
		}
		outcolors += size;
	}

	fclose(out_rgb);
	delete[] buffer;

	return true;
}

bool CTGAImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
	if ((NULL == I) || (fname.empty()))
		return false;

	unsigned char	head[18];
	FILE			*in_rgb = NULL;
	long			outcolor = 0;
	unsigned char	*outcolors = NULL;

	size_t			w = 0;
    size_t          h = 0;
	int				size = 0;

	if ((in_rgb = FOPEN(fname.data(), "rb")) == NULL)
		return false;
	
	if (fread( head, sizeof(char), 18, in_rgb )!=18)
		return false ;

	w = (head[13]*256)+head[12];
	h = (head[15]*256)+head[14];
	size = head[16]/8;

    I->allocatePixels(w, h, CImage::CGL_COLOR24_ALPHA);
    uint8_t *texturedata = I->getPixels();

    //  allocate extra size for fast unchecked reading here under.
	outcolors = new unsigned char[size*w*h+4];
	if (fread(outcolors, size, w*h, in_rgb) != w*h)
    {
        fclose( in_rgb );
	    return false;
    }

	int t_pos = 0;
	int i_pos = 0;
	for ( size_t i=0; i < w*h; i++ )
	{
		outcolor = *((long*)&outcolors[i_pos]);

		switch (size)
		{
			case 2:
			{
				texturedata[t_pos] = (unsigned char)(((outcolor >> 11) & 0x1F) << 3);
				texturedata[t_pos + 1] = (unsigned char)(((outcolor >> 5) & 0x3F) << 2);
				texturedata[t_pos + 2] = (unsigned char)((outcolor & 0x1F) << 3);
				texturedata[t_pos + 3] = 0xFF;	// default value to ensure image is visible
				break;
			}
			case 3:
			case 4:	// most 32 bits TGA images are defined with alpha as 0 !!!, so manage like 24bits
			{	// Colors are managed big endian in TGA:
				//	from:	a b g r
				//	to:		. r g b
				//__asm { lea edi,outcolor ; mov eax, [edi]; bswap eax; shr eax, 8; mov [edi], eax }
				*((unsigned int*)&texturedata[t_pos]) = outcolor;
				unsigned char t = texturedata[t_pos];
				texturedata[t_pos] = texturedata[t_pos + 2];
				texturedata[t_pos + 2] = t;
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

		t_pos += 4;
		i_pos += size;
	}

	delete[] outcolors;

	if (fclose(in_rgb))
		return false;

	return true;
}

