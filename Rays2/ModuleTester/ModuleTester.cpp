// ModuleTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ModuleTester.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\Plugin.cxx"

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = -1;
	}
	else
	{
		cout << "Module tester for Rays2 Raytracer" << endl;
		cout << endl;

		if (argc < 2)
		{
			cout << "Usage: ModuleTester <module_name>" << endl;
			cout << "       module_name is the complete file name ( e.g. ModuleDOF.dll )" << endl;
		}
		else 
		{
			cout << "Opening module: " << argv[1] << " ... " << endl;
			PLUGIN plugin = OpenPlugin(argv[1]);

			if (plugin.moduleInstance == NULL)
			{
				cout << "Could not open file." << endl;
				cout << argv[1] << " is corrupted or is not a valid Rays2 plugin." << endl;
				nRetCode = -2;
			}
			else
			{
				if ((plugin.pGetModuleAuthor == NULL) ||
					(plugin.pGetModuleDescription == NULL) ||
					(plugin.pGetModuleName == NULL) ||
					(plugin.pGetModuleType == NULL) ||
					(plugin.pGetModuleVersion == NULL) ||
					(plugin.pInitModule == NULL) ||
					(plugin.pGetParameters == NULL) ||
					(plugin.pSetParameters == NULL))
				{
					cout << "Some of the basic entry points are missing." << endl;
					cout << argv[1] << " is corrupted or is not a valid Rays2 plugin." << endl;
					nRetCode = -3;
				}
				else
				{
					cout << endl;
					cout << "       Plugin Name: [" << plugin.pGetModuleName() << "]" << endl;
					cout << "Plugin Description: [" << plugin.pGetModuleDescription() << "]" << endl;
					cout << "     Plugin Author: [" << plugin.pGetModuleAuthor() << "]" << endl;

					if (plugin.pGetModuleType() >= NB_PLUGIN_TYPE)
						cout << "       Unsupported or unknown plugin type !" << endl;
					else
						cout << "       Plugin Type: [" << PLUGIN_STR[plugin.pGetModuleType()] << "]" << endl;

					union
					{
						unsigned char	b[4];
						unsigned int	i;
					} version;
					version.i = plugin.pGetModuleVersion();

					cout << "    Plugin Version: [" << (int)version.b[3] << "." << (int)version.b[2]
						<< "." << (int)version.b[1] << "." << (int)version.b[0] << "]" << endl;

					PARAMETER_DATA *data = plugin.pGetParameters();
					if (data == NULL)
					{
						cout << endl << "Plugin failed to return parameters. Module is corrupted." << endl;
						nRetCode = -4;
					}
					else
					{
						cout << endl;
						cout << "Plugin has " << data->nbParameters << " parameter(s):" << endl;
						for (int i=0;i<data->nbParameters ; i++)
						{
							PARAMETER_DATA::parameter_t param = data->parameters[i];

							cout << "     Parameter " << i << ":" << endl;
							
							if (param.name == NULL)
								cout << " Parameter name is unknown. Plugin is corrupted." << endl;
							else
								cout << "     Parameter name: [" << param.name << "]" << endl;
							
							if (param.type >= NB_PARAMETER_TYPE)
								cout << " Unknown or unsupported parameter type!" << endl;
							else
								cout << "     Parameter type: [" << PARAMETER_STR2[param.type]<< "]" << endl;
				
							if (param.kind >= NB_INTERNAL_PARAMETER)
								cout << " Unknown or unsupported parameter kind!" << endl;
							else
								cout << "     Parameter kind: [" << PARAMETER_STR[param.kind]<< "]" << endl;
						}

						nRetCode = 1;
						cout << endl;

						switch(plugin.pGetModuleType())
						{
							case AFTER_EFFECT:
								if (plugin.pAfterEffect != NULL)
									cout << "After effect function is accessible." << endl;
								else
								{
									nRetCode = -5;
									cout << "Failed to access After effect function." << endl;
								}
								break;
							case GEOMETRY:
								if (plugin.pNormal != NULL)
									cout << "Geometry Normal function is accessible." << endl;

								else
								{
									nRetCode = -5;
									cout << "Failed to access After effect function." << endl;
								}
								break;
							default:
								nRetCode = -5;
								cout << "Unable to vaidate plugin functions. Upgrade module tester." << endl;
						}

						if (nRetCode == 1)
							cout << argv[1] << " is a valid Rays2 plugin." << endl;
						else
							cout << argv[1] << " is cannot be used properly by Rays2. Check install or update" << endl;
					}
				}
			}
		}
	}

	cout << endl << "done." << endl;

	return nRetCode;
}


