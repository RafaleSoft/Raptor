// RaysServer.cpp�: fichier projet principal.

#include "stdafx.h"
#include "RaysServerDlg.h"

using namespace RaysServer;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Activation des effets visuels de Windows�XP avant la cr�ation de tout contr�le
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Cr�er la fen�tre principale et l'ex�cuter
	Application::Run(gcnew RaysServerDlg());
	return 0;
}
