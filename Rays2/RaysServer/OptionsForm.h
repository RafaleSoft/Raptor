#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace RaysServer {

	/// <summary>
	/// Description résumée de OptionsForm
	///
	/// AVERTISSEMENT : si vous modifiez le nom de cette classe, vous devrez modifier la
	///          propriété 'Nom du fichier de ressources' de l'outil de compilation de ressource managée
	///          pour tous les fichiers .resx dont dépend cette classe. Dans le cas contraire,
	///          les concepteurs ne pourront pas interagir correctement avec les ressources
	///          localisées associées à ce formulaire.
	/// </summary>
	public ref class OptionsForm : public System::Windows::Forms::Form
	{
	public:
		OptionsForm(void)
		{
			InitializeComponent();
			//
			//TODO : ajoutez ici le code du constructeur
			//
		}

	protected:
		/// <summary>
		/// Nettoyage des ressources utilisées.
		/// </summary>
		~OptionsForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NumericUpDown^  CPUs;


	private: System::Windows::Forms::RadioButton^  Idle;
	private: System::Windows::Forms::RadioButton^  Normal;


	private: System::Windows::Forms::RadioButton^  High;
	private: System::Windows::Forms::RadioButton^  RealTime;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::NumericUpDown^  Polling;

	private: System::Windows::Forms::ListBox^  WorkUnits;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  OK;
	private: System::Windows::Forms::Button^  New;

	private: System::ComponentModel::IContainer^  components;

	protected: 

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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->CPUs = (gcnew System::Windows::Forms::NumericUpDown());
			this->Idle = (gcnew System::Windows::Forms::RadioButton());
			this->Normal = (gcnew System::Windows::Forms::RadioButton());
			this->High = (gcnew System::Windows::Forms::RadioButton());
			this->RealTime = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->Polling = (gcnew System::Windows::Forms::NumericUpDown());
			this->WorkUnits = (gcnew System::Windows::Forms::ListBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->OK = (gcnew System::Windows::Forms::Button());
			this->New = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CPUs))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Polling))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(13, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(96, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Work Unit per Job:";
			// 
			// CPUs
			// 
			this->CPUs->Location = System::Drawing::Point(196, 13);
			this->CPUs->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {8, 0, 0, 0});
			this->CPUs->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->CPUs->Name = L"CPUs";
			this->CPUs->Size = System::Drawing::Size(70, 20);
			this->CPUs->TabIndex = 1;
			this->CPUs->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// Idle
			// 
			this->Idle->AutoSize = true;
			this->Idle->Location = System::Drawing::Point(6, 19);
			this->Idle->Name = L"Idle";
			this->Idle->Size = System::Drawing::Size(42, 17);
			this->Idle->TabIndex = 3;
			this->Idle->Text = L"Idle";
			this->Idle->UseVisualStyleBackColor = true;
			// 
			// Normal
			// 
			this->Normal->AutoSize = true;
			this->Normal->Checked = true;
			this->Normal->Location = System::Drawing::Point(54, 19);
			this->Normal->Name = L"Normal";
			this->Normal->Size = System::Drawing::Size(58, 17);
			this->Normal->TabIndex = 4;
			this->Normal->TabStop = true;
			this->Normal->Text = L"Normal";
			this->Normal->UseVisualStyleBackColor = true;
			// 
			// High
			// 
			this->High->AutoSize = true;
			this->High->Location = System::Drawing::Point(118, 19);
			this->High->Name = L"High";
			this->High->Size = System::Drawing::Size(47, 17);
			this->High->TabIndex = 5;
			this->High->Text = L"High";
			this->High->UseVisualStyleBackColor = true;
			// 
			// RealTime
			// 
			this->RealTime->AutoSize = true;
			this->RealTime->Location = System::Drawing::Point(171, 19);
			this->RealTime->Name = L"RealTime";
			this->RealTime->Size = System::Drawing::Size(70, 17);
			this->RealTime->TabIndex = 6;
			this->RealTime->Text = L"RealTime";
			this->RealTime->UseVisualStyleBackColor = true;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->RealTime);
			this->groupBox1->Controls->Add(this->Idle);
			this->groupBox1->Controls->Add(this->High);
			this->groupBox1->Controls->Add(this->Normal);
			this->groupBox1->Location = System::Drawing::Point(12, 39);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(254, 50);
			this->groupBox1->TabIndex = 7;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Work Unit Priority";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(13, 96);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(142, 13);
			this->label2->TabIndex = 8;
			this->label2->Text = L"Deamon polling delay (secs):";
			// 
			// Polling
			// 
			this->Polling->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->Polling->Location = System::Drawing::Point(161, 95);
			this->Polling->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {3600, 0, 0, 0});
			this->Polling->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->Polling->Name = L"Polling";
			this->Polling->Size = System::Drawing::Size(105, 20);
			this->Polling->TabIndex = 9;
			this->Polling->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			// 
			// WorkUnits
			// 
			this->WorkUnits->FormattingEnabled = true;
			this->WorkUnits->Location = System::Drawing::Point(12, 136);
			this->WorkUnits->Name = L"WorkUnits";
			this->WorkUnits->Size = System::Drawing::Size(254, 95);
			this->WorkUnits->TabIndex = 10;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(13, 117);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(63, 13);
			this->label3->TabIndex = 11;
			this->label3->Text = L"Work Units:";
			// 
			// OK
			// 
			this->OK->Location = System::Drawing::Point(287, 13);
			this->OK->Name = L"OK";
			this->OK->Size = System::Drawing::Size(75, 23);
			this->OK->TabIndex = 12;
			this->OK->Text = L"OK";
			this->OK->UseVisualStyleBackColor = true;
			this->OK->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &OptionsForm::OnOK);
			// 
			// New
			// 
			this->New->Location = System::Drawing::Point(287, 43);
			this->New->Name = L"New";
			this->New->Size = System::Drawing::Size(75, 23);
			this->New->TabIndex = 13;
			this->New->Text = L"New";
			this->New->UseVisualStyleBackColor = true;
			// 
			// OptionsForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(374, 245);
			this->Controls->Add(this->New);
			this->Controls->Add(this->OK);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->WorkUnits);
			this->Controls->Add(this->Polling);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->CPUs);
			this->Controls->Add(this->label1);
			this->Name = L"OptionsForm";
			this->Text = L"OptionsForm";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->CPUs))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Polling))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void OnOK(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 Close();
		 }
};
}
