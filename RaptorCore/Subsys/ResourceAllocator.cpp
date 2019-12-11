/***************************************************************************/
/*                                                                         */
/*  ResourceAllocator.cpp                                                  */
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


#include "Subsys\CodeGeneration.h"

#if !defined(AFX_RESOURCEALLOCATOR_H__4BAB58CE_942B_450D_88C9_AF0DDDF03718__INCLUDED_)
	#include "ResourceAllocator.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_PROGRAMPARAMETERS_H__E28A74BB_DE78_470A_A8A2_5A3EBB3F4F90__INCLUDED_)
	#include "GLHierarchy/ProgramParameters.h"
#endif


RAPTOR_NAMESPACE

static bool arrays_initialized = false;

CRaptorDisplayConfig::GL_ARRAYS_STATE	CResourceAllocator::CResourceBinder::bindingState;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResourceAllocator::CResourceAllocator()
	:m_bLocked(false),
	deviceMemoryManager(NULL)
{
}


CResourceAllocator::~CResourceAllocator()
{
}

CResourceAllocator::CResourceBinder::CResourceBinder(void)
{
	if (!arrays_initialized)
	{
		memset(&bindingState, 0, sizeof(CRaptorDisplayConfig::GL_ARRAYS_STATE));
		arrays_initialized = true;

		bindingState.vertexArray.arrayIndex = CProgramParameters::POSITION;
		bindingState.vertexArray.arrayName = GL_VERTEX_ARRAY;
		bindingState.vertexArray.arrayType = GL_FLOAT;
		bindingState.vertexArray.arraySize = 4;
	}
}

CResourceAllocator::CResourceBinder::~CResourceBinder(void)
{
	glvkUnbindArrays();
}

bool CResourceAllocator::CResourceBinder::setVertexArray(void *vertexPointer, int stride)
{
	if (NULL != vertexPointer)
	{
		bindings.vertexArray.enable = true;
		bindings.vertexArray.arrayPointer = vertexPointer;
		bindings.vertexArray.arrayStride = stride;
	}
	else
		bindings.vertexArray.enable = false;

	return true;
}

bool CResourceAllocator::CResourceBinder::glvkBindArrays(void)
{
	return	bindArray(bindings.vertexArray) &&
			bindArray(bindings.colorArray);

	//MAX_VERTEX_ATTRIBS_ARB

	/*
	if (0)
	{
		const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);
		pExtensions->glVertexAttribPointerARB(CProgramParameters::ADDITIONAL_PARAM1, 
											  4, 
											  GL_FLOAT, 
											  false, 
											  0, 
											  NULL);
	}
	else
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);
		*/

	return true;
}


bool CResourceAllocator::CResourceBinder::glvkUnbindArrays(void)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	return	unbindArray(bindings.vertexArray) &&
			unbindArray(bindings.colorArray);

	if (bindingState.vertexArray.enable)
	{
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glDisableClientState(GL_VERTEX_ARRAY);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::POSITION);
#else
		return false;
#endif
	}

	if (bindingState.colorArray.enable)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);

	if (0)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);

	return true;
}


bool CResourceAllocator::CResourceBinder::bindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (state.enable)
	{
		if (!bindingState.vertexArray.enable)
		{
			bindingState.vertexArray.enable = true;
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
			glEnableClientState(state.arrayName);
#elif defined(GL_ARB_vertex_program)
			pExtensions->glEnableVertexAttribArrayARB(state.arrayIndex);
#endif
		}

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glVertexPointer(state.arraySize,
						state.arrayType,
						state.arrayStride,
						state.arrayPointer);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glVertexAttribPointerARB(state.arrayIndex,
											  state.arraySize,
											  state.arrayType,
											  false,
											  state.arrayStride,
											  state.arrayPointer);
#else
		return false;
#endif
	}
	else if (bindingState.vertexArray.enable)
	{
		bindingState.vertexArray.enable = false;
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glDisableClientState(state.arrayName);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glDisableVertexAttribArrayARB(state.arrayIndex);
#else
		return false;
#endif
	}

	return true;
}


bool CResourceAllocator::CResourceBinder::unbindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (bindingState.vertexArray.enable)
	{
		bindingState.vertexArray.enable = false;
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glDisableClientState(state.arrayName);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glDisableVertexAttribArrayARB(state.arrayIndex);
#else
		return false;
#endif
	}

	return true;
}
