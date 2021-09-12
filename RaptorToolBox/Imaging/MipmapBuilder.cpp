/***************************************************************************/
/*                                                                         */
/*  MipmapBuilder.h                                                        */
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

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_MIPMAPBUILDER_H__9C508D96_B614_4920_8816_B670295B6CE2__INCLUDED_)
    #include "MipmapBuilder.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif
#if !defined(AFX_TEXTUREFACTORYCONFIG_H__7A20D208_423F_4E02_AA4D_D736E0A7959F__INCLUDED_)
	#include "GLHierarchy/TextureFactoryConfig.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif

#if (!defined(GL_VERSION_3_0) && !defined(GL_EXT_framebuffer_object) && !defined(GL_VERSION_1_4))
	#include <GL/glu.h>	// gluBuild2DMipmaps
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMipmapBuilder::CMipmapBuilder()
{

}

CMipmapBuilder::~CMipmapBuilder()
{

}

bool CMipmapBuilder::apply(	ITextureObject* const src,
							unsigned int innerFormat,
							unsigned int texelFormat,
							unsigned int texelType,
							const CTextureFactoryConfig& ) const
{
	GLuint target = GL_TEXTURE_2D;
	void* pTexels = NULL;	// src->getTexels();
	if (NULL == pTexels)
	{
		pTexels = NULL; // src->getFloatTexels();
        if (NULL == pTexels)
			return false;
	}

#if (defined(GL_VERSION_3_0) || defined(GL_EXT_framebuffer_object))
	glTexImage2D(	target,
					src->getCurrentMipMapLevel(),
					innerFormat,
					src->getWidth(),
					src->getHeight(),
					0,texelFormat,
					texelType,
					pTexels);
	const CRaptorGLExtensions * const pExtensions = IRaptor::glGetExtensions();
	if (pExtensions->glGenerateMipmapEXT != NULL)
	{
		pExtensions->glGenerateMipmapEXT(GL_TEXTURE_2D);
		return true;
	}
	else
		return false;
#elif defined(GL_VERSION_1_4)
	glTexParameteri(target,GL_GENERATE_MIPMAP,GL_TRUE);
	glTexImage2D(	target,
					src->getCurrentMipMapLevel(),
					innerFormat,
					src->getWidth(),
					src->getHeight(),
					0,texelFormat,
					texelType,
					pTexels);
	return true;
#else
	return (0 == gluBuild2DMipmaps(	target,
									innerFormat,
									src->getWidth(),
									src->getHeight(),
									texelFormat,
									texelType,
									pTexels));

#endif
}


