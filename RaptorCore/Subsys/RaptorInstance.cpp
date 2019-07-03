/***************************************************************************/
/*                                                                         */
/*  RaptorInstance.cpp                                                     */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
	#include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)
	#include "Subsys/Vulkan/RaptorVulkan.h"
#endif

//! Default Imaging functionnalities
#if !defined(AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_)
	#include "Subsys/BufferImage.h"
#endif
#if !defined(AFX_DEFAULTBUMPMAPLOADER_H__3841D5F8_284B_4DC5_9E4B_56EF18AF80F4__INCLUDED_)
	#include "Subsys/DefaultBumpmapLoader.h"
#endif
#if !defined(AFX_DEFAULTIMAGESCALER_H__E3E63A13_79FC_4E46_A1D5_BCD41CF86360__INCLUDED_)
	#include "Subsys/DefaultImageScaler.h"
#endif
#if !defined(AFX_DEFAULTALPHATRANSPARENCY_H__8EAD8C9F_DC40_4063_8E0A_0C2AB300AD16__INCLUDED_)
	#include "Subsys/DefaultAlphaTransparency.h"
#endif
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif



RAPTOR_NAMESPACE


CRaptorInstance *CRaptorInstance::m_pInstance = NULL;

CRaptorInstance::CRaptorInstance()
{
	p3DEngine = NULL;
	engineTaskMgr = NULL;
	pMessages = NULL;
	pErrorMgr = NULL;
	pAnimator = NULL;
	pConsole = NULL;
	
	terminate = false;
	initialised = false;
	forceSSE = false;
	runAsShareware = false;
	iRenderedObjects = 0;
	iRenderedTriangles = 0;
	
	defaultDisplay = NULL;
	defaultWindow.handle(0);
	defaultWindow.hClass(0);

	m_bFragmentProgramReady = false;
	m_bVertexProgramReady = false;
	m_bGeometryProgramReady = false;
	m_bVertexReady = false;
	m_bFragmentReady = false;
}

CRaptorInstance &CRaptorInstance::GetInstance(void)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CRaptorInstance();
	}

	return *m_pInstance;
}

CRaptorInstance* CRaptorInstance::createNewInstance(void)
{
	return NULL;
}

bool CRaptorInstance::destroy(void)
{
	if (m_pInstance == NULL)
		return false;

	delete m_pInstance;
	m_pInstance = NULL;

	return true;
}

CRaptorInstance::~CRaptorInstance()
{
	terminate = true;
	initialised = false;

	//! Destroy glObjects : we need a context.
	CContextManager::GetInstance()->glMakeCurrentContext(defaultWindow, defaultContext);

	//! Terminate Engine:
	if (engineTaskMgr != NULL)
	{
		delete engineTaskMgr;
		engineTaskMgr = NULL;
	}

	p3DEngine = NULL;
	pAnimator = NULL;

	if (pConsole != NULL)
	{
		delete pConsole;
		pConsole = NULL;
	}

	CShaderLibrary* pShaderLib = CShaderLibrary::GetInstance();
	if (pShaderLib != NULL)
		pShaderLib->destroy();

	CImage::IImageOP *op = CImage::getImageKindOP(CImage::IImageOP::BUMPMAP_LOADER);
	delete op;

	op = CImage::getImageKindOP(CImage::IImageOP::IMAGE_SCALER);
	delete op;

	op = CImage::getImageKindOP(CImage::IImageOP::ALPHA_TRANSPARENCY);
	delete op;

	CImage::IImageIO *pIO = CImage::getImageKindIO("BUFFER");
	delete pIO;

#if defined (VK_VERSION_1_0)
	if (config.m_bVulkan)
		if (!CContextManager::GetInstance()->vkRelease())
		{
			RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
						 "Unable to release Vulkan module !");
		}

#endif

	RAPTOR_HANDLE noDevice;
	CContextManager::GetInstance()->glMakeCurrentContext(noDevice, defaultContext);

	CContextManager::GetInstance()->glDestroyContext(defaultContext);
	defaultContext = 0;

	//  Default display is NULL, because it was created with GENERIC attribute.
	//    glDestroyDisplay(status.defaultDisplay);
	CContextManager::GetInstance()->glDestroyWindow(defaultWindow);


	delete CContextManager::GetInstance();
	delete CHostMemoryManager::GetInstance();

	if (pMessages != NULL)
		delete pMessages;
	pMessages = NULL;
	if (pErrorMgr != NULL)
		delete pErrorMgr;
	pErrorMgr = NULL;
}

void CRaptorInstance::initInstance()
{
	//!	Initialize error manager.
	pErrorMgr = new CRaptorErrorManager();
	pErrorMgr->logToFile(config.m_logFile);

	//! Initialise error mesasges.
	pMessages = new CRaptorMessages();
	CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
	if (dataManager != NULL)
	{
		string filepath = dataManager->ExportFile("RaptorMessages.xml");
		if (!filepath.empty())
			pMessages->LoadMessages(filepath);
	}

	//	Initialise CPU
#if defined(RAPTOR_SSE_CODE_GENERATION)
	const CPU_INFO &info = getCPUINFO();
	forceSSE = info.hasFeature(CPU_INFO::SSE);
#endif

	//	Initialise memory first
	CHostMemoryManager::GetInstance()->init();
	CHostMemoryManager::GetInstance()->setGarbageMaxSize(config.m_uiGarbageSize);

	//  Initialise engine
	engineTaskMgr = C3DEngineTaskManager::Create();
	if (engineTaskMgr)
		engineTaskMgr->initEngine();

	// Initialises the main create struct for display settings
	defaultConfig.caption = "Raptor";
	defaultConfig.display_mode = GL_RGB;
	defaultConfig.height = 480;
	defaultConfig.width = 640;
	defaultConfig.refresh_rate.fps = CGL_MAXREFRESHRATE;
	defaultConfig.refresh_rate.sync_to_monitor = false;
	defaultConfig.x = 0;
	defaultConfig.y = 0;

	CDefaultBumpmapLoader *pDefaultBumpmapLoader = new CDefaultBumpmapLoader(1.0f);
	CImage::setImageKindOP(pDefaultBumpmapLoader);

	CDefaultImageScaler *pDefaultImageScaler = new CDefaultImageScaler(1.0f, 1.0f);
	CImage::setImageKindOP(pDefaultImageScaler);

	CDefaultAlphaTransparency *pDefaultAlphaTransparency = new CDefaultAlphaTransparency(255);
	CImage::setImageKindOP(pDefaultAlphaTransparency);

	CImage::IImageIO *pIO = new CBufferImage();
	CImage::setImageKindIO(pIO);

#if defined (VK_VERSION_1_0)
	if (config.m_bVulkan)
		if (!CContextManager::GetInstance()->vkInit())
		{
			RAPTOR_ERROR(CVulkan::CVulkanClassID::GetClassId(),
						 "Unable to load Vulkan module !");
		}
#endif

	initialised = true;
}

void CRaptorInstance::setDefaultConfig(const CRaptorDisplayConfig& pcs)
{
	// avoid self assignment
	if (&pcs == &defaultConfig)
		return;

	defaultConfig = pcs;

	runAsShareware = runAsShareware || pcs.draw_logo;

	//	validates window position to be fully visible
	//	if window not fully visible, hardware very slow...
	CContextManager::GetInstance()->validateConfig(defaultConfig);
}
