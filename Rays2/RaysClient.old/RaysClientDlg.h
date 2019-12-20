#pragma once

#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/ClientSocket.h"
#endif
#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Client.h"
#endif

#include "RaysClientForm.h"


#if !defined(AFX_RAYSCLIENTUTILS_H__D5EDE275_AF42_4BF5_80F7_2C408934AEFF__INCLUDED_)
	#include "RaysClientUtils.h"
#endif


namespace RaysClient {

	class RaysClientData;

	public ref class RaysClientDlg : public RaysClientForm
	{
	public:
		RaysClientDlg(void);
		virtual ~RaysClientDlg(void);

	protected:
		virtual bool Start(int argc,char *argv[]) override;
		virtual bool Quit(void) override;
		virtual bool LoadData(String^ filename) override;
	
	private:
		ref class RaysLogger : public RaysClientUtils::ILogger
		{
		public:
			RaysLogger(RaysClientDlg^ userOutput) :m_pLogger(userOutput) {};
			virtual ~RaysLogger() {};
			virtual void Log(System::String^ msg)
			{ m_pLogger->AddLog(msg); }
		private:
			RaysClientDlg^ m_pLogger;
		};

		bool	m_started;
		RaysClientData			*m_pData;
		CClient<CClientSocket>	*m_raysClient;
	};
}
