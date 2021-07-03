/***************************************************************************/
/*                                                                         */
/*  ReadBufferTask.h                                                       */
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


#if !defined(AFX_READ_BUFFER_TASK_H__52DD0201_11DA_4876_A524_56FD02F8C17F)
#define AFX_READ_BUFFER_TASK_H__52DD0201_11DA_4876_A524_56FD02F8C17F

#if !defined(AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_)
	#include "RaptorComputeTask.h"
#endif


//!	This class defines read buffer task to be added to 
//! the list of tasks of a job.
template <class P>
class CReadBufferTask : public CRaptorComputeTask
{
public:
	//! Constructor
	//! @param bo : a valid buffer object
	//! @param userBuffer : a valid pointer to user (host) memory block where data will be received.
	//!	If a user memory block is not supplied, the class will provide an internal one, unaccessible to the user.
	CReadBufferTask(const CRaptorComputeMemory::IBufferObject &bo,
					P *userBuffer = NULL)
		:CRaptorComputeTask("ReadBuffer",bo.getSize(),0),
		m_pBuffer(userBuffer)
	{
		// TODO : use Raptor memory allocator to reuse buffers ...
		destroyBuffer = (userBuffer == NULL);
		if (destroyBuffer)
			m_pBuffer = new P[bo.getSize() / sizeof(P) + 1];

		addParameter(bo);
		addParameter(m_pBuffer);
	};

	//! Destructor
	virtual ~CReadBufferTask(void)
	{
		if ((destroyBuffer) && (m_pBuffer != NULL))
			delete [] m_pBuffer;
	};

	//! Returns the receive buffer.
	const P* getResult(void) const { return m_pBuffer; };

	const P& operator[](unsigned int pos) const { return m_pBuffer[pos]; };
	P& operator[](unsigned int pos) { return m_pBuffer[pos]; };

private:
	bool	destroyBuffer;
	P		*m_pBuffer;
};

#endif	//  #define AFX_READ_BUFFER_TASK_H__52DD0201_11DA_4876_A524_56FD02F8C17F