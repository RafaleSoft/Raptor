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
#include "GLHierarchy/GeometryEditor.h"
#include "GLHierarchy/TextureFactory.h"
#include "GLHierarchy/TextureUnitSetup.h"
#include "System/Image.h"
#include "System/RaptorConsole.h"
#include "System/RaptorGLExtensions.h"

#include "ToolBox/BasicObjects.h"
#include "ToolBox/RaptorToolBox.h"
#include "ToolBox/Imaging.h"
#include "DataManager/RaptorDataManager.h"

#include <iostream>
#include <fstream>

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

CShadedGeometry *object = NULL;

void CRenderer::glInitRenderer()
{
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
		object->getEditor().minimize();
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

bool RAPTOR_WRAPPER_API glSetMaps(const char* diffuse, const char* normal)
{
	bool upadateMap = false;

	if ((NULL != diffuse) && (NULL != object) && (strlen(diffuse) > 0))
	{
		CImage i;
		CVaArray<CImage::IImageOP*> ops;
		if (i.loadImage(diffuse, ops))
		{
			CShader *shader = object->getShader();
			CTextureUnitSetup *tmu = shader->glGetTextureUnitsSetup();

			CTextureFactory &factory = CTextureFactory::glGetDefaultFactory();
			ITextureObject *t = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
			factory.glLoadTexture(t, i);
			tmu->setDiffuseMap(t);

			object->addModel(CGeometry::RENDERING_MODEL::CGL_TEXTURE);

			upadateMap = true;
			IRenderingProperties &props = CRaptorDisplay::GetCurrentDisplay()->getRenderingProperties();
			props.enableLighting.enableTexturing;
		}
	}
		
	if ((NULL != normal) && (NULL != object) && (strlen(normal) > 0))
	{
		CImage i;
		CVaArray<CImage::IImageOP*> ops;
		if (i.loadImage(diffuse, ops))
		{
			CShader *shader = object->getShader();
			CTextureUnitSetup *tmu = shader->glGetTextureUnitsSetup();

			CTextureFactory &factory = CTextureFactory::glGetDefaultFactory();
			ITextureObject *t = factory.glCreateTexture(ITextureObject::CGL_COLOR24_ALPHA, ITextureObject::CGL_BILINEAR);
			factory.glLoadTexture(t, i);
			tmu->setNormalMap(t);

			object->addModel(CGeometry::RENDERING_MODEL::CGL_TEXTURE);

			upadateMap = true;
			IRenderingProperties &props = CRaptorDisplay::GetCurrentDisplay()->getRenderingProperties();
			props.enableLighting.enableTexturing;
		}
	}

	return upadateMap;
}

void write_int(GLenum pval, const char* pname, ofstream& out)
{
	GLint iparam = 0;
	glGetIntegerv(pval, &iparam);
	out.write(pname, strlen(pname));
	stringstream str;
	str << iparam;
	str << std::endl;
	out.write(str.str().c_str(), str.str().length());
}

bool RAPTOR_WRAPPER_API glDiag(void)
{
	char buffer[MAX_PATH];
	GetEnvironmentVariable("TMP", buffer, MAX_PATH);
	std::string filename = buffer;
	filename += "\\";
	filename += "gldiag.txt";

	ofstream gldiag;
	gldiag.open(filename, ios_base::trunc);
	if (gldiag.is_open())
	{
		std::streamsize nl = strlen("\n");

		const char *vendor = (char*)glGetString(GL_VENDOR);
		gldiag.write(vendor, strlen(vendor)); gldiag.write("\n", nl);

		const char *renderer = (char*)glGetString(GL_RENDERER);
		gldiag.write(renderer, strlen(renderer)); gldiag.write("\n", nl);

		const char *version = (char*)glGetString(GL_VERSION);
		gldiag.write(version, strlen(version)); gldiag.write("\n", nl);

		const char *glsl = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
		gldiag.write(glsl, strlen(glsl)); gldiag.write("\n", nl);

		gldiag.write("18\n", 2 + nl);

		write_int(GL_MAX_LIGHTS, "MAX_LIGHTS ", gldiag);
		write_int(GL_MAX_TEXTURE_SIZE, "MAX_TEXTURE_SIZE ", gldiag);
		write_int(GL_MAX_TEXTURE_UNITS_ARB, "MAX_TEXTURE_UNITS ", gldiag);
		write_int(GL_MAX_3D_TEXTURE_SIZE, "MAX_3D_TEXTURE_SIZE ", gldiag);
		write_int(GL_MAX_DRAW_BUFFERS, "MAX_DRAW_BUFFERS ", gldiag);
		write_int(GL_MAX_VERTEX_ATTRIBS, "MAX_VERTEX_ATTRIBS ", gldiag);
		write_int(GL_MAX_TEXTURE_COORDS, "MAX_TEXTURE_COORDS ", gldiag);
		write_int(GL_MAX_TEXTURE_IMAGE_UNITS, "MAX_TEXTURE_IMAGE_UNITS ", gldiag);
		write_int(GL_MAX_ATTRIB_STACK_DEPTH, "MAX_ATTRIB_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_MODELVIEW_STACK_DEPTH, "MAX_MODELVIEW_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_NAME_STACK_DEPTH, "MAX_NAME_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_PROJECTION_STACK_DEPTH, "MAX_PROJECTION_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_TEXTURE_STACK_DEPTH, "MAX_TEXTURE_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, "MAX_CLIENT_ATTRIB_STACK_DEPTH ", gldiag);
		write_int(GL_MAX_ELEMENTS_VERTICES, "MAX_ELEMENTS_VERTICES ", gldiag);
		write_int(GL_MAX_ELEMENTS_INDICES, "MAX_ELEMENTS_VERTICES ", gldiag);
		write_int(GL_MAX_CUBE_MAP_TEXTURE_SIZE, "MAX_CUBE_MAP_TEXTURE_SIZE ", gldiag);
		write_int(GL_MAX_TEXTURE_LOD_BIAS, "MAX_TEXTURE_LOD_BIAS ", gldiag);

		//GL_MAX_LIST_NESTING               0x0B31
		// GL_MAX_EVAL_ORDER                 0x0D30
		// GL_MAX_CLIP_PLANES                0x0D32
		// GL_MAX_PIXEL_MAP_TABLE            0x0D34
		// GL_MAX_VIEWPORT_DIMS              0x0D3A
				
		// GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
		// GL_MAX_VERTEX_UNIFORM_COMPONENTS
		// GL_MAX_VARYING_FLOATS
		// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
		// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
		// GL_MAX_CLIP_DISTANCES
		// GL_MAX_ARRAY_TEXTURE_LAYERS
		// GL_MAX_PROGRAM_TEXEL_OFFSET
		// GL_MAX_VARYING_COMPONENTS
		// GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS
		// GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
		// GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
		// GL_MAX_RENDERBUFFER_SIZE          0x84E8
		// GL_MAX_COLOR_ATTACHMENTS          0x8CDF
		// GL_MAX_SAMPLES                    0x8D57
		// GL_MAX_TEXTURE_BUFFER_SIZE        0x8C2B
		// GL_MAX_RECTANGLE_TEXTURE_SIZE     0x84F8
		// GL_MAX_VERTEX_UNIFORM_BLOCKS      0x8A2B
		// GL_MAX_GEOMETRY_UNIFORM_BLOCKS    0x8A2C
		// GL_MAX_FRAGMENT_UNIFORM_BLOCKS    0x8A2D
		// GL_MAX_COMBINED_UNIFORM_BLOCKS    0x8A2E
		// GL_MAX_UNIFORM_BUFFER_BINDINGS    0x8A2F
		// GL_MAX_UNIFORM_BLOCK_SIZE         0x8A30
		// GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
		// GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
		// GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
		// GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
		// GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
		// GL_MAX_GEOMETRY_OUTPUT_VERTICES   0x8DE0
		// GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
		// GL_MAX_VERTEX_OUTPUT_COMPONENTS   0x9122
		// GL_MAX_GEOMETRY_INPUT_COMPONENTS  0x9123
		// GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
		// GL_MAX_FRAGMENT_INPUT_COMPONENTS  0x9125
		// GL_MAX_SERVER_WAIT_TIMEOUT        0x9111
		// GL_MAX_SAMPLE_MASK_WORDS          0x8E59
		// GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
		// GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F
		// GL_MAX_INTEGER_SAMPLES            0x9110
		// GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
		// GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
		// GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
		// GL_MAX_FRAMEBUFFER_WIDTH          0x9315
		// GL_MAX_FRAMEBUFFER_HEIGHT         0x9316
		// GL_MAX_FRAMEBUFFER_LAYERS         0x9317
		// GL_MAX_FRAMEBUFFER_SAMPLES        0x9318
		// GL_MAX_MATRIX_PALETTE_STACK_DEPTH_ARB 0x8841
		// GL_MAX_PALETTE_MATRICES_ARB       0x8842
		// GL_MAX_TEXTURE_UNITS_ARB            0x84E2
		// GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB	0x851C
		// GL_MAX_3D_TEXTURE_SIZE_EXT			0x8073
		// GL_MAX_GENERAL_COMBINERS_NV			0x854D
		// GL_MAX_VERTEX_UNITS_ARB           0x86A4
		// GL_MAX_CONVOLUTION_WIDTH			0x801A
		// GL_MAX_CONVOLUTION_HEIGHT			0x801B
		// GL_MAX_COLOR_MATRIX_STACK_DEPTH		0x80B3
		// GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF
		// GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV \
		// GL_MAX_PROGRAM_INSTRUCTIONS_ARB		0x88A1
		// GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A3
		// GL_MAX_PROGRAM_TEMPORARIES_ARB		0x88A5
		// GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A7
		// GL_MAX_PROGRAM_PARAMETERS_ARB		0x88A9
		// GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AB
		// GL_MAX_PROGRAM_ATTRIBS_ARB			0x88AD
		// GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB	0x88AF
		// GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB 0x88B1
		// GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B3
		// GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB	0x88B4
		// GL_MAX_PROGRAM_ENV_PARAMETERS_ARB	0x88B5
		// GL_MAX_VERTEX_ATTRIBS_ARB			0x8869
		// GL_MAX_PROGRAM_MATRICES_ARB			0x862F
		// GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E
		// GL_MAX_PROGRAM_INSTRUCTIONS_ARB	0x88A1
		// GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A3
		// GL_MAX_PROGRAM_TEMPORARIES_ARB 0x88A5
		// GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A7
		// GL_MAX_PROGRAM_PARAMETERS_ARB 0x88A9
		// GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AB
		// GL_MAX_PROGRAM_ATTRIBS_ARB 0x88AD
		// GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB 0x88AF
		// GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB 0x88B4
		// GL_MAX_PROGRAM_ENV_PARAMETERS_ARB 0x88B5
		// GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB 0x880B
		// GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB 0x880C
		// GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB 0x880D
		// GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB 0x880E
		// GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB 0x880F
		// GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB 0x8810
		// GL_MAX_PROGRAM_MATRICES_ARB 0x862F
		// GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E
		// GL_MAX_TEXTURE_COORDS_ARB			0x8871
		// GL_MAX_TEXTURE_IMAGE_UNITS_ARB		0x8872
		// GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB   0x84F8
		// GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB	0x8B4A
		// GL_MAX_VARYING_FLOATS_ARB				0x8B4B
		// GL_MAX_VERTEX_ATTRIBS_ARB				0x8869
		// GL_MAX_TEXTURE_IMAGE_UNITS_ARB			0x8872
		// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB	0x8B4C
		// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB	0x8B4D
		// GL_MAX_TEXTURE_COORDS_ARB				0x8871
		// GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB	0x8B49
		// GL_MAX_TEXTURE_COORDS_ARB				0x8871
		// GL_MAX_TEXTURE_IMAGE_UNITS_ARB			0x8872
		// GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_ARB		0x8C29
		// GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB		0x8DDD
		// GL_MAX_VERTEX_VARYING_COMPONENTS_ARB		0x8DDE
		// GL_MAX_VARYING_COMPONENTS					0x8B4B	// alias GL_MAX_VARYING_FLOATS
		// GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_ARB		0x8DDF
		// GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB			0x8DE0
		// GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB	0x8DE1
		// GL_MAX_DRAW_BUFFERS_ARB	0x8824
		// GL_MAX_COLOR_ATTACHMENTS_EXT						0x8CDF
		// GL_MAX_RENDERBUFFER_SIZE_EXT						0x84E8
		// GL_MAX_SAMPLES_EXT							0x8D57
		// GL_MAX_CONVOLUTION_WIDTH_EXT      0x801A
		// GL_MAX_CONVOLUTION_HEIGHT_EXT     0x801B
		// GL_MAX_TEXTURE_BUFFER_SIZE_EXT				0x8C2B
		// GL_MAX_VERTEX_UNIFORM_BLOCKS_ARB					0x8A2B
		// GL_MAX_GEOMETRY_UNIFORM_BLOCKS_ARB					0x8A2C
		// GL_MAX_FRAGMENT_UNIFORM_BLOCKS_ARB					0x8A2D
		// GL_MAX_COMBINED_UNIFORM_BLOCKS_ARB					0x8A2E
		// GL_MAX_UNIFORM_BUFFER_BINDINGS_ARB					0x8A2F
		// GL_MAX_UNIFORM_BLOCK_SIZE_ARB						0x8A30
		// GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS_ARB		0x8A31
		// GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_ARB		0x8A32
		// GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS_ARB		0x8A33
		// GL_MAX_DEBUG_MESSAGE_LENGTH_ARB                      0x9143
		// GL_MAX_DEBUG_LOGGED_MESSAGES_ARB

		const CRaptorGLExtensions *ext = Raptor::glGetExtensions();
		const std::string &exts = ext->glExtensions();
		
		size_t p1 = 0;
		size_t p2 = exts.find(' ', p1);
		while (std::string::npos != p2)
		{
			std::string e = exts.substr(p1,p2-p1);
			gldiag.write(e.c_str(), e.length()); gldiag.write("\n", nl);

			p1 = p2 + 1;
			p2 = exts.find(' ', p1);
		}

		std::string e = exts.substr(p1);
		gldiag.write(e.c_str(), e.length()); gldiag.write("\n", nl);

		gldiag.close();
	}
	
	return true;
}
