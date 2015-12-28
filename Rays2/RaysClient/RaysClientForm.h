#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Net;

#include "RaysClientUtils.h"

namespace RaysClient {

	/// <summary>
	/// Description résumée de RaysClientForm
	///
	/// AVERTISSEMENT : si vous modifiez le nom de cette classe, vous devrez modifier la
	///          propriété 'Nom du fichier de ressources' de l'outil de compilation de ressource managée
	///          pour tous les fichiers .resx dont dépend cette classe. Dans le cas contraire,
	///          les concepteurs ne pourront pas interagir correctement avec les ressources
	///          localisées associées à ce formulaire.
	/// </summary>
	public ref class RaysClientForm abstract : public System::Windows::Forms::Form
	{
	public:
		RaysClientForm(void)
		{
			InitializeComponent();
			if (!RaysClientUtils::loadConfig())
			{
				MessageBox::Show(this,
								"Unable to load Rays Client configuration file !",
								"Error",
								MessageBoxButtons::OK);
			}
		}

	protected:
		/// <summary>
		/// Nettoyage des ressources utilisées.
		/// </summary>
		~RaysClientForm()
		{
			if (!RaysClientUtils::saveConfig())
			{
				MessageBox::Show(this,
					"Unable to save Rays Client configuration file !",
					"Error",
					MessageBoxButtons::OK);
			}
			if (components)
			{
				delete components;
			}
		}

		void AddLog(String^ log)
		{
			Log->Items->Add(log);
		}

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Button^  Connect;

	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::TextBox^  host;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  port;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ListBox^  Log;

	protected:
		virtual bool Start(int argc,char *argv[]) = 0;
		virtual bool Quit(void) = 0;

	private:
		/// <summary>
		/// Variable nécessaire au concepteur.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
		/// le contenu de cette méthode avec l'éditeur de code.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(RaysClientForm::typeid));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->port = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->host = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->Connect = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->Log = (gcnew System::Windows::Forms::ListBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->port);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->host);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->Connect);
			this->groupBox1->Location = System::Drawing::Point(10, 13);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(311, 46);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Server connection";
			// 
			// port
			// 
			this->port->Location = System::Drawing::Point(161, 16);
			this->port->Name = L"port";
			this->port->Size = System::Drawing::Size(54, 20);
			this->port->TabIndex = 4;
			this->port->Text = L"2048";
			this->port->TextChanged += gcnew System::EventHandler(this, &RaysClientForm::ValidateHost);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(133, 20);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(29, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Port:";
			// 
			// host
			// 
			this->host->Location = System::Drawing::Point(26, 16);
			this->host->Name = L"host";
			this->host->Size = System::Drawing::Size(100, 20);
			this->host->TabIndex = 2;
			this->host->Text = L"0.0.0.0";
			this->host->TextChanged += gcnew System::EventHandler(this, &RaysClientForm::ValidateHost);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(7, 20);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(20, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"IP:";
			// 
			// Connect
			// 
			this->Connect->Enabled = false;
			this->Connect->Location = System::Drawing::Point(229, 13);
			this->Connect->Name = L"Connect";
			this->Connect->Size = System::Drawing::Size(75, 23);
			this->Connect->TabIndex = 0;
			this->Connect->Text = L"Connect";
			this->Connect->UseVisualStyleBackColor = true;
			this->Connect->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &RaysClientForm::OnConnect);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(239, 65);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Quit";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &RaysClientForm::OnQuit);
			// 
			// Log
			// 
			this->Log->FormattingEnabled = true;
			this->Log->HorizontalScrollbar = true;
			this->Log->Location = System::Drawing::Point(10, 127);
			this->Log->Name = L"Log";
			this->Log->Size = System::Drawing::Size(311, 95);
			this->Log->TabIndex = 2;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(13, 65);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 3;
			this->button1->Text = L"Open...";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &RaysClientForm::OnOpenFile);
			// 
			// RaysClientForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(333, 235);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->Log);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"RaysClientForm";
			this->Text = L"RaysClientForm";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &RaysClientForm::OnFormClosed);
			this->Load += gcnew System::EventHandler(this, &RaysClientForm::OnFormLoad);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void OnFormLoad(System::Object^  sender, System::EventArgs^  e);
	private: System::Void OnOpenFile(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void OnConnect(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void ValidateHost(System::Object^  sender, System::EventArgs^  e) {
			IPAddress^ address;
			Connect->Enabled = IPAddress::TryParse(host->Text,address) &&
									(port->Text->Length > 0);
		}
	private: System::Void OnQuit(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			if (Quit())
				Close();
		}
	private: System::Void OnFormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
		if (sender != this)
			Enabled = true;
	}
};
}
