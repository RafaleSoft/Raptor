#include "pch.h"

#include "Wrapper.h"
#include "System/Raptor.h"
#include "System/RaptorConfig.h"
#include "Engine/3DScene.h"
#include "Engine/Animator.h"
#include "Engine/IViewPoint.h"
#include "Engine/ViewModifier.h"
#include "GLHierarchy/IRenderingProperties.h"
#include "GLHierarchy/Object3DInstance.h"
#include "GLHierarchy/Light.h"
#include "GLHierarchy/3DSet.h"
#include "GLHierarchy/ShadedGeometry.h"
#include "GLHierarchy/Shader.h"
#include "System/RaptorConsole.h"

#include "ToolBox/BasicObjects.h"
#include "ToolBox/RaptorToolBox.h"
#include "ToolBox/Imaging.h"
#include "DataManager/RaptorDataManager.h"


RAPTOR_NAMESPACE

bool RAPTOR_WRAPPER_API glInitRaptor(CRaptorConfig_t &config)
{
	CRaptorConfig cfg;

	cfg.m_bAutoDestroy = config.m_bAutoDestroy;
	cfg.m_bCompute = config.m_bCompute;
	cfg.m_bRelocation = config.m_bRelocation;
	cfg.m_bVulkan = config.m_bVulkan;
	cfg.m_logFile = config.m_logFile;
	cfg.m_uiGarbageSize = config.m_uiGarbageSize;
	cfg.m_uiPolygons = config.m_uiPolygons;
	cfg.m_uiTexels = config.m_uiTexels;
	cfg.m_uiUniforms = config.m_uiUniforms;
	cfg.m_uiVertices = config.m_uiVertices;
	cfg.setFilterSizeFactor(config.m_fSizeFactor);

	bool res = Raptor::glInitRaptor(cfg);
	res = res && CImaging::installImagers();

	return res;
}

bool RAPTOR_WRAPPER_API glQuitRaptor(void)
{
	return Raptor::glQuitRaptor();
}

bool RAPTOR_WRAPPER_API glCreateDisplay(CRaptorDisplayConfig_t &config, CRaptorDisplay* &display)
{
	CRaptorDisplayConfig glcs;

	glcs.x = config.x;
	glcs.y = config.y;
	glcs.width = config.width;
	glcs.height = config.height;
	glcs.draw_logo = true; // config.draw_logo;
	glcs.status_bar = false;	// No status bar when embedded in external API.
	glcs.display_mode = config.display_mode;

	glcs.acceleration = config.acceleration;
	glcs.antialias = config.antialias;
	glcs.swap_buffer = config.swap_buffer;
	glcs.renderer = config.renderer;
	
	glcs.bind_to_texture = config.bind_to_texture;
	glcs.overlay = false;	// No overlay when embedded in external API.
	glcs.double_buffer = config.double_buffer;
	glcs.depth_buffer = config.depth_buffer;
	glcs.stencil_buffer = config.stencil_buffer;
	glcs.accumulator_buffer = config.accumulator_buffer;
	glcs.refresh_rate.fps = config.fps;
	glcs.refresh_rate.sync_to_monitor = config.sync_to_monitor;
	glcs.caption = config.caption;

	display = NULL;
	CRaptorDisplay *pDisplay = Raptor::glCreateDisplay(glcs);

	if (NULL == pDisplay)
		return false;

	display = pDisplay;
	return true;
}

bool RAPTOR_WRAPPER_API glDestroyDisplay(CRaptorDisplay* display, HDC dc)
{
	if (NULL == display)
		return false;

	RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS, dc);
	display->glvkBindDisplay(device);
	display->glvkReleaseResources();
	display->glvkUnBindDisplay();

	Raptor::glDestroyDisplay(display);

	return true;
}

bool RAPTOR_WRAPPER_API glBindDisplay(CRaptorDisplay* display, HDC dc)
{
	if (NULL == display)
		return false;
	if (NULL == dc)
		return false;

	RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS, dc);
	return display->glvkBindDisplay(device);
}

bool RAPTOR_WRAPPER_API glUnBindDisplay(CRaptorDisplay* display)
{
	if (NULL == display)
		return false;

	return display->glvkUnBindDisplay();
}

bool RAPTOR_WRAPPER_API glRender(void)
{
	Raptor::glRender();

	return true;
}

