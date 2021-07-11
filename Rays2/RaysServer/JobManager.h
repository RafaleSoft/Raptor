/***************************************************************************/
/*                                                                         */
/*  JobManager.h                                                           */
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

#if !defined(AFX_JOBMANAGER_H__4E78312A_6362_46AF_A327_07208468529A__INCLUDED_)
#define AFX_JOBMANAGER_H__4E78312A_6362_46AF_A327_07208468529A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include <vector>
using namespace std;


#if !defined(AFX_SERVER_H__A2920B8C_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Server.h"
#endif



namespace RaysServer
{
	class CPostProcessor;

	class CJobManager
	{
	public:
		typedef struct job_struct_t
		{
			//	Identification
			uint32_t		jobID;				// ID of the current job
			server_base_t::request_handler_t::request_id	clientID;			// client external ID ( for resume identification)

			//	Work units management
			// unsigned int		nbWorkUnits;		// nb of WU per job
			// unsigned int		nbWorkUnitsReady;	// nb of identified work units
			//const CDeamonManager::WORKUNITSTRUCT ** workUnits;		// array of pointers to work units

			//	Job stats
			time_t				jobStart;			// duration of job
			time_t				jobEnd;				// duration of job
			unsigned int		processedAcks;		// nb ackowledgements from WU

			//	Job data
			uint32_t		jobWidth;			// width of job ( image )
			uint32_t		jobHeight;			// height of job ( image )
			unsigned char	*globalImageBuffer;	// buffer for computed image
			float			*ZBuffer;

			//	Job preprocessing
			CPostProcessor		*processor;
		} job_struct;

	public:
		CJobManager();
		virtual ~CJobManager();

		//!	Allocate a new job structure and return the unique job ID.
		uint32_t createJob(	server_base_t::request_handler_t::request_id id, 
							uint32_t width, 
							uint32_t height);


	private:
		//! counter for unique job IDs
		int32_t	m_counter;

		//! array of all current jobs
		std::vector<job_struct*>	m_pJobs;

		//! array of all finished jobs for further query
		std::vector<job_struct*>	m_pFinishedJobs;
	};
}


#endif // !defined(AFX_JOBMANAGER_H__4E78312A_6362_46AF_A327_07208468529A__INCLUDED_)