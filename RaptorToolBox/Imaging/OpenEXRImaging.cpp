// OpenEXRImaging.cpp: implementation of the COpenEXRImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_)
	#include "Imaging/OpenEXRImaging.h"
#endif

#include <algorithm>

// the source from Ilm cannot compile with old compilers like VC++6.0
// Exr support will only be enabled in later versions.
#if _MSC_VER > 1200 
	#define OPENEXR_DLL
    //	exr 1.6.1 support
    #ifndef INCLUDED_IMF_RGBA_FILE_H
	    #include "ImfRgbaFile.h"
    #endif
	#ifndef INCLUDED_IMF_STRING_ATTRIBUTE_H
		#include <ImfStringAttribute.h>
	#endif
	#ifndef INCLUDED_IMATHBOX_H
		#include "ImathBox.h"
	#endif
#endif


COpenEXRImaging::COpenEXRImaging(void)
{
}

COpenEXRImaging::~COpenEXRImaging(void)
{
}

bool COpenEXRImaging::isOfKind(const std::string &kind) const
{
	return _isOfKind(kind);
}

bool COpenEXRImaging::_isOfKind(const std::string &kind)
{
	std::string ext = kind;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

	return ("EXR" == ext);
}

std::vector<std::string> COpenEXRImaging::getImageKind(void) const
{
	std::vector<std::string> result;

	result.push_back("EXR");

	return result;
}

bool COpenEXRImaging::storeImageFile(const std::string& fname, CImage* const I) const
{
// Old compilers that do not support namespaces cannot compile
// with OpenEXR library headers, so I exclude the support
#if _MSC_VER > 1200
    if (I == NULL)
        return false;

	float *image_buffer = I->getFloatPixels();
    if (image_buffer == NULL)
        return false;

	unsigned int w = I->getWidth();
	unsigned int h = I->getHeight();

	Imf::Header header (w, h);
	header.insert ("origin", Imf::StringAttribute ("Raptor OpenEXR image"));

	try
	{
		Imf::RgbaOutputFile file(fname.data(), header, Imf::WRITE_RGBA);

		Imf::Rgba *pixels = new Imf::Rgba[w*h];
		for (unsigned int i=0;i<w*h;i++)
		{
			pixels[i].r = image_buffer[4*i+0];
			pixels[i].g = image_buffer[4*i+1];
			pixels[i].b = image_buffer[4*i+2];
			pixels[i].a = image_buffer[4*i+3];
		}

		file.setFrameBuffer (pixels, 1, w);
		file.writePixels (h);

		delete [] pixels;
	}
	catch(...)
	{
		return false;
	}

	return true;
#else
	return false;
#endif
}

bool COpenEXRImaging::loadImageFile(const std::string& fname, CImage* const I) const
{
	// Old compilers that do not support namespaces cannot compile
	// with OpenEXR library headers, so I exclude the support
#if _MSC_VER > 1200
	if (NULL == I)
		return false;

	try
	{
		Imf::RgbaInputFile file(fname.data());

		//const Imf::StringAttribute *comments =
		//	file.header().findTypedAttribute <Imf::StringAttribute> ("origin");

		Imath::Box2i dw = file.dataWindow();

		unsigned int w = dw.max.x - dw.min.x + 1;
		unsigned int h = dw.max.y - dw.min.y + 1;

		Imf::Rgba *pixels = new Imf::Rgba[w*h];

		file.setFrameBuffer(pixels - dw.min.x - dw.min.y * w, 1, w);
		file.readPixels(dw.min.y, dw.max.y);

		I->allocatePixels(w, h, CImage::CGL_COLOR_FLOAT16_ALPHA);
		float *image_buffer = I->getFloatPixels();

		for (unsigned int i = 0; i<w*h; i++)
		{
			image_buffer[4 * i + 0] = pixels[i].r;
			image_buffer[4 * i + 1] = pixels[i].g;
			image_buffer[4 * i + 2] = pixels[i].b;
			image_buffer[4 * i + 3] = pixels[i].a;
		}

		delete[] pixels;
	}
	catch (...)
	{
		return false;
	}

	return true;
#else
	return false;
#endif
}

