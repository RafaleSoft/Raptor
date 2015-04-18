// Imaging.cpp: implementation of the CImaging class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif

#if !defined(AFX_BMPIMAGING_H__C9C42555_D3BF_42BE_8CC2_FA35410B79AD__INCLUDED_)
	#include "Imaging/BMPImaging.h"
#endif
#if !defined(AFX_ICOIMAGING_H__6E2B8113_3111_4770_97A8_A786982CF05F__INCLUDED_)
	#include "Imaging/ICOImaging.h"
#endif
#if !defined(AFX_TGAIMAGING_H__EF9F065C_C2BE_4C41_BD50_3DA107B5050B__INCLUDED_)
	#include "Imaging/TGAImaging.h"
#endif
#if !defined(AFX_JPGIMAGING_H__E268D39E_46E2_4CE0_B8C7_D5C3DE2B5F91__INCLUDED_)
	#include "Imaging/JPGImaging.h"
#endif
#if !defined(AFX_TIFFIMAGING_H__3AD77410_776F_446A_860E_496C8D13CB0F__INCLUDED_)
	#include "Imaging/TIFFImaging.h"
#endif
#if !defined(AFX_OPENEXRIMAGING_H__A2B2F9B8_4DA0_4D06_AD64_40805C2A14C3__INCLUDED_)
	#include "Imaging/OpenEXRImaging.h"
#endif
#if !defined(AFX_PNGIMAGING_H__73E7FD82_0C63_4CB0_BFEC_33475DAABE35__INCLUDED_)
	#include "Imaging/PNGImaging.h"
#endif

#if !defined(AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_)
	#include "Imaging.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImaging::CImaging()
{

}

CImaging::~CImaging()
{

}

bool CImaging::installImagers(const CTextureFactory &factory) 
{
	CTextureFactoryConfig &pConfig = factory.getConfig();

    pConfig.setImageKindIO(new CBMPImaging());
    pConfig.setImageKindIO(new CICOImaging());
    pConfig.setImageKindIO(new CTGAImaging());
    pConfig.setImageKindIO(new CJPGImaging());
    pConfig.setImageKindIO(new CTIFFImaging());
#if _MSC_VER > 1200 
	pConfig.setImageKindIO(new COpenEXRImaging());
#endif
    pConfig.setImageKindIO(new CPNGImaging());

    return true;
}

bool CImaging::installImager(const CTextureFactory &factory,const std::string& imageType)
{
    bool res = true;

    string ext = imageType;
	for (size_t pos=0;pos<imageType.size();pos++)
        ext[pos] = toupper(imageType[pos]);

	CTextureFactoryConfig &pConfig = factory.getConfig();

    if (ext == "BMP")
        pConfig.setImageKindIO(new CBMPImaging());
    else if (ext == "ICO")
        pConfig.setImageKindIO(new CICOImaging());
    else if (ext == "TGA")
        pConfig.setImageKindIO(new CTGAImaging());
    else if (ext == "JPG")
        pConfig.setImageKindIO(new CJPGImaging());
    else if (ext == "TIF")
        pConfig.setImageKindIO(new CTIFFImaging());
#if _MSC_VER > 1200 
	else if (ext == "EXR")
        pConfig.setImageKindIO(new COpenEXRImaging());
#endif
    else if (ext == "PNG")
        pConfig.setImageKindIO(new CPNGImaging());
	else
		res = false;

    return res;
}

