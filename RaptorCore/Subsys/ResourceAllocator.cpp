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
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


RAPTOR_NAMESPACE


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
#if defined(GL_ARB_VERTEX_ARRAY_OBJECT_EXTENSION_NAME)
	: array(0), updateArray(false)
#endif
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	if (!instance.arrays_initialized)
	{
		memset(&instance.bindingState, 0, sizeof(CRaptorDisplayConfig::GL_ARRAYS_STATE));
		instance.arrays_initialized = true;

		instance.bindingState.vertexArray.arrayIndex = CProgramParameters::POSITION;
		instance.bindingState.vertexArray.arrayName = GL_VERTEX_ARRAY;
		instance.bindingState.vertexArray.arrayType = GL_FLOAT;
		instance.bindingState.vertexArray.arraySize = 4;

		instance.bindingState.colorArray.arrayIndex = CProgramParameters::PRIMARY_COLOR;
		instance.bindingState.colorArray.arrayName = GL_COLOR_ARRAY;
		instance.bindingState.colorArray.arrayType = GL_FLOAT;
		instance.bindingState.colorArray.arraySize = 4;

		instance.bindingState.colorArray.arrayIndex = CProgramParameters::NORMAL;
		instance.bindingState.colorArray.arrayName = GL_NORMAL_ARRAY;
		instance.bindingState.colorArray.arrayType = GL_FLOAT;
		instance.bindingState.colorArray.arraySize = 4;
	}
}

CResourceAllocator::CResourceBinder::~CResourceBinder(void)
{
	glvkUnbindArrays();
}

bool CResourceAllocator::CResourceBinder::setArray(CProgramParameters::GL_VERTEX_ATTRIB attribute, 
												   void *attribPointer, 
												   int stride)
{
	CRaptorDisplayConfig::GL_ARRAY_STATE *array = NULL;
	switch (attribute)
	{
		case CProgramParameters::POSITION:
			array = &bindings.vertexArray;
			break;
		case CProgramParameters::PRIMARY_COLOR:
			array = &bindings.colorArray;
			break;
		case CProgramParameters::NORMAL:
			array = &bindings.normalArray;
			break;
		default:
			array = NULL;
			break;
	}

	if (NULL != attribPointer)
	{
		array->enable = true;
		array->arrayPointer = attribPointer;
		array->arrayStride = stride;
	}
	else
		array->enable = false;

	updateArray = true;
	return true;
}

bool CResourceAllocator::CResourceBinder::glvkBindArrays(void)
{
#if defined(GL_ARB_VERTEX_ARRAY_OBJECT_EXTENSION_NAME)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (array == 0)
		pExtensions->glGenVertexArraysARB(1, &array);
	
	pExtensions->glBindVertexArrayARB(array);

	if (updateArray)
	{
		bindAttribArray(bindings.vertexArray);
		updateArray = false;
	}

	return true;
#else
	return	bindArray(bindings.vertexArray) &&
			bindArray(bindings.colorArray);
#endif
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
}


bool CResourceAllocator::CResourceBinder::glvkUnbindArrays(void)
{
#if defined(GL_ARB_VERTEX_ARRAY_OBJECT_EXTENSION_NAME)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glBindVertexArrayARB(0);
	return true;
#else
	return	unbindArray(bindings.vertexArray) &&
			unbindArray(bindings.colorArray);
#endif
}

bool CResourceAllocator::CResourceBinder::bindAttribArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state)
{
#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (state.enable)
	{
		pExtensions->glEnableVertexAttribArrayARB(state.arrayIndex);
		pExtensions->glVertexAttribPointerARB(state.arrayIndex,
											  state.arraySize,
											  state.arrayType,
											  false,
											  state.arrayStride,
											  state.arrayPointer);
	}
	else
	{
		pExtensions->glDisableVertexAttribArrayARB(state.arrayIndex);
	}
	return true;
#endif
	return false;
}

bool CResourceAllocator::CResourceBinder::bindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	if (state.enable)
	{
		if (!instance.bindingState.vertexArray.enable)
		{
			instance.bindingState.vertexArray.enable = true;
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
	else if (instance.bindingState.vertexArray.enable)
	{
		instance.bindingState.vertexArray.enable = false;
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
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	if (instance.bindingState.vertexArray.enable)
	{
		instance.bindingState.vertexArray.enable = false;
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
