#pragma once

#include "OptionsForm.h"
#include "RaysServerUtils.h"

namespace RaysServer {

	class CDeamonManager;

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Description résumée de Form1
	///
	/// AVERTISSEMENT : si vous modifiez le nom de cette classe, vous devrez modifier la
	///          propriété 'Nom du fichier de ressources' de l'outil de compilation de ressource managée
	///          pour tous les fichiers .resx dont dépend cette classe. Dans le cas contraire,
	///          les concepteurs ne pourront pas interagir correctement avec les ressources
	///          localisées associées à ce formulaire.
	/// </summary>
	public ref class RaysServerForm abstract : public System::Windows::Forms::Form
	{
	public:
		RaysServerForm(void)
		{
			InitializeComponent();
			if (!RaysServerUtils::loadConfig())
			{
				MessageBox::Show(	this,
									"Unable to load Rays Server configuration file !",
									"Error",
									MessageBoxButtons::OK);
			}
		}

	protected:
		/// <summary>
		/// Nettoyage des ressources utilisées.
		/// </summary>
		virtual ~RaysServerForm()
		{
			if (!RaysServerUtils::saveConfig())
			{
				MessageBox::Show(	this,
									"Unable to save Rays Server configuration file !",
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

	private: System::Windows::Forms::Button^  StartServer;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  NumberJobs;
	private: System::Windows::Forms::TextBox^  BasePort;
	private: System::Windows::Forms::TextBox^  Host;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  JobPercent;
	private: System::Windows::Forms::NumericUpDown^  JobID;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::ListBox^  Log;
	private: System::Windows::Forms::Button^  button3;
	private: System::ComponentModel::IContainer^  components;

	protected:
		virtual bool Start(int argc,char *argv[]) = 0;
		virtual bool Quit(void) = 0;
		virtual CDeamonManager* getDeamonManager() = 0;

	private:
		/// <summary>
		/// Variable nécessaire au concepteur.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
		/// le contenu de cette méthode avec l'éditeur de code.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(RaysServerForm::typeid));
			this->StartServer = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->Host = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->NumberJobs = (gcnew System::Windows::Forms::TextBox());
			this->BasePort = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->JobID = (gcnew System::Windows::Forms::NumericUpDown());
			this->JobPercent = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->Log = (gcnew System::Windows::Forms::ListBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->JobID))->BeginInit();
			this->SuspendLayout();
			// 
			// StartServer
			// 
			this->StartServer->Location = System::Drawing::Point(197, 12);
			this->StartServer->Name = L"StartServer";
			this->StartServer->Size = System::Drawing::Size(75, 23);
			this->StartServer->TabIndex = 0;
			this->StartServer->Text = L"Start Server";
			this->StartServer->UseVisualStyleBackColor = true;
			this->StartServer->Click += gcnew System::EventHandler(this, &RaysServerForm::OnStart);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(197, 41);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Options ...";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &RaysServerForm::OnOptions);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->Host);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->NumberJobs);
			this->groupBox1->Controls->Add(this->BasePort);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(13, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(178, 100);
			this->groupBox1->TabIndex = 2;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"General infos";
			// 
			// Host
			// 
			this->Host->Location = System::Drawing::Point(72, 66);
			this->Host->Name = L"Host";
			this->Host->Size = System::Drawing::Size(100, 20);
			this->Host->TabIndex = 5;
			this->Host->Text = L"0.0.0.0";
			this->Host->TextChanged += gcnew System::EventHandler(this, &RaysServerForm::OnHostChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 66);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(32, 13);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Host:";
			// 
			// NumberJobs
			// 
			this->NumberJobs->Location = System::Drawing::Point(72, 42);
			this->NumberJobs->Name = L"NumberJobs";
			this->NumberJobs->Size = System::Drawing::Size(100, 20);
			this->NumberJobs->TabIndex = 3;
			// 
			// BasePort
			// 
			this->BasePort->Location = System::Drawing::Point(72, 20);
			this->BasePort->Name = L"BasePort";
			this->BasePort->Size = System::Drawing::Size(100, 20);
			this->BasePort->TabIndex = 2;
			this->BasePort->Text = L"1024";
			this->BasePort->TextChanged += gcnew System::EventHandler(this, &RaysServerForm::OnPortChanged);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 42);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(69, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Number jobs:";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 20);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(55, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Base port:";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->progressBar1);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->JobID);
			this->groupBox2->Controls->Add(this->JobPercent);
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->label4);
			this->groupBox2->Location = System::Drawing::Point(13, 119);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(259, 88);
			this->groupBox2->TabIndex = 3;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Job status";
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(6, 57);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(247, 23);
			this->progressBar1->TabIndex = 6;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(7, 41);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(84, 13);
			this->label6->TabIndex = 5;
			this->label6->Text = L"Global Progress:";
			// 
			// JobID
			// 
			this->JobID->Location = System::Drawing::Point(54, 18);
			this->JobID->Name = L"JobID";
			this->JobID->Size = System::Drawing::Size(39, 20);
			this->JobID->TabIndex = 4;
			// 
			// JobPercent
			// 
			this->JobPercent->AutoSize = true;
			this->JobPercent->Location = System::Drawing::Point(140, 20);
			this->JobPercent->Name = L"JobPercent";
			this->JobPercent->Size = System::Drawing::Size(13, 13);
			this->JobPercent->TabIndex = 3;
			this->JobPercent->Text = L"0";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(99, 20);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(38, 13);
			this->label5->TabIndex = 2;
			this->label5->Text = L"Job %:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(7, 20);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(41, 13);
			this->label4->TabIndex = 0;
			this->label4->Text = L"Job ID:";
			// 
			// Log
			// 
			this->Log->FormattingEnabled = true;
			this->Log->Location = System::Drawing::Point(13, 212);
			this->Log->Name = L"Log";
			this->Log->Size = System::Drawing::Size(259, 69);
			this->Log->TabIndex = 4;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(197, 89);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 5;
			this->button3->Text = L"Quit";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &RaysServerForm::OnQuit);
			// 
			// RaysServerForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 287);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->Log);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->StartServer);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"RaysServerForm";
			this->Text = L"Rays Server v3.0";
			this->Load += gcnew System::EventHandler(this, &RaysServerForm::RaysServer_Load);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &RaysServerForm::OnFormClosed);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->JobID))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void OnStart(System::Object^  sender, System::EventArgs^  e) {
				int argc = 4;
				char *argv[4];
				argv[0] = "-p";
				argv[1] = RaysServerUtils::convertSystemString(BasePort->Text);
				argv[2] = "-a";
				argv[3] = RaysServerUtils::convertSystemString(Host->Text);
				if (Start(argc,argv))
					this->StartServer->Text = L"Stop Server";
				else
					this->StartServer->Text = L"Start Server";
			 }
private: System::Void OnQuit(System::Object^  sender, System::EventArgs^  e) {
			 if (Quit())
				 this->Close();
		 }
private: System::Void OnPortChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ((BasePort->Text->Length > 0) &&
				 (Host->Text->Length > 0))
				 StartServer->Enabled = true;
			 else
				 StartServer->Enabled = false;
		 }
private: System::Void OnHostChanged(System::Object^  sender, System::EventArgs^  e) {
			 if ((BasePort->Text->Length > 0) &&
				 (Host->Text->Length > 0))
				 StartServer->Enabled = true;
			 else
				 StartServer->Enabled = false;
		}
private: System::Void RaysServer_Load(System::Object^  sender, System::EventArgs^  e) {
			RaysServerUtils::RAYS_CONFIG^ config = RaysServerUtils::getConfig();
			BasePort->Text = config->port.ToString();
			Host->Text = config->host;
		 }
private: System::Void OnOptions(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			OptionsForm^ options = gcnew OptionsForm(getDeamonManager());
			options->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &RaysServerForm::OnFormClosed);
			this->Enabled = false;
			options->Show();
		 }
	private: System::Void OnFormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			if (sender != this)
				this->Enabled = true;
		 }
	};
}

