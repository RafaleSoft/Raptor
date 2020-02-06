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

#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
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
	: array(0), updateArray(false), legacy(true), vao(false)
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

		s.sColorArray.arrayIndex = CProgramParameters::SECONDARY_COLOR;
		s.sColorArray.arrayName = GL_SECONDARY_COLOR_ARRAY;
		s.sColorArray.arrayType = GL_FLOAT;
		s.sColorArray.arraySize = 4;

		s.normalArray.arrayIndex = CProgramParameters::NORMAL;
		s.normalArray.arrayName = GL_NORMAL_ARRAY;
		s.normalArray.arrayType = GL_FLOAT;
		s.normalArray.arraySize = 4;

		s.texture0Array.arrayIndex = CProgramParameters::TEXCOORD0;
		s.texture0Array.arrayName = GL_TEXTURE_COORD_ARRAY;
		s.texture0Array.arrayType = GL_FLOAT;
		s.texture0Array.arraySize = 2;

		s.texture1Array.arrayIndex = CProgramParameters::TEXCOORD1;
		s.texture1Array.arrayName = GL_TEXTURE_COORD_ARRAY;
		s.texture1Array.arrayType = GL_FLOAT;
		s.texture1Array.arraySize = 2;

		s.additionalArray.arrayIndex = CProgramParameters::ADDITIONAL_PARAM1;
		s.additionalArray.arrayName = 0;
		s.additionalArray.arrayType = GL_FLOAT;
		s.additionalArray.arraySize = 4;

		s.additionalArray.arrayIndex = CProgramParameters::FOG_COORDINATE;
#ifdef GL_EXT_fog_coord
		s.additionalArray.arrayName = GL_FOG_COORDINATE_ARRAY_EXT;
#else
		s.additionalArray.arrayName = 0;
#endif
		s.additionalArray.arrayType = GL_FLOAT;
		s.additionalArray.arraySize = 1;

		s.additionalArray.arrayIndex = CProgramParameters::WEIGHTS;
#ifdef GL_EXT_vertex_weighting
		s.additionalArray.arrayName = GL_VERTEX_WEIGHT_ARRAY_EXT;
#else
		s.additionalArray.arrayName = 0;
#endif
		s.additionalArray.arrayType = GL_FLOAT;
		s.additionalArray.arraySize = 1;
	}
}

bool CResourceAllocator::CResourceBinder::useVertexArrayObjects(void)
{
	if (!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_ARRAY_OBJECT_EXTENSION_NAME))
	{
		vao = false;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_ERROR,
													   "Resource Binder has no Vertex Arrays Objects to render Geometry");
#endif
	}
	else
	{
		vao = false; // true;
		legacy = false;
		return true;
	}

#if defined(GL_ARB_vertex_program) || defined(GL_CORE_profile)
	//! If VAO are not available, at least use Vertey Attrib Arrays.
	if (!Raptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME))
	{
	#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_FATAL,
														   "Resource Binder has no Vertex Arrays to render Geometry");
	#endif
		legacy = true;
		return false;
	}
	else
	{
		legacy = false;
		return true;
	}
#else
	legacy = true;
	return false;
#endif
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
			array->arrayName = GL_VERTEX_ARRAY;
			break;
		case CProgramParameters::PRIMARY_COLOR:
			array = &bindings.attributes.colorArray;
			array->arraySize = size;
			array->arrayName = GL_COLOR_ARRAY;
			break;
		case CProgramParameters::SECONDARY_COLOR:
			array = &bindings.attributes.sColorArray;
			array->arraySize = size;
			array->arrayName = GL_SECONDARY_COLOR_ARRAY;
			break;
		case CProgramParameters::NORMAL:
			array = &bindings.attributes.normalArray;
			array->arraySize = size;
			array->arrayName = GL_NORMAL_ARRAY;
			break;
		case CProgramParameters::TEXCOORD0:
			array = &bindings.attributes.texture0Array;
			array->arraySize = size;
			array->arrayName = GL_TEXTURE_COORD_ARRAY;
			break;
		case CProgramParameters::TEXCOORD1:
			array = &bindings.attributes.texture1Array;
			array->arraySize = size;
			array->arrayName = 0;
			break;
		case CProgramParameters::ADDITIONAL_PARAM1:
			array = &bindings.attributes.additionalArray;
			array->arraySize = size;
			array->arrayName = 0;
			break;
		case CProgramParameters::FOG_COORDINATE:
			array = &bindings.attributes.fogCoordArray;
			array->arraySize = size;
			array->arrayName = 0;
