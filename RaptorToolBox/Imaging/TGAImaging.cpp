// TGAImaging.cpp: implementation of the CTGAImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_)
	#include "Imaging/TGAImaging.h"
#endif

#ifdef LINUX
	#include <stdio.h>
	#include <string.h>
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

bool CTGAImaging::storeImageFile(const std::string& fname,CTextureObject* const T)
{
    if (T == NULL)
        return false;

	unsigned char	*outcolors = T->getTexels();
    if (outcolors == NULL)
        return false;

    FILE			*out_rgb = NULL;
    if (NULL == (out_rgb = fopen( fname.data(), "wb" )))
        return false;

    unsigned char	head[18] ;
    memset(head,0,18);

    head[2] = 2;
    head[12] = T->getWidth() % 256;
    head[13] = T->getWidth() / 256;
    head[14] = T->getHeight() % 256;
    head[15] = T->getHeight() / 256;

    //  pixel format is forced to RGBA 32. Maybe future enhancements will support others...
    head[16] = 32;
	
	if (18 != fwrite( head, sizeof(char), 18, out_rgb ))
    {
        fclose(out_rgb);
        return false;
    }

    unsigned int size = T->getWidth()*4;
    unsigned char *buffer = new unsigned char[size];

    for (unsigned int row = 0; row < T->getHeight(); row++)
    {
        unsigned int texel = 0;
        while (texel < size)
        {
            buffer[texel] = outcolors[texel+2];
            buffer[texel+1] = outcolors[texel+1];
            buffer[texel+2] = outcolors[texel];
            buffer[texel+3] = 255;
            texel +=4;
        }
	    if (1 != fwrite( buffer, size,1, out_rgb ))
        {
            fclose(out_rgb);
            return false;
        }
        outcolors += size;
    }

    fclose(out_rgb);
    delete [] buffer;

    return true;    
}

bool CTGAImaging::loadImageFile(const std::string& fname,CTextureObject* const T)
{
    if (T==NULL)
        return false;

    unsigned char	head[18] ;
	FILE			*in_rgb = NULL;
	long			outcolor = 0;
	unsigned char	*outcolors = NULL;
	long			w = 0;
    long            h = 0;
	int				size = 0;

	if ((in_rgb = fopen( fname.data(), "rb" ))==NULL)
		return false;
	
	if (fread( head, sizeof(char), 18, in_rgb )!=18)
		return false ;

	w = (head[13]*256)+head[12];
	h = (head[15]*256)+head[14];
	size = head[16]/8;

    T->setSize(w,h);
    T->allocateTexels();
    unsigned char *texturedata = T->getTexels();

    //  allocate extra size for fast unchecked reading here under.
	outcolors = new unsigned char[size*w*h+4];
	if (fread( outcolors, size, w*h, in_rgb ) != w*h)
    {
        fclose( in_rgb );
	    return false;
    }

	int t_pos = 0;
	int i_pos = 0;
	for ( int i=0; i < w*h; i++ )
	{
		outcolor = *((long*)&outcolors[i_pos]);

		switch(size)
		{
			case 2:
				texturedata[t_pos] = (unsigned char)(((outcolor>>11)&0x1F)<<3);
				texturedata[t_pos+1] = (unsigned char)(((outcolor>>5)&0x3F)<<2);
				texturedata[t_pos+2] = (unsigned char)((outcolor&0x1F)<<3);

				if (T->getTransparency()>0)
					texturedata[t_pos+3] = (unsigned char)(T->getTransparency() & 0xFF);
				else if ((texturedata[t_pos]==0)&&(texturedata[t_pos+1]==0)&&(texturedata[t_pos+2]==0))
					texturedata[t_pos+3]=0;

				break;
			case 3:
			case 4:	// most 32 bits TGA images are defined with alpha as 0 !!!, so manage like 24bits
#ifdef RAPTOR_SSE_CODE_GENERATION
				__asm
				{
					lea edi,outcolor
					mov eax, [edi]
					bswap eax
					shr eax, 8
					mov [edi], eax
				}
#endif
				*((unsigned int*)&texturedata[t_pos]) = outcolor;
				
				
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

		t_pos += 4;
		i_pos += size;
	}

	delete [] outcolors;

    if (fclose( in_rgb ))
		return false;

	return true;
}

