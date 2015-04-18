// TextureFilter.h: interface for the CTextureFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREFILTER_H__B090C282_EC70_4B33_9270_8D4C7654484E__INCLUDED_)
#define AFX_TEXTUREFILTER_H__B090C282_EC70_4B33_9270_8D4C7654484E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "System/RaptorDisplayFilter.h"
#endif
#if !defined(AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_)
	#include "GLHierarchy/Reference.cxx"
#endif

RAPTOR_NAMESPACE

//! This class implements a texture filter with a texture2D and 
//!	a DisplayFilter.
class RAPTOR_API CTextureFilter
{
public:
	CTextureFilter(CTextureObject* pSource, CRaptorDisplayFilter* pFilter);

	virtual ~CTextureFilter(void);

	virtual void glRender(void);

	operator CTextureObject*(void)
	{ return m_pFilterTexture; }

private:
	CReference<CTextureObject> m_pTextureSource;
	CTextureObject* m_pFilterTexture;
	CRaptorDisplayFilter* m_pFilter;
};

#endif // !defined(AFX_TEXTUREFILTER_H__B090C282_EC70_4B33_9270_8D4C7654484E__INCLUDED_)