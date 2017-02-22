#include "StdAfx.h"

#if !defined(AFX_RAPTOR_COMPUTE_H__7FDED2CB_7AA2_4D6E_BF22_50923FD45212__INCLUDED_)
	#include "RaptorCompute.h"
#endif

#if !defined(AFX_RAPTOR_COMPUTE_JOB_H__BCAF8B25_FC2C_49BB_A710_A0D51F6DDFD0__INCLUDED_)
	#include "RaptorComputeJob.h"
#endif

#if !defined(AFX_RAPTOR_COMPUTE_MANAGER_H__876B757E_63F1_4E8A_847E_205C3AE2E132__INCLUDED_)
	#include "RaptorComputeManager.h"
#endif

#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

#if !defined(AFX_RAPTOR_COMPUTE_TASK_H__F05CF7F9_151A_4760_885A_19FF9C648278__INCLUDED_)
	#include "RaptorComputeTask.h"
#endif

#if !defined(AFX_BUFFER_OBJECT_PARAMETER_H__58A06F22_D02F_4073_8F73_6C3FA9E8A9C6__INCLUDED_)
	#include "BufferObjectParameter.h"
#endif


RAPTOR_NAMESPACE

#include <sstream>

static const unsigned int MAX_INTEROP = 16;

void CL_CALLBACK job_event_callback(cl_event event,cl_int event_command_exec_status,void *user_data)
{
}

