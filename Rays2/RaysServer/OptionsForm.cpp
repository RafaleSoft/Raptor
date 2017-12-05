#include "StdAfx.h"
#include "OptionsForm.h"

using namespace RaysServer;

void OptionsForm::DisplayWorkUnits()
{
	if (NULL != m_pMgr)
	{
		WorkUnits->Items->Clear();
		for (unsigned int i = 0; i<m_pMgr->getNbDeamons(); i++)
		{
			const CDeamonManager::LPDEAMONSTRUCT wu = m_pMgr->getDeamon(i);
			String^ item = gcnew String(wu->deamonIP.c_str());
			WorkUnits->Items->Add(item);
		}
	}
}

System::Void OptionsForm::OnOK(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	RaysServerUtils::RAYS_CONFIG^ config = RaysServerUtils::getConfig();
	config->nb_wu_per_job = (unsigned int)CPUs->Value;
	config->deamon_delay = (unsigned int)Polling->Value;
	
	for (int i = 0; i < groupBox1->Controls->Count; i++)
	{
		System::Windows::Forms::RadioButton^ button = (System::Windows::Forms::RadioButton^)(this->groupBox1->Controls[i]);
		if (button->Checked)
		{
			config->wu_priority = i;
			break;
		}
	}
	
	Close();
}

System::Void OptionsForm::OnPollingUpdated(System::Object^  sender, System::EventArgs^  e)
{
	if (NULL != m_pMgr)
	{
		m_pMgr->setPollingDelay(unsigned int(Polling->Value));
	}
}

System::Void OptionsForm::WUContextMenuOpening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
{
	WUListContextMenu->Items[0]->Enabled = true;
	int selection = WorkUnits->SelectedIndex;
	WUListContextMenu->Items[1]->Enabled = (selection >= 0);
	WUListContextMenu->Items[2]->Enabled = (selection >= 0);
}

System::Void OptionsForm::NewDeamon(System::Object^  sender, System::EventArgs^  e)
{
	WUProperties^ props = gcnew WUProperties(m_pMgr);
	props->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &OptionsForm::OnFormClosed);
	Enabled = false;
	props->Show();
}

System::Void OptionsForm::DeamonProperties(System::Object^  sender, System::EventArgs^  e)
{
	WUProperties^ props = gcnew WUProperties(NULL);
	props->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &OptionsForm::OnFormClosed);
	Enabled = false;
	props->Show();

	if (!m_pMgr->DeamonStatus(WorkUnits->SelectedIndex))
	{
		MessageBox::Show(this,
			"Unable to update deamon status!",
			"Error",
			MessageBoxButtons::OK);
	}
	else
	{
		CDeamonManager::LPDEAMONSTRUCT deamon = m_pMgr->getDeamon(WorkUnits->SelectedIndex);
		props->SetDeamon(deamon);
	}
}

System::Void OptionsForm::DeleteDeamon(System::Object^  sender, System::EventArgs^  e)
{
	if (!m_pMgr->destroyDeamon(WorkUnits->SelectedIndex))
	{
		MessageBox::Show(this,
			"Unable to delete deamon, jobs are still in progress!",
			"Error",
			MessageBoxButtons::OK);
	}
	else
		DisplayWorkUnits();
}

System::Void OptionsForm::OnFormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e)
{
	if (sender != this)
	{
		Enabled = true;
		DisplayWorkUnits();
	}
}

System::Void OptionsForm::OptionsLoad(System::Object^  sender, System::EventArgs^  e)
{
	RaysServerUtils::RAYS_CONFIG^ config = RaysServerUtils::getConfig();
	CPUs->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { config->nb_wu_per_job, 0, 0, 0 });
	Polling->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { config->deamon_delay, 0, 0, 0 });
	System::Windows::Forms::RadioButton^ button = (System::Windows::Forms::RadioButton^)(groupBox1->Controls[config->wu_priority]);
	button->Checked = true;
}
