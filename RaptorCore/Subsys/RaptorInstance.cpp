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
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif

//! Default Imaging functionnalities
#if !defined(AFX_BUFFERIMAGE_H__B28C75CD_81D5_473F_A247_608FB6E02949__INCLUDED_)
	#include "Subsys/BufferImage.h"
#endif
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_VERTEXPROGRAM_H__F2D3BBC6_87A1_4695_B667_2B8C3C4CF022__INCLUDED_)
	#include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_FRAGMENTPROGRAM_H__DD0AD51D_3BFF_4C65_8099_BA7696D7BDDF__INCLUDED_)
	#include "GLHierarchy/FragmentProgram.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
	#include "GLHierarchy/GeometryShader.h"
#endif
#if !defined(AFX_TEXUREUNITSETUP_H__4A6ADC72_02E5_4F2A_931E_A736B6D6E0F0__INCLUDED_)
	#include "GLHierarchy/TextureUnitSetup.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif

#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
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
	m_bInitialised = false;
	forceSSE = false;
	runAsShareware = false;
	iRenderedObjects = 0;
	iRenderedTriangles = 0;
	
	defaultDisplay = NULL;
	defaultWindow.handle(0);
	defaultWindow.hClass(0);

	m_bFragmentShaderReady = false;
	m_bVertexShaderReady = false;
	m_bGeometryShaderReady = false;
	m_bVertexProgramReady = false;
	m_bFragmentProgramReady = false;

	m_pAttributes = NULL;
	m_pIdentity = NULL;

	arrays_initialized = false;
}

CRaptorInstance &CRaptorInstance::GetInstance(void)
{
	if (m_pInstance == NULL)
		m_pInstance = new CRaptorInstance();

	return *m_pInstance;
}

CRaptorInstance* CRaptorInstance::createNewInstance(void)
{
	CRaptorInstance *previous = m_pInstance;
	m_pInstance = new CRaptorInstance();
	return previous;
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
	m_bInitialised = false;

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

#if defined(GL_COMPATIBILITY_profile)
	if (m_drawBuffer.handle() > 0)
	{
		glDeleteLists(m_drawBuffer.handle(), 1);
		m_drawBuffer.handle(0);
	}
#else
	if (NULL != m_pAttributes)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		pAllocator->releaseVertices(*m_pAttributes);
	}
#endif

	
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

	//	Release imagers
	map<std::string, CImage::IImageIO*>::iterator it = imageKindIO.begin();
	while (it != imageKindIO.end())
	{
		CImage::IImageIO *op = it->second;
		if (NULL != op)
		{
			std::vector<std::string> exts = op->getImageKind();
			for (size_t i = 0; i < exts.size(); i++)
				imageKindIO.erase(exts[i]);

			delete op;
		}
		it = imageKindIO.begin();
	}
	imageKindIO.clear();
}

void CRaptorInstance::initInstance()
{
	//!	Initialize error manager.
	if (pErrorMgr != NULL)
		delete pErrorMgr;
	pErrorMgr = new CRaptorErrorManager();
	pErrorMgr->logToFile(config.m_logFile);

	//! Initialise error mesasges.
	if (pMessages != NULL)
		delete pMessages;
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

	m_bInitialised = true;
}

bool CRaptorInstance::glInitShaders(void)
{
	//!	Check shaders extensions availability
	m_bVertexProgramReady = false;
	m_bFragmentProgramReady = false;
	m_bVertexProgramReady = false;

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	CRaptorMessages::MessageArgument arg;
	vector<CRaptorMessages::MessageArgument> args;
#endif

	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (Raptor::glIsExtensionSupported(GL_ARB_VERTEX_PROGRAM_EXTENSION_NAME))
#if defined(GL_ARB_vertex_program)
		m_bVertexProgramReady = (NULL != pExtensions->glGenProgramsARB);
#endif
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		arg.arg_sz = "ASM vertex";
		args.clear(); args.push_back(arg);
		Raptor::GetErrorManager()->generateRaptorError(CVertexProgram::CVertexProgramClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   CRaptorMessages::ID_NO_GPU_PROGRAM, 
													   __FILE__, __LINE__, args);
#endif
	}
	if (Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_PROGRAM_EXTENSION_NAME))
#if defined(GL_ARB_fragment_program)
		m_bFragmentProgramReady = (NULL != pExtensions->glGenProgramsARB);
