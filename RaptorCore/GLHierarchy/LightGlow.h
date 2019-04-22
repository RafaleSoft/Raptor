/***************************************************************************/
/*                                                                         */
/*  LightGlow.h                                                            */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#if !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)
#define AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureQuad;


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
	float			m_glowSize;

	CTextureQuad	*m_pGlow;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTGLOW_H__577C39B3_EE0B_4A07_8974_BC250BA2960A__INCLUDED_)