class CRenderer : public CRenderEntryPoint
{
public:
	CRenderer(CRaptorDisplay* display): m_pDisplay(display) {};
	virtual  ~CRenderer() {};

	virtual void glRender()
	{
		m_pDisplay->glRender();
	}

	void glInitRenderer();

private:
	CRaptorDisplay* m_pDisplay;
};

void CRenderer::glInitRenderer()
{
	CShadedGeometry *object = NULL;

	CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
	if (dataManager != NULL)
	{
		//	Change package and erase previous files in case of updates
		dataManager->managePackage("ShaderCompiler.pck");
		std::string teapot = dataManager->exportFile("TEAPOT.3DS");

		C3DSet *set = NULL;
		CRaptorToolBox::load3DStudioScene(teapot, set);
		set->scale(0.06f, 0.06f, 0.06f);
		C3DSet::C3DSetIterator it = set->getIterator();
		object = (CShadedGeometry*)(set->getChild(it++));
		GL_COORD_VERTEX c;
		object->getCenter(c);
		object->translate(-c.x, -c.y, -c.z);
		object->rotationX(-90.0f);
		object->addModel(CGeometry::RENDERING_MODEL::CGL_NORMALS);
	}
	else
	{
		CBasicObjects::CCube *obj = new CBasicObjects::CCube();
		CShader* s = obj->getShader();
		CMaterial *pMat = s->getMaterial();
		pMat->setAmbient(0.1f, 0.1f, 0.1f, 1.0f);
		pMat->setDiffuse(0.3f, 0.4f, 0.8f, 1.0f);
		pMat->setSpecular(0.7f, 0.7f, 0.7f, 1.0f);
		pMat->setEmission(0.0f, 0.0f, 0.0f, 1.0f);
		pMat->setShininess(128.0f);
		object = obj;
	}

	IRenderingProperties &props = m_pDisplay->getRenderingProperties();
	props.enableLighting.disableTexturing;

	CRaptorConsole *pConsole = Raptor::GetConsole();
	pConsole->glInit("", true);
	pConsole->showStatus(true);
	pConsole->showFPS(true);
	pConsole->showFrameTime(true);
	pConsole->showObjectCount(true);
	pConsole->showTriangleCount(true);
	pConsole->activateConsole(true);

	CLight *pLight = new CLight("MAIN_LIGHT");
	pLight->setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	pLight->setSpecular(1.0f, 0.0f, 1.0f, 1.0f);
	pLight->setLightPosition(GL_COORD_VERTEX(10.0f, 10.0f, 10.0f, 1.0f));
	pLight->setLightDirection(GL_COORD_VERTEX(0.0f, 0.0f, 0.0f, 1.0f));

	CViewModifier *vm = new CViewModifier("MyCube");
	CModifier::TIME_FUNCTION tz;
	tz.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tz.a0 = 0.2f;
	CModifier::TIME_FUNCTION tx;
	tx.timeFunction = CModifier::CGL_TIME_CONSTANT;
	tx.a0 = 0.3f;
	CModifier::TIME_FUNCTION ty;
	ty.timeFunction = CModifier::CGL_TIME_COSINE;
	ty.a2 = 0.3f;
	ty.a1 = 0.2f;
	ty.a0 = 0.005f; //0;
	vm->addAction(CViewModifier::ROTATE_VIEW, tx, ty, tz);
	vm->setObject(object);

	C3DScene *pScene = m_pDisplay->getRootScene();
	pScene->addObject(vm->getObject());
	pScene->addLight(pLight);

	IViewPoint *vp = m_pDisplay->getViewPoint();
	vp->setPosition(0.0f, 0.0f, 2.75f, IViewPoint::EYE);
	vp->setPosition(0.0f, 0.0f, 0.0f, IViewPoint::TARGET);

	CTimeObject::setTimeFactor(1.0f);
	CAnimator *pAnimator = new CAnimator();
	CAnimator::SetAnimator(pAnimator);
}

bool RAPTOR_WRAPPER_API glCreateRaptorRenderer(CRaptorDisplay* display, CRenderEntryPoint* &renderer)
{
	if (NULL == display)
		return false;

	CRenderer *r = new CRenderer(display);
	r->glInitRenderer();
	renderer = r;

	return true;
}