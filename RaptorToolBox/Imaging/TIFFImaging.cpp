// TIFFImaging.cpp: implementation of the CTIFFImaging class.
//
//////////////////////////////////////////////////////////////////////
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

//  tiff support
#include "tif_config.h"
#ifndef _TIFFIO_
	#include "tiffio.h"
#endif

#include <algorithm>


void RaptorTIFFErrorHandler(const char* module , const char* fmt, va_list ap)
{
	std::string szTitle;
	if (module == NULL)
		szTitle = "LIBTIFF";
	else
		szTitle = module;
	szTitle += " Error";

	char szTmp[256];
	memset(szTmp, 0, 256);
	vsprintf(szTmp, fmt, ap);
	szTitle += szTmp;

    Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_ERROR,
													szTitle);
}

void RaptorTIFFWarningHandler(const char* module , const char* fmt, va_list ap)
{
	std::string szTitle;
	if (module == NULL)
		szTitle = "LIBTIFF";
	else
		szTitle = module;
	szTitle += " Warning";

	char szTmp[256];
	memset(szTmp, 0, 256);
	vsprintf(szTmp, fmt, ap);
	szTitle += szTmp;

    Raptor::GetErrorManager()->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
													CRaptorErrorManager::RAPTOR_WARNING,
													szTitle);
}


CTIFFImaging::CTIFFImaging(void)
{
}

CTIFFImaging::~CTIFFImaging(void)
{
}

bool CTIFFImaging::isOfKind(const std::string &kind) const 
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return (("TIF" == ext) ||
			("TIFF" == ext));
}

vector<std::string> CTIFFImaging::getImageKind(void) const
{
	vector<string> result;

	result.push_back("TIF");
	result.push_back("TIFF");

	return result;
}

bool CTIFFImaging::storeImageFile(const std::string& fname, CImage* const I) const
{
	if (I == NULL)
		return false;

	unsigned char *texturedata = I->getPixels();
	if (texturedata == NULL)
		return false;

	TIFFSetErrorHandler(RaptorTIFFErrorHandler);
	TIFFSetWarningHandler(RaptorTIFFWarningHandler);

	TIFF *out = NULL;
	if ((out = TIFFOpen(fname.data(), "w")) == NULL)
		return false;

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, I->getWidth());
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, I->getHeight());
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4); //  only 32 bits supported
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);	// only 8bits per sample ( channel ? )
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB); //  only RGB types are supported
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, I->getWidth());

	// Tiff library has a bug : output image has only data in a square region.
	// So I cannot use JPEG compression !!!
	//TIFFSetField(out, TIFFTAG_COMPRESSION,	COMPRESSION_JPEG);
	TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	//TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);

	unsigned int size = I->getWidth() * 4;

	for (unsigned int row = 0; row < I->getHeight(); row++)
	{
		int texel = (I->getHeight() - row - 1) * size;

		if (TIFFWriteScanline(out, texturedata + texel, row, 0) < 0)
		{
			TIFFClose(out);
			return false;
		}
	}

	TIFFClose(out);

	return true;
}

bool CTIFFImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
	if ((NULL == I) || (fname.empty()))
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
	I->allocatePixels(w, h, CImage::CGL_COLOR24_ALPHA);
	uint8_t *image_buffer = I->getPixels();

	// Allocate 4 more bytes to safely erad past the end of the buffer
	unsigned char *inbuf = (unsigned char *)_TIFFmalloc(4 + TIFFScanlineSize(in));
	for (unsigned int row = 0; row < h; row++)
	{
		if (TIFFReadScanline(in, inbuf, row, 0) < 0)
			break;

		//	converting from RGB to RGBA format for
		//	eventual extra processing
		//	starting at the end of the buffer
		int base_4 = w * 4 * (h - row - 1);
		int base_3 = 0;

		for (unsigned int i = 0; i<w; i++)
		{
			//	I know i read an octet off the end of the buffer
			//	but this is faster. Moreover, the data should be 
			//	aligned to a dword address for each row, so ...
			*((unsigned int*)(&image_buffer[base_4])) = *((unsigned int*)(&(inbuf[base_3])));

			base_3 += 3;
			base_4 += 4;
		}
	}

	TIFFClose(in);

	return true;
}
