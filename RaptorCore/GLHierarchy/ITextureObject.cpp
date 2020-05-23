/***************************************************************************/
/*                                                                         */
/*  ITextureObject.cpp                                                     */
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

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
		#include "System/RaptorErrorManager.h"
	#endif
	#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
		#include "Subsys/OpenGL/RaptorOpenGL.h"
	#endif
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ITextureObject::ITextureObject(TEXEL_TYPE type)
	:CObjectReference(),
	m_type(type), m_width(0), m_height(0), m_depth(0), m_alpha(255),
	m_filter(CGL_UNFILTERED), m_name("<unknown>")
{
}


ITextureObject::~ITextureObject()
{
}


ITextureObject::ITextureObject(const ITextureObject& rsh)
	:CObjectReference(), 
	m_type(rsh.m_type), m_width(rsh.m_width),
	m_height(rsh.m_height), m_depth(rsh.m_depth),
	m_alpha(rsh.m_alpha), 
	m_filter(rsh.m_filter),
	m_name(rsh.m_name)
{
}


void ITextureObject::setSize(uint32_t width, uint32_t height, uint32_t depth)
{
	if ((width == 0) || (height == 0) || (depth == 0))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_WARNING(	COpenGL::COpenGLClassID::GetClassId(),
						"CTextureObject wrong size update");
#endif
		return;
	}

	if ((width != m_width) || (height != m_height) || (depth != m_depth))
	{
		m_width = width;
		m_height = height;
		m_depth = depth;
	}
}
