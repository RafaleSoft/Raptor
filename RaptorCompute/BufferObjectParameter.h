#if !defined(AFX_BUFFER_OBJECT_PARAMETER_H__58A06F22_D02F_4073_8F73_6C3FA9E8A9C6__INCLUDED_)
#define AFX_BUFFER_OBJECT_PARAMETER_H__58A06F22_D02F_4073_8F73_6C3FA9E8A9C6__INCLUDED_

#if !defined(AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_)
	#include "RaptorComputeTask.h"
#endif

class CBufferObjectParameter : public CRaptorComputeTask::Parameter<cl_mem>
{
public:
	CBufferObjectParameter(cl_mem &buffer,size_t size,bool acq = false)
		:CRaptorComputeTask::Parameter<cl_mem>(buffer),
		m_bufferSize(size),m_bAcquire(acq)
	{ };

	virtual ~CBufferObjectParameter() {};

	virtual const void* addr(void) const 
	{
		if (p != NULL)
			return &p;
		else
			return NULL;
	};
	virtual size_t size(void) const 
	{
		if (p != NULL)
			return sizeof(p);
		else
			return m_bufferSize; 
	};
	virtual ParameterBase* clone(void) const 
	{
		cl_mem _p = p;
		return new CBufferObjectParameter(_p,m_bufferSize,m_bAcquire); 
	};

	bool acquire(void) const
	{ return m_bAcquire; };

private:
	size_t	m_bufferSize;
	bool	m_bAcquire;
};

#endif