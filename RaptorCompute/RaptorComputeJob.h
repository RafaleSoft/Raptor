/***************************************************************************/
/*                                                                         */
/*  RaptorComputeJob.h                                                     */
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


#if !defined(AFX_RAPTOR_COMPUTE_JOB_H__BCAF8B25_FC2C_49BB_A710_A0D51F6DDFD0__INCLUDED_)
#define AFX_RAPTOR_COMPUTE_JOB_H__BCAF8B25_FC2C_49BB_A710_A0D51F6DDFD0__INCLUDED_

class CRaptorComputeTask;

#include <map>

#include <System/RaptorDisplay.h>

RAPTOR_NAMESPACE


// Cette classe est exportée de RaptorCompute.dll
class RAPTOR_API CRaptorComputeJob
{
public:
	//!	RaptorComputeJob factory.
	//!	@param numPlatform is the plaftorm number selected ( returned by RaptorCompute )
	//! @param numDevice is the device number selected in the platform ( returned by RaptorCompute )
	//! @return a valid CRaptorComputeObject, or NULL in case of errors ( wrong platform, ... )
	static CRaptorComputeJob *clCreateJob(	unsigned int numPlatform = 0,
											unsigned int numDevice = 0,
											CRaptorDisplay *dsp = NULL);

	//!	Releases all OpenCL resources used by the job, 
	//! including the job itself (this) and tasks if requested
	//!	@param destroyTasks : if true, all tasks held by this job are also deleted
	//! @return true if no error.
	bool clDestroyJob(bool destroyTasks = true);

	//!	Destroy all tasks and parameters inserted into this job.
	//!	@return true if no error.
	bool clDestroyTasks(void);

	//! Loads a program from a file containing an OpenCL source code.
	//!	@param programFile : the file name of the source to load.
	//!	@return true if program successfully loaded, false if errors encoutered.
	bool clLoadProgram(const std::string &programFile);

	//! Loads a program from a string containing an OpenCL source code.
	//!	@param programString : the source to load.
	//!	@return true if program successfully loaded, false if errors encoutered.
	bool clLoadProgramString(const std::string &programString);

	//! Loads a list of programs from files containing OpenCL source code.
	//!	@param programFiles : the list of file names of the source to load.
	//!	@return true if programs successfully loaded, false if errors encoutered.
	bool clLoadPrograms(const std::vector<std::string> &programFiles);


#ifdef CL_USE_DEPRECATED_OPENCL_1_0_APIS
	//!	Actives or deactivates out of order execution model
	//! @param enable is true if out of order is enabled
	//! @return true if mode changed without errors
	bool clEnableOutOfOrder(bool enable);

	//!	Actives or deactivates profiling execution model
	//! @param enable is true if profiling is enabled
	//! @return true if mode changed without errors
	bool clEnableProfiling(bool enable);
#endif

	//!	Add a copy of a task to the job queue
	//!	@param pTask : a reference to the new task to clone and add to job queue
	//!	@return true if task added without errors
	bool clAddTask(const CRaptorComputeTask &pTask);

	//!	Executes all the tasks of this job.
	//! @param waitCompletion : if true, returns ontly when all tasks have notified
	//!	their completion.
	//! @return false in case of error
	bool run(bool waitCompletion = false);



private:
	CRaptorComputeJob();
	virtual ~CRaptorComputeJob(void);

	//!	A helper method to load a text file
	//!	@param programFile : the file name to load
	//! @return the content of the file if no error, an empty string othrerwise
	std::string loadProgramSource(const std::string &programFile);


	//!	Mapping to internal execution Queue
	void*	m_pQueue;
	//!	Mapping to internal compiled program
	void*	m_pProgram;
	//!	Mapping to internal holding context
	void*	m_pContext;
	//!	Map of all invoked kernels
	std::map<std::string,void*>	m_kernels;
	//!	The list of all referenced tasks
	std::vector<CRaptorComputeTask*>	m_pTasks;
	//!	The list of all referenced tasks events
	std::vector<void*> m_pTaskEvents;
};

#endif	// defined(AFX_RAPTOR_COMPUTE_JOB_H__BCAF8B25_FC2C_49BB_A710_A0D51F6DDFD0__INCLUDED_)


