/***************************************************************************/
/*                                                                         */
/*  LightFlare.h                                                           */
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


#if !defined(AFX_LIGHTFLARE_H__373B5695_C92B_4ED8_8DDF_81273BF34FE3__INCLUDED_)
#define AFX_LIGHTFLARE_H__373B5695_C92B_4ED8_8DDF_81273BF34FE3__INCLUDED_

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

class CTextureQuad;
class ITextureObject;


class RAPTOR_API CLightFlare : public CPersistence
{
public:
	CLightFlare(const std::string& name = "LIGHTFLARE");
	virtual ~CLightFlare(void);

	//!	Lens Flare rendering
	void glRender(float dx, float dy);

	//!	Returns the light volume with which the flare size is computed.
	float getLightVolumeSize(void) const { return m_fLightVolumeSize; }

	//!	Returns the light volume with which the flare size is computed.
	void setLightVolumeSize(float s) { m_fLightVolumeSize = s; }

	//!	Returns the number of flares.
	size_t getNbFlares(void) const { return mFlares.size(); }


	//! Inherited from CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CLightFlareClassID,"LightFlare",CPersistence)


private:
	//!	The list of texture quad flares.
	std::vector<CTextureQuad*>	mFlares;

	float				m_fLightVolumeSize;
	unsigned int		m_volumeVisibility;
	unsigned int		m_visibilityQuery;
	float				m_fLightVolumeVisibility;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_LIGHTFLARE_H__373B5695_C92B_4ED8_8DDF_81273BF34FE3__INCLUDED_)

