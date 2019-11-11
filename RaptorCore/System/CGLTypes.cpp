/***************************************************************************/
/*                                                                         */
/*  GLTypes.cpp			                                                   */
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

#ifndef __CGLTYPES_HPP__
    #include "CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

#ifndef _ANDROID
	#if defined(GL_ARB_transpose_matrix)
		void glGetTransposeFloatv(int e,GL_MATRIX &m)
		{ glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX_ARB + (e-GL_MODELVIEW_MATRIX),m); };
		void glGetTransposeFloatv(int e,CGenericMatrix<float> &m)
		{ glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX_ARB + (e-GL_MODELVIEW_MATRIX),m.matrix()); };
	#else
		void glGetTransposeFloatv(int e,GL_MATRIX &m)
		{
			CGenericMatrix<float> T;
			glGetFloatv(e,T.matrix());
			T.Transpose();
			memcpy(m,T.matrix(),sizeof(GL_MATRIX));
		}

		void glGetTransposeFloatv(int e,CGenericMatrix<float> &m)
		{
			glGetFloatv(e,m.matrix());
			m.Transpose();
		}
	#endif
#endif


#if defined(_WIN32) || defined(_WIN64)
    CRaptorMutex::CRaptorMutex()
    {
        ::InitializeCriticalSection (&_mutex);
    }

    CRaptorMutex::~CRaptorMutex()
    {
        ::DeleteCriticalSection (&_mutex);
    }

    void CRaptorMutex::lock(void) const
    {
        ::EnterCriticalSection (&_mutex);
    }

    void CRaptorMutex::unlock(void) const
    {
        ::LeaveCriticalSection (&_mutex);
    }
#elif defined(LINUX) || defined(_ANDROID)
    CRaptorMutex::CRaptorMutex()
    {
		::pthread_mutex_init (&_mutex, 0);
    }

    CRaptorMutex::~CRaptorMutex()
    {
        ::pthread_mutex_destroy (&_mutex);
    }

    void CRaptorMutex::lock(void) const
    {
		::pthread_mutex_lock (&_mutex);
    }

    void CRaptorMutex::unlock(void) const
    {
		::pthread_mutex_unlock (&_mutex);
    }
#endif

void CRaptorSemaphore::P() const
{
#if defined(_WIN32) || defined(_WIN64)
	::ReleaseSemaphore(_sem,1,NULL);
#elif defined(LINUX) || defined(_ANDROID)
	sem_post(&_sem);
#endif
}


#if defined(_WIN32) || defined(_WIN64)
	CRaptorSemaphore::CRaptorSemaphore(int value)
	{
		_sem = CreateSemaphore(NULL,value,LONG_MAX,NULL);
	}
	CRaptorSemaphore::~CRaptorSemaphore()
	{
		::CloseHandle(_sem);
	}
	void CRaptorSemaphore::V() const
	{
		::WaitForSingleObject(_sem,INFINITE);
	}
#elif defined(LINUX) || defined(_ANDROID)
	CRaptorSemaphore::CRaptorSemaphore(int value)
	{
		sem_init(&_sem,0,value);
	}
	CRaptorSemaphore::~CRaptorSemaphore()
	{
		sem_destroy(&_sem);
	}
	void CRaptorSemaphore::V() const
	{
		sem_wait(&_sem);
	}
#endif

RAPTOR_NAMESPACE_END


