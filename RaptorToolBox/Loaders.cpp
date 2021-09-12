#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_RAPTORTOOLBOX_H__7FFBBFE8_DBCD_4F8C_872E_EA2692DE1A11__INCLUDED_)
	#include "RaptorToolBox.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "GLHierarchy/ObjectFactory.h"
#endif
#if !defined(AFX_3DSET_H__DB24F01B_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/3DSet.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif


#include "Loaders/3DSfile.h"

extern bool _glLoadScene(const std::string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options);
extern bool _glLoadWavefront(const std::string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options);
extern bool _glSaveWavefront(const std::string &fname,C3DSet *set);

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for reading a 3DS file
//
bool CRaptorToolBox::load3DStudioScene(const std::string &fname,C3DSet *&set,CRaptorToolBox::SCENE_LOADER_OPTIONS *options )
{
	return _glLoadScene(fname,set,options);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for reading an OBJ file
//
bool CRaptorToolBox::loadWavefrontScene(const std::string &fname,C3DSet *&set,LP_SCENE_LOADER_OPTIONS options)
{
	return _glLoadWavefront(fname,set,options);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for saving an OBJ file
//
bool CRaptorToolBox::saveWavefrontScene(const std::string &fname,C3DSet *scene)
{
	return _glSaveWavefront(fname,scene);
}

/////////////////////////////////////////////////////////////////////////////
// 
//	Main function for reading an XML file
//
bool CRaptorToolBox::loadRaptorData(const std::string &fname, LP_SCENE_LOADER_OPTIONS )
{
	CRaptorErrorManager *err = IRaptor::GetErrorManager();

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
			RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(), msg);
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

	std::string data;
	//  skip data intro
	*pLoader >> data;
	std::string name_data = pLoader->getValueName();

	//  skip data intro
	if (name_data == "Data")
	{
		//	enter first attribute
		*pLoader >> data;
		name_data = pLoader->getValueName();

		*pLoader >> data;
		while (!data.empty())
		{
			name_data = pLoader->getValueName();
			*pLoader >> data;
		}

		// go to first child node
		name_data = pLoader->getValueName();
	}
	else
	{
		std::string msg = "Invalid Raptor Data XML file format: first tag should be 'Data'";
		if (NULL == err)
			std::cout <<  msg << std::endl;
		else
		{
			RAPTOR_ERROR(CPersistence::CPersistenceClassID::GetClassId(), msg);
		}

		delete pLoader;
		return false;
	}

	while (pLoader->hasMoreValues())
	{
		CPersistence* obj = NULL;

		if (name_data == "Update")
		{
			*pLoader >> data;
			name_data = pLoader->getValueName();
			if (name_data == "name")
			{
				*pLoader >> data;
				obj = CPersistence::FindObject(data);
			}
		}
		else if (!name_data.empty())
		{
			const CPersistentObject & po = CObjectFactory::GetInstance()->createObject(name_data);
			obj = po;
		}
		else
			obj = NULL;

		if (obj != NULL)
			obj->importObject(*pLoader);
		else
			*pLoader >> data;

		name_data = pLoader->getValueName();
	}

	delete pLoader;
	return true;
}
