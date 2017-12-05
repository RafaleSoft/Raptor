// LightGlow.h: interface for the CLightGlow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)
#define AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureObject;

class RAPTOR_API CLightGlow : public CPersistence
{
public:
	CLightGlow(const std::string& name="LIGHTGLOW");
	virtual ~CLightGlow(void);

	//!	Glow rendering
	void glRender(void);

	//! Inherited from CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CLightGlowClassID,"LightGlow",CPersistence)

private:
	unsigned int	m_uiGlow;
	bool			m_bRebuildGlow;
	float			m_glowSize;
	CReference<CTextureObject> m_glow;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)

