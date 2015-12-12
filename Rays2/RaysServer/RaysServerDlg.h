#pragma once

#include "RaysServerForm.h"

class CServerTransport;
class CDeamonManager;

namespace RaysServer {

	public ref class RaysServerDlg : public RaysServerForm
	{
	public:
		RaysServerDlg(void);
		virtual ~RaysServerDlg(void);

	protected:
		virtual bool Start(int argc,char *argv[]) override;
		virtual bool Quit(void) override;
		virtual char* convertSystemString(System::String^ str) override;
	
	private:
		bool	m_started;
		CServerTransport *m_pTransport;
		CDeamonManager	*m_pDeamonManager;
	};
}
