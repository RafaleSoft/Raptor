// ResourceAllocator.cpp: implementation of the CResourceAllocator class.
//
//////////////////////////////////////////////////////////////////////

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

bool bindArray(CProgramParameters::GL_VERTEX_ATTRIB index, CRaptorDisplayConfig::GL_ARRAY_STATE &state)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (state.enable)
	{
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(state.arraySize,
						state.arrayType,
						state.arrayStride,
						state.arrayPointer);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glEnableVertexAttribArrayARB(index);
		pExtensions->glVertexAttribPointerARB(index,
											  state.arraySize,
											  state.arrayType,
											  false,
											  state.arrayStride,
											  state.arrayPointer);
#else
		return false;
#endif
	}
	else
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glDisableClientState(GL_VERTEX_ARRAY);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glDisableVertexAttribArrayARB(index);
#else
		return false;
#endif

	return true;
}

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
		bindingState.vertexArray.enable = true;
		bindingState.vertexArray.arrayPointer = vertexPointer;
		bindingState.vertexArray.arrayStride = stride;
	}
	else
		bindingState.vertexArray.enable = false;

	return true;
}

bool CResourceAllocator::CResourceBinder::glvkBindArrays(void)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (bindingState.vertexArray.enable)
	{
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(bindingState.vertexArray.arraySize,
						bindingState.vertexArray.arrayType,
						bindingState.vertexArray.arrayStride,
						bindingState.vertexArray.arrayPointer);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::POSITION);
		pExtensions->glVertexAttribPointerARB(CProgramParameters::POSITION, 
											  bindingState.vertexArray.arraySize, 
											  bindingState.vertexArray.arrayType,
											  false, 
											  bindingState.vertexArray.arrayStride,
											  bindingState.vertexArray.arrayPointer);
#else
		return false;
#endif
	}
	else
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glDisableClientState(GL_VERTEX_ARRAY);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::POSITION);
#else
		return false;
#endif


	if (bindingState.colorArray.enable)
	{
		pExtensions->glEnableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);
		pExtensions->glVertexAttribPointerARB(CProgramParameters::PRIMARY_COLOR,
											  bindingState.vertexArray.arraySize,
											  bindingState.vertexArray.arrayType,
											  false, 
											  bindingState.vertexArray.arrayStride,
											  bindingState.vertexArray.arrayPointer);
	}
	else
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);

	if (0)
	{
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


	return true;
}


bool CResourceAllocator::CResourceBinder::glvkUnbindArrays(void)
{
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

	if (bindingState.vertexArray.enable)
#if defined(GL_COMPATIBILITY_profile) || defined (GL_FULL_profile)
		glDisableClientState(GL_VERTEX_ARRAY);
#elif defined(GL_ARB_vertex_program)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::POSITION);
#else
		return false;
#endif

	if (bindingState.colorArray.enable)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::PRIMARY_COLOR);

	if (0)
		pExtensions->glDisableVertexAttribArrayARB(CProgramParameters::ADDITIONAL_PARAM1);

	return true;
}
