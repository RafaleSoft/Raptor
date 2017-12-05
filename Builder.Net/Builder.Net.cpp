// Builder.Net.cpp : fichier projet principal.

#include "stdafx.h"
#include "Form1.h"
#include "BuilderForm.h"

using namespace BuilderNet;

[STAThreadAttribute]
int main()
{
	// Activation des effets visuels de Windows XP avant la création de tout contrôle
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Créer la fenêtre principale et l'exécuter
	Application::Run(gcnew BuilderForm());
	return 0;
}
