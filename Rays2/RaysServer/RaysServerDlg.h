/***************************************************************************/
/*                                                                         */
/*  RaysServerDlg.h                                                        */
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

#pragma once

#include "RaysServerForm.h"

#if !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
	#include "RaysServerUtils.h"
#endif

namespace RaysServer
{

	class CServerTransport;
	class CDeamonManager;

	class RaysServerDlg
	{
	public:
		RaysServerDlg(void);
		virtual ~RaysServerDlg(void);

	protected:
		
	
	private:
		class RaysLogger : public RaysServerUtils::ILogger
		{
		public:
			RaysLogger(RaysServerDlg^ userOutput):m_pLogger(userOutput) {};
			virtual ~RaysLogger() {};
			virtual void Log(System::String^ msg)
			{ m_pLogger->AddLog(msg); }
		private:
			RaysServerDlg^ m_pLogger;
		};


	};
}
