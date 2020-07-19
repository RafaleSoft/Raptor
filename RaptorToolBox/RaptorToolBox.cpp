// RaptorToolBox.cpp : Defines the initialization routines for the DLL.
//

#include "Subsys/CodeGeneration.h"
#include "RaptorToolBox.h"

#include "Engine/3DScene.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/ObjectFactory.h"
#include "System/RaptorIO.h"
#include "System/Raptor.h"
#include "System/RaptorErrorManager.h"


extern "C" int APIENTRY
DllMain(HINSTANCE , DWORD dwReason)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{

	}
	return 1;   // ok
}

bool CRaptorToolBox::loadRaptorData(const std::string &fname, LP_SCENE_LOADER_OPTIONS options)
{
	CRaptorErrorManager *err = Raptor::GetErrorManager();

	char shemaLocation[MAX_PATH];
	char* root = getenv("RAPTOR_ROOT");
	if (NULL == root)
	{
		std::string msg = "Raptor root data path is unknown. Is RAPTOR_ROOT defined ? \nUsing current path. ";
		strcpy(shemaLocation, "./Raptor.xsd");

		if (NULL == err)
			std::cout << msg << std::endl;
		else
		{
			err->generateRaptorError(	CPersistence::CPersistenceClassID::GetClassId(),
										CRaptorErrorManager::RAPTOR_GL_ERROR,
										msg);
		}
	}
	else
	{
		strcpy(shemaLocation, root);
		strcat(shemaLocation, "/Redist/bin/Raptor.xsd");
	}

	CRaptorIO *pLoader = CRaptorIO::Create("XMLIO", CRaptorIO::DISK_READ, CRaptorIO::ASCII_XML);
	pLoader->parse(shemaLocation, 0);
	pLoader->parse(fname.c_str(), 0);

	CRaptorDisplay *pDisplay = CRaptorDisplay::GetCurrentDisplay();

	std::string name;
	//  skip data intro
	*pLoader >> name;
	std::string name_data = pLoader->getValueName();
	while (!name_data.empty())
	{
		CPersistence* obj = NULL;

		//  skip data intro
		if (name_data == "Update")
		{
			*pLoader >> name;
			name_data = pLoader->getValueName();
			if (name_data == "name")
			{
				*pLoader >> name;
				obj = CPersistence::FindObject(name);
			}
		}
		else
		{
			const CPersistentObject & po = CObjectFactory::GetInstance()->createObject(name_data);
			obj = po;
		}

		if (obj != NULL)
			obj->importObject(*pLoader);
		else
			*pLoader >> name;

		name_data = pLoader->getValueName();
	}

	return true;
}