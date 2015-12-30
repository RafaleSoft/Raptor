#include "StdAfx.h"
#include "WUProperties.h"

using namespace RaysServer;

System::Void WUProperties::OnClose(System::Object^ , System::Windows::Forms::MouseEventArgs^ )
{
	bool regDeamon = true;
	if (NULL != m_pMgr)
	{
		IPAddress^ address = IPAddress::Parse(this->DeamonIP->Text);
		String^ str = address->ToString();
		pin_ptr<const wchar_t> wch = PtrToStringChars(str);
		size_t convertedChars = 0;
		size_t  sizeInBytes = ((str->Length + 1) * 2);
		char    *ch = (char *)malloc(sizeInBytes);
		wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

		this->Cursor = Cursors::WaitCursor;
		regDeamon = m_pMgr->registerDeamon(std::string(ch));
		this->Cursor = Cursors::Default;
	}
	if (!regDeamon)
	{
		MessageBox::Show(this,
			"Unable to join deamon !",
			"Error",
			MessageBoxButtons::OK);
	}
	else
		Close();
}

void WUProperties::SetDeamon(CDeamonManager::LPDEAMONSTRUCT deamon)
{
	if (NULL != deamon)
	{
		this->DeamonIP->Text = gcnew String(deamon->deamonIP.c_str());
		this->NbProcessors->Text = System::Int32(deamon->nbProcs).ToString();
		this->NbProcessorsAvailable->Text = System::Int32(deamon->nbProcsAvailable).ToString();
	}
}
