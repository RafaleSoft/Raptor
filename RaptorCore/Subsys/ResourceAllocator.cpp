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
	memset(&bindings, 0, sizeof(CRaptorDisplayConfig::GL_ARRAYS_STATE));

	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	if (!instance.arrays_initialized)
	{
		memset(&instance.bindingState, 0, sizeof(CRaptorDisplayConfig::GL_ARRAYS_STATE));
		instance.arrays_initialized = true;

		CRaptorDisplayConfig::GL_ARRAYS_STATE::GL_ARRAYS_STATE_TAG &s = instance.bindingState.attributes;
		s.vertexArray.arrayIndex = CProgramParameters::POSITION;
		s.vertexArray.arrayName = GL_VERTEX_ARRAY;
		s.vertexArray.arrayType = GL_FLOAT;
		s.vertexArray.arraySize = 4;

		s.colorArray.arrayIndex = CProgramParameters::PRIMARY_COLOR;
		s.colorArray.arrayName = GL_COLOR_ARRAY;
		s.colorArray.arrayType = GL_FLOAT;
		s.colorArray.arraySize = 4;

		s.colorArray.arrayIndex = CProgramParameters::NORMAL;
		s.colorArray.arrayName = GL_NORMAL_ARRAY;
		s.colorArray.arrayType = GL_FLOAT;
		s.colorArray.arraySize = 4;

		s.textureArray.arrayIndex = CProgramParameters::TEXCOORD0;
		s.textureArray.arrayName = GL_TEXTURE_COORD_ARRAY;
		s.textureArray.arrayType = GL_FLOAT;
		s.textureArray.arraySize = 2;

		s.additionalArray.arrayIndex = CProgramParameters::ADDITIONAL_PARAM1;
		s.additionalArray.arrayName = 0;
		s.additionalArray.arrayType = GL_FLOAT;
		s.additionalArray.arraySize = 4;
	}
}

CResourceAllocator::CResourceBinder::~CResourceBinder(void)
{
	glvkUnbindArrays();
}

bool CResourceAllocator::CResourceBinder::setArray(CProgramParameters::GL_VERTEX_ATTRIB attribute, 
												   void *attribPointer,
												   size_t size,
												   size_t stride)
{
	CRaptorDisplayConfig::GL_ARRAY_STATE *array = NULL;
	switch (attribute)
	{
		case CProgramParameters::POSITION:
			array = &bindings.attributes.vertexArray;
			array->arraySize = size;
			break;
		case CProgramParameters::PRIMARY_COLOR:
			array = &bindings.attributes.colorArray;
			array->arraySize = size;
			break;
		case CProgramParameters::NORMAL:
			array = &bindings.attributes.normalArray;
			array->arraySize = size;
			break;
		case CProgramParameters::TEXCOORD0:
			array = &bindings.attributes.textureArray;
			array->arraySize = size;
			break;
		case CProgramParameters::ADDITIONAL_PARAM1:
			array = &bindings.attributes.additionalArray;
			array->arraySize = size;
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
		array->arrayIndex = attribute;
		array->arrayType = GL_FLOAT;
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

	bool res = true;
	if (updateArray)
	{
		for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
			res = res && bindAttribArray(bindings.arrays[i]);

		updateArray = false;
	}

	return res;
#elif defined(GL_ARB_vertex_program)
	bool res = true;
	for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
		res = res && bindAttribArray(bindings.arrays[i]);
	
	return res;
#else
	bool res = true;
	for (size_t i = 0; i < 16; i++)
		res = res && bindArray(bindings.arrays[i]);

	return res;
#endif
}


bool CResourceAllocator::CResourceBinder::glvkUnbindArrays(void)
{
#if defined(GL_ARB_VERTEX_ARRAY_OBJECT_EXTENSION_NAME)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glBindVertexArrayARB(0);
	return true;
#elif defined(GL_ARB_vertex_program)
	bool res = true;
	for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
		res = res && unbindAttribArray(bindings.arrays[i]);
	return res;
#else
	bool res = true;
	for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
		res = res && unbindArray(bindings.arrays[i]);
	return res;
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
	return true;
#else
	return false;
#endif
}

bool CResourceAllocator::CResourceBinder::unbindAttribArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state)
{
#if defined(GL_ARB_vertex_program)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (state.enable)
		pExtensions->glDisableVertexAttribArrayARB(state.arrayIndex);
	return true;
#else
	return false;
#endif
}

bool CResourceAllocator::CResourceBinder::bindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	if (state.enable)
	{
		if (!instance.bindingState.attributes.vertexArray.enable)
		{
			instance.bindingState.attributes.vertexArray.enable = true;
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
			glEnableClientState(state.arrayName);
#elif defined(GL_ARB_vertex_program)
			pExtensions->glEnableVertexAttribArrayARB(state.arrayIndex);
#endif
		}

#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		switch(state.arrayName)
		{
			case GL_VERTEX_ARRAY:
				glVertexPointer(state.arraySize, state.arrayType, state.arrayStride, state.arrayPointer);
				break;
			case GL_NORMAL_ARRAY:
				glNormalPointer(state.arrayType, state.arrayStride, state.arrayPointer);
				break;
			case GL_COLOR_ARRAY:
				glColorPointer(state.arraySize, state.arrayType, state.arrayStride, state.arrayPointer);
				break;
			case GL_TEXTURE_COORD_ARRAY:
				glTexCoordPointer(state.arraySize, state.arrayType, state.arrayStride, state.arrayPointer);
				break;
		}
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
	else if (instance.bindingState.attributes.vertexArray.enable)
	{
		instance.bindingState.attributes.vertexArray.enable = false;
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

	if (instance.bindingState.attributes.vertexArray.enable)
	{
		instance.bindingState.attributes.vertexArray.enable = false;
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
