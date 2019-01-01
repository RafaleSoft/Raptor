#include "StdAfx.h"
#include "RaysServerForm.h"

using namespace RaysServer;

System::Void RaysServerForm::OnStart(System::Object^  sender, System::EventArgs^  e)
{
	int argc = 4;
	char *argv[4];
	argv[0] = "-p";
	argv[1] = RaysServerUtils::convertSystemString(BasePort->Text);
	argv[2] = "-a";
	argv[3] = RaysServerUtils::convertSystemString(Host->Text);
	if (Start(argc, argv))
	{
		this->StartServer->Text = L"Stop Server";
		this->QuitButton->Enabled = false;
	}
	else
	{
		this->StartServer->Text = L"Start Server";
		this->QuitButton->Enabled = true;
	}
}

System::Void RaysServerForm::RaysServer_Load(System::Object^  sender, System::EventArgs^  e)
{
	RaysServerUtils::RAYS_CONFIG^ config = RaysServerUtils::getConfig();
	BasePort->Text = config->port.ToString();
	Host->Text = config->host;
	if (NULL != getDeamonManager())
		getDeamonManager()->setPollingDelay(config->deamon_delay);
}
