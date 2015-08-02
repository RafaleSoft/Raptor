// PostProcessor.cpp: implementation of the CPostProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RaysServer.h"
#include "PostProcessor.h"
#include "FileManager.h"
#include "..\Plugin.cxx"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPostProcessor::~CPostProcessor()
{

}

UINT runAfterEffects(LPVOID pParam)
{
	CString msg;
	CPostProcessor *pProcessor = (CPostProcessor*)pParam;

	msg = "Server applying post processor effect...";
	Server.WriteMessage(IDS_SERVER_STRING,msg);

	pProcessor->ApplyAfterEffects();

	for (int i=0;i<pProcessor->effects.GetSize();i++)
	{
		PLUGIN plugin = pProcessor->effects[i].module;
		if (plugin.moduleInstance != NULL)
		{
			if (FALSE == AfxFreeLibrary(pProcessor->effects[i].module.moduleInstance))
			{
				msg = "Failed to release after effect " + CString(pProcessor->effects[i].call.name);
				Server.WriteMessage(IDS_SERVER_STRING,msg);
			}
		}
	}

	Server.SaveFinishedJob(pProcessor->m_pOwnerJob);

	msg = "Post processor effect done !";
	Server.WriteMessage(IDS_SERVER_STRING,msg);

	return 0;
}

void CPostProcessor::Run(void)
{
	mpThread = AfxBeginThread( runAfterEffects, 
							this, 
							THREAD_PRIORITY_NORMAL, 
							0, 0, NULL );
}

void CPostProcessor::AddData(MSGSTRUCT& msg,unsigned char raw_data[],CFileManager* /*pFileManager*/)
{
	if ((msg.msg_id == OBJ_DATA) && (msg.msg_data[1] == OBJ_PLUGIN))
	{
		int offset = 0;
		unsigned int nbPlugin = msg.msg_data[2];

		for (unsigned int i=0;i<nbPlugin;i++)
		{
			rays_plugin_t plugin;
			memcpy(&plugin,raw_data+offset,sizeof(rays_plugin_t));
			CString pname = plugin.name;

			CString msg;

			PLUGIN p;
			p = OpenPlugin(pname);

			//	First attempt, create from server files
			/*
			if (p.moduleInstance == NULL)
			{
				UINT size = 0;
				UINT size2 = 0;
				unsigned char *buffer = NULL;

				CFileManager::LPRAYSFILE pFile = pFileManager->GetFile(pname);

				if (pFile != NULL)
				{
					if (pFileManager->GetFile(pname,size,size2,buffer))
					{
						//CFile plugin(LPCTSTR(pname),CFile::modeCreate|CFile::modeWrite);
						//plugin.Write(buffer,size);
						//plugin.Close();

					//	p = OpenPlugin(pname);
					}
				}
			}
			*/

			//	Last try
			if (p.moduleInstance != NULL)
			{
				if (p.pGetModuleType() == AFTER_EFFECT)
				{
					msg.Format("Loading AfterEffect plugin : %s(%d params)",LPCTSTR(pname),plugin.nbParams);
					Server.WriteMessage(IDS_SERVER_STRING,msg);

					after_effect_t effect;
					effect.call = plugin;
					effect.module = p;
					effects.Add(effect);
				}
			}
			else
			{
				msg.Format("# Unable to load AfterEffect plugin : %s",LPCTSTR(pname));
				Server.WriteMessage(IDS_SERVER_STRING,msg);
			}

			offset += sizeof(rays_plugin_t);
		}
	}
	if ((msg.msg_id == OBJ_DATA) && (msg.msg_data[1] == OBJ_CAMERA))
	{
		memcpy(&mConfig,raw_data,msg.msg_size);
	}
}



/////////////////////////////////////////////////////////////////////////////
//ApplyAfterEffects:
//	Apply all available plugins of type AFTER_EFFECTS to the result image.
//	they are applyied in sequence, output from one, being input of the next one.
//
void CPostProcessor::ApplyAfterEffects(void)
{
	for (int i=0;i<effects.GetSize();i++)
	{
		CString pname;
		PLUGIN plugin = effects[i].module;
		
		if ((plugin.moduleInstance != NULL) && (plugin.pAfterEffect != NULL))
		{
			PARAMETER_DATA* pData = plugin.pGetParameters();
			if (pData != NULL)
			{
				bool apply = true;

				for (int i=0;i<pData->nbParameters;i++)
				{
					PARAMETER_DATA::parameter_t &param = pData->parameters[i];

					switch(param.kind)
					{
						case IMAGE_WIDTH:
							param.value.uint32Value = mConfig.width;
							param.type = UINT32_VALUE;
							break;
						case IMAGE_HEIGHT:
							param.value.uint32Value = mConfig.height;
							param.type = UINT32_VALUE;
							break;
						case IMAGE_COLOR:
							param.value.uint8Pointer = m_pOwnerJob->globalImageBuffer;
							param.type = UINT8_POINTER;
							break;
						case IMAGE_DEPTH:
							param.value.float32Pointer = m_pOwnerJob->ZBuffer;
							param.type = FLOAT32_POINTER;
							break;
						case CAMERA_FOCALE:
							param.value.float32Value = mConfig.focale * 0.001f;
							param.type = FLOAT32_VALUE;
							break;
						case CAMERA_APERTURE:
							param.value.float32Value = 1000 * 0.024f; //DIM_FILM; //raytracer_data.camera.aperture;
							param.type = FLOAT32_VALUE;
							break;
						case CAMERA_OBJECTPLANE:
							param.value.float32Value = mConfig.object_plane;
							param.type = FLOAT32_VALUE;
							break;
						default:
							apply = false;
					}
				}

				if (apply)
				{
					CString msg;
					if (0 != plugin.pSetParameters(pData))
					{
						if (0 != plugin.pInitModule())
						{
							if (0 != plugin.pAfterEffect())
							{
							}
							else
							{
								msg = "# Failed to apply post processor effect.";
								Server.WriteMessage(IDS_SERVER_STRING,msg);
							}
						}
						else
						{
							msg = "# Failed to initialise post processor effect.";
							Server.WriteMessage(IDS_SERVER_STRING,msg);
						}
					}
					else
					{
						msg = "# Failed to set post processor parameters.";
						Server.WriteMessage(IDS_SERVER_STRING,msg);
					}
				}
			}
		}
	}
}