#ifdef GL_EXT_fog_coord
			array->arrayName = GL_FOG_COORDINATE_ARRAY_EXT;
#else
			array->arrayName = 0;
#endif
			break;
		case CProgramParameters::WEIGHTS:
			array = &bindings.attributes.weightArray;
			array->arraySize = size;
#ifdef GL_EXT_vertex_weighting
			array->arrayName = GL_VERTEX_WEIGHT_ARRAY_EXT;
#else
			array->arrayName = 0;
#endif
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
	if (!legacy)
	{
		bool res = true;
		if (vao)
		{
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

			if (array == 0)
				pExtensions->glGenVertexArraysARB(1, &array);

			pExtensions->glBindVertexArrayARB(array);

			// TODO : Re-bind buffers.

			if (updateArray)
			{
				for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
					res = res && bindAttribArray(bindings.arrays[i]);

				updateArray = false;
			}
		}
		else
		{
			for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
				res = res && bindAttribArray(bindings.arrays[i]);
		}
		return res;
	}
	else
	{
		bool res = true;
		CRaptorInstance &instance = CRaptorInstance::GetInstance();

		for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
			res = res && bindArray(bindings.arrays[i], instance.bindingState.arrays[i]);

		return res;
	}

	CATCH_GL_ERROR
}


bool CResourceAllocator::CResourceBinder::glvkUnbindArrays(void)
{
	if (!legacy)
	{
		if (vao)
		{
			const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
			pExtensions->glBindVertexArrayARB(0);
			return true;
		}
		else
		{
			bool res = true;
			for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
				res = res && unbindAttribArray(bindings.arrays[i]);
			return res;
		}
	}
	else
	{
		bool res = true;
		CRaptorInstance &instance = CRaptorInstance::GetInstance();

		for (size_t i = 0; i < CProgramParameters::GL_VERTEX_ATTRIB_t::MAX_VERTEX_ATTRIB; i++)
			res = res && unbindArray(bindings.arrays[i], instance.bindingState.arrays[i]);
		return res;
	}

	CATCH_GL_ERROR
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
	//	TODO: Need an else to disable existing array in current state ?
	//	Likely if vertex array object not supperted.
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

bool CResourceAllocator::CResourceBinder::bindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state,
													CRaptorDisplayConfig::GL_ARRAY_STATE &global_state)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (state.enable)
	{
		if (!global_state.enable)
		{
			global_state.enable = true;
			glEnableClientState(state.arrayName);
		}

		switch (state.arrayName)
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
#ifdef GL_EXT_fog_coord
			case GL_FOG_COORDINATE_ARRAY_EXT:
				pExtensions->glFogCoordPointerEXT(state.arrayType, state.arrayStride, state.arrayPointer);
				break;
#endif
#ifdef GL_EXT_vertex_weighting
			case GL_VERTEX_WEIGHT_ARRAY_EXT:
				pExtensions->glVertexWeightPointerEXT(state.arraySize, state.arrayType, state.arrayStride, state.arrayPointer);
				break;
#endif
		}
	}
	else if (global_state.enable)
	{
		global_state.enable = false;
		glDisableClientState(state.arrayName);
	}

	return true;
}


bool CResourceAllocator::CResourceBinder::unbindArray(CRaptorDisplayConfig::GL_ARRAY_STATE &state,
													  CRaptorDisplayConfig::GL_ARRAY_STATE &global_state)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (global_state.enable)
	{
		global_state.enable = false;
		glDisableClientState(state.arrayName);
	}

	return true;
}
