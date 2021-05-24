/***************************************************************************/
/*                                                                         */
/*  Imaging.cpp                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "Subsys/CodeGeneration.h"


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

bool CImaging::installImagers(void) 
{
    CImage::setImageKindIO(new CBMPImaging());
	CImage::setImageKindIO(new CICOImaging());
	CImage::setImageKindIO(new CTGAImaging());
	CImage::setImageKindIO(new CJPGImaging());
	CImage::setImageKindIO(new CTIFFImaging());
#if _MSC_VER > 1200 
	CImage::setImageKindIO(new COpenEXRImaging());
#endif
	CImage::setImageKindIO(new CPNGImaging());

    return true;
}

bool CImaging::installImager(const std::string& imageType)
{
    bool res = true;

    string ext = imageType;
	std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

    if (CBMPImaging::_isOfKind(ext))
		CImage::setImageKindIO(new CBMPImaging());
    else if (CICOImaging::_isOfKind(ext))
		CImage::setImageKindIO(new CICOImaging());
    else if (CTGAImaging::_isOfKind(ext))
		CImage::setImageKindIO(new CTGAImaging());
    else if (CJPGImaging::_isOfKind(ext))
		CImage::setImageKindIO(new CJPGImaging());
    else if (CTIFFImaging::_isOfKind(ext))
		CImage::setImageKindIO(new CTIFFImaging());
#if _MSC_VER > 1200 
	else if (COpenEXRImaging::_isOfKind(ext))
		CImage::setImageKindIO(new COpenEXRImaging());
#endif
    else if (CPNGImaging::_isOfKind(ext))
		CImage::setImageKindIO(new CPNGImaging());
	else
		res = false;

    return res;
}

