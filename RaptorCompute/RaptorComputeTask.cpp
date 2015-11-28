#include "StdAfx.h"

#if !defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)
	#include "RaptorCompute.h"
#endif

#if !defined(AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_)
	#include "RaptorComputeTask.h"
#endif

#if !defined(AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_)
	#include "RaptorComputeManager.h"
#endif

#if !defined(AFX_BUFFER_OBJECT_PARAMETER_H__58A06F22_D02F_4073_8F73_6C3FA9E8A9C6__INCLUDED_)
	#include "BufferObjectParameter.h"
#endif


RAPTOR_NAMESPACE

#include <strstream>


CRaptorComputeTask::CRaptorComputeTask(	const std::string& name,
										size_t globalSize,
										size_t localSize)
	:m_name(name)
{
	memset(m_globalSize,0,sizeof(m_globalSize));
	memset(m_localSize,0,sizeof(m_localSize));
	m_globalSize[0] = globalSize;
	m_localSize[0] = localSize;

	if (localSize > 0)
	{
		if ((globalSize % localSize) != 0)
		{
			ostrstream str;
			str << "Task " << name;
			str << " initialised with incompatible sizes: ";
			str << globalSize << " is not a multiple of " << LocalSize << ends;
			RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());
			str.freeze(0);
		}
	}
}

CRaptorComputeTask::CRaptorComputeTask(const CRaptorComputeTask& task)
	:m_name(task.m_name)
{
	for (size_t i=0;i<task.m_parameters.size();i++)
	{
		ParameterBase *param = task.m_parameters[i];
		if (param->isA((cl_mem)0))
			::clRetainMemObject(((Parameter<cl_mem>*)param)->p);
		
		m_parameters.push_back(param->clone());
	}

	memcpy(m_globalSize,task.m_globalSize,sizeof(m_globalSize));
	memcpy(m_localSize,task.m_localSize,sizeof(m_localSize));
}

CRaptorComputeTask::~CRaptorComputeTask(void)
{
	while (!m_parameters.empty())
	{
		ParameterBase* param = m_parameters[0];
		if (NULL != param)
		{
			m_parameters.erase(m_parameters.begin());
			if (param->isA((cl_mem)0))
				::clReleaseMemObject(((Parameter<cl_mem>*)param)->p);

			delete param;
		}
	}
}

void CRaptorComputeTask::setGlobalSize(const size_t gs)
{
	m_globalSize[0] = gs;

	if (m_localSize[0] != 0)
	{
		if ((m_globalSize[0] % m_localSize[0]) != 0)
		{
			ostrstream str;
			str << "Task " << m_name;
			str << " initialised with incompatible sizes: ";
			str << m_globalSize[0] << " is not a multiple of " << m_localSize[0] << ends;
			RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());
			str.freeze(0);
		}
	} 
}

void CRaptorComputeTask::setLocalSize(const size_t ls)
{ 
	m_localSize[0] = ls;

	if (m_localSize[0] != 0)
	{
		if ((m_globalSize[0] % m_localSize[0]) != 0)
		{
			ostrstream str;
			str << "Task " << m_name;
			str << " initialised with incompatible sizes: ";
			str << m_globalSize[0] << " is not a multiple of " << m_localSize[0] << ends;
			RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());
			str.freeze(0);
		}
	}
}

void CRaptorComputeTask::addParameter(const CRaptorComputeMemory::IBufferObject &bo)
{
	cl_mem buffer = (cl_mem)bo.getBaseAddress();

	if (buffer != NULL)
	{
		bool isInterop = (bo.getStorage() == CRaptorComputeMemory::IBufferObject::INTEROP_COMPUTE_BUFFER);
		cl_int res = ::clRetainMemObject(buffer);
		m_parameters.push_back(new CBufferObjectParameter(buffer,bo.getSize(),isInterop));
	}
	else if (bo.getStorage() == CRaptorComputeMemory::IBufferObject::LOCAL_BUFFER)
		m_parameters.push_back(new CBufferObjectParameter(buffer,bo.getSize()));
}

void CRaptorComputeTask::setParameter(size_t pos,const CRaptorComputeMemory::IBufferObject &bo)
{
	if (pos < m_parameters.size())
	{
		cl_mem buffer = (cl_mem)bo.getBaseAddress();

		if ((buffer != NULL) && (m_parameters[pos]->isA(buffer)))
		{
			Parameter<cl_mem>* param = (Parameter<cl_mem>*)m_parameters[pos];
			cl_int res = ::clReleaseMemObject(param->p);
			res = ::clRetainMemObject(buffer);

			((Parameter<cl_mem>*)m_parameters[pos])->p = buffer;
		}
	}
}
