#pragma once

#include <vcclr.h>
#include "DeamonManager.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Net;

namespace RaysServer {

	/// <summary>
	/// Description r�sum�e de WUProperties
	///
	/// AVERTISSEMENT�: si vous modifiez le nom de cette classe, vous devrez modifier la
	///          propri�t� 'Nom du fichier de ressources' de l'outil de compilation de ressource manag�e
	///          pour tous les fichiers .resx dont d�pend cette classe. Dans le cas contraire,
	///          les concepteurs ne pourront pas interagir correctement avec les ressources
	///          localis�es associ�es � ce formulaire.
	/// </summary>
	public ref class WUProperties : public System::Windows::Forms::Form
	{
	public:
		WUProperties(CDeamonManager* mgr):m_pMgr(mgr)
		{
			InitializeComponent();
			if (NULL != mgr)
				this->CloseProperties->Enabled = false;
			else
			{
				this->CloseProperties->Enabled = true;
				this->DeamonIP->Enabled = false;
			}
		}

		void SetDeamon(CDeamonManager::LPWORKUNITSTRUCT deamon)
		{
			if (NULL != deamon)
			{
				this->DeamonIP->Text = gcnew String(deamon->deamonIP.c_str());
			}
		}

	protected:
		/// <summary>
		/// Nettoyage des ressources utilis�es.
		/// </summary>
		~WUProperties()
		{
			if (components)
			{
				delete components;
			}
		}

	private: CDeamonManager* m_pMgr;

	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::TextBox^  DeamonIP;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  NbProcessors;
	private: System::Windows::Forms::Label^  NbProcessorsAvailable;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Button^  CloseProperties;



	protected: 

	private:
		/// <summary>
		/// Variable n�cessaire au concepteur.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// M�thode requise pour la prise en charge du concepteur - ne modifiez pas
		/// le contenu de cette m�thode avec l'�diteur de code.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->DeamonIP = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->NbProcessors = (gcnew System::Windows::Forms::Label());
			this->NbProcessorsAvailable = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->CloseProperties = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(0, 0);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(100, 20);
			this->textBox1->TabIndex = 0;
			// 
			// DeamonIP
			// 
			this->DeamonIP->Location = System::Drawing::Point(88, 9);
			this->DeamonIP->Name = L"DeamonIP";
			this->DeamonIP->Size = System::Drawing::Size(100, 20);
			this->DeamonIP->TabIndex = 0;
			this->DeamonIP->TextChanged += gcnew System::EventHandler(this, &WUProperties::ValidateIPAddress);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(13, 12);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(63, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Deamon IP:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 40);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(78, 13);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Nb processors:";
			// 
			// NbProcessors
			// 
			this->NbProcessors->AutoSize = true;
			this->NbProcessors->Location = System::Drawing::Point(135, 40);
			this->NbProcessors->Name = L"NbProcessors";
			this->NbProcessors->Size = System::Drawing::Size(53, 13);
			this->NbProcessors->TabIndex = 3;
			this->NbProcessors->Text = L"Unknown";
			// 
			// NbProcessorsAvailable
			// 
			this->NbProcessorsAvailable->AutoSize = true;
			this->NbProcessorsAvailable->Location = System::Drawing::Point(135, 64);
			this->NbProcessorsAvailable->Name = L"NbProcessorsAvailable";
			this->NbProcessorsAvailable->Size = System::Drawing::Size(53, 13);
			this->NbProcessorsAvailable->TabIndex = 5;
			this->NbProcessorsAvailable->Text = L"Unknown";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(13, 65);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(123, 13);
			this->label4->TabIndex = 4;
			this->label4->Text = L"Nb processors available:";
			// 
			// CloseProperties
			// 
			this->CloseProperties->Location = System::Drawing::Point(197, 9);
			this->CloseProperties->Name = L"CloseProperties";
			this->CloseProperties->Size = System::Drawing::Size(75, 23);
			this->CloseProperties->TabIndex = 6;
			this->CloseProperties->Text = L"Close";
			this->CloseProperties->UseVisualStyleBackColor = true;
			this->CloseProperties->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &WUProperties::OnClose);
			// 
			// WUProperties
			// 
			this->ClientSize = System::Drawing::Size(284, 91);
			this->Controls->Add(this->CloseProperties);
			this->Controls->Add(this->NbProcessorsAvailable);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->NbProcessors);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->DeamonIP);
			this->Name = L"WUProperties";
			this->Text = L"Deamon Properties";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void OnClose(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
				if (NULL != m_pMgr)
				{
					IPAddress^ address = IPAddress::Parse(this->DeamonIP->Text);
					String^ str = address->ToString();
					pin_ptr<const wchar_t> wch = PtrToStringChars(str);
					size_t convertedChars = 0;
					size_t  sizeInBytes = ((str->Length + 1) * 2);
					char    *ch = (char *)malloc(sizeInBytes);
					wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

					m_pMgr->registerDeamon(std::string(ch));
				}
				Close();
			 }
	private: System::Void ValidateIPAddress(System::Object^  sender, System::EventArgs^  e) {
				 IPAddress^ address;
				 this->CloseProperties->Enabled = IPAddress::TryParse(this->DeamonIP->Text,address);
			 }
};
}
