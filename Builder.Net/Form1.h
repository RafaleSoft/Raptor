#pragma once


namespace BuilderNet {

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
	public ref class Form1 abstract : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
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
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:
		virtual void onOK(void) = 0;
		virtual void onCancel(void) = 0;
		virtual void onSelectAll(void) = 0;
		virtual void onSelectNone(void) = 0;

		System::Windows::Forms::TreeView^ getTreeExtensions()
		{
			return TREE_EXTENSIONS;
		};

	protected:
		System::Windows::Forms::CheckBox^  SSE2;
		System::Windows::Forms::CheckBox^  SSE;
		System::Windows::Forms::CheckBox^  SMP;
		System::Windows::Forms::CheckBox^  BUILD_REDIST;
		System::Windows::Forms::CheckBox^  BUILD_DEBUG;
		System::Windows::Forms::CheckBox^  BUILD_COMPUTE;

	private:
		System::Windows::Forms::TreeView^  TREE_EXTENSIONS;
		System::Windows::Forms::Button^  SelectAll;
		System::Windows::Forms::Button^  OK;
		System::Windows::Forms::Button^  SelectNone;
		System::Windows::Forms::Button^  Cancel;
		System::Windows::Forms::GroupBox^  groupBox1;
		System::Windows::Forms::GroupBox^  CPU;
		System::Windows::Forms::PictureBox^  pictureBox1;


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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->TREE_EXTENSIONS = (gcnew System::Windows::Forms::TreeView());
			this->SelectAll = (gcnew System::Windows::Forms::Button());
			this->OK = (gcnew System::Windows::Forms::Button());
			this->SelectNone = (gcnew System::Windows::Forms::Button());
			this->Cancel = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->BUILD_DEBUG = (gcnew System::Windows::Forms::CheckBox());
			this->BUILD_COMPUTE = (gcnew System::Windows::Forms::CheckBox());
			this->BUILD_REDIST = (gcnew System::Windows::Forms::CheckBox());
			this->CPU = (gcnew System::Windows::Forms::GroupBox());
			this->SSE2 = (gcnew System::Windows::Forms::CheckBox());
			this->SSE = (gcnew System::Windows::Forms::CheckBox());
			this->SMP = (gcnew System::Windows::Forms::CheckBox());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox1->SuspendLayout();
			this->CPU->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// TREE_EXTENSIONS
			// 
			this->TREE_EXTENSIONS->CheckBoxes = true;
			this->TREE_EXTENSIONS->Location = System::Drawing::Point(12, 12);
			this->TREE_EXTENSIONS->Name = L"TREE_EXTENSIONS";
			this->TREE_EXTENSIONS->Size = System::Drawing::Size(313, 269);
			this->TREE_EXTENSIONS->TabIndex = 0;
			// 
			// SelectAll
			// 
			this->SelectAll->Location = System::Drawing::Point(336, 12);
			this->SelectAll->Name = L"SelectAll";
			this->SelectAll->Size = System::Drawing::Size(75, 23);
			this->SelectAll->TabIndex = 1;
			this->SelectAll->Text = L"Select All";
			this->SelectAll->UseVisualStyleBackColor = true;
			this->SelectAll->Click += gcnew System::EventHandler(this, &Form1::SelectAll_Click);
			// 
			// OK
			// 
			this->OK->Location = System::Drawing::Point(430, 13);
			this->OK->Name = L"OK";
			this->OK->Size = System::Drawing::Size(75, 23);
			this->OK->TabIndex = 2;
			this->OK->Text = L"OK";
			this->OK->UseVisualStyleBackColor = true;
			this->OK->Click += gcnew System::EventHandler(this, &Form1::OK_Click);
			// 
			// SelectNone
			// 
			this->SelectNone->Location = System::Drawing::Point(336, 42);
			this->SelectNone->Name = L"SelectNone";
			this->SelectNone->Size = System::Drawing::Size(75, 23);
			this->SelectNone->TabIndex = 3;
			this->SelectNone->Text = L"Select None";
			this->SelectNone->UseVisualStyleBackColor = true;
			this->SelectNone->Click += gcnew System::EventHandler(this, &Form1::SelectNone_Click);
			// 
			// Cancel
			// 
			this->Cancel->Location = System::Drawing::Point(430, 42);
			this->Cancel->Name = L"Cancel";
			this->Cancel->Size = System::Drawing::Size(75, 23);
			this->Cancel->TabIndex = 4;
			this->Cancel->Text = L"Cancel";
			this->Cancel->UseVisualStyleBackColor = true;
			this->Cancel->Click += gcnew System::EventHandler(this, &Form1::Cancel_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->BUILD_DEBUG);
			this->groupBox1->Controls->Add(this->BUILD_COMPUTE);
			this->groupBox1->Controls->Add(this->BUILD_REDIST);
			this->groupBox1->Location = System::Drawing::Point(337, 72);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(168, 91);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Build ...";
			// 
			// BUILD_DEBUG
			// 
			this->BUILD_DEBUG->AutoSize = true;
			this->BUILD_DEBUG->Location = System::Drawing::Point(12, 68);
			this->BUILD_DEBUG->Name = L"BUILD_DEBUG";
			this->BUILD_DEBUG->Size = System::Drawing::Size(111, 17);
			this->BUILD_DEBUG->TabIndex = 2;
			this->BUILD_DEBUG->Text = L"Build DEBUG Info";
			this->BUILD_DEBUG->UseVisualStyleBackColor = true;
			// 
			// BUILD_COMPUTE
			// 
			this->BUILD_COMPUTE->AutoSize = true;
			this->BUILD_COMPUTE->Location = System::Drawing::Point(12, 44);
			this->BUILD_COMPUTE->Name = L"BUILD_COMPUTE";
			this->BUILD_COMPUTE->Size = System::Drawing::Size(131, 17);
			this->BUILD_COMPUTE->TabIndex = 1;
			this->BUILD_COMPUTE->Text = L"Build GPU Computing ";
			this->BUILD_COMPUTE->UseVisualStyleBackColor = true;
			// 
			// BUILD_REDIST
			// 
			this->BUILD_REDIST->AutoSize = true;
			this->BUILD_REDIST->Location = System::Drawing::Point(12, 20);
			this->BUILD_REDIST->Name = L"BUILD_REDIST";
			this->BUILD_REDIST->Size = System::Drawing::Size(124, 17);
			this->BUILD_REDIST->TabIndex = 0;
			this->BUILD_REDIST->Text = L"Build Documentation";
			this->BUILD_REDIST->UseVisualStyleBackColor = true;
			// 
			// CPU
			// 
			this->CPU->Controls->Add(this->SSE2);
			this->CPU->Controls->Add(this->SSE);
			this->CPU->Controls->Add(this->SMP);
			this->CPU->Location = System::Drawing::Point(337, 169);
			this->CPU->Name = L"CPU";
			this->CPU->Size = System::Drawing::Size(170, 57);
			this->CPU->TabIndex = 6;
			this->CPU->TabStop = false;
			this->CPU->Text = L"CPU";
			// 
			// SSE2
			// 
			this->SSE2->AutoSize = true;
			this->SSE2->Location = System::Drawing::Point(114, 20);
			this->SSE2->Name = L"SSE2";
			this->SSE2->Size = System::Drawing::Size(53, 17);
			this->SSE2->TabIndex = 2;
			this->SSE2->Text = L"SSE2";
			this->SSE2->UseVisualStyleBackColor = true;
			// 
			// SSE
			// 
			this->SSE->AutoSize = true;
			this->SSE->Location = System::Drawing::Point(64, 20);
			this->SSE->Name = L"SSE";
			this->SSE->Size = System::Drawing::Size(47, 17);
			this->SSE->TabIndex = 1;
			this->SSE->Text = L"SSE";
			this->SSE->UseVisualStyleBackColor = true;
			// 
			// SMP
			// 
			this->SMP->AutoSize = true;
			this->SMP->Location = System::Drawing::Point(13, 20);
			this->SMP->Name = L"SMP";
			this->SMP->Size = System::Drawing::Size(49, 17);
			this->SMP->TabIndex = 0;
			this->SMP->Text = L"SMP";
			this->SMP->UseVisualStyleBackColor = true;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(337, 232);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(170, 49);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 7;
			this->pictureBox1->TabStop = false;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(519, 292);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->CPU);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->Cancel);
			this->Controls->Add(this->SelectNone);
			this->Controls->Add(this->OK);
			this->Controls->Add(this->SelectAll);
			this->Controls->Add(this->TREE_EXTENSIONS);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"Form1";
			this->Text = L"Raptor Builder.Net  [Prepare Raptor Code to use the following options] ";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->CPU->ResumeLayout(false);
			this->CPU->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

private: System::Void OK_Click(System::Object^ , System::EventArgs^ ) 
		 {
			 onOK();
		 }
private: System::Void Cancel_Click(System::Object^ , System::EventArgs^ ) 
		 {
			 onCancel();
		 }
private: System::Void SelectNone_Click(System::Object^ , System::EventArgs^ ) 
		 {
			 onSelectNone();
		 }
private: System::Void SelectAll_Click(System::Object^ , System::EventArgs^ ) 
		 {
			 onSelectAll();
		 }
};

}