#endif
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		arg.arg_sz = "ASM fragment";
		args.clear(); args.push_back(arg);
		Raptor::GetErrorManager()->generateRaptorError(CFragmentProgram::CFragmentProgramClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING, 
													   CRaptorMessages::ID_NO_GPU_PROGRAM, 
													   __FILE__, __LINE__, args);
#endif
	}
	if (Raptor::glIsExtensionSupported(GL_ARB_VERTEX_SHADER_EXTENSION_NAME))
#if defined(GL_ARB_vertex_shader)
		m_bVertexShaderReady = (NULL != pExtensions->glCreateShaderObjectARB);
#endif
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		arg.arg_sz = "GLSL vertex";
		args.clear(); args.push_back(arg);
		Raptor::GetErrorManager()->generateRaptorError(CVertexShader::CVertexShaderClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING, 
													   CRaptorMessages::ID_NO_GPU_PROGRAM, 
													   __FILE__, __LINE__, args);
#endif
	}
	if (Raptor::glIsExtensionSupported(GL_ARB_GEOMETRY_SHADER4_EXTENSION_NAME))
#if defined(GL_ARB_geometry_shader4)
		m_bGeometryShaderReady = (NULL != pExtensions->glCreateShaderObjectARB);
#endif
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		arg.arg_sz = "GLSL geometry";
		args.clear(); args.push_back(arg);
		Raptor::GetErrorManager()->generateRaptorError(CGeometryShader::CGeometryShaderClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING, 
													   CRaptorMessages::ID_NO_GPU_PROGRAM, 
													   __FILE__, __LINE__, args);
#endif
	}
	if (Raptor::glIsExtensionSupported(GL_ARB_FRAGMENT_SHADER_EXTENSION_NAME))
#if defined(GL_ARB_fragment_shader)
		m_bFragmentShaderReady = (NULL != pExtensions->glCreateShaderObjectARB);
#endif
	else
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		arg.arg_sz = "GLSL fragment";
		args.clear(); args.push_back(arg);
		Raptor::GetErrorManager()->generateRaptorError(CFragmentShader::CFragmentShaderClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING, 
													   CRaptorMessages::ID_NO_GPU_PROGRAM, 
													   __FILE__, __LINE__, args);
#endif
	}

	//!	Initialise the shader library
	CShaderLibrary* pShaderLib = CShaderLibrary::GetInstance();
	if (pShaderLib == NULL)
		return false;
	else
		pShaderLib->glInitFactory();

	//!	Create minimal shaders for Raptor displays logo.
#if defined(GL_COMPATIBILITY_profile)
	if (0 == m_drawBuffer.handle())
	{
		m_drawBuffer.handle(glGenLists(1));
		glNewList(m_drawBuffer.handle(), GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex4f(-1.0, -1.0, -1.0f, 1.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex4f(1.0, -1.0, -1.0f, 1.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex4f(1.0, 1.0, -1.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex4f(-1.0, 1.0, -1.0f, 1.0f);
			glEnd();
		glEndList();
	}
#else
	if (NULL == m_pIdentity)
	{
		m_pIdentity = new CShader("HDR_IDENTITY");
		COpenGLShaderStage *stage = m_pIdentity->glGetOpenGLShader();

		stage->glGetVertexShader("EMPTY_PROGRAM");
		stage->glGetGeometryShader("FULL_SCREEN_GEO_PROGRAM");
		stage->glGetFragmentShader("DIFFUSE_PROGRAM");

		CProgramParameters identityParams;
		identityParams.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);
		stage->setProgramParameters(identityParams);
		if (!stage->glCompileShader())
			return false;
	}

	if (NULL == m_pAttributes)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		bool lock = pAllocator->isMemoryLocked();
		if (lock)
			pAllocator->glvkLockMemory(false);

		size_t s = sizeof(GL_COORD_VERTEX) / sizeof(float);
		m_pAttributes = (GL_COORD_VERTEX*)(pAllocator->allocateVertices(s));

		m_pAttributes = (GL_COORD_VERTEX*)pAllocator->glvkMapPointer((float*)m_pAttributes);
		*m_pAttributes = GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 0.0f);
		m_pAttributes = (GL_COORD_VERTEX*)pAllocator->glvkUnMapPointer((float*)m_pAttributes);

		if (lock)
			pAllocator->glvkLockMemory(true);
	}
#endif

	return true;
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
