// RaysServer.cpp : fichier projet principal.

#include "stdafx.h"
#include "RaysServerDlg.h"

using namespace RaysServer;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Activation des effets visuels de Windows XP avant la création de tout contrôle
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Créer la fenêtre principale et l'exécuter
	Application::Run(gcnew RaysServerDlg());
	return 0;
}
