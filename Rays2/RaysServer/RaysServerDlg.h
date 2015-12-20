#pragma once

#include "RaysServerForm.h"


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
		bool	m_started;
		CServerTransport *m_pTransport;
		CDeamonManager	*m_pDeamonManager;
	};
}
