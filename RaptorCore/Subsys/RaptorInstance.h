/***************************************************************************/
/*                                                                         */
/*  RaptorInstance.h                                                       */
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


#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
#define AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
	#include "System/RaptorConfig.h"
#endif
#if !defined(AFX_RAPTORCONSOLE_H__27656611_2DF3_4416_8124_F608CFAC2122__INCLUDED_)
	#include "System/RaptorConsole.h"
#endif
#if !defined(AFX_RAPTORDISPLAYCONFIG_H__DA0759DF_6CF9_44A7_9ADE_D404FEEC2DDF__INCLUDED_)
	#include "System/RaptorDisplayConfig.h"
#endif
#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif


RAPTOR_NAMESPACE_BEGIN

#define SHAREWARE_RELEASE 1

class C3DEngine;
class C3DEngineTaskManager;
class CAnimator;
class CRaptorMessages;
class CRaptorErrorManager;
class CRaptorDisplay;
class CRenderEntryPoint;
class CRaptorConsole;
class CShader;


class CRaptorInstance
{
public:
	//!	Create a singleton instance or returns the current isntance.
	static CRaptorInstance &GetInstance(void);

	//!	Creates a new RaptorInstance intance and returns the previous instance.
	//!	The user shall manage the list of instance.
	static CRaptorInstance* createNewInstance(void);

	//! (Re)Initialise all instance objects.
	void initInstance();


	//!	Raptor Instance has been initialised.
	bool					isInitialised(void) const { return m_bInitialised; };
	//!	The second pipeline has exited, raptor can be closed safely.
	bool					terminate(void) const { return m_bTerminate; };
	//!	Number of objects rendered in the current frame.
	uint32_t				iRenderedObjects;
	//!	Number of triangles rendered in the current frame.
	uint32_t				iRenderedTriangles;
	//!	3Dengine to use for geometric queries.
	C3DEngine				*p3DEngine;
	//!	The engine task manager to execute parallel jobs.
	C3DEngineTaskManager	*engineTaskMgr;
	//	current animator
	CAnimator				*pAnimator;
	//!	Raptor Error manager.
	CRaptorErrorManager		*pErrorMgr;
	//!	The Raptor Console for current instance.
	CRaptorConsole			*pConsole;
	//!	Raptor Error messages database.
	CRaptorMessages			*pMessages;
	//!	Raptor global configuration.
	CRaptorConfig			config;
	//!	Specific SSE implementation is forced (not dynamically checked)
	bool					forceSSE;
	//!	Activite shareware specific features (not all capabilities are enabled)
	bool					runAsShareware;
	//!	Raptor rendering entry points, the base call of all renders.
	std::vector<CRenderEntryPoint*>	  renderEntryPoints;
	//!	The default display to query GL states.
	CRaptorDisplay			*defaultDisplay;
	//!	The default window mapping the default context on the default display.
	RAPTOR_HANDLE			defaultWindow;
	//!	The default context for the default dispplay.
	long					defaultContext;
	
	//!	Imaging input/output interfaces.
	map<std::string, CImage::IImageIO*>	imageKindIO;

	//! The set of all Raptor displays for this instance.
	std::vector<CRaptorDisplay*>	displays;
	//!	Raptor Console interactors.
	std::vector<CRaptorConsole::CInputCollectorBase*>	inputCollectors;
	//!	The full list of persistence objects active in this instance.
	MapStringToPtr	objects;

	//! Fragment Program state.
	bool isFragmentShaderReady(void) const { return m_bFragmentShaderReady; };
	//! Vertex Program state.
	bool isVertexShaderReady(void) const { return m_bVertexShaderReady; };
	//! Geometry Program state.
	bool isGeometryShaderReady(void) const { return m_bGeometryShaderReady; };
	//!	Vertex Shader state
	bool isVertexProgramReady(void) const { return m_bVertexProgramReady; };
	//!	Fragment Shader state
	bool isFragmentProgramReady(void) const { return m_bFragmentProgramReady; };

#if defined(GL_COMPATIBILITY_profile)
	//!	Full screen quad rendering display list
	RAPTOR_HANDLE	m_drawBuffer;
#else
	//!	Center vertex attributes for full screen rendering (0,0,0,0).
	GL_COORD_VERTEX	*m_pAttributes;
	//!	Identity shader for full screen quad texture mapping.
	CShader	*m_pIdentity;
#endif

	//! Global ResourceAllocator arrays binding state.
	CRaptorDisplayConfig::GL_ARRAYS_STATE	bindingState;
	//!	Default ResourceAllocator arrays bindings initialised.
	bool arrays_initialized;
	//!	Texture Quad global shader.
	CShader	*m_pQuadShader;


	//! Stores Display attributes for delayed creation.
	//! The physical display creation is delegated to the underlying API, 
	//! and might not be synchronous to the return of the creation method.
	//! Thus, attributes can be saved temporarily until the real creation happens.
	//! When a display is created directly (@see Raptor class ), this method is unnecessary
	void setDefaultConfig(const CRaptorDisplayConfig &pcs);

	//! Returns the defaut initial state of the renderer.
	//! Data is gathered from RaptorData, so different run state can be used with various RaptorData.
	const CRaptorDisplayConfig& getDefaultConfig(void) const { return defaultConfig; }

	//! Delete Raptor status and any allocated resource.
	bool destroy(void);

	//!	Initialise base shaders for this instance.
	bool glInitShaders(void);


private:
	//! Constructor.
	CRaptorInstance();
	//! Copy Constructor.
	CRaptorInstance(const CRaptorInstance&);
	//!	Destructor.
	~CRaptorInstance();

	static CRaptorInstance *m_pInstance;

	//!	Raptor default display creation structure and defaut initial state of the renderer.
	CRaptorDisplayConfig	defaultConfig;
	//!	Raptor Instance has been initialised.
	bool					m_bInitialised;
	//!	Raptor Instance has been requested to terminate.
	bool					m_bTerminate;
	//! Fragment Program state.
	bool					m_bFragmentShaderReady;
	//! Vertex Program state.
	bool					m_bVertexShaderReady;
	//! Geometry Program state.
	bool					m_bGeometryShaderReady;
	//!	Vertex Shader state
	bool					m_bVertexProgramReady;
	//!	Fragment Shader state
	bool					m_bFragmentProgramReady;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)