CRaptorComputeJob *CRaptorComputeJob::clCreateJob(	unsigned int numPlatform,
													unsigned int numDevice,
													CRaptorDisplay *dsp)
{
	CRaptorComputeManager &manager = CRaptorComputeManager::GetInstance();

	bool res = manager.clCreateContext(	numPlatform,
										numDevice,
										dsp);
	if (!res)
	{
		RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),
						"Impossible to initialise an OpenCL context on requested platform.");
		return NULL;
	}

	CRaptorComputeJob *pJob = NULL;
	cl_int ciErrNum = CL_SUCCESS;

	const std::vector<CRaptorComputeManager::PLATFORM_ID>& ids = manager.getPlatformIDs();
	const CRaptorComputeManager::PLATFORM_ID &id = ids[numPlatform];

	pJob = new CRaptorComputeJob();
	pJob->m_pContext = id.contexts[numDevice];

	// CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE seems to be unsupported on intel processors, despite device info.
	pJob->m_pQueue = clCreateCommandQueue(	id.contexts[numDevice],
											id.deviceIDs[numDevice],
											0/*CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/,
											&ciErrNum);

	if (ciErrNum != CL_SUCCESS)
		pJob->m_pQueue = NULL;

	return pJob;
}

CRaptorComputeJob::CRaptorComputeJob()
	:m_pQueue(NULL),m_pProgram(NULL),m_pContext(NULL)
{
	m_kernels["ReadBuffer"] = NULL;
	m_kernels["WriteBuffer"] = NULL;
}

CRaptorComputeJob::~CRaptorComputeJob(void)
{
}

bool CRaptorComputeJob::clDestroyTasks(void)
{
	vector<CRaptorComputeTask*>::iterator it = m_pTasks.begin();
	while (it != m_pTasks.end())
	{
		CRaptorComputeTask* pTask = *it++;
		delete pTask;
	}

	m_pTasks.clear();
	return true;
}

bool CRaptorComputeJob::clDestroyJob(bool destroyTasks)
{
	cl_int ciErrNum = CL_SUCCESS;

	if (destroyTasks)
		clDestroyTasks();
	
	map<std::string,void*>::const_iterator k = m_kernels.begin();
	while ((k != m_kernels.end()) && (ciErrNum == CL_SUCCESS))
	{
		cl_kernel kernel = (cl_kernel)((*k).second);
		if (kernel != 0)	// prevent invalid kernels. Raise warning ?
			ciErrNum |= clReleaseKernel(kernel);
		k++;
	}

	if ((m_pProgram != NULL) && (ciErrNum == CL_SUCCESS))
		clReleaseProgram((cl_program)m_pProgram);
	if ((m_pQueue != NULL) && (ciErrNum == CL_SUCCESS))
		clReleaseCommandQueue((cl_command_queue)m_pQueue);

	delete this;

	return (ciErrNum == CL_SUCCESS);
}

#ifdef CL_USE_DEPRECATED_OPENCL_1_0_APIS
bool CRaptorComputeJob::clEnableOutOfOrder(bool enable)
{
	if (m_pQueue == NULL)
		return false;

	cl_command_queue queue = (cl_command_queue)m_pQueue;
	cl_int ciErrNum = clSetCommandQueueProperty(queue,CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,enable,NULL);

	return (ciErrNum == CL_SUCCESS);
}


bool CRaptorComputeJob::clEnableProfiling(bool enable)
{
	if (m_pQueue == NULL)
		return false;

	cl_command_queue queue = (cl_command_queue)m_pQueue;
	cl_int ciErrNum = clSetCommandQueueProperty(queue,CL_QUEUE_PROFILING_ENABLE,enable,NULL);

	return (ciErrNum == CL_SUCCESS);
}
#endif

bool CRaptorComputeJob::run(bool waitCompletion)
{
	if (((cl_program)0 == m_pProgram) || 
		((cl_command_queue)0 == m_pQueue) ||
		((cl_context)0 == m_pContext))
		return false;

	if (CL_SUCCESS != ::clRetainContext((cl_context)m_pContext))
		return false;

	if (CL_SUCCESS != ::clRetainCommandQueue((cl_command_queue)m_pQueue))
		return false;

	cl_mem interop[MAX_INTEROP];
	int numInterop = 0;
	vector<CRaptorComputeTask*>::iterator it = m_pTasks.begin();
	while (it != m_pTasks.end())
	{
		const std::vector<CProgramParameters::ParameterBase*>& params = (*it++)->getParameters();
		size_t i = 0;
		for (i=0 ; (i<params.size()) ; i++)
		{
			const CProgramParameters::ParameterBase *param = params[i];
			if (param->isA((cl_mem)0))
			{
				CBufferObjectParameter *bo = (CBufferObjectParameter*)param;
				if (bo->acquire())
					interop[numInterop++] = bo->p;
			}
		}
	}

	if (numInterop > 0)
	{
		if (CL_SUCCESS != ::clEnqueueAcquireGLObjects((cl_command_queue)m_pQueue,numInterop,interop,0,0,0))
		{
			stringstream str;
			str << "unable to acquire glBuffer parameters." << ends;
			RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());

			return false;
		}
	}

	it = m_pTasks.begin();
	while (it != m_pTasks.end())
	{
		CRaptorComputeTask *pTask = (*it);
		it++;
		map<std::string,void*>::const_iterator k = m_kernels.find(pTask->getName());
		if (k != m_kernels.end())
		{
			cl_int err = CL_SUCCESS;
			cl_kernel kernel = (cl_kernel)(k->second);

			if (kernel != NULL)
			{
				const std::vector<CProgramParameters::ParameterBase*>& params = pTask->getParameters();
				size_t i = 0;
				for (i=0 ; (i<params.size()) && (err == CL_SUCCESS) ; i++)
				{
					const CProgramParameters::ParameterBase *param = params[i];
					err = ::clSetKernelArg(kernel, i, param->size(), param->addr());
				}

				if (CL_SUCCESS != err)
				{
					stringstream str;
					str << "Kernel " << pTask->getName();
					str << " failed to set argument: " << i << ends;
					RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());

					return false;
				}

				cl_event task_evt;
				cl_event *pevt = NULL;
				if (waitCompletion)
					pevt = &task_evt;
				err = ::clEnqueueNDRangeKernel(	(cl_command_queue)m_pQueue, 
												kernel, 
												1, 
												NULL, 
												pTask->getGlobalSize(), 
												pTask->getLocalSize(),
												0, NULL, pevt);
				if (CL_SUCCESS != err)
				{
					stringstream str;
					str << "Kernel " << pTask->getName();
					str << " failed to enqueue." << ends;
					RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());

					return false;
				}
				else if (waitCompletion)
				{
					clSetEventCallback(task_evt,CL_COMPLETE,job_event_callback,pTask);
					m_pTaskEvents.push_back(task_evt);
				}
			}
			else if (!pTask->getName().compare("ReadBuffer"))
			{
				const std::vector<CProgramParameters::ParameterBase*>& params = pTask->getParameters();

				CProgramParameters::Parameter<cl_mem> &buffer =
					(CProgramParameters::Parameter<cl_mem>&)(*params[0]);
				CProgramParameters::Parameter<void*> &dst =
					(CProgramParameters::Parameter<void*>&)(*params[1]);

				cl_event task_evt;
				cl_event *pevt = NULL;
				if (waitCompletion)
					pevt = &task_evt;
				err = clEnqueueReadBuffer(	(cl_command_queue)m_pQueue,
											buffer.p,
											true,		// blocking
											0,			// offset in buffer
											pTask->getGlobalSize()[0],
											dst.p,
											NULL,NULL,pevt);
				if (CL_SUCCESS != err)
				{
					stringstream str;
					str << "Kernel " << pTask->getName();
					str << " failed to read buffer." << ends;
					RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());

					return false;
				}
				else if (waitCompletion)
				{
					m_pTaskEvents.push_back(task_evt);
					clSetEventCallback(task_evt,CL_COMPLETE,job_event_callback,pTask);
				}
			}
		}
	}

	if (numInterop > 0)
	{
		if (CL_SUCCESS != ::clEnqueueReleaseGLObjects((cl_command_queue)m_pQueue,numInterop,interop,0,0,0))
		{
			stringstream str;
			str << "unable to release glBuffer parameters." << ends;
			RAPTOR_ERROR(	CRaptorComputeManager::COpenCLClassID::GetClassId(),str.str());

			return false;
		}
	}

	bool evtsReleased = true;
	if (waitCompletion)
	{
		cl_int wait = ::clWaitForEvents(m_pTaskEvents.size(),
										(const cl_event *)m_pTaskEvents.data());

		for (size_t i=0;i<m_pTaskEvents.size();i++)
		{
			cl_event evt = (cl_event)m_pTaskEvents[i];
			evtsReleased &= (CL_SUCCESS == ::clReleaseEvent(evt));
		}
		m_pTaskEvents.clear();

		evtsReleased &= (CL_SUCCESS == wait);
	}

	return ((CL_SUCCESS == ::clReleaseCommandQueue((cl_command_queue)m_pQueue)) &&
			(CL_SUCCESS == ::clReleaseContext((cl_context)m_pContext)) &&
			evtsReleased);
}

bool CRaptorComputeJob::clAddTask(const CRaptorComputeTask &rTask)
{
	CRaptorComputeTask *pTask = new CRaptorComputeTask(rTask);

	map<std::string,void*>::const_iterator k = m_kernels.find(pTask->getName());
	if (k == m_kernels.end())
	{
		cl_kernel kernel = ::clCreateKernel((cl_program)m_pProgram, pTask->getName().c_str(), NULL);
		if (kernel == (cl_kernel)0)
		{
			printf("ERROR: Failed to create kernel...\n");
			return false;
		}
		else
			m_kernels[pTask->getName()] = kernel;
	}

	m_pTasks.push_back(pTask);

	return true;
}


bool CRaptorComputeJob::clLoadPrograms(const std::vector<std::string> &programFiles)
{
	if (programFiles.empty())
		return false;

	size_t nbPrograms = programFiles.size();
	char** sources = new char*[nbPrograms];
	size_t pos = 0;
	for (size_t i = 0; i<nbPrograms; i++)
	{
		if(programFiles[i].empty())
			continue;

		string programstr = loadProgramSource(programFiles[i]);
		sources[pos] = new char[programstr.length()+1];
		strcpy(sources[pos++],programstr.c_str());
	}

	//! Release existing program ?

	cl_int	err = CL_SUCCESS;
	cl_context ctx = (cl_context)m_pContext;

	cl_program program = ::clCreateProgramWithSource(ctx, pos, (const char**)sources, NULL, &err);
	if (CL_SUCCESS != err)
		return false;

	err = ::clBuildProgram(program, 0, NULL, NULL, NULL, NULL);						// Compilation du kernel
	if (CL_SUCCESS != err)
		return false;
	else
		m_pProgram = program;

	for  (size_t i = 0;i<pos;i++)
		delete [] sources[i];
	delete [] sources;

	return false;
}

bool CRaptorComputeJob::clLoadProgram(const std::string &programFile)
{
	if (programFile.empty())
		return false;
	string programstr = loadProgramSource(programFile);

	return clLoadProgramString(programstr);
}

bool CRaptorComputeJob::clLoadProgramString(const std::string &programString)
{
	if (programString.empty())
		return false;

	cl_int	err = CL_SUCCESS;
	cl_context ctx = (cl_context)m_pContext;

	//! Release existing program ?

	const char *code = programString.c_str();
	cl_program program = clCreateProgramWithSource(ctx, 1, &code, NULL, &err);
	if (CL_SUCCESS != err)
		return false;

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);						// Compilation du kernel
	if (CL_SUCCESS != err)
		return false;
	else
		m_pProgram = program;

	return true;
}

std::string CRaptorComputeJob::loadProgramSource(const std::string &programFile)
{
	if (programFile.empty())
		return false;

	CRaptorIO *stream = CRaptorIO::Create(programFile.c_str(),CRaptorIO::DISK_READ);
	if (NULL == stream)
		return false;

	string programstr;

	while (stream->getStatus() == CRaptorIO::IO_OK)
	{
		string line;
		*stream >> line;
		if (stream->getStatus() == CRaptorIO::IO_OK)
			programstr = programstr + line + "\n";
	}

	//	TODO: log line error 

	delete stream;

	return programstr;
}

