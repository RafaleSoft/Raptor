// ServerTransport.h: interface for the CServerTransport class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
#define AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

namespace RaysServer {

	public ref class RaysServerUtils
	{
	public:
		ref class RAYS_CONFIG
		{
		public:
			RAYS_CONFIG();
			~RAYS_CONFIG() {};

			System::String^	host;
			System::Int32	port;
			System::Int32	wu_priority;	// NORMAL_PRIORITY_CLASS = 1
			System::Int32	deamon_delay;
			System::Int32	nb_wu_per_job;
			System::Collections::ArrayList^	deamons;
		};

		interface class ILogger
		{
		public:
			void Log(System::String^ msg);
		};

		//!	Convert a System::String into a char*.
		static char* convertSystemString(System::String^ str);

		//!	Load application config file
		static bool loadConfig(void);

		//!	Save application config file
		static bool saveConfig(void);

		//!	Returns the server configuration structure (from file)
		static RaysServerUtils::RAYS_CONFIG^ getConfig(void);

		//!	Returns the current logger.
		static ILogger^ getLog();

		//!	Defines the current logger.
		//!	Returns the previous logger if any.
		static ILogger^ setLog(ILogger^ logger);


	private:
		static RaysServerUtils();
		~RaysServerUtils() {};

		//!	Server configuration for deamons management
		static RAYS_CONFIG^	rays_config;

		//!	A unique logger for simplicity
		static ILogger^ m_logger;
	};
}

#endif // !defined(AFX_RAYSSERVERUTILS_H__1CC878E3_B301_4A19_8211_F3B5977D3781__INCLUDED_)
