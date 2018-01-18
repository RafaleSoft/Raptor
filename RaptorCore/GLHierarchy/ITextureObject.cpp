// ITextureObject.cpp: implementation of the ITextureObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "ITextureObject.h"
#endif
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
		#include "System/RaptorErrorManager.h"
	#endif
	#ifndef __GLOBAL_H__
		#include "System/Global.h"
	#endif
	#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
		#include "System/Raptor.h"
	#endif
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ITextureObject::ITextureObject()
	:m_width(0), m_height(0), m_depth(0)
{
}


ITextureObject::~ITextureObject()
{
}


ITextureObject::ITextureObject(const ITextureObject& rsh)
{
	m_width = rsh.m_width;
	m_height = rsh.m_height;
	m_depth = rsh.m_depth;
}


void ITextureObject::setSize(uint32_t width, uint32_t height, uint32_t depth)
{
	if ((width == 0) || (height == 0) || (depth == 0))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
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
