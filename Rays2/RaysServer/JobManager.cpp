/***************************************************************************/
/*                                                                         */
/*  JobManager.cpp                                                         */
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


#include "stdafx.h"

#include "../Messages.h"			// io messages IDs and structs

#if !defined(AFX_JOBMANAGER_H__4E78312A_6362_46AF_A327_07208468529A__INCLUDED_)
	#include "JobManager.h"
#endif

using namespace RaysServer;

CJobManager::CJobManager()
	:m_counter(0)
{

}

CJobManager::~CJobManager()
{

}

uint32_t CJobManager::createJob(server_base_t::request_handler_t::request_id id, 
								uint32_t width, uint32_t height)
{
	m_counter++;

	job_struct *job = new job_struct;
	job->jobID = m_counter;
	
	job->clientID = id;
	job->jobWidth = width;
	job->jobHeight = height;

	//	create job data storage: image is a 32 bits RGBA buffer
	// TODO : use 64bits for EXR images
	unsigned char *imageBuffer = new unsigned char[width * height * 4];
	float *zBuffer = new float[width * height];
	job->globalImageBuffer = imageBuffer;
	job->ZBuffer = zBuffer;
	// TODO.
	job->processor = NULL; // new CPostProcessor(lpJob);

	m_pJobs.push_back(job);

	return m_counter;
}