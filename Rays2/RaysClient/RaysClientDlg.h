#pragma once

#if !defined(AFX_CLIENTSOCKET_H__A2920B8E_12E4_11D3_9142_D3B83905F198__INCLUDED_)
    #include "RaptorNetwork/ClientSocket.h"
#endif
#if !defined(AFX_CLIENT_H__A2920B8F_12E4_11D3_9142_D3B83905F198__INCLUDED_)
	#include "RaptorNetwork/Client.h"
#endif

#include "RaysClientForm.h"

namespace RaysClient {

	public ref class RaysClientDlg : public RaysClientForm
	{
	public:
		RaysClientDlg(void);
		virtual ~RaysClientDlg(void);

	protected:
		virtual bool Start(int argc,char *argv[]) override;
		virtual bool Quit(void) override;
		virtual char* convertSystemString(System::String^ str) override;
	
	private:
		/*
		ref class RaysLogger : public RaysServerUtils::ILogger
		{
		public:
			RaysLogger(RaysServerDlg^ userOutput):m_pLogger(userOutput) {};
			virtual ~RaysLogger() {};
			virtual void Log(System::String^ msg)
			{ m_pLogger->AddLog(msg); }
		private:
			RaysServerDlg^ m_pLogger;
		};
		*/
		bool	m_started;
		CClient<CClientSocket>	*m_raysClient;
	};
}
