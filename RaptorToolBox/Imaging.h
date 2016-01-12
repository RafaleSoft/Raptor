// Imaging.h: interface for the CImaging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_)
#define AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif

RAPTOR_NAMESPACE


class RAPTOR_API CImaging  
{
public:
    //! Install image manipulators into current instance of Raptor
    static bool installImagers(CTextureFactory &factory) ;

    //! Install one selected image manipulator into current instance of Raptor
	static bool installImager(CTextureFactory &factory, const std::string& imageType);

private:
    CImaging();
	virtual ~CImaging();
};

#endif // !defined(AFX_IMAGING_H__BD40E48F_EE12_49CF_BFBD_93658FCD0529__INCLUDED_)
