/***************************************************************************/
/*                                                                         */
/*  ShaderBloc.cpp                                                         */
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


#if !defined(AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_)
	#include "GLHierarchy/ShaderBloc.h"
#endif
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
#endif

RAPTOR_NAMESPACE



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShaderBloc::CShaderBloc(BLOC_KIND kind, uint8_t *uniform, uint64_t size, uint64_t offset)
	:m_buffer(uniform), m_size(size), m_offset(offset), m_bExternal(true), m_index(0)
{
	if (NULL == m_buffer)
	{
		m_bExternal = false;
		m_offset = 0;

		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		m_buffer = pUAllocator->allocateUniforms(size);
	}
}

CShaderBloc::~CShaderBloc()
{
	if (!m_bExternal)
	{
		CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
		pUAllocator->releaseUniforms(m_buffer);
	}
}

void CShaderBloc::glvkUpdateBloc(uint8_t *src)
{
	if (NULL == src)
		return;

	CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();

	if (!m_bExternal)
		pUAllocator->glvkSetPointerData(m_buffer, src, m_size);
}

void CShaderBloc::glRender()
{
	CUniformAllocator*	pUAllocator = CUniformAllocator::GetInstance();
	pUAllocator->glvkBindUniform(m_buffer, m_index, m_offset, m_size);
}

