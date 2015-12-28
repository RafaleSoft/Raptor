// RaysClientUtils.h: interface for the RaysClientUtils class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_RAYSCLIENTUTILS_H__D5EDE275_AF42_4BF5_80F7_2C408934AEFF__INCLUDED_)
#define AFX_RAYSCLIENTUTILS_H__D5EDE275_AF42_4BF5_80F7_2C408934AEFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

namespace RaysClient {

	public ref class RaysClientUtils
	{
	public:
		ref class RAYS_CONFIG
		{
		public:
			RAYS_CONFIG();
			~RAYS_CONFIG() {};

			System::String^	host;
			System::Int32	port;
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
		static RaysClientUtils::RAYS_CONFIG^ getConfig(void);

		//!	Returns the current logger.
		static ILogger^ getLog();

		//!	Defines the current logger.
		//!	Returns the previous logger if any.
		static ILogger^ setLog(ILogger^ logger);


	private:
		static RaysClientUtils();
		~RaysClientUtils() {};

		//!	Server configuration for deamons management
		static RAYS_CONFIG^	rays_config;

		//!	A unique logger for simplicity
		static ILogger^ m_logger;
	};
}

#endif // !defined(AFX_RAYSCLIENTUTILS_H__D5EDE275_AF42_4BF5_80F7_2C408934AEFF__INCLUDED_)
