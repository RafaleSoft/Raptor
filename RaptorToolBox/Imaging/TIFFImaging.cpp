// TIFFImaging.cpp: implementation of the CTIFFImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif
#if !defined(AFX_TIFFIMAGING_H__3AD77410_776F_446A_860E_496C8D13CB0F__INCLUDED_)
	#include "Imaging/TIFFImaging.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif

//  tiff 3.8.2 support
#include "tif_config.h"
#ifndef _TIFFIO_
	#include "tiffio.h"
#endif


void RaptorTIFFErrorHandler(const char* module , const char* fmt, va_list ap)
{
	const char *szTitleText = "%s Error";
	const char *szDefaultModule = "LIBTIFF";
	const char *szTmpModule = (module == NULL) ? szDefaultModule : module;

	char *szTitle = (char*)new char[(strlen(szTmpModule) + strlen(szTitleText) + strlen(fmt) + 128)];
    if (szTitle == NULL)
		return;

	sprintf(szTitle, szTitleText, szTmpModule);
	char *szTmp = szTitle + (strlen(szTitle)+2)*sizeof(char);
	vsprintf(szTmp, fmt, ap);

    Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_ERROR,
													szTmp);

    delete [] szTitle;
}

void RaptorTIFFWarningHandler(const char* module , const char* fmt, va_list ap)
{
	const char *szTitleText = "%s Warning";
	const char *szDefaultModule = "LIBTIFF";
	const char *szTmpModule = (module == NULL) ? szDefaultModule : module;

	char *szTitle = (char*)new char[(strlen(szTmpModule) + strlen(szTitleText) + strlen(fmt) + 128)];
    if (szTitle == NULL)
		return;

	sprintf(szTitle, szTitleText, szTmpModule);
	char *szTmp = szTitle + (strlen(szTitle)+2)*sizeof(char);
	vsprintf(szTmp, fmt, ap);

    Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_WARNING,
													szTmp);

    delete [] szTitle;
}


CTIFFImaging::CTIFFImaging(void)
{
}

CTIFFImaging::~CTIFFImaging(void)
{
}

bool CTIFFImaging::isOfKind(const std::string &kind) const 
{ 
	return (("TIF" == kind) ||
			("TIFF" == kind));
}

vector<std::string> CTIFFImaging::getImageKind(void) const
{
	vector<string> result;

	result.push_back("TIF");
	result.push_back("TIFF");

	return result;
}

bool CTIFFImaging::storeImageFile(const std::string& fname,CTextureObject* const T)
{
    if (T == NULL)
        return false;

	unsigned char *texturedata = T->getTexels();
    if (texturedata == NULL)
        return false;

    TIFFSetErrorHandler(RaptorTIFFErrorHandler);
    TIFFSetWarningHandler(RaptorTIFFWarningHandler);

    TIFF *out = NULL;
	if ((out = TIFFOpen(fname.data(), "w")) == NULL)
		return false;

    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, T->getWidth());
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, T->getHeight());
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4); //  only 32 bits supported
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);	// only 8bits per sample ( channel ? )
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB); //  only RGB types are supported
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, T->getWidth());
    
    // Tiff library has a bug : output image has only data in a square region.
    // So I cannot use JPEG compression !!!
    //TIFFSetField(out, TIFFTAG_COMPRESSION,	COMPRESSION_JPEG);
    TIFFSetField(out, TIFFTAG_COMPRESSION,	COMPRESSION_LZW);
    //TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);

    unsigned int size = T->getWidth()*4;

    for (unsigned int row = 0; row < T->getHeight(); row++)
    {
        int texel = (T->getHeight() - row - 1) * size;

	    if (TIFFWriteScanline(out, texturedata+texel, row, 0) < 0)
        {
            TIFFClose(out);
            return false;
        }
    }
	
    TIFFClose(out);

	return true;
}

bool CTIFFImaging::loadImageFile(const std::string& fname,CTextureObject* const T)
{
	if (T==NULL)
        return false;

    TIFFSetErrorHandler(RaptorTIFFErrorHandler);
    TIFFSetWarningHandler(RaptorTIFFWarningHandler);

    TIFF *in = NULL;
	if ((in = TIFFOpen(fname.data(), "r")) == NULL)
		return false;

    unsigned short photometric = 0;
    TIFFGetField(in, TIFFTAG_PHOTOMETRIC, &photometric);
    if (photometric != PHOTOMETRIC_RGB)
        return false;   //  only RGB types are supported

    unsigned short samplesperpixel = 0;
    TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
	if (samplesperpixel != 3)
        return false;   //  only 24 && 32 bits supported

    unsigned short bitspersample = 0;
    TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bitspersample);
	if (bitspersample != 8)
        return false;   // only 8bits per sample ( channel ? )

    unsigned int w = 0;
    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &w);

    unsigned int h = 0;
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &h);

    unsigned short config = 0;
	TIFFGetField(in, TIFFTAG_PLANARCONFIG, &config);
	
	//	I want GL_RGBA buffer format
    T->setSize(w,h);
    T->allocateTexels();
	unsigned char *image_buffer = T->getTexels();

    unsigned char *inbuf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(in));
    for (unsigned int row = 0; row < h; row++) 
    {
		if (TIFFReadScanline(in, inbuf, row, 0) < 0)
			break;

        //	converting from RGB to RGBA format for
		//	eventual extra processing
		//	starting at the end of the buffer
		int base_4 = w*4*(h - row - 1);
		int base_3 = 0;

		for (unsigned int i=0;i<w;i++)
		{
			//	I know i read an octet off the end of the buffer
			//	but this is faster. Moreover, the data should be 
			//	aligned to a dword address for each row, so ...
			*((unsigned int*)(&image_buffer[base_4])) = *((unsigned int*)(&(inbuf[base_3])));
			
			if (T->getTransparency()>255)
			{
				if ((image_buffer[base_4]==0)&&(image_buffer[base_4+1]==0)&&(image_buffer[base_4+2]==0))
					image_buffer[base_4+3] = 0;
				else 
					image_buffer[base_4+3] = 255;
			}
			else if (T->getTransparency()>0)
			{
				image_buffer[base_4+3] = (unsigned char)(T->getTransparency() & 0xFF);
			}
			else
			{	
				image_buffer[base_4+3] = 
				 (unsigned char)((image_buffer[base_4] + image_buffer[base_4+1] + image_buffer[base_4+2]) / 3);
			}

			base_3+=3;
			base_4 += 4;
		}
	}
	
    TIFFClose(in);

	return true;
}

