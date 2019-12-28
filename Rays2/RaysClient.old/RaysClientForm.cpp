#include "StdAfx.h"
#include "RaysClientForm.h"


#if !defined(AFX_RAYSCLIENTUTILS_H__D5EDE275_AF42_4BF5_80F7_2C408934AEFF__INCLUDED_)
	#include "RaysClientUtils.h"
#endif


using namespace RaysClient;

System::Void RaysClientForm::OnConnect(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	int argc = 4;
	char *argv[4];
	argv[0] = "-p";
	argv[1] = RaysClientUtils::convertSystemString(port->Text);
	argv[2] = "-a";
	argv[3] = RaysClientUtils::convertSystemString(host->Text);
	if (Start(argc, argv))
		Connect->Text = L"Disconnect";
	else
		Connect->Text = L"Connect";
}

System::Void RaysClientForm::OnOpenFile(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	Enabled = false;
	OpenFileDialog^ open = gcnew OpenFileDialog();
	Windows::Forms::DialogResult result = open->ShowDialog(this);
	for (int i = 0; i < open->FileNames->GetLength(0); i++)
		LoadData(open->FileNames[i]->ToString());
	Enabled = true;
}

System::Void RaysClientForm::OnFormLoad(System::Object^  sender, System::EventArgs^  e)
{
	RaysClientUtils::RAYS_CONFIG^ config = RaysClientUtils::getConfig();
	port->Text = config->port.ToString();
	host->Text = config->host;
}
