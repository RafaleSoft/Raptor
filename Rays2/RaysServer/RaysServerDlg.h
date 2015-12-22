#pragma once

#include "RaysServerForm.h"
#include "RaysServerUtils.h"

namespace RaysServer {

	class CServerTransport;
	class CDeamonManager;

	public ref class RaysServerDlg : public RaysServerForm
	{
	public:
		RaysServerDlg(void);
		virtual ~RaysServerDlg(void);

	protected:
		virtual bool Start(int argc,char *argv[]) override;
		virtual bool Quit(void) override;
		virtual CDeamonManager* getDeamonManager() override
		{ return m_pDeamonManager; };
	
	private:
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

		bool	m_started;
		CServerTransport *m_pTransport;
		CDeamonManager	*m_pDeamonManager;
	};
}
