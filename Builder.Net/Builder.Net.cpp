// Builder.Net.cpp�: fichier projet principal.

#include "stdafx.h"
#include "Form1.h"
#include "BuilderForm.h"

using namespace BuilderNet;

[STAThreadAttribute]
int main()
{
	// Activation des effets visuels de Windows�XP avant la cr�ation de tout contr�le
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Cr�er la fen�tre principale et l'ex�cuter
	Application::Run(gcnew BuilderForm());
	return 0;
}
