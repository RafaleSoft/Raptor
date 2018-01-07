#include "Subsys/CodeGeneration.h"

#ifndef __GLOBAL_H__
    #include "Global.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_MEMORY_H__81A6CA9A_4ED9_4260_B6E4_C03276C38DBC__INCLUDED_)
	#include "System/Memory.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_3DENGINETASKMANAGER_H__04149C60_C594_4009_A2C9_F852497146A3__INCLUDED_)
    #include "Engine/3DEngineTaskManager.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
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
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif

RAPTOR_NAMESPACE

Global	*Global::pInstance = NULL;

static Global::COpenGLClassID openglID;
const CPersistence::CPersistenceClassID& Global::COpenGLClassID::GetClassId(void)
{
	return openglID;
}
static Global::CVulkanClassID vulkanID;
const CPersistence::CPersistenceClassID& Global::CVulkanClassID::GetClassId(void)
{
	return vulkanID;
}

Global::Global()
{
	raptorStatus.runAsShareware = false;
	raptorStatus.initialised = false;
    raptorStatus.messages = NULL;
    raptorStatus.errorMgr = NULL;
	raptorStatus.terminate = false;
	raptorStatus.forceSSE = false;
	raptorStatus.current3DEngine = NULL;
	raptorStatus.currentAnimator = NULL;
    raptorStatus.engineTaskMgr = NULL;
    raptorStatus.console = NULL;

    raptorStatus.defaultDisplay = NULL;
	raptorStatus.defaultWindow.handle = 0;
	raptorStatus.defaultWindow.hClass = 0;

    raptorStatus.iRenderedObjects = 0;
    raptorStatus.iRenderedTriangles = 0;
}

Global::~Global()
{
	//! Terminate Engine:
    raptorStatus.terminate = true;
    raptorStatus.currentAnimator = NULL;
    if (raptorStatus.engineTaskMgr != NULL)
	{
        delete raptorStatus.engineTaskMgr;
		raptorStatus.engineTaskMgr = NULL;
	}

	//! Destroy glObjects : we need a context.
    CContextManager::GetInstance()->glMakeCurrentContext(raptorStatus.defaultWindow,
														 raptorStatus.defaultContext);

    CShaderLibrary* pShaderLib = CShaderLibrary::GetInstance();
    if (pShaderLib != NULL)
		pShaderLib->destroy();

    if (raptorStatus.console != NULL)
    {
		delete raptorStatus.console;
        raptorStatus.console = NULL;
    }

    Raptor::glPurgeRaptor(false);

    RAPTOR_HANDLE noDevice;
	CContextManager::GetInstance()->glMakeCurrentContext(noDevice,
														 raptorStatus.defaultContext);

    CContextManager::GetInstance()->glDestroyContext(raptorStatus.defaultContext);
    raptorStatus.defaultContext = 0;


	CImage::IImageOP *op = CImage::getImageKindOP(CImage::IImageOP::BUMPMAP_LOADER);
	delete op;

	op = CImage::getImageKindOP(CImage::IImageOP::IMAGE_SCALER);
	delete op;

	CImage::IImageIO *pIO = CImage::getImageKindIO("BUFFER");
	delete pIO;

    //  Default display is NULL, because it was created with GENERIC attribute.
    //    glDestroyDisplay(status.defaultDisplay);
    CContextManager::GetInstance()->glDestroyWindow(raptorStatus.defaultWindow);

    delete CHostMemoryManager::GetInstance();
	delete CContextManager::GetInstance();
    delete CRaptorDataManager::GetInstance();

    if (raptorStatus.messages != NULL)
        delete raptorStatus.messages;
    if (raptorStatus.errorMgr != NULL)
        delete raptorStatus.errorMgr;

	raptorStatus.initialised = false;
}

Global&	Global::GetInstance()
{
	if (pInstance == NULL)
		pInstance = new Global();

	return *pInstance;
}

bool Global::destroy(void)
{
    if (pInstance == NULL)
        return false;

    delete pInstance;
    pInstance = NULL;

    return true;
}

bool Global::init(const CRaptorConfig& config)
{
	if (!raptorStatus.initialised)
	{
		globalConfig = config; 

		//	Initialise memory first
		CHostMemoryManager::GetInstance()->init();
		CHostMemoryManager::GetInstance()->setGarbageMaxSize(config.m_uiGarbageSize);

        //Initialize messages & errors
		raptorStatus.errorMgr = new CRaptorErrorManager();
		raptorStatus.errorMgr->logToFile(config.m_logFile);
        raptorStatus.messages = new CRaptorMessages();
		CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
		if (dataManager != NULL)
		{
			//	Erase previous files in case of updates
			dataManager->ClearExports();
			string filepath = dataManager->ExportFile("RaptorMessages.xml");
			if (!filepath.empty())
				raptorStatus.messages->LoadMessages(filepath);
		}

		//	Initialise CPU
#if defined(RAPTOR_SSE_CODE_GENERATION)
		const CPU_INFO &info = getCPUINFO();
		raptorStatus.forceSSE = info.hasFeature(CPU_INFO::SSE);
#endif

		// Initialises the main create struct for display settings
		raptorStatus.defaultConfig.caption = "Raptor";
		raptorStatus.defaultConfig.display_mode = GL_RGB;
		raptorStatus.defaultConfig.height = 480;
		raptorStatus.defaultConfig.width = 640;
		raptorStatus.defaultConfig.refresh_rate.fps = CGL_MAXREFRESHRATE;
		raptorStatus.defaultConfig.refresh_rate.sync_to_monitor = false;
		raptorStatus.defaultConfig.x = 0;
		raptorStatus.defaultConfig.y = 0;

        //  Initialise engine
		raptorStatus.currentAnimator = NULL;
		raptorStatus.current3DEngine = NULL;
        raptorStatus.engineTaskMgr = C3DEngineTaskManager::Create();
        if (raptorStatus.engineTaskMgr)
            raptorStatus.engineTaskMgr->initEngine();

		CContextManager *pContext = CContextManager::GetInstance();

		CDefaultBumpmapLoader *pDefaultBumpmapLoader = new CDefaultBumpmapLoader(1.0f);
		CImage::setImageKindOP(pDefaultBumpmapLoader);

		CDefaultImageScaler *pDefaultImageScaler = new CDefaultImageScaler(1.0f,1.0f);
		CImage::setImageKindOP(pDefaultImageScaler);

		CImage::IImageIO *pIO = new CBufferImage();
		CImage::setImageKindIO(pIO);

#if defined (VK_VERSION_1_0)
		if (!pContext->vkInit())
		{
			raptorStatus.errorMgr->generateRaptorError(	CVulkanClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_VK_ERROR,
														"Unable to load Vulkan module !");
		}
#endif

		raptorStatus.initialised = true;
	}

	return raptorStatus.initialised;
}

void Global::setDefaultConfig(const CRaptorDisplayConfig& pcs)
{
	// avoid self assignment
	if (&pcs == &raptorStatus.defaultConfig)
		return;

	raptorStatus.defaultConfig = pcs;

	raptorStatus.runAsShareware = raptorStatus.runAsShareware || pcs.draw_logo;

	//	validates window position to be fully visible
	//	if window not fully visible, hardware very slow...
	CContextManager::GetInstance()->validateConfig(raptorStatus.defaultConfig);
